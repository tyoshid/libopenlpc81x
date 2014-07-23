/*
 * SysTick
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
 * Chapter 14: LPC81x ARM Cortex SysTick Timer (SysTick)
 */

#include <mmio.h>
#include <memorymap.h>

/* --- SysTick registers --------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	SYST_CSR		System Timer Control and status
 *					register
 * 0x004	SYST_RVR		System Timer Reload value register
 * 0x008	SYST_CVR		System Timer Current value register
 * 0x00c	SYST_CALIB		System Timer Calibration value register
 */

#define SYST_CSR			MMIO32(STK_BASE + 0x000)
#define SYST_RVR			MMIO32(STK_BASE + 0x004)
#define SYST_CVR			MMIO32(STK_BASE + 0x008)
#define SYST_CALIB			MMIO32(STK_BASE + 0x00c)

/* --- SYST_CSR values ----------------------------------------------------- */

#define SYST_CSR_COUNTFLAG		(1 << 16)
#define SYST_CSR_CLKSOURCE		(1 << 2)
#define SYST_CSR_TICKINT		(1 << 1)
#define SYST_CSR_ENABLE			(1 << 0)

/* --- SYST_RVR values ----------------------------------------------------- */

/* SYST_RVR[23:0]: RELOAD */

/* --- SYST_CVR values ----------------------------------------------------- */

/* SYST_CVR[23:0]: CURRENT */

/* --- SYST_CALIB values --------------------------------------------------- */

#define SYST_CALIB_NOREF		(1 << 31)
#define SYST_CALIB_SKEW			(1 << 30)

/* SYST_CALIB[23:0]: TENMS */

/* --- Function prototypes ------------------------------------------------- */

/* Option and status */
enum {
	SYST_INT_ENABLE = (1 << 1),
	SYST_SYSCLK = (1 << 2),
	SYST_SYSCLK_2 = (0 << 2),

	SYST_COUNTFLAG = (1 << 16)
};

void systick_enable_timer(int reload, int option);
void systick_disable_timer(void);
int systick_get_status(int status);
int systick_get_timer(void);
void systick_clear_timer(void);
void systick_set_reload(int reload);
int systick_get_calibration(void);
