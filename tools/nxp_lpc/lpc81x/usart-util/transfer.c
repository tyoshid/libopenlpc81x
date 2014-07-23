/*
 * transfer.c - Download or upload file.
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

#include "command.h"
#include "transfer.h"

extern bool debug;
extern int sram_size;

int upload(int fd, FILE *stream, int bytes)
{
	uint32_t a;
	int i;
	int n;
	int r;
	uint8_t buf[1024];

	a = FLASH_ADDRESS;
	i = 0;
	while (i < bytes) {
		n = (bytes - i) > sizeof(buf) ? sizeof(buf) : bytes - i;

		r = read_memory(fd, a, n, buf);
		if (r)
			return -1;
		r = fwrite(buf, 1, n, stream);
		if (r != n) {
			fprintf(stderr, "fwrite() failed\n");
			return -1;
		}

		if (debug)
			printf("%08x\n", a);

		a += n;
		i += n;
	}

	printf("read %d bytes\n", bytes);
	return bytes;
}

int download(int fd, FILE *stream)
{
	uint32_t ramaddr;
	int ramsize;
	int sector;
	int bytes;
	uint32_t flashaddr;
	int r;
	uint8_t buf[SECTOR_SIZE];
	int n;
	int i;
	uint32_t w;
	int m;
	uint8_t flash[SECTOR_SIZE];

	if (sram_size <= 1024) {
		ramaddr = SRAM_ADDRESS + RESERVE_SIZE;
		ramsize = sram_size - RESERVE_SIZE;
	} else {
		ramaddr = SRAM_ADDRESS + SECTOR_SIZE;
		ramsize = SECTOR_SIZE;
	}

	if (unlock(fd))
		return -1;

	sector = 0;
	bytes = 0;
	flashaddr = FLASH_ADDRESS;
	while (!feof(stream)) {
		r = fread(buf, 1, sizeof(buf), stream);
		n = (r + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
		if (r < sizeof(buf)) {
			if (ferror(stream)) {
				fprintf(stderr, "File read error\n");
				return -1;
			}

			if (r == 0)
				break;

			for (i = r; i < n; i++)
				buf[i] = 0xff;
		}
		bytes += r;

		/* Set checksum */
		if (sector == 0) {
			w = 0;
			for (i = 0; i < 7; i++)
				w += buf[i * 4 + 3] << 24 |
					buf[i * 4 + 2] << 16 |
					buf[i * 4 + 1] << 8 |
					buf[i * 4];
			w = -w;
			buf[7 * 4] = w & 0xff;
			buf[7 * 4 + 1] = w >> 8 & 0xff;
			buf[7 * 4 + 2] = w >> 16 & 0xff;
			buf[7 * 4 + 3] = w >> 24;

			printf("Checksum = 0x%08x\n", w);
		}

		if (debug)
			printf("- Blank check -\n");
		/* Blank check */
		r = blank_check_sectors(fd, sector, sector);
		if (r == RESULT_SECTOR_NOT_BLANK) {
			if (debug)
				printf("- Erase -\n");
			/* Erase */
			if (prepare_sectors(fd, sector, sector))
				return -1;

			if (erase_sectors(fd, sector, sector))
				return -1;

			/* Erase check */
			if (blank_check_sectors(fd, sector, sector))
				return -1;
		} else if (r) {
			return -1;
		}

		if (debug)
			printf("- Write data -\n");
		/* Write data */
		for (i = 0; i < n; i += ramsize) {
			m = ramsize < n - i ? ramsize : n - i;
			if (write_to_ram(fd, ramaddr, m, &buf[i]))
				return -1;

			if (prepare_sectors(fd, sector, sector))
				return -1;

			if (copy_ram_to_flash(fd, flashaddr + i, ramaddr, m))
				return -1;
		}

		if (debug)
			printf("- Verify data -\n");
		/* Verify data */
		if (read_memory(fd, flashaddr, n, flash))
			return -1;

		for (i = 0; i < n; i++) {
			if (buf[i] != flash[i]) {
				fprintf(stderr, "Verify failed\n");
				return -1;
			}
		}

		sector++;
		flashaddr += n;
	}

	/* Check Code Read Protection */
	if (read_memory(fd, CRP, 4, flash))
		return -1;

	w = flash[PAGE_SIZE - 1] << 24 |
		flash[PAGE_SIZE - 2] << 16 |
		flash[PAGE_SIZE - 3] << 8 |
		flash[PAGE_SIZE - 4];

	switch (w) {
	case 0x12345678:
		printf("CRP: CRP1\n");
		break;
	case 0x87654321:
		printf("CRP: CRP2\n");
		break;
	case 0x43218765:
		printf("CRP: CRP3\n");
		break;
	case 0x4e697370:
		printf("CRP: NO_ISP\n");
		break;
	default:
		printf("CRP: NONE\n");
		break;
	}

	printf("wrote %d bytes\n", bytes);
	return bytes;
}
