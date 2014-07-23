/*
 * Switch matrix
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
 * Chapter 9: LPC81x Switch matrix
 */

#include <mmio.h>
#include <memorymap.h>

/* --- SWM registers ------------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	SWM_PINASSIGN0		Pin assign register 0
 * 0x004	SWM_PINASSIGN1		Pin assign register 1
 * 0x008	SWM_PINASSIGN2		Pin assign register 2
 * 0x00c	SWM_PINASSIGN3		Pin assign register 3
 * 0x010	SWM_PINASSIGN4		Pin assign register 4
 * 0x014	SWM_PINASSIGN5		Pin assign register 5
 * 0x018	SWM_PINASSIGN6		Pin assign register 6
 * 0x01c	SWM_PINASSIGN7		Pin assign register 7
 * 0x020	SWM_PINASSIGN8		Pin assign register 8
 * 0x1c0	SWM_PINENABLE0		Pin enable register 0
 */

#define SWM_PINASSIGN0			MMIO32(SWM_BASE + 0x000)
#define SWM_PINASSIGN1			MMIO32(SWM_BASE + 0x004)
#define SWM_PINASSIGN2			MMIO32(SWM_BASE + 0x008)
#define SWM_PINASSIGN3			MMIO32(SWM_BASE + 0x00c)
#define SWM_PINASSIGN4			MMIO32(SWM_BASE + 0x010)
#define SWM_PINASSIGN5			MMIO32(SWM_BASE + 0x014)
#define SWM_PINASSIGN6			MMIO32(SWM_BASE + 0x018)
#define SWM_PINASSIGN7			MMIO32(SWM_BASE + 0x01c)
#define SWM_PINASSIGN8			MMIO32(SWM_BASE + 0x020)
#define SWM_PINENABLE0			MMIO32(SWM_BASE + 0x1c0)

#define SWM_PINASSIGN_REGNUM		9

/* --- SWM_PINASSIGN0 values ----------------------------------------------- */

/* SWM_PINASSIN0[31:24]: U0_CTS_I */
#define SWM_PINASSIGN0_U0_CTS_I		24
/* SWM_PINASSIN0[23:16]: U0_RTS_O */
#define SWM_PINASSIGN0_U0_RTS_O		16
/* SWM_PINASSIN0[15:8]: U0_RXD_I */
#define SWM_PINASSIGN0_U0_RXD_I		8
/* SWM_PINASSIN0[7:0]: U0_TXD_O */
#define SWM_PINASSIGN0_U0_TXD_O		0

/* --- SWM_PINASSIGN1 values ----------------------------------------------- */

/* SWM_PINASSIN1[31:24]: U1_RTS_O */
#define SWM_PINASSIGN1_U1_RTS_O		24
/* SWM_PINASSIN1[23:16]: U1_RXD_I */
#define SWM_PINASSIGN1_U1_RXD_I		16
/* SWM_PINASSIN1[15:8]: U1_TXD_O */
#define SWM_PINASSIGN1_U1_TXD_O		8
/* SWM_PINASSIN1[7:0]: U0_SCLK_IO */
#define SWM_PINASSIGN1_U0_SCLK_IO	0

/* --- SWM_PINASSIGN2 values ----------------------------------------------- */

/* SWM_PINASSIN2[31:24]: U2_RXD_I */
#define SWM_PINASSIGN2_U2_RXD_I		24
/* SWM_PINASSIN2[23:16]: U2_TXD_O */
#define SWM_PINASSIGN2_U2_TXD_O		16
/* SWM_PINASSIN2[15:8]: U1_SCLK_IO */
#define SWM_PINASSIGN2_U1_SCLK_IO	8
/* SWM_PINASSIN2[7:0]: U1_CTS_I */
#define SWM_PINASSIGN2_U1_CTS_I		0

/* --- SWM_PINASSIGN3 values ----------------------------------------------- */

/* SWM_PINASSIN3[31:24]: SPI0_SCK_IO */
#define SWM_PINASSIGN3_SPI0_SCK_IO	24
/* SWM_PINASSIN3[23:16]: U2_SCLK_IO */
#define SWM_PINASSIGN3_U2_SCLK_IO	16
/* SWM_PINASSIN3[15:8]: U2_CTS_I */
#define SWM_PINASSIGN3_U2_CTS_I		8
/* SWM_PINASSIN3[7:0]: U2_RTS_O */
#define SWM_PINASSIGN3_U2_RTS_O		0

/* --- SWM_PINASSIGN4 values ----------------------------------------------- */

/* SWM_PINASSIN4[31:24]: SPI1_SCK_IO */
#define SWM_PINASSIGN4_SPI1_SCK_IO	24
/* SWM_PINASSIN4[23:16]: SPI0_SSEL_IO */
#define SWM_PINASSIGN4_SPI0_SSEL_IO	16
/* SWM_PINASSIN4[15:8]: SPI0_MISO_IO */
#define SWM_PINASSIGN4_SPI0_MISO_IO	8
/* SWM_PINASSIN4[7:0]: SPI0_MOSI_IO */
#define SWM_PINASSIGN4_SPI0_MOSI_IO	0

/* --- SWM_PINASSIGN5 values ----------------------------------------------- */

/* SWM_PINASSIN5[31:24]: CTIN_0_I */
#define SWM_PINASSIGN5_CTIN_0_I		24
/* SWM_PINASSIN5[23:16]: SPI1_SSEL_IO */
#define SWM_PINASSIGN5_SPI1_SSEL_IO	16
/* SWM_PINASSIN5[15:8]: SPI1_MISO_IO */
#define SWM_PINASSIGN5_SPI1_MISO_IO	8
/* SWM_PINASSIN5[7:0]: SPI1_MOSI_IO */
#define SWM_PINASSIGN5_SPI1_MOSI_IO	0

/* --- SWM_PINASSIGN6 values ----------------------------------------------- */

/* SWM_PINASSIN6[31:24]: CTOUT_0_O */
#define SWM_PINASSIGN6_CTOUT_0_O	24
/* SWM_PINASSIN6[23:16]: CTIN_3_I */
#define SWM_PINASSIGN6_CTIN_3_I		16
/* SWM_PINASSIN6[15:8]: CTIN_2_I */
#define SWM_PINASSIGN6_CTIN_2_I		8
/* SWM_PINASSIN6[7:0]: CTIN_1_I */
#define SWM_PINASSIGN6_CTIN_1_I		0

/* --- SWM_PINASSIGN7 values ----------------------------------------------- */

/* SWM_PINASSIN7[31:24]: I2C_SDA_IO */
#define SWM_PINASSIGN7_I2C_SDA_IO	24
/* SWM_PINASSIN7[23:16]: CTOUT_3_O */
#define SWM_PINASSIGN7_CTOUT_3_O	16
/* SWM_PINASSIN7[15:8]: CTOUT_2_O */
#define SWM_PINASSIGN7_CTOUT_2_O	8
/* SWM_PINASSIN7[7:0]: CTOUT_1_O */
#define SWM_PINASSIGN7_CTOUT_1_O	0

/* --- SWM_PINASSIGN8 values ----------------------------------------------- */

/* SWM_PINASSIN8[31:24]: GPIO_INT_BMAT_O */
#define SWM_PINASSIGN8_GPIO_INT_BMAT_O	24
/* SWM_PINASSIN8[23:16]: CLKOUT_O */
#define SWM_PINASSIGN8_CLKOUT_O		16
/* SWM_PINASSIN8[15:8]: ACMP_O_O */
#define SWM_PINASSIGN8_ACMP_O_O		8
/* SWM_PINASSIN8[7:0]: I2C_SCL_IO */
#define SWM_PINASSIGN8_I2C_SCL_IO	0

/* --- SWM_PINENABLE0 values ----------------------------------------------- */

#define SWM_PINENABLE0_VDDCMP		(1 << 8) /* PIO0_6 */
#define SWM_PINENABLE0_CLKIN		(1 << 7) /* PIO0_1 */
#define SWM_PINENABLE0_RESET_EN		(1 << 6) /* PIO0_5 */
#define SWM_PINENABLE0_XTALOUT_EN	(1 << 5) /* PIO0_9 */
#define SWM_PINENABLE0_XTALIN_EN	(1 << 4) /* PIO0_8 */
#define SWM_PINENABLE0_SWDIO_EN		(1 << 3) /* PIO0_2 */
#define SWM_PINENABLE0_SWCLK_EN		(1 << 2) /* PIO0_3 */
#define SWM_PINENABLE0_ACMP_I2_EN	(1 << 1) /* PIO0_1 */
#define SWM_PINENABLE0_ACMP_I1_EN	(1 << 0) /* PIO0_0 */
