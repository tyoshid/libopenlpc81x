/*
 * CRC functions
 *
 * Copyright 2014 Toshiaki Yoshida <yoshida@mpc.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <crc.h>

void crc_set_mode(int mode)
{
	CRC_MODE = mode;
}

int crc_calc(int crc, char *buffer, int len)
{
	int i;

	CRC_SEED = crc;
	for (i = 0; i < len; i++)
		CRC_WR_DATA8 = *buffer++;
	return CRC_SUM;
}

int crc_calc16(int crc, short *buffer, int len)
{
	int i;

	CRC_SEED = crc;
	for (i = 0; i < len; i++)
		CRC_WR_DATA16 = *buffer++;
	return CRC_SUM;
}

int crc_calc32(int crc, int *buffer, int len)
{
	int i;

	CRC_SEED = crc;
	for (i = 0; i < len; i++)
		CRC_WR_DATA = *buffer++;
	return CRC_SUM;
}
