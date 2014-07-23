/*
 * SCTimer/PWM (SCT)
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
 * Chapter 10: LPC81x SCTimer/PWM (SCT)
 */

#include <mmio.h>
#include <memorymap.h>

/* --- SCT registers ------------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	SCT_CONFIG		SCT configuration register
 * 0x004	SCT_CTRL		SCT control register
 * 0x004	SCT_CTRL_L		SCT control register low
 * 0x006	SCT_CTRL_H		SCT control register high
 * 0x008	SCT_LIMIT		SCT limit register
 * 0x008	SCT_LIMIT_L		SCT limit register low
 * 0x00a	SCT_LIMIT_H		SCT limit register high
 * 0x00c	SCT_HALT		SCT halt condition register
 * 0x00c	SCT_HALT_L		SCT halt condition register low
 * 0x00e	SCT_HALT_H		SCT halt condition register high
 * 0x010	SCT_STOP		SCT stop condition register
 * 0x010	SCT_STOP_L		SCT stop condition register low
 * 0x012	SCT_STOP_H		SCT stop condition register high
 * 0x014	SCT_START		SCT start condition register
 * 0x014	SCT_START_L		SCT start condition register low
 * 0x016	SCT_START_H		SCT start condition register high
 * 0x040	SCT_COUNT		SCT counter register
 * 0x040	SCT_COUNT_L		SCT counter register low
 * 0x042	SCT_COUNT_H		SCT counter register high
 * 0x044	SCT_STATE		SCT state register
 * 0x044	SCT_STATE_L		SCT state register low
 * 0x046	SCT_STATE_H		SCT state register high
 * 0x048	SCT_INPUT		SCT input register
 * 0x04c	SCT_REGMODE		SCT match/capture registers mode
 *					register
 * 0x04c	SCT_REGMODE_L		SCT match/capture registers mode
 *					register low
 * 0x04e	SCT_REGMODE_H		SCT match/capture registers mode
 *					register high
 * 0x050	SCT_OUTPUT		SCT output register
 * 0x054	SCT_OUTPUTDIRCTRL	SCT output counter direction control
 *					register
 * 0x058	SCT_RES			SCT conflict resolution register
 * 0x0f0	SCT_EVEN		SCT event enable register
 * 0x0f4	SCT_EVFLAG		SCT event flag register
 * 0x0f8	SCT_CONEN		SCT conflict enable register
 * 0x0fc	SCT_CONFLAG		SCT conflict flag register
 * 0x100	SCT_MATCH0		SCT match register 0
 * 0x100	SCT_MATCH0_L		SCT match register 0 low
 * 0x102	SCT_MATCH0_H		SCT match register 0 high
 * 0x104	SCT_MATCH1		SCT match register 1
 * 0x104	SCT_MATCH1_L		SCT match register 1 low 
 * 0x106	SCT_MATCH1_H		SCT match register 1 high
 * 0x108	SCT_MATCH2		SCT match register 2
 * 0x108	SCT_MATCH2_L		SCT match register 2 low
 * 0x10a	SCT_MATCH2_H		SCT match register 2 high
 * 0x10c	SCT_MATCH3		SCT match register 3
 * 0x10c	SCT_MATCH3_L		SCT match register 3 low
 * 0x10e	SCT_MATCH3_H		SCT match register 3 high
 * 0x110	SCT_MATCH4		SCT match register 4
 * 0x110	SCT_MATCH4_L		SCT match register 4 low
 * 0x112	SCT_MATCH4_H		SCT match register 4 high
 * 0x100	SCT_CAP0		SCT capture register 0
 * 0x100	SCT_CAP0_L		SCT capture register 0 low
 * 0x102	SCT_CAP0_H		SCT capture register 0 high
 * 0x104	SCT_CAP1		SCT capture register 1
 * 0x104	SCT_CAP1_L		SCT capture register 1 low
 * 0x106	SCT_CAP1_H		SCT capture register 1 high
 * 0x108	SCT_CAP2		SCT capture register 2
 * 0x108	SCT_CAP2_L		SCT capture register 2 low
 * 0x10a	SCT_CAP2_H		SCT capture register 2 high
 * 0x10c	SCT_CAP3		SCT capture register 3
 * 0x10c	SCT_CAP3_L		SCT capture register 3 low
 * 0x10e	SCT_CAP3_H		SCT capture register 3 high
 * 0x110	SCT_CAP4		SCT capture register 4
 * 0x110	SCT_CAP4_L		SCT capture register 4 low
 * 0x112	SCT_CAP4_H		SCT capture register 4 high
 * 0x200	SCT_MATCHREL0		SCT match reload register 0
 * 0x200	SCT_MATCHREL0_L		SCT match reload register 0 low
 * 0x202	SCT_MATCHREL0_H		SCT match reload register 0 high
 * 0x204	SCT_MATCHREL1		SCT match reload register 1
 * 0x204	SCT_MATCHREL1_L		SCT match reload register 1 low
 * 0x206	SCT_MATCHREL1_H		SCT match reload register 1 high
 * 0x208	SCT_MATCHREL2		SCT match reload register 2
 * 0x208	SCT_MATCHREL2_L		SCT match reload register 2 low
 * 0x20a	SCT_MATCHREL2_H		SCT match reload register 2 high
 * 0x20c	SCT_MATCHREL3		SCT match reload register 3
 * 0x20c	SCT_MATCHREL3_L		SCT match reload register 3 low
 * 0x20e	SCT_MATCHREL3_H		SCT match reload register 3 high
 * 0x210	SCT_MATCHREL4		SCT match reload register 4
 * 0x210	SCT_MATCHREL4_L		SCT match reload register 4 low
 * 0x212	SCT_MATCHREL4_H		SCT match reload register 4 high
 * 0x200	SCT_CAPCTRL0		SCT capture control register 0
 * 0x200	SCT_CAPCTRL0_L		SCT capture control register 0 low
 * 0x202	SCT_CAPCTRL0_H		SCT capture control register 0 high
 * 0x204	SCT_CAPCTRL1		SCT capture control register 1
 * 0x204	SCT_CAPCTRL1_L		SCT capture control register 1 low
 * 0x206	SCT_CAPCTRL1_H		SCT capture control register 1 high
 * 0x208	SCT_CAPCTRL2		SCT capture control register 2
 * 0x208	SCT_CAPCTRL2_L		SCT capture control register 2 low
 * 0x20a	SCT_CAPCTRL2_H		SCT capture control register 2 high
 * 0x20c	SCT_CAPCTRL3		SCT capture control register 3
 * 0x20c	SCT_CAPCTRL3_L		SCT capture control register 3 low
 * 0x20e	SCT_CAPCTRL3_H		SCT capture control register 3 high
 * 0x210	SCT_CAPCTRL4		SCT capture control register 4
 * 0x210	SCT_CAPCTRL4_L		SCT capture control register 4 low
 * 0x212	SCT_CAPCTRL4_H		SCT capture control register 4 high
 * 0x300	SCT_EV0_STATE		SCT event 0 state register
 * 0x304	SCT_EV0_CTRL		SCT event 0 control register
 * 0x308	SCT_EV1_STATE		SCT event 1 state register
 * 0x30c	SCT_EV1_CTRL		SCT event 1 control register
 * 0x310	SCT_EV2_STATE		SCT event 2 state register
 * 0x314	SCT_EV2_CTRL		SCT event 2 control register
 * 0x318	SCT_EV3_STATE		SCT event 3 state register
 * 0x31c	SCT_EV3_CTRL		SCT event 3 control register
 * 0x320	SCT_EV4_STATE		SCT event 4 state register
 * 0x324	SCT_EV4_CTRL		SCT event 4 control register
 * 0x328	SCT_EV5_STATE		SCT event 5 state register
 * 0x32c	SCT_EV5_CTRL		SCT event 5 control register
 * 0x500	SCT_OUT0_SET		SCT output 0 set register
 * 0x504	SCT_OUT0_CLR		SCT output 0 clear register
 * 0x508	SCT_OUT1_SET		SCT output 1 set register
 * 0x50c	SCT_OUT1_CLR		SCT output 1 clear register
 * 0x510	SCT_OUT2_SET		SCT output 2 set register
 * 0x514	SCT_OUT2_CLR		SCT output 2 clear register
 * 0x518	SCT_OUT3_SET		SCT output 3 set register
 * 0x51c	SCT_OUT3_CLR		SCT output 3 clear register
 */

#define SCT_CONFIG			MMIO32(SCT_BASE + 0x000)
#define SCT_CTRL			MMIO32(SCT_BASE + 0x004)
#define SCT_CTRL_L			MMIO16(SCT_BASE + 0x004)
#define SCT_CTRL_H			MMIO16(SCT_BASE + 0x006)
#define SCT_LIMIT			MMIO32(SCT_BASE + 0x008)
#define SCT_LIMIT_L			MMIO16(SCT_BASE + 0x008)
#define SCT_LIMIT_H			MMIO16(SCT_BASE + 0x00a)
#define SCT_HALT			MMIO32(SCT_BASE + 0x00c)
#define SCT_HALT_L			MMIO16(SCT_BASE + 0x00c)
#define SCT_HALT_H			MMIO16(SCT_BASE + 0x00e)
#define SCT_STOP			MMIO32(SCT_BASE + 0x010)
#define SCT_STOP_L			MMIO16(SCT_BASE + 0x010)
#define SCT_STOP_H			MMIO16(SCT_BASE + 0x012)
#define SCT_START			MMIO32(SCT_BASE + 0x014)
#define SCT_START_L			MMIO16(SCT_BASE + 0x014)
#define SCT_START_H			MMIO16(SCT_BASE + 0x016)
#define SCT_COUNT			MMIO32(SCT_BASE + 0x040)
#define SCT_COUNT_L			MMIO16(SCT_BASE + 0x040)
#define SCT_COUNT_H			MMIO16(SCT_BASE + 0x042)
#define SCT_STATE			MMIO32(SCT_BASE + 0x044)
#define SCT_STATE_L			MMIO16(SCT_BASE + 0x044)
#define SCT_STATE_H			MMIO16(SCT_BASE + 0x046)
#define SCT_INPUT			MMIO32(SCT_BASE + 0x048)
#define SCT_REGMODE			MMIO32(SCT_BASE + 0x04c)
#define SCT_REGMODE_L			MMIO16(SCT_BASE + 0x04c)
#define SCT_REGMODE_H			MMIO16(SCT_BASE + 0x04e)
#define SCT_OUTPUT			MMIO32(SCT_BASE + 0x050)
#define SCT_OUTPUTDIRCTRL		MMIO32(SCT_BASE + 0x054)
#define SCT_RES				MMIO32(SCT_BASE + 0x058)
#define SCT_EVEN			MMIO32(SCT_BASE + 0x0f0)
#define SCT_EVFLAG			MMIO32(SCT_BASE + 0x0f4)
#define SCT_CONEN			MMIO32(SCT_BASE + 0x0f8)
#define SCT_CONFLAG			MMIO32(SCT_BASE + 0x0fc)

#define SCT_MATCH(match)		MMIO32(SCT_BASE + 0x100 + (match) * 4)
#define SCT_MATCH0			SCT_MATCH(0)
#define SCT_MATCH1			SCT_MATCH(1)
#define SCT_MATCH2			SCT_MATCH(2)
#define SCT_MATCH3			SCT_MATCH(3)
#define SCT_MATCH4			SCT_MATCH(4)
#define SCT_MATCH_L(match)		MMIO16(SCT_BASE + 0x100 + (match) * 4)
#define SCT_MATCH0_L			SCT_MATCH_L(0)
#define SCT_MATCH1_L			SCT_MATCH_L(1)
#define SCT_MATCH2_L			SCT_MATCH_L(2)
#define SCT_MATCH3_L			SCT_MATCH_L(3)
#define SCT_MATCH4_L			SCT_MATCH_L(4)
#define SCT_MATCH_H(match)		MMIO16(SCT_BASE + 0x102 + (match) * 4)
#define SCT_MATCH0_H			SCT_MATCH_H(0)
#define SCT_MATCH1_H			SCT_MATCH_H(1)
#define SCT_MATCH2_H			SCT_MATCH_H(2)
#define SCT_MATCH3_H			SCT_MATCH_H(3)
#define SCT_MATCH4_H			SCT_MATCH_H(4)

#define SCT_CAP(cap)			MMIO32(SCT_BASE + 0x100 + (cap) * 4)
#define SCT_CAP0			SCT_CAP(0)
#define SCT_CAP1			SCT_CAP(1)
#define SCT_CAP2			SCT_CAP(2)
#define SCT_CAP3			SCT_CAP(3)
#define SCT_CAP4			SCT_CAP(4)
#define SCT_CAP_L(cap)			MMIO16(SCT_BASE + 0x100 + (cap) * 4)
#define SCT_CAP0_L			SCT_CAP_L(0)
#define SCT_CAP1_L			SCT_CAP_L(1)
#define SCT_CAP2_L			SCT_CAP_L(2)
#define SCT_CAP3_L			SCT_CAP_L(3)
#define SCT_CAP4_L			SCT_CAP_L(4)
#define SCT_CAP_H(cap)			MMIO16(SCT_BASE + 0x102 + (cap) * 4)
#define SCT_CAP0_H			SCT_CAP_H(0)
#define SCT_CAP1_H			SCT_CAP_H(1)
#define SCT_CAP2_H			SCT_CAP_H(2)
#define SCT_CAP3_H			SCT_CAP_H(3)
#define SCT_CAP4_H			SCT_CAP_H(4)

#define SCT_MATCHREL(match)		MMIO32(SCT_BASE + 0x200 + (match) * 4)
#define SCT_MATCHREL0			SCT_MATCHREL(0)
#define SCT_MATCHREL1			SCT_MATCHREL(1)
#define SCT_MATCHREL2			SCT_MATCHREL(2)
#define SCT_MATCHREL3			SCT_MATCHREL(3)
#define SCT_MATCHREL4			SCT_MATCHREL(4)
#define SCT_MATCHREL_L(match)		MMIO16(SCT_BASE + 0x200 + (match) * 4)
#define SCT_MATCHREL0_L			SCT_MATCHREL_L(0)
#define SCT_MATCHREL1_L			SCT_MATCHREL_L(1)
#define SCT_MATCHREL2_L			SCT_MATCHREL_L(2)
#define SCT_MATCHREL3_L			SCT_MATCHREL_L(3)
#define SCT_MATCHREL4_L			SCT_MATCHREL_L(4)
#define SCT_MATCHREL_H(match)		MMIO16(SCT_BASE + 0x202 + (match) * 4)
#define SCT_MATCHREL0_H			SCT_MATCHREL_H(0)
#define SCT_MATCHREL1_H			SCT_MATCHREL_H(1)
#define SCT_MATCHREL2_H			SCT_MATCHREL_H(2)
#define SCT_MATCHREL3_H			SCT_MATCHREL_H(3)
#define SCT_MATCHREL4_H			SCT_MATCHREL_H(4)

#define SCT_CAPCTRL(cap)		MMIO32(SCT_BASE + 0x200 + (cap) * 4)
#define SCT_CAPCTRL0			SCT_CAPCTRL(0)
#define SCT_CAPCTRL1			SCT_CAPCTRL(1)
#define SCT_CAPCTRL2			SCT_CAPCTRL(2)
#define SCT_CAPCTRL3			SCT_CAPCTRL(3)
#define SCT_CAPCTRL4			SCT_CAPCTRL(4)
#define SCT_CAPCTRL_L(cap)		MMIO16(SCT_BASE + 0x200 + (cap) * 4)
#define SCT_CAPCTRL0_L			SCT_CAPCTRL_L(0)
#define SCT_CAPCTRL1_L			SCT_CAPCTRL_L(1)
#define SCT_CAPCTRL2_L			SCT_CAPCTRL_L(2)
#define SCT_CAPCTRL3_L			SCT_CAPCTRL_L(3)
#define SCT_CAPCTRL4_L			SCT_CAPCTRL_L(4)
#define SCT_CAPCTRL_H(cap)		MMIO16(SCT_BASE + 0x202 + (cap) * 4)
#define SCT_CAPCTRL0_H			SCT_CAPCTRL_H(0)
#define SCT_CAPCTRL1_H			SCT_CAPCTRL_H(1)
#define SCT_CAPCTRL2_H			SCT_CAPCTRL_H(2)
#define SCT_CAPCTRL3_H			SCT_CAPCTRL_H(3)
#define SCT_CAPCTRL4_H			SCT_CAPCTRL_H(4)

#define SCT_EV_STATE(ev)		MMIO32(SCT_BASE + 0x300 + (ev) * 8)
#define SCT_EV0_STATE			SCT_EV_STATE(0)
#define SCT_EV1_STATE			SCT_EV_STATE(1)
#define SCT_EV2_STATE			SCT_EV_STATE(2)
#define SCT_EV3_STATE			SCT_EV_STATE(3)
#define SCT_EV4_STATE			SCT_EV_STATE(4)
#define SCT_EV5_STATE			SCT_EV_STATE(5)

#define SCT_EV_CTRL(ev)			MMIO32(SCT_BASE + 0x304 + (ev) * 8)
#define SCT_EV0_CTRL			SCT_EV_CTRL(0)
#define SCT_EV1_CTRL			SCT_EV_CTRL(1)
#define SCT_EV2_CTRL			SCT_EV_CTRL(2)
#define SCT_EV3_CTRL			SCT_EV_CTRL(3)
#define SCT_EV4_CTRL			SCT_EV_CTRL(4)
#define SCT_EV5_CTRL			SCT_EV_CTRL(5)

#define SCT_OUT_SET(out)		MMIO32(SCT_BASE + 0x500 + (out) * 8)
#define SCT_OUT0_SET			SCT_OUT_SET(0)
#define SCT_OUT1_SET			SCT_OUT_SET(1)
#define SCT_OUT2_SET			SCT_OUT_SET(2)
#define SCT_OUT3_SET			SCT_OUT_SET(3)

#define SCT_OUT_CLR(out)		MMIO32(SCT_BASE + 0x504 + (out) * 8)
#define SCT_OUT0_CLR			SCT_OUT_CLR(0)
#define SCT_OUT1_CLR			SCT_OUT_CLR(1)
#define SCT_OUT2_CLR			SCT_OUT_CLR(2)
#define SCT_OUT3_CLR			SCT_OUT_CLR(3)

/* --- SCT_CONFIG values --------------------------------------------------- */

#define SCT_CONFIG_AUTOLIMIT_H		(1 << 18)
#define SCT_CONFIG_AUTOLIMIT_L		(1 << 17)
#define SCT_CONFIG_INSYNC7		(1 << 16)
#define SCT_CONFIG_INSYNC6		(1 << 15)
#define SCT_CONFIG_INSYNC5		(1 << 14)
#define SCT_CONFIG_INSYNC4		(1 << 13)
#define SCT_CONFIG_INSYNC3		(1 << 12)
#define SCT_CONFIG_INSYNC2		(1 << 11)
#define SCT_CONFIG_INSYNC1		(1 << 10)
#define SCT_CONFIG_INSYNC0		(1 << 9)
#define SCT_CONFIG_NORELOAD_H		(1 << 8)
#define SCT_CONFIG_NORELOAD_L		(1 << 7)
#define SCT_CONFIG_CKSEL3		(1 << 6)
#define SCT_CONFIG_CKSEL2		(1 << 5)
#define SCT_CONFIG_CKSEL1		(1 << 4)
#define SCT_CONFIG_CKSEL0		(1 << 3)
#define SCT_CONFIG_CLKMODE1		(1 << 2)
#define SCT_CONFIG_CLKMODE0		(1 << 1)
#define SCT_CONFIG_UNIFY		(1 << 0)

/* CKSEL */
#define SCT_CONFIG_CKSEL_INPUT0_RISING	(0 << 3)
#define SCT_CONFIG_CKSEL_INPUT0_FALLING	(1 << 3)
#define SCT_CONFIG_CKSEL_INPUT1_RISING	(2 << 3)
#define SCT_CONFIG_CKSEL_INPUT1_FALLING	(3 << 3)
#define SCT_CONFIG_CKSEL_INPUT2_RISING	(4 << 3)
#define SCT_CONFIG_CKSEL_INPUT2_FALLING	(5 << 3)
#define SCT_CONFIG_CKSEL_INPUT3_RISING	(6 << 3)
#define SCT_CONFIG_CKSEL_INPUT3_FALLING	(7 << 3)

/* CLKMODE */
#define SCT_CONFIG_CLKMODE_BUS_CLOCK	(0 << 1)
#define SCT_CONFIG_CLKMODE_PRESCALE	(1 << 1)
#define SCT_CONFIG_CLKMODE_INPUT	(2 << 1)

/* --- SCT_CTRL values ----------------------------------------------------- */

/* SCT_CTRL[28:21]: PRE_H */

#define SCT_CTRL_BIDIR_H		(1 << 20)
#define SCT_CTRL_CLRCTR_H		(1 << 19)
#define SCT_CTRL_HALT_H			(1 << 18)
#define SCT_CTRL_STOP_H			(1 << 17)
#define SCT_CTRL_DOWN_H			(1 << 16)

/* SCT_CTRL[12:5]: PRE_L */

#define SCT_CTRL_BIDIR_L		(1 << 4)
#define SCT_CTRL_CLRCTR_L		(1 << 3)
#define SCT_CTRL_HALT_L			(1 << 2)
#define SCT_CTRL_STOP_L			(1 << 1)
#define SCT_CTRL_DOWN_L			(1 << 0)

/* --- SCT_LIMIT values ---------------------------------------------------- */

/* SCT_LIMIT[21:16]: LIMMSK_H */

/* SCT_LIMIT[5:0]: LIMMSK_L */

/* --- SCT_HALT values ----------------------------------------------------- */

/* SCT_HALT[21:16]: HALTMSK_H */

/* SCT_HALT[5:0]: HALTMSK_L */

/* --- SCT_STOP values ----------------------------------------------------- */

/* SCT_STOP[21:16]: STOPMSK_H */

/* SCT_STOP[5:0]: STOPMSK_L */

/* --- SCT_START values ---------------------------------------------------- */

/* SCT_START[21:16]: STARTMSK_H */

/* SCT_START[5:0]: STARTMSK_L */

/* --- SCT_COUNT values ---------------------------------------------------- */

/* SCT_COUNT[31:16]: CTR_H */
/* SCT_COUNT[15:0]: CTR_L */

/* --- SCT_STATE values ---------------------------------------------------- */

/* SCT_STATE[20:16]: STATE_H */

/* SCT_STATE[4:0]: STATE_L */

/* --- SCT_INPUT values ---------------------------------------------------- */

#define SCT_INPUT_SIN3			(1 << 19)
#define SCT_INPUT_SIN2			(1 << 18)
#define SCT_INPUT_SIN1			(1 << 17)
#define SCT_INPUT_SIN0			(1 << 16)
#define SCT_INPUT_AIN3			(1 << 3)
#define SCT_INPUT_AIN2			(1 << 2)
#define SCT_INPUT_AIN1			(1 << 1)
#define SCT_INPUT_AIN0			(1 << 0)

/* --- SCT_REGMODE values -------------------------------------------------- */

#define SCT_REGMODE_REGMOD_H4		(1 << 20)
#define SCT_REGMODE_REGMOD_H3		(1 << 19)
#define SCT_REGMODE_REGMOD_H2		(1 << 18)
#define SCT_REGMODE_REGMOD_H1		(1 << 17)
#define SCT_REGMODE_REGMOD_H0		(1 << 16)
#define SCT_REGMODE_REGMOD_L4		(1 << 4)
#define SCT_REGMODE_REGMOD_L3		(1 << 3)
#define SCT_REGMODE_REGMOD_L2		(1 << 2)
#define SCT_REGMODE_REGMOD_L1		(1 << 1)
#define SCT_REGMODE_REGMOD_L0		(1 << 0)

/* --- SCT_OUTPUT values --------------------------------------------------- */

#define SCT_OUTPUT_OUT3			(1 << 3)
#define SCT_OUTPUT_OUT2			(1 << 2)
#define SCT_OUTPUT_OUT1			(1 << 1)
#define SCT_OUTPUT_OUT0			(1 << 0)

/* --- SCT_OUTPUTDIRCTRL values -------------------------------------------- */

#define SCT_OUTPUTDIRCTRL_SETCLR31	(1 << 7)
#define SCT_OUTPUTDIRCTRL_SETCLR30	(1 << 6)
#define SCT_OUTPUTDIRCTRL_SETCLR21	(1 << 5)
#define SCT_OUTPUTDIRCTRL_SETCLR20	(1 << 4)
#define SCT_OUTPUTDIRCTRL_SETCLR11	(1 << 3)
#define SCT_OUTPUTDIRCTRL_SETCLR10	(1 << 2)
#define SCT_OUTPUTDIRCTRL_SETCLR01	(1 << 1)
#define SCT_OUTPUTDIRCTRL_SETCLR00	(1 << 0)

/* --- SCT_RES values ------------------------------------------------------ */

#define SCT_RES_O3RES1			(1 << 7)
#define SCT_RES_O3RES0			(1 << 6)
#define SCT_RES_O2RES1			(1 << 5)
#define SCT_RES_O2RES0			(1 << 4)
#define SCT_RES_O1RES1			(1 << 3)
#define SCT_RES_O1RES0			(1 << 2)
#define SCT_RES_O0RES1			(1 << 1)
#define SCT_RES_O0RES0			(1 << 0)

/* --- SCT_EVEN values ----------------------------------------------------- */

#define SCT_EVEN_IEN5			(1 << 5)
#define SCT_EVEN_IEN4			(1 << 4)
#define SCT_EVEN_IEN3			(1 << 3)
#define SCT_EVEN_IEN2			(1 << 2)
#define SCT_EVEN_IEN1			(1 << 1)
#define SCT_EVEN_IEN0			(1 << 0)

/* --- SCT_EVFLAG values --------------------------------------------------- */

#define SCT_EVFLAG_FLAG5		(1 << 5)
#define SCT_EVFLAG_FLAG4		(1 << 4)
#define SCT_EVFLAG_FLAG3		(1 << 3)
#define SCT_EVFLAG_FLAG2		(1 << 2)
#define SCT_EVFLAG_FLAG1		(1 << 1)
#define SCT_EVFLAG_FLAG0		(1 << 0)

/* --- SCT_CONEN values ---------------------------------------------------- */

#define SCT_CONEN_NCEN3			(1 << 3)
#define SCT_CONEN_NCEN2			(1 << 2)
#define SCT_CONEN_NCEN1			(1 << 1)
#define SCT_CONEN_NCEN0			(1 << 0)

/* --- SCT_CONFLAG values -------------------------------------------------- */

#define SCT_CONFLAG_BUSERRH		(1 << 31)
#define SCT_CONFLAG_BUSERRL		(1 << 30)
#define SCT_CONFLAG_NCFLAG3		(1 << 3)
#define SCT_CONFLAG_NCFLAG2		(1 << 2)
#define SCT_CONFLAG_NCFLAG1		(1 << 1)
#define SCT_CONFLAG_NCFLAG0		(1 << 0)

/* --- SCT_MATCH[0:4] values ----------------------------------------------- */

/* SCT_MATCHn[31:16]: VALMATCH_H */
/* SCT_MATCHn[15:0]: VALMATCH_L */

/* --- SCT_CAP[0:4] values ------------------------------------------------- */

/* SCT_CAPn[31:16]: VALCAP_H */
/* SCT_CAPn[15:0]: VALCAP_L */

/* --- SCT_MATCHREL[0:4] values -------------------------------------------- */

/* SCT_MATCHRELn[31:16]: RELOAD_H */
/* SCT_MATCHRELn[15:0]: RELOAD_L */

/* --- SCT_CAPCTRL[0:4] values --------------------------------------------- */

#define SCT_CAPCTRL_CAPCON5_H		(1 << 21)
#define SCT_CAPCTRL_CAPCON4_H		(1 << 20)
#define SCT_CAPCTRL_CAPCON3_H		(1 << 19)
#define SCT_CAPCTRL_CAPCON2_H		(1 << 18)
#define SCT_CAPCTRL_CAPCON1_H		(1 << 17)
#define SCT_CAPCTRL_CAPCON0_H		(1 << 16)
#define SCT_CAPCTRL_CAPCON5_L		(1 << 5)
#define SCT_CAPCTRL_CAPCON4_L		(1 << 4)
#define SCT_CAPCTRL_CAPCON3_L		(1 << 3)
#define SCT_CAPCTRL_CAPCON2_L		(1 << 2)
#define SCT_CAPCTRL_CAPCON1_L		(1 << 1)
#define SCT_CAPCTRL_CAPCON0_L		(1 << 0)

/* --- SCT_EV[0:5]_STATE values -------------------------------------------- */

#define SCT_EV_STATE_STATEMASK1		(1 << 1)
#define SCT_EV_STATE_STATEMASK0		(1 << 0)

/* --- SCT_EV[0:5]_CTRL values --------------------------------------------- */

#define SCT_EV_CTRL_DIRECTION1		(1 << 22)
#define SCT_EV_CTRL_DIRECTION0		(1 << 21)
#define SCT_EV_CTRL_MATCHMEM		(1 << 20)
#define SCT_EV_CTRL_STATEV4		(1 << 19)
#define SCT_EV_CTRL_STATEV3		(1 << 18)
#define SCT_EV_CTRL_STATEV2		(1 << 17)
#define SCT_EV_CTRL_STATEV1		(1 << 16)
#define SCT_EV_CTRL_STATEV0		(1 << 15)
#define SCT_EV_CTRL_STATELD		(1 << 14)
#define SCT_EV_CTRL_COMBMODE1		(1 << 13)
#define SCT_EV_CTRL_COMBMODE0		(1 << 12)
#define SCT_EV_CTRL_IOCOND1		(1 << 11)
#define SCT_EV_CTRL_IOCOND0		(1 << 10)
#define SCT_EV_CTRL_IOSEL3		(1 << 9)
#define SCT_EV_CTRL_IOSEL2		(1 << 8)
#define SCT_EV_CTRL_IOSEL1		(1 << 7)
#define SCT_EV_CTRL_IOSEL0		(1 << 6)
#define SCT_EV_CTRL_OUTSEL		(1 << 5)
#define SCT_EV_CTRL_HEVENT		(1 << 4)
#define SCT_EV_CTRL_MATCHSEL3		(1 << 3)
#define SCT_EV_CTRL_MATCHSEL2		(1 << 2)
#define SCT_EV_CTRL_MATCHSEL1		(1 << 1)
#define SCT_EV_CTRL_MATCHSEL0		(1 << 0)

/* COMBMODE */
#define SCT_EV_CTRL_COMBMODE_OR		(0 << 12)
#define SCT_EV_CTRL_COMBMODE_MATCH	(1 << 12)
#define SCT_EV_CTRL_COMBMODE_IO		(2 << 12)
#define SCT_EV_CTRL_COMBMODE_AND	(3 << 12)

/* IOCOND */
#define SCT_EV_CTRL_IOCOND_LOW		(0 << 10)
#define SCT_EV_CTRL_IOCOND_RISE		(1 << 10)
#define SCT_EV_CTRL_IOCOND_FALL		(2 << 10)
#define SCT_EV_CTRL_IOCOND_HIGH		(3 << 10)

/* --- SCT_OUT[0:3]_SET values --------------------------------------------- */

#define SCT_OUT_SET_SET5		(1 << 5)
#define SCT_OUT_SET_SET4		(1 << 4)
#define SCT_OUT_SET_SET3		(1 << 3)
#define SCT_OUT_SET_SET2		(1 << 2)
#define SCT_OUT_SET_SET1		(1 << 1)
#define SCT_OUT_SET_SET0		(1 << 0)

/* --- SCT_OUT[0:3]_CLR values --------------------------------------------- */

#define SCT_OUT_CLR_CLR5		(1 << 5)
#define SCT_OUT_CLR_CLR4		(1 << 4)
#define SCT_OUT_CLR_CLR3		(1 << 3)
#define SCT_OUT_CLR_CLR2		(1 << 2)
#define SCT_OUT_CLR_CLR1		(1 << 1)
#define SCT_OUT_CLR_CLR0		(1 << 0)

/* --- Function prototypes ------------------------------------------------- */

/* Clock mode */
typedef enum {
	SCT_CLOCK_BUS,
	SCT_CLOCK_PRESCALED,
	SCT_CLOCK_INPUT
} sct_clock_t;

/* Input clock selection */
typedef enum {
	SCT_INPUT0_RISING,
	SCT_INPUT0_FALLING,
	SCT_INPUT1_RISING,
	SCT_INPUT1_FALLING,
	SCT_INPUT2_RISING,
	SCT_INPUT2_FALLING,
	SCT_INPUT3_RISING,
	SCT_INPUT3_FALLING
} sct_input_t;

/* Configuration */
enum {
	SCT_UNIFY = (1 << 0),
	SCT_NORELOAD = (1 << 7),
	SCT_NORELOAD_L = (1 << 7),
	SCT_NORELOAD_H = (1 << 8),
	SCT_INSYNC0 = (1 << 9),
	SCT_INSYNC1 = (1 << 10),
	SCT_INSYNC2 = (1 << 11),
	SCT_INSYNC3 = (1 << 12),
	SCT_AUTOLIMIT = (1 << 17),
	SCT_AUTOLIMIT_L = (1 << 17),
	SCT_AUTOLIMIT_H = (1 << 18)
};

/* Counter mode */
typedef enum {
	SCT_COUNT_UP,
	SCT_COUNT_BIDIR_UP,
	SCT_COUNT_BIDIR_DOWN
} sct_counter_t;

/* Event (6) */
enum {
	SCT_EVENT0 = (1 << 0),
	SCT_EVENT1 = (1 << 1),
	SCT_EVENT2 = (1 << 2),
	SCT_EVENT3 = (1 << 3),
	SCT_EVENT4 = (1 << 4),
	SCT_EVENT5 = (1 << 5)
};

/* Input (4) */
enum {
	SCT_INPUT0 = (1 << 0),
	SCT_INPUT1 = (1 << 1),
	SCT_INPUT2 = (1 << 2),
	SCT_INPUT3 = (1 << 3)
};

/* Match/capture register (5) */
enum {
	SCT_REGISTER0 = (1 << 0),
	SCT_REGISTER1 = (1 << 1),
	SCT_REGISTER2 = (1 << 2),
	SCT_REGISTER3 = (1 << 3),
	SCT_REGISTER4 = (1 << 4)
};

/* Output (4) */
enum {
	SCT_OUTPUT0 = (1 << 0),
	SCT_OUTPUT1 = (1 << 1),
	SCT_OUTPUT2 = (1 << 2),
	SCT_OUTPUT3 = (1 << 3)
};

/* Conflict resolution */
typedef enum {
	SCT_OUTPUT_NO,
	SCT_OUTPUT_SET,
	SCT_OUTPUT_CLEAR,
	SCT_OUTPUT_TOGGLE
} sct_res_t;

/* Interrupt */
enum {
	/* Event */

	SCT_CONFLICT_OUT0 = (1 << 8),
	SCT_CONFLICT_OUT1 = (1 << 9),
	SCT_CONFLICT_OUT2 = (1 << 10),
	SCT_CONFLICT_OUT3 = (1 << 11)
};

/* Event control */
enum {
	/* Operation */
	SCT_MATCH_OR_IO = (0 << 12),
	SCT_MATCH_ONLY = (1 << 12),
	SCT_IO_ONLY = (2 << 12),
	SCT_MATCH_AND_IO = (3 << 12),
	/* Match / State */
	SCT_REG_L = (0 << 4),
	SCT_REG_H = (1 << 4),
	SCT_MATCH_MEM = (1 << 20),
	SCT_STATE0 = (1 << 0),
	SCT_STATE1 = (1 << 1),
	/* I/O */
	SCT_IO_IN = (0 << 5),
	SCT_IO_OUT = (1 << 5),
	SCT_LOW = (0 << 10),
	SCT_RISE = (1 << 10),
	SCT_FALL = (2 << 10),
	SCT_HIGH = (3 << 10),
	/* Direction */
	SCT_UP = (1 << 21),
	SCT_DOWN = (1 << 21),

	/* Action */
	SCT_EV_LIMIT = (1 << 0),
	SCT_EV_LIMIT_H = (1 << 1),
	SCT_EV_HALT = (1 << 2),
	SCT_EV_HALT_H = (1 << 3),
	SCT_EV_STOP = (1 << 4),
	SCT_EV_STOP_H = (1 << 5),
	SCT_EV_START = (1 << 6),
	SCT_EV_START_H = (1 << 7),
	SCT_EV_INT = (1 << 8),
	SCT_EV_OUT0_SET = (1 << 9),
	SCT_EV_OUT0_CLR = (1 << 10),
	SCT_EV_OUT1_SET = (1 << 11),
	SCT_EV_OUT1_CLR = (1 << 12),
	SCT_EV_OUT2_SET = (1 << 13),
	SCT_EV_OUT2_CLR = (1 << 14),
	SCT_EV_OUT3_SET = (1 << 15),
	SCT_EV_OUT3_CLR = (1 << 16),
	SCT_EV_CAP0 = (1 << 17),
	SCT_EV_CAP0_H = (1 << 18),
	SCT_EV_CAP1 = (1 << 19),
	SCT_EV_CAP1_H = (1 << 20),
	SCT_EV_CAP2 = (1 << 21),
	SCT_EV_CAP2_H = (1 << 22),
	SCT_EV_CAP3 = (1 << 22),
	SCT_EV_CAP3_H = (1 << 23),
	SCT_EV_CAP4 = (1 << 24),
	SCT_EV_CAP4_H = (1 << 25),
	SCT_EV_STATE_ADD = (0 << 26),
	SCT_EV_STATE_LOAD = (1 << 26)
};

void sct_config(sct_clock_t clkmode, sct_input_t cksel, int config);
void sct_clear_counter(void);
void sct_clear_counter_l(void);
void sct_clear_counter_h(void);
void sct_set_prescaler(int prescaler);
void sct_set_prescaler_l(int prescaler);
void sct_set_prescaler_h(int prescaler);
void sct_start_counter(void);
void sct_start_counter_l(void);
void sct_start_counter_h(void);
void sct_stop_counter(void);
void sct_stop_counter_l(void);
void sct_stop_counter_h(void);
void sct_halt_counter(void);
void sct_halt_counter_l(void);
void sct_halt_counter_h(void);
void sct_set_counter_mode(sct_counter_t mode);
void sct_set_counter_mode_l(sct_counter_t mode);
void sct_set_counter_mode_h(sct_counter_t mode);
void sct_set_limit(int event);
void sct_set_limit_l(int event);
void sct_set_limit_h(int event);
void sct_set_halt(int event);
void sct_set_halt_l(int event);
void sct_set_halt_h(int event);
void sct_set_stop(int event);
void sct_set_stop_l(int event);
void sct_set_stop_h(int event);
void sct_set_start(int event);
void sct_set_start_l(int event);
void sct_set_start_h(int event);
void sct_set_counter(int value);
void sct_set_counter_l(int value);
void sct_set_counter_h(int value);
int sct_get_counter(void);
int sct_get_counter_l(void);
int sct_get_counter_h(void);
void sct_set_state(int state);
void sct_set_state_l(int state);
void sct_set_state_h(int state);
int sct_get_state(void);
int sct_get_state_l(void);
int sct_get_state_h(void);
int sct_get_input(void);
void sct_enable_capture(int registers);
void sct_enable_capture_l(int registers);
void sct_enable_capture_h(int registers);
void sct_disable_capture(int registers);
void sct_disable_capture_l(int registers);
void sct_disable_capture_h(int registers);
void sct_set_output(int output);
int sct_get_output(void);
void sct_enable_output_counter_control(int output);
void sct_enable_output_counter_h_control(int output);
void sct_disable_output_counter_control(int output);
void sct_set_conflict_resolution(sct_res_t res, int output);
void sct_enable_interrupt(int interrupt);
void sct_disable_interrupt(int interrupt);
int sct_get_interrupt_mask(int interrupt);
int sct_get_interrupt_status(int interrupt);
void sct_clear_interrupt(int interrupt);
void sct_set_match(int match, int value);
void sct_set_match_l(int match, int value);
void sct_set_match_h(int match, int value);
void sct_set_match_reload(int match, int value);
void sct_set_match_reload_l(int match, int value);
void sct_set_match_reload_h(int match, int value);
void sct_set_match_and_reload(int match, int value);
void sct_set_match_and_reload_l(int match, int value);
void sct_set_match_and_reload_h(int match, int value);
int sct_get_capture(int cap);
int sct_get_capture_l(int cap);
int sct_get_capture_h(int cap);
void sct_set_capture(int cap, int event);
void sct_set_capture_h(int cap, int event);
void sct_setup_event(int ev, int match, int io, int op, int action, int state);
void sct_set_output_set(int out, int event);
void sct_set_output_clear(int out, int event);
