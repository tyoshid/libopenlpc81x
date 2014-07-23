/*
 * Analog comparator
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
 * Chapter 18: LPC81x Analog comparator
 */

#include <mmio.h>
#include <memorymap.h>

/* --- ACMP registers ------------------------------------------------------ */
/*
 * Offset	Register
 * 0x000	ACMP_CTRL		Comparator control register
 * 0x004	ACMP_LAD		Voltage ladder register
 */

#define ACMP_CTRL			MMIO32(ACMP_BASE + 0x000)
#define ACMP_LAD			MMIO32(ACMP_BASE + 0x004)

/* --- ACMP_CTRL values ---------------------------------------------------- */

#define ACMP_CTRL_HYS1			(1 << 26)
#define ACMP_CTRL_HYS0			(1 << 25)
#define ACMP_CTRL_COMPEDGE		(1 << 23)
#define ACMP_CTRL_COMPSTAT		(1 << 21)
#define ACMP_CTRL_EDGECLR		(1 << 20)
#define ACMP_CTRL_COMP_VM_SEL2		(1 << 13)
#define ACMP_CTRL_COMP_VM_SEL1		(1 << 12)
#define ACMP_CTRL_COMP_VM_SEL0		(1 << 11)
#define ACMP_CTRL_COMP_VP_SEL2		(1 << 10)
#define ACMP_CTRL_COMP_VP_SEL1		(1 << 9)
#define ACMP_CTRL_COMP_VP_SEL0		(1 << 8)
#define ACMP_CTRL_COMPSA		(1 << 6)
#define ACMP_CTRL_EDGESEL1		(1 << 4)
#define ACMP_CTRL_EDGESEL0		(1 << 3)

/* HYS */
#define ACMP_CTRL_HYS_NONE		(0 << 25)
#define ACMP_CTRL_HYS_5MV		(1 << 25)
#define ACMP_CTRL_HYS_10MV		(2 << 25)
#define ACMP_CTRL_HYS_20MV		(3 << 25)

/* COMP_VM_SEL */
#define ACMP_CTRL_COMP_VM_SEL_LADDER	(0 << 11)
#define ACMP_CTRL_COMP_VM_SEL_ACMP_I1	(1 << 11)
#define ACMP_CTRL_COMP_VM_SEL_ACMP_I2	(2 << 11)
#define ACMP_CTRL_COMP_VM_SEL_BANDGAP	(6 << 11)

/* COMP_VP_SEL */
#define ACMP_CTRL_COMP_VP_SEL_LADDER	(0 << 8)
#define ACMP_CTRL_COMP_VP_SEL_ACMP_I1	(1 << 8)
#define ACMP_CTRL_COMP_VP_SEL_ACMP_I2	(2 << 8)
#define ACMP_CTRL_COMP_VP_SEL_BANDGAP	(6 << 8)

/* EDGESEL */
#define ACMP_CTRL_EDGESEL_FALL		(0 << 3)
#define ACMP_CTRL_EDGESEL_RISE		(1 << 3)
#define ACMP_CTRL_EDGESEL_BOTH		(2 << 3)

/* --- ACMP_LAD values ----------------------------------------------------- */

#define ACMP_LAD_LADREF			(1 << 6)
/* ACMP_LAD[5:1]: LADSEL */
#define ACMP_LAD_LADEN			(1 << 0)

/* --- Comparator voltage ladder dynamic characteristics  ------------------ */

/* power-up settling time */
#define ACMP_TS_PU				30 /* 30 usec */

/* switching settling time */
#define ACMP_TS_SW				15 /* 15 usec */

/* --- Function prototypes ------------------------------------------------- */

/* Input */
typedef enum {
	ACMP_LADDER = 0,
	ACMP_I1 = 1,
	ACMP_I2 = 2,
	ACMP_BANDGAP = 6
} acmp_input_t;

/* Hysteresis */
typedef enum {
	ACMP_NONE,
	ACMP_5MV,
	ACMP_10MV,
	ACMP_20MV
} acmp_hys_t;

/* Edge */
typedef enum {
	ACMP_FALL,
	ACMP_RISE,
	ACMP_BOTH
} acmp_edge_t;

/* Status */
enum {
	ACMP_COMPSTAT = (1 << 21),
	ACMP_COMPEDGE = (1 << 23)
};

void acmp_init(acmp_input_t p, acmp_input_t n, acmp_hys_t hys, bool sync,
	       acmp_edge_t edge);
void acmp_enable_ladder(bool vddcmp);
void acmp_disable_ladder(void);
void acmp_set_ladder(int ladder);
int acmp_get_status(int status);
void acmp_clear_interrupt(void);
