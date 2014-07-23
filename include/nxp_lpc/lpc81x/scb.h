/*
 * System Control Block (SCB)
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

#include <mmio.h>
#include <memorymap.h>

/* --- System control and ID registers ------------------------------------- */
/*
 * Offset	Register
 * 0xe000e008	SCB_ACTLR		Auxiliary Control Register
 *
 * 0x000	SCB_CPUID		CPUID Base Register
 * 0x004	SCB_ICSR		Interrupt Control State Register
 * 0x008	SCB_VTOR		Vector Table Offset Register
 * 0x00c	SCB_AIRCR		Application Interrupt and Reset Control
 *					Register
 * 0x010	SCB_SCR			Optional System Control Register
 * 0x014	SCB_CCR			Configuration and Control Register
 * 0x01c	SCB_SHPR2		System Handler Priority Register 2
 * 0x020	SCB_SHPR3		System Handler Priority Register 3
 * 0x024	SCB_SHCSR		System Handler Control and State
 *					Register
 * 0x030	SCB_DFSR		Debug Fault Status Register
 */

#define SCB_ACTLR			MMIO32(SCS_BASE + 0x008)
#define SCB_CPUID			MMIO32(SCB_BASE + 0x000)
#define SCB_ICSR			MMIO32(SCB_BASE + 0x004)
#define SCB_VTOR			MMIO32(SCB_BASE + 0x008)
#define SCB_AIRCR			MMIO32(SCB_BASE + 0x00c)
#define SCB_SCR				MMIO32(SCB_BASE + 0x010)
#define SCB_CCR				MMIO32(SCB_BASE + 0x014)
#define SCB_SHPR2			MMIO32(SCB_BASE + 0x01c)
#define SCB_SHPR3			MMIO32(SCB_BASE + 0x020)
#define SCB_SHCSR			MMIO32(SCB_BASE + 0x024)
#define SCB_DFSR			MMIO32(SCB_BASE + 0x030)

/* --- SCB_ACTLR values ---------------------------------------------------- */

/* --- SCB_CPUID values ---------------------------------------------------- */

/* SCB_CPUID[21:24]: IMPLEMENTER (0x41  ARM Limited.) */
/* SCB_CPUID[23:20]: VARIANT */
/* SCB_CPUID[19:16]: ARCHITECTURE (0xc  ARMv6-M) */
/* SCB_CPUID[15:4]: PARTNO */
/* SCB_CPUID[3:0]: REVISION */

/* --- SCB_ICSR values ----------------------------------------------------- */

#define SCB_ICSR_NMIPENDSET		(1 << 31)
#define SCB_ICSR_PENDSVSET		(1 << 28)
#define SCB_ICSR_PENDSVCLR		(1 << 27)
#define SCB_ICSR_PENDSTSET		(1 << 26)
#define SCB_ICSR_PENDSTCLR		(1 << 25)
#define SCB_ICSR_ISRPREEMPT		(1 << 23)
#define SCB_ICSR_ISRPENDING		(1 << 22)
/* SCB_ICSR[20:12]: VECTPENDING */
/* SCB_ICSR[8:0]: VECTACTIVE */

/* --- SCB_VTOR values ----------------------------------------------------- */

/* SCB_VTOR[31:7]: TBLOFF */

/* --- SCB_AIRCR values ---------------------------------------------------- */

/* SCB_AIRCR[31:16]: VECTKEY */
#define SCB_AIRCR_VECTKEY		(0x05fa << 16)
#define SCB_AIRCR_ENDIANNESS		(1 << 15)
#define SCB_AIRCR_SYSRESETREQ		(1 << 2)
#define SCB_AIRCR_VECTCLRACTIVE		(1 << 1)

/* --- SCB_SCR values ------------------------------------------------------ */

#define SCB_SCR_SEVONPEND		(1 << 4)
#define SCB_SCR_SLEEPDEEP		(1 << 2)
#define SCB_SCR_SLEEPONEXIT		(1 << 1)

/* --- SCB_CCR values ------------------------------------------------------ */

#define SCB_CCR_STKALIGN		(1 << 9)
#define SCB_CCR_UNALIGN_TRP		(1 << 3)

/* --- SCB_SHPR2 values ---------------------------------------------------- */

/* SCB_SHPR2[31:30]: PRI_11 (SVCall) */

/* --- SCB_SHPR3 values ---------------------------------------------------- */

/* SCB_SHPR3[31:30]: PRI_15 (SysTick) */
/* SCB_SHPR3[23:22]: PRI_14 (PendSV) */

/* --- SCB_SHCSR values ---------------------------------------------------- */

#define SCB_SHCSR_SVCALLPENDED		(1 << 15)

/* --- SCB_DFSR values ----------------------------------------------------- */

#define SCB_DFSR_EXTERNAL		(1 << 4)
#define SCB_DFSR_VCATCH			(1 << 3)
#define SCB_DFSR_DWTTRAP		(1 << 2)
#define SCB_DFSR_BKPT			(1 << 1)
#define SCB_DFSR_HALTED			(1 << 0)

/* --- Function prototypes ------------------------------------------------- */

/* Sleep */
enum {
	SCB_SEVONPEND = (1 << 4),
	SCB_SLEEPDEEP = (1 << 2),
	SCB_SLEEPONEXIT = (1 << 1)
};

void scb_reset_system(void);
void scb_set_sleep(int sleep);
int scb_get_sleep(int sleep);
void scb_set_vector_table_offset(int tbloff);
