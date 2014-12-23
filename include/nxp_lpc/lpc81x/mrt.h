/*
 * Multi-Rate Timer (MRT)
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
 * Chapter 11: LPC81x Multi-Rate Timer (MRT)
 */

#include <mmio.h>
#include <memorymap.h>

/* --- MRT registers ------------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	MRT_INTVAL0		MRT0 Time interval value register
 * 0x004	MRT_TIMER0		MRT0 Timer register
 * 0x008	MRT_CTRL0		MRT0 Control register
 * 0x00c	MRT_STAT0		MRT0 Status register
 * 0x010	MRT_INTVAL1		MRT1 Time interval value register
 * 0x014	MRT_TIMER1		MRT1 Timer register
 * 0x018	MRT_CTRL1		MRT1 Control register
 * 0x01c	MRT_STAT1		MRT1 Status register
 * 0x020	MRT_INTVAL2		MRT2 Time interval value register
 * 0x024	MRT_TIMER2		MRT2 Timer register
 * 0x028	MRT_CTRL2		MRT2 Control register
 * 0x02c	MRT_STAT2		MRT2 Status register
 * 0x030	MRT_INTVAL3		MRT3 Time interval value register
 * 0x034	MRT_TIMER3		MRT3 Timer register
 * 0x038	MRT_CTRL3		MRT3 Control register
 * 0x03c	MRT_STAT3		MRT3 Status register
 * 0x0f4	MRT_IDLE_CH		Idle channel register
 * 0x0f8	MRT_IRQ_FLAG		Global interrupt flag register
 */

#define MRT_INTVAL(ch)			MMIO32(MRT_BASE + 0x000 + (ch) * 0x10)
#define MRT_INTVAL0			MRT_INTVAL(0)
#define MRT_INTVAL1			MRT_INTVAL(1)
#define MRT_INTVAL2			MRT_INTVAL(2)
#define MRT_INTVAL3			MRT_INTVAL(3)

#define MRT_TIMER(ch)			MMIO32(MRT_BASE + 0x004 + (ch) * 0x10)
#define MRT_TIMER0			MRT_TIMER(0)
#define MRT_TIMER1			MRT_TIMER(1)
#define MRT_TIMER2			MRT_TIMER(2)
#define MRT_TIMER3			MRT_TIMER(3)

#define MRT_CTRL(ch)			MMIO32(MRT_BASE + 0x008 + (ch) * 0x10)
#define MRT_CTRL0			MRT_CTRL(0)
#define MRT_CTRL1			MRT_CTRL(1)
#define MRT_CTRL2			MRT_CTRL(2)
#define MRT_CTRL3			MRT_CTRL(3)

#define MRT_STAT(ch)			MMIO32(MRT_BASE + 0x00c + (ch) * 0x10)
#define MRT_STAT0			MRT_STAT(0)
#define MRT_STAT1			MRT_STAT(1)
#define MRT_STAT2			MRT_STAT(2)
#define MRT_STAT3			MRT_STAT(3)

#define MRT_IDLE_CH			MMIO32(MRT_BASE + 0x0f4)
#define MRT_IRQ_FLAG			MMIO32(MRT_BASE + 0x0f8)

/* --- MRT_INTVALx values -------------------------------------------------- */

#define MRT_INTVAL_LOAD			(1 << 31)

/* MRT_INTVALx[30:0]: IVALUE */

/* --- MRT_TIMERx values --------------------------------------------------- */

/* MRT_TIMERx[30:0]: VALUE */

/* --- MRT_CTRLx values ---------------------------------------------------- */

#define MRT_CTRL_MODE1			(1 << 2)
#define MRT_CTRL_MODE0			(1 << 1)
#define MRT_CTRL_INTEN			(1 << 0)

/* MODE */
#define MRT_CTRL_MODE_REPEAT_IRQ	(0 << 1)
#define MRT_CTRL_MODE_ONE_SHOT_IRQ	(1 << 1)
#define MRT_CTRL_MODE_ONE_SHOT_STALL	(2 << 1)

/* --- MRT_STATx values ---------------------------------------------------- */

#define MRT_STAT_RUN			(1 << 1)
#define MRT_STAT_INTFLAG		(1 << 0)

/* --- MRT_IDLE_CH values -------------------------------------------------- */

/* MRT_IDLE_CH[7:4]: CHAN */

/* --- MRT_IRQ_FLAG values ------------------------------------------------- */

#define MRT_IRQ_FLAG_GFLAG3		(1 << 3)
#define MRT_IRQ_FLAG_GFLAG2		(1 << 2)
#define MRT_IRQ_FLAG_GFLAG1		(1 << 1)
#define MRT_IRQ_FLAG_GFLAG0		(1 << 0)

/* --- Function prototypes ------------------------------------------------- */

/* Channel */
enum  mrt_channel {
	MRT0,
	MRT1,
	MRT2,
	MRT3
};

/* Timer mode */
enum  mrt_mode {
	MRT_REPEAT = (0 << 1),
	MRT_ONE_SHOT = (1 << 1),
	MRT_BUS_STALL = (2 << 1)
};

/* Interrupt */
enum {
	MRT_INT0 = (1 << 0),
	MRT_INT1 = (1 << 1),
	MRT_INT2 = (1 << 2),
	MRT_INT3 = (1 << 3)
};

/* Channel status */
enum {
	MRT_INT = (1 << 0),
	MRT_RUN = (1 << 1)
};

void mrt_set_mode(enum mrt_channel mrt, enum mrt_mode mode);
void mrt_set_interval(enum mrt_channel mrt, int ivalue);
int mrt_get_timer(enum mrt_channel mrt);
void mrt_enable_interrupt(int interrupt);
void mrt_disable_interrupt(int interrupt);
int mrt_get_interrupt_mask(int interrupt);
int mrt_get_interrupt_status(int interrupt);
void mrt_clear_interrupt(int interrupt);
int mrt_get_channel_status(enum mrt_channel mrt, int status);
void mrt_clear_channel_status(enum mrt_channel mrt, int status);
int mrt_get_idle_channel(void);
