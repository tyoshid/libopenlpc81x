/*
 * I/O configuratio (IOCON)
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
 * Chapter 6: LPC81x I/O configuration (IOCON)
 */

#include <mmio.h>
#include <memorymap.h>

/* --- IOCON registers ----------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	IOCON_PIO0_17		I/O configuration for pin PIO0_17
 * 0x004	IOCON_PIO0_13		I/O configuration for pin PIO0_13
 * 0x008	IOCON_PIO0_12		I/O configuration for pin PIO0_12
 * 0x00c	IOCON_PIO0_5		I/O configuration for pin PIO0_5
 * 0x010	IOCON_PIO0_4		I/O configuration for pin PIO0_4
 * 0x014	IOCON_PIO0_3		I/O configuration for pin PIO0_3
 * 0x018	IOCON_PIO0_2		I/O configuration for pin PIO0_2
 * 0x01c	IOCON_PIO0_11		I/O configuration for pin PIO0_11
 * 0x020	IOCON_PIO0_10		I/O configuration for pin PIO0_10
 * 0x024	IOCON_PIO0_16		I/O configuration for pin PIO0_16
 * 0x028	IOCON_PIO0_15		I/O configuration for pin PIO0_15
 * 0x02c	IOCON_PIO0_1		I/O configuration for pin PIO0_1
 * 0x034	IOCON_PIO0_9		I/O configuration for pin PIO0_9
 * 0x038	IOCON_PIO0_8		I/O configuration for pin PIO0_8
 * 0x03c	IOCON_PIO0_7		I/O configuration for pin PIO0_7
 * 0x040	IOCON_PIO0_6		I/O configuration for pin PIO0_6
 * 0x044	IOCON_PIO0_0		I/O configuration for pin PIO0_0
 * 0x048	IOCON_PIO0_14		I/O configuration for pin PIO0_14
 */

#define IOCON_PIO0_17			MMIO32(IOCON_BASE + 0x000)
#define IOCON_PIO0_13			MMIO32(IOCON_BASE + 0x004)
#define IOCON_PIO0_12			MMIO32(IOCON_BASE + 0x008)
#define IOCON_PIO0_5			MMIO32(IOCON_BASE + 0x00c)
#define IOCON_PIO0_4			MMIO32(IOCON_BASE + 0x010)
#define IOCON_PIO0_3			MMIO32(IOCON_BASE + 0x014)
#define IOCON_PIO0_2			MMIO32(IOCON_BASE + 0x018)
#define IOCON_PIO0_11			MMIO32(IOCON_BASE + 0x01c)
#define IOCON_PIO0_10			MMIO32(IOCON_BASE + 0x020)
#define IOCON_PIO0_16			MMIO32(IOCON_BASE + 0x024)
#define IOCON_PIO0_15			MMIO32(IOCON_BASE + 0x028)
#define IOCON_PIO0_1			MMIO32(IOCON_BASE + 0x02c)
#define IOCON_PIO0_9			MMIO32(IOCON_BASE + 0x034)
#define IOCON_PIO0_8			MMIO32(IOCON_BASE + 0x038)
#define IOCON_PIO0_7			MMIO32(IOCON_BASE + 0x03c)
#define IOCON_PIO0_6			MMIO32(IOCON_BASE + 0x040)
#define IOCON_PIO0_0			MMIO32(IOCON_BASE + 0x044)
#define IOCON_PIO0_14			MMIO32(IOCON_BASE + 0x048)

/* --- IOCON_PIO0_x values ------------------------------------------------- */

#define IOCON_CLK_DIV2			(1 << 15)
#define IOCON_CLK_DIV1			(1 << 14)
#define IOCON_CLK_DIV0			(1 << 13)
#define IOCON_S_MODE1			(1 << 12)
#define IOCON_S_MODE0			(1 << 11)
#define IOCON_OD			(1 << 10)
#define IOCON_I2CMODE1			(1 << 9)
#define IOCON_I2CMODE0			(1 << 8)
#define IOCON_INV			(1 << 6)
#define IOCON_HYS			(1 << 5)
#define IOCON_MODE1			(1 << 4)
#define IOCON_MODE0			(1 << 3)

/* CLK_DIV */
#define IOCON_CLK_DIV_IOCONCLKDIV0	(0 << 13)
#define IOCON_CLK_DIV_IOCONCLKDIV1	(1 << 13)
#define IOCON_CLK_DIV_IOCONCLKDIV2	(2 << 13)
#define IOCON_CLK_DIV_IOCONCLKDIV3	(3 << 13)
#define IOCON_CLK_DIV_IOCONCLKDIV4	(4 << 13)
#define IOCON_CLK_DIV_IOCONCLKDIV5	(5 << 13)
#define IOCON_CLK_DIV_IOCONCLKDIV6	(6 << 13)

/* S_MODE */
#define IOCON_S_MODE_BYPASS		(0 << 11)
#define IOCON_S_MODE_1CLOCK		(1 << 11)
#define IOCON_S_MODE_2CLOCK		(2 << 11)
#define IOCON_S_MODE_3CLOCK		(3 << 11)

/* I2CMODE */
#define IOCON_I2CMODE_I2C		(0 << 8)
#define IOCON_I2CMODE_IO		(1 << 8)
#define IOCON_I2CMODE_FAST_PLUS		(2 << 8)

/* MODE */
#define IOCON_MODE_INACTIVE		(0 << 3)
#define IOCON_MODE_PULL_DOWN		(1 << 3)
#define IOCON_MODE_PULL_UP		(2 << 3)
#define IOCON_MODE_REPEATER		(3 << 3)
