/*
 * command.c - LPC81x Flash ISP (v13.4) interface
 *
 * Copyright 2014 Toshiaki Yoshida <yoshida@mpc.net>
 *
 * This file is part of usart-util.
 *
 * usart-util is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * usart-util is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "command.h"

extern bool debug;

uint32_t offset;
uint32_t contents;

static bool echo_disable;

static int com_write(int fd, char *s, int size)
{
	int i;
	int r;
	char d;

	if (echo_disable) {
		if (write(fd, s, size) < 0) {
			perror("write() failed");
			return -1;
		}
	} else {
		for (i = 0; i < size; i++) {
			r = write(fd, s, 1);
			if (r < 0) {
				perror("write() failed");
				return -1;
			}

			/* Wait for echo */
			r = read(fd, &d, 1);
			if (r < 0) {
				perror("read() failed");
				return -1;
			}
			if (r != 1)
				return ERROR_TIMEOUT;
			if (d != *s)
				return ERROR_INVALID_VALUE;

			s++;
		}
	}

	return 0;
}

static int com_puts(int fd, char *s)
{
	return com_write(fd, s, strlen(s));
}

static int com_read(int fd, char *s, int size)
{
	int i;
	int r;

	for (i = 0; i < size; i++) {
		r = read(fd, s, 1);
		if (r < 0) {
			perror("read() failed");
			return -1;
		}
		if (r != 1)
			return ERROR_TIMEOUT;

		s++;
	}
	return 0;
}

static int com_gets(int fd, char *s, int size)
{
	int i;
	int r;

	for (i = 0; i < size - 1; i++) {
		r = read(fd, s, 1);
		if (r < 0) {
			perror("read() failed");
			return -1;
		}
		if (r != 1)
			return ERROR_TIMEOUT;

		if (*s++ == '\n') {
			*s = '\0';
			return 0;
		}
	}
	return ERROR_SIZE;
}

static int print_error(int code)
{
	/* UART ISP Return Codes */
	static char *code_table[] = {
		"CMD_SUCCESS",
		"INCALID_COMMAND",
		"SRC_ADDR_ERROR",
		"DST_ADDR_ERROR",
		"SRC_ADDR_NOT_MAPPED",
		"DST_ADDR_NOT_MAPPED",
		"COUNT_ERROR",
		"INVALID_SECTOR",
		"SECTOR_NOT_BLANK",
		"SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION",
		"COMPARE_ERROR",
		"BUSY",
		"PARAM_ERROR",
		"ADDR_ERROR",
		"ADDR_NOT_MAPPED",
		"CMD_LOCKED",
		"INVALID_CODE",
		"INVALID_BAUD_RATE",
		"INVALID_STOP_BIT",
		"CODE_READ_PROTECTION_ENABLED"
	};

	if (code < 0 || code >= sizeof(code_table) / sizeof(char *)) {
		fprintf(stderr, "unknown return code\n");
		return ERROR_INVALID_VALUE;
	}
	fprintf(stderr, "%s\n", code_table[code]);
	return code;
}

/* ISP initialization */
int isp_init(int fd, int retry)
{
	int i;
	char buf[32];
	int r;

	for (i = 0; i < retry; i++) {
		/* Send '?'(0x3F) */
		buf[0] = '?';
		if (write(fd, buf, 1) < 0) {
			perror("write() failed");
			return -1;
		}

		/* Wait for "Synchronized" */
		r = com_gets(fd, buf, sizeof(buf));
		if (r < 0)
			return r;
		if (!r && !strcmp(buf, "Synchronized\r\n")) {
			break;
		}
	}
	if (i >= retry) {
		fprintf(stderr, "can't get \"Synchronized\"\n");
		return -1;
	}

	if (debug)
		printf("Synchronized\n");

	/* Send "Synchronized" */
	r = com_puts(fd, "Synchronized\r\n");
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send \"Synchronized\"\n");
		return r;
	}

	/* Wait for "OK" */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r || strcmp(buf, "OK\r\n")) {
		fprintf(stderr, "can't get \"OK\"\n");
		if (r)
			return r;
		return ERROR_INVALID_VALUE;
	}

	if (debug)
		printf("OK\n");

	/* Send clock frequency */
	r = com_puts(fd, "12000\r\n");
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send clock frequency\n");
		return r;
	}

	/* Wait for "OK" */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r || strcmp(buf, "OK\r\n")) {
		fprintf(stderr, "can't get \"OK\"\n");
		if (r)
			return r;
		return ERROR_INVALID_VALUE;
	}

	return 0;
}

/* Unlock */
int unlock(int fd)
{
	int r;
	char buf[32];

	if (debug)
		printf("Unlock\n");

	/* Send Unlock command */
	r = com_puts(fd, "U 23130\r\n");
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Unlock command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	return 0;
}

/* Set Baud Rate */
int set_baud_rate(int fd, char *baud, int stop)
{
	char buf[32];
	int r;

	if (debug)
		printf("Set Baud Rate (%s %d)\n", baud, stop);

	/* Send Set Baud Rate command */
	sprintf(buf, "B %s %d\r\n", baud, stop);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Set Baud Rate command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	return 0;
}

/* Echo */
int echo(int fd, int setting)
{
	char buf[32];
	int r;

	if (debug)
		printf("Echo (%d)\n", setting);

	/* Send Echo command */
	sprintf(buf, "A %d\r\n", setting);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Echo command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	echo_disable = setting ? false : true;

	return 0;
}

/* Write to RAM */
int write_to_ram(int fd, uint32_t addr, int bytes, uint8_t *data)
{
	char buf[32];
	int r;

	if (debug)
		printf("Write to RAM (0x%08x %d)\n", addr, bytes);

	/* Send Write to RAM command */
	sprintf(buf, "W %u %d\r\n", addr, bytes);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Write to RAM command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	/* Send data */
	r = com_write(fd, (char *)data, bytes);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send data\n");
		return r;
	}

	return 0;
}

/* Read Memory */
int read_memory(int fd, uint32_t addr, int bytes, uint8_t *data)
{
	char buf[32];
	int r;

	if (debug)
		printf("Read Memory (0x%08x %d)\n", addr, bytes);

	/* Send Read Memory command */
	sprintf(buf, "R %u %d\r\n", addr, bytes);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Read Memory command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	/* Get data */
	r = com_read(fd, (char *)data, bytes);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get data\n");
		return r;
	}

	return 0;
}

/* Prepare sectors for write operation */
int prepare_sectors(int fd, int start, int end)
{
	char buf[32];
	int r;

	if (debug)
		printf("Prepare sectors for write operation (%d %d)\n",
		       start, end);

	/* Send Prepare sectors for write operation command */
	sprintf(buf, "P %d %d\r\n", start, end);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Prepare sectors for write "
			"operation  command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	return 0;
}

/* Copy RAM to flash */
int copy_ram_to_flash(int fd, uint32_t flash, uint32_t ram, int bytes)
{
	char buf[32];
	int r;

	if (debug)
		printf("Copy RAM to flash (0x%08x 0x%08x %d)\n",
		       flash, ram, bytes);

	/* Send Copy RAM to flash command */
	sprintf(buf, "C %u %u %d\r\n", flash, ram, bytes);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Copy RAM to flash command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	return 0;
}

/* Go */
int go(int fd, uint32_t addr)
{
	char buf[32];
	int r;

	if (debug)
		printf("Go (0x%08x)\n", addr);

	/* Send Go command */
	sprintf(buf, "G %u T\r\n", addr);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Go command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	return 0;
}

/* Erase sectors */
int erase_sectors(int fd, int start, int end)
{
	char buf[32];
	int r;

	if (debug)
		printf("Erase sectors (%d %d)\n", start, end);

	/* Send Erase sectors command */
	sprintf(buf, "E %d %d\r\n", start, end);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Erase sectors command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	return 0;
}

/* Blank check sectors */
int blank_check_sectors(int fd, int start, int end)
{
	char buf[32];
	int r;

	if (debug)
		printf("Blank check sectors (%d %d)\n", start, end);

	/* Send Blank check sectors command */
	sprintf(buf, "I %d %d\r\n", start, end);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Blank check sectors command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r == RESULT_SECTOR_NOT_BLANK) {
		/* Offset */
		r = com_gets(fd, buf, sizeof(buf));
		if (r < 0)
			return r;
		if (r) {
			fprintf(stderr, "can't get offset\n");
			return r;
		}
		if (sscanf(buf, "%u/r/n", &offset) != 1) {
			fprintf(stderr, "invalid offset\n");
			return ERROR_INVALID_VALUE;
		}

		/* Contents */
		r = com_gets(fd, buf, sizeof(buf));
		if (r < 0)
			return r;
		if (r) {
			fprintf(stderr, "can't get contents\n");
			return r;
		}
		if (sscanf(buf, "%u/r/n", &contents) != 1) {
			fprintf(stderr, "invalid contents\n");
			return ERROR_INVALID_VALUE;
		}

		if (debug)
			printf("SECTOR_NOT_BLANK %u 0x%08x\n",
			       offset, contents);

		return RESULT_SECTOR_NOT_BLANK;
	} else if (r) {
		return print_error(r);
	}

	return 0;
}

/* Read Boot code version numver */
int read_boot_code_version(int fd, uint8_t *version)
{
	int r;
	char buf[32];

	if (debug)
		printf("Read Boot code version number\n");

	/* Send Read Boot code version command */
	r = com_puts(fd, "K\r\n");
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Read Boot code version command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	/* Get Minor version */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get minor version number\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid minor version number\n");
		return ERROR_INVALID_VALUE;
	}
	if (r < 0 || r > 255) {
		fprintf(stderr, "invalid minor version number\n");
		return ERROR_INVALID_VALUE;
	}
	*version++ = r;

	/* Get Major version */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get major version number\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid major version number\n");
		return ERROR_INVALID_VALUE;
	}
	if (r < 0 || r > 255) {
		fprintf(stderr, "invalid major version number\n");
		return ERROR_INVALID_VALUE;
	}
	*version = r;

	return 0;
}

/* Read Part Identidication numver */
int read_part_id(int fd, uint32_t *pid)
{
	int r;
	char buf[32];

	if (debug)
		printf("Read Part Identification number\n");

	/* Send Read Part Identification command */
	r = com_puts(fd, "J\r\n");
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Read Part Identification "
			"command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	/* Get Part Identification number */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get Part Identification number\n");
		return r;
	}
	if (sscanf(buf, "%u/r/n", pid) != 1) {
		fprintf(stderr, "invalid Part Identification number\n");
		return ERROR_INVALID_VALUE;
	}

	return 0;
}

/* Compare */
int compare(int fd, uint32_t addr1, uint32_t addr2, int bytes)
{
	char buf[32];
	int r;

	if (debug)
		printf("Compare (0x%08x 0x%08x %d)\n", addr1, addr2, bytes);

	/* Send Compare command */
	sprintf(buf, "M %u %u %d\r\n", addr1, addr2, bytes);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Compare command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r == RESULT_COMPARE_ERROR) {
		/* Offset */
		r = com_gets(fd, buf, sizeof(buf));
		if (r < 0)
			return r;
		if (r) {
			fprintf(stderr, "can't get offset\n");
			return r;
		}
		if (sscanf(buf, "%u/r/n", &offset) != 1) {
			fprintf(stderr, "invalid offset\n");
			return ERROR_INVALID_VALUE;
		}

		if (debug)
			printf("COMPARE_ERROR %u\n", offset);

		return RESULT_COMPARE_ERROR;
	} else if (r) {
		return print_error(r);
	}

	return 0;
}

/* Read UID */
int read_uid(int fd, uint32_t *uid)
{
	int r;
	char buf[32];

	if (debug)
		printf("Read UID\n");

	/* Send Read UID command */
	r = com_puts(fd, "N\r\n");
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Read UID command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	/* Get UID */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get UID\n");
		return r;
	}
	if (sscanf(buf, "%u/r/n", uid) != 1) {
		fprintf(stderr, "invalid UID\n");
		return ERROR_INVALID_VALUE;
	}

	return 0;
}

/* Read CRC checksum */
int read_crc_checksum(int fd, uint32_t addr, int bytes, uint32_t *checksum)
{
	char buf[32];
	int r;

	if (debug)
		printf("Read CRC checksum\n");

	/* Send Read CRC checksum command */
	sprintf(buf, "S %u %d\r\n", addr, bytes);
	r = com_puts(fd, buf);
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't send Read CRC checksum command\n");
		return r;
	}

	/* Get UART ISP Return code */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get return code\n");
		return r;
	}
	if (sscanf(buf, "%d/r/n", &r) != 1) {
		fprintf(stderr, "invalid return code\n");
		return ERROR_INVALID_VALUE;
	}
	if (r)
		return print_error(r);

	/* Get checksum */
	r = com_gets(fd, buf, sizeof(buf));
	if (r < 0)
		return r;
	if (r) {
		fprintf(stderr, "can't get checksum\n");
		return r;
	}
	if (sscanf(buf, "%u/r/n", checksum) != 1) {
		fprintf(stderr, "invalid checksum\n");
		return ERROR_INVALID_VALUE;
	}

	return 0;
}
