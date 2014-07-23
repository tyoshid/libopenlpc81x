/*
 * main.c
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
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <strings.h>

#include "command.h"
#include "transfer.h"

#define RETRY 10

bool debug;
int sram_size = 1024;

static struct {
	char *name;
	speed_t speed;
} baud_table[] = {
	{"9600", B9600},
	{"19200", B19200},
	{"38400", B38400},
	{"57600", B57600},
	{"115200", B115200},
	{0, 0}
};

static struct {
	uint32_t pid;
	char *dev_name;
	int sram;
} device_table[] = {
	{PID_LPC810M021FN8, "LPC810M021FN8", 1024},
	{PID_LPC811M001JDH16, "LPC811M001JDH16", 2048},
	{PID_LPC812M101JDH16, "LPC812M101JDH16", 4096},
	{PID_LPC812M101JD20, "LPC812M101JD20", 4096},
	{PID_LPC812M101JDH20, "LPC812M101JDH20/JTB16", 4096},
	{0, NULL, 0}
};

static void usage(char *prog)
{
	printf("Usage: %s [options]\n", prog);
	printf("  -h\t\tPrint this message\n");
	printf("  -v\t\tPrint verbose debug statements\n");
	printf("  -d <dev>\tSpecify USART device (default: /dev/ttyUSB0)\n");
	printf("  -b <baud>\tSpecify baud rate (default: 115200)\n");
	printf("  -t <bytes>\tSpecify the number of upload transfer bytes\n");
	printf("  -U <file>\tRead firmware from device into file\n");
	printf("  -D <file>\tWrite firmware from file into device\n");
}

int main(int argc, char *argv[])
{
	int opt;
	int i;
	speed_t baudrate = B115200;
	bool download_flag = false;
	char *fname = NULL;
	char *usart = "/dev/ttyUSB0";
	int size = 256;
	bool upload_flag = false;
	FILE *stream;
	int fd;
	struct termios oldtio;
	struct termios newtio;
	uint8_t version[2];
	uint32_t pid;

	while ((opt = getopt(argc, argv, "b:D:d:ht:U:v")) != -1) {
		switch (opt) {
		case 'b':
			for (i = 0; baud_table[i].name; i++) {
				if (strcmp(optarg, baud_table[i].name) == 0) {
					baudrate = baud_table[i].speed;
					break;
				}
			}
			if (!baud_table[i].name) {
				fprintf(stderr, "Invalid baud rate (%s).\n",
					optarg);
				return 1;
			}
			break;
		case 'D':
			download_flag = true;
			fname = optarg;
			break;
		case 'd':
			usart = optarg;
			break;
		case 'h':
			usage(argv[0]);
			return 0;
		case 't':
			size = atoi(optarg);
			break;
		case 'U':
			upload_flag = true;
			fname = optarg;
			break;
		case 'v':
			debug = true;
			break;
		default:
			usage(argv[0]);
			return 1;
		}
	}

	if ((download_flag && upload_flag) ||
	    (!download_flag && !upload_flag)) {
		fprintf(stderr, "You need to specify one of -D or -U\n");
		return 1;
	}

	/* Open data file. */
	if (download_flag)
		stream = fopen(fname, "r");
	else
		stream = fopen(fname, "w");
	if (stream == NULL) {
		perror(fname);
		return 1;
	}

	/* Open serial port. */
	fd = open(usart, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror(usart);
		fclose(stream);
		return 1;
	}

	if (tcgetattr(fd, &oldtio)) {
		perror("tcgetattr() failed");
		close(fd);
		fclose(stream);
		return 1;
	}

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = baudrate | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	newtio.c_lflag = 0;

	newtio.c_cc[VTIME] = 5;
	newtio.c_cc[VMIN] = 0;

	if (tcflush(fd, TCIFLUSH)) {
		perror("tcflush() failed");
		close(fd);
		fclose(stream);
		return 1;
	}
	if (tcsetattr(fd, TCSANOW, &newtio)) {
		perror("tcsetattr() failed");
		close(fd);
		fclose(stream);
		return 1;
	}

	/* ISP initialization */
	if (isp_init(fd, RETRY))
		goto ioerror;

	/* Echo off */
	if (echo(fd, 0))
		goto ioerror;

	/* Read ISP version */
	if (read_boot_code_version(fd, version))
		goto ioerror;
	printf("ISP version %d.%d\n", version[1], version[0]);

	/* Read PID */
	if (read_part_id(fd, &pid))
		goto ioerror;
	printf("PID 0x%x  ", pid);
	for (i = 0; device_table[i].dev_name; i++) {
		if (device_table[i].pid == pid) {
			printf("%s\n", device_table[i].dev_name);
			sram_size = device_table[i].sram;
			break;
		}
	}
	if (!device_table[i].dev_name)
		printf("unknown device\n");

	/* Transfer data. */
	if (upload_flag) {
		if (upload(fd, stream, size) < 0)
			goto ioerror;
	}
	if (download_flag) {
		if (download(fd, stream) < 0)
			goto ioerror;
	}

	/* Close serial port. */
	if (tcsetattr(fd, TCSANOW, &oldtio)) {
		perror("tcsetattr() failed");
		close(fd);
		fclose(stream);
		return 1;
	}
	if (close(fd)) {
		perror("close() failed");
		fclose(stream);
		return 1;
	}

	/* Close data file. */
	if (fclose(stream)) {
		perror("fclose() failed");
		return 1;
	}

	return 0;

ioerror:
	tcsetattr(fd, TCSANOW, &oldtio);
	close(fd);
	fclose(stream);
	return 1;
}
