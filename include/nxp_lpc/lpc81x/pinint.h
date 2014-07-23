/*
 * Pin interrupts/pattern match engine
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
 * Chapter 8: LPC81x Pin interrupts/pattern match engine
 */

#include <mmio.h>
#include <memorymap.h>

/* --- PININT registers ---------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	PININT_ISEL		Pin interrupt mode register
 * 0x004	PININT_IENR		Pin interrupt level or rising edge
 *					interrupt enable register
 * 0x008	PININT_SIENR		Pin interrupt level or rising edge
 *					interrupt set register
 * 0x00c	PININT_CIENR		Pin interrupt level or rising edge
 *					interrupt clear register
 * 0x010	PININT_IENF		Pin interrupt active level or falling
 *					edge interrupt enable register
 * 0x014	PININT_SIENF		Pin interrupt active level or falling
 *					edge interrupt set register
 * 0x018	PININT_CIENF		Pin interrupt active level or falling
 *					edge interrupt clear register
 * 0x01c	PININT_RISE		Pin interrupt rising edge register
 * 0x020	PININT_FALL		Pin interrupt falling edge register
 * 0x024	PININT_IST		Pin interrupt status register
 * 0x028	PININT_PMCTRL		Pattern match interrupt control
 *					register
 * 0x02c	PININT_PMSRC		Pattern match interrupt bit-slice
 *					source register
 * 0x030	PININT_PMCFG		Pattern match interrupt bit slice
 *					configuration register
 */

#define PININT_ISEL			MMIO32(PININT_BASE + 0x000)
#define PININT_IENR			MMIO32(PININT_BASE + 0x004)
#define PININT_SIENR			MMIO32(PININT_BASE + 0x008)
#define PININT_CIENR			MMIO32(PININT_BASE + 0x00c)
#define PININT_IENF			MMIO32(PININT_BASE + 0x010)
#define PININT_SIENF			MMIO32(PININT_BASE + 0x014)
#define PININT_CIENF			MMIO32(PININT_BASE + 0x018)
#define PININT_RISE			MMIO32(PININT_BASE + 0x01c)
#define PININT_FALL			MMIO32(PININT_BASE + 0x020)
#define PININT_IST			MMIO32(PININT_BASE + 0x024)
#define PININT_PMCTRL			MMIO32(PININT_BASE + 0x028)
#define PININT_PMSRC			MMIO32(PININT_BASE + 0x02c)
#define PININT_PMCFG			MMIO32(PININT_BASE + 0x030)

/* --- PININT_ISEL values -------------------------------------------------- */

/* PININT_ISEL[7:0]: PMODE: 0 = Edge sensitive, 1 = Level sensitive */

/* --- PININT_IENR values -------------------------------------------------- */

/* PININT_IENR[7:0]: ENRL */
/*
 * Edge sensitive:  0 = Disable rising edge interrupt
 *                  1 = Enable rising edge interrupt
 * Level sensitive: 0 = Disable interrupt
 *                  1 = Enable interrupt
 */

/* --- PININT_SIENR values ------------------------------------------------- */

/* PININT_SIENR[7:0]: SETENRL */

/* --- PININT_CIENR values ------------------------------------------------- */

/* PININT_CIENR[7:0]: CENRL */

/* --- PININT_IENF values -------------------------------------------------- */

/* PININT_IENF[7:0]: ENAF */
/*
 * Edge sensitive:  0 = Disable falling edge interrupt
 *                  1 = Enable falling edge interrupt
 * Level sensitive: 0 = Set active interrupt level LOW
 *                  1 = Set active interrupt level HIGH
 */

/* --- PININT_SIENF values ------------------------------------------------- */

/* PININT_SIENF[7:0]: SETENAF */

/* --- PININT_CIENF values ------------------------------------------------- */

/* PININT_CIENF[7:0]: CENAF */

/* --- PININT_RISE values -------------------------------------------------- */

/* PININT_RISE[7:0]: RDET */

/* --- PININT_FALL values -------------------------------------------------- */

/* PININT_FALL[7:0]: FDET */

/* --- PININT_IST values --------------------------------------------------- */

/* PININT_IST[7:0]: PSTAT */

/* --- PININT_PMCTRL values ------------------------------------------------ */

/* PININT_PMCTRL[31:24]: PMAT */

#define PININT_PMCTRL_ENA_RXEV		(1 << 1)
#define PININT_PMCTRL_SEL_PMATCH	(1 << 0)

/* --- PININT_PMSRC values ------------------------------------------------- */

/* PININT_PMSRC[31:29]: SRC7 */
/* PININT_PMSRC[28:26]: SRC6 */
/* PININT_PMSRC[25:23]: SRC5 */
/* PININT_PMSRC[22:20]: SRC4 */
/* PININT_PMSRC[19:17]: SRC3 */
/* PININT_PMSRC[16:14]: SRC2 */
/* PININT_PMSRC[13:11]: SRC1 */
/* PININT_PMSRC[10:8]: SRC0 */

/* --- PININT_PMCFG values ------------------------------------------------- */

/* PININT_PMCFG[31:29]: CFG7 */
/* PININT_PMCFG[28:26]: CFG6 */
/* PININT_PMCFG[25:23]: CFG5 */
/* PININT_PMCFG[22:20]: CFG4 */
/* PININT_PMCFG[19:17]: CFG3 */
/* PININT_PMCFG[16:14]: CFG2 */
/* PININT_PMCFG[13:11]: CFG1 */
/* PININT_PMCFG[10:8]: CFG0 */

/* PININT_PMCFG[6:0]: PROD_ENDPTS6-0 */

/* --- Function prototypes ------------------------------------------------- */

/* Pin interrupts */
enum {
	PININT0 = (1 << 0),
	PININT1 = (1 << 1),
	PININT2 = (1 << 2),
	PININT3 = (1 << 3),
	PININT4 = (1 << 4),
	PININT5 = (1 << 5),
	PININT6 = (1 << 6),
	PININT7 = (1 << 7)
};

/* Interrupt mode */
typedef enum {
	PININT_DISABLE,
	PININT_RISING,
	PININT_FALLING,
	PININT_BOTH,
	PININT_LOW,
	PININT_HIGH,
	PININT_UNKNOWN
} pinint_mode_t;

/* Match contribution condition */
typedef enum {
	PININT_CONST_HIGH,
	PININT_STICKY_RISING,
	PININT_STICKY_FALLING,
	PININT_STICKY_BOTH,
	PININT_HIGH_LEVEL,
	PININT_LOW_LEVEL,
	PININT_CONST_LOW,
	PININT_EVENT
} pinint_match_t;

void pinint_set_interrupt_mode(pinint_mode_t mode, int pinints);
pinint_mode_t pinint_get_interrupt_mode(int pinints);
int pinint_get_interrupt_status(int pinints);
void pinint_clear_interrupt(int pinints);
int pinint_rising_edge_detected(int pinints);
int pinint_falling_edge_detected(int pinints);
void pinint_clear_rising_edge(int pinints);
void pinint_clear_falling_edge(int pinints);
void pinint_set_bit_slice(int bitslice, int pinint, pinint_match_t match,
			  bool endpoint);
void pinint_enable_pattern_match(void);
void pinint_disable_pattern_match(void);
bool pinint_pattern_match_enabled(void);
void pinint_enable_rxev(void);
void pinint_disable_rxev(void);
int pinint_get_pattern_match_state(int pinints);
