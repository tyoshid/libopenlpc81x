/*
 * Cyclic Redundancy Check (CRC)
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

/*
 * NXP Semiconductors(www.nxp.com)
 *
 * UM10601: LPC81x User manual
 * (Rev.1.6 - 2 April 2014)
 *
 * Chapter 19: LPC81x Cyclic Redundancy Check (CRC) engine
 */

#include <mmio.h>
#include <memorymap.h>

/* --- CRC registers ------------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	CRC_MODE		CRC mode register
 * 0x004	CRC_SEED		CRC seed register
 * 0x008	CRC_SUM			CRC checksum register
 * 0x008	CRC_WR_DATA		CRC data register
 */

#define CRC_MODE			MMIO32(CRC_BASE + 0x000)
#define CRC_SEED			MMIO32(CRC_BASE + 0x004)
#define CRC_SUM				MMIO32(CRC_BASE + 0x008)
#define CRC_WR_DATA			MMIO32(CRC_BASE + 0x008)
#define CRC_WR_DATA16			MMIO16(CRC_BASE + 0x008)
#define CRC_WR_DATA8			MMIO8(CRC_BASE + 0x008)

/* --- CRC_MODE values ----------------------------------------------------- */

#define CRC_MODE_CMPL_SUM		(1 << 5)
#define CRC_MODE_BIT_RVS_SUM		(1 << 4)
#define CRC_MODE_CMPL_WR		(1 << 3)
#define CRC_MODE_BIT_RVS_WR		(1 << 2)
#define CRC_MODE_CRC_POLY1		(1 << 1)
#define CRC_MODE_CRC_POLY0		(1 << 0)

/* CRC_POLY */
#define CRC_MODE_CRC_POLY_CRC_CCITT	(0 << 0)
#define CRC_MODE_CRC_POLY_CRC_16	(1 << 0)
#define CRC_MODE_CRC_POLY_CRC_32	(2 << 0)

/* --- CRC_SEED values ----------------------------------------------------- */

/* CRC_SEED[31:0]: CRC_SEED */

/* --- CRC_SUM values ------------------------------------------------------ */

/* CRC_SUM[31:0]: CRC_SUM */

/* --- CRC_WR_DATA values -------------------------------------------------- */

/* CRC_WR_DATA[31:0]: CRC_WR_DATA */

/* --- Function prototypes ------------------------------------------------- */

/* Mode */
enum {
	CRC_CCITT = (0 << 0),
	CRC_16 = (1 << 0),
	CRC_32 = (2 << 0),
	CRC_BIT_RVS_WR = (1 << 2),
	CRC_CMPL_WR = (1 << 3),
	CRC_BIT_RVS_SUM = (1 << 4),
	CRC_CMPL_SUM = (1 << 5)
};

void crc_set_mode(int mode);
int crc_calc(int crc, char *buffer, int len);
int crc_calc16(int crc, short *buffer, int len);
int crc_calc32(int crc, int *buffer, int len);
