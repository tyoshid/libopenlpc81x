/*
 * Windowed Watchdog Timer (WWDT)
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
 * Chapter 12: LPC81x Windowed Watchdog Timer (WWDT)
 */

#include <mmio.h>
#include <memorymap.h>

/* --- WWDT registers ------------------------------------------------------ */
/*
 * Offset	Register
 * 0x000	WWDT_MOD		Watchdog mode register
 * 0x004	WWDT_TC			Watchdog timer constat register
 * 0x008	WWDT_FEED		Watchdog feed sequence register
 * 0x00c	WWDT_TV			Watchdog timer value register
 * 0x014	WWDT_WARNINT		Watchdog Warning Interrupt register
 * 0x018	WWDT_WINDOW		Watchdog Window register
 */

#define WWDT_MOD			MMIO32(WWDT_BASE + 0x000)
#define WWDT_TC				MMIO32(WWDT_BASE + 0x004)
#define WWDT_FEED			MMIO32(WWDT_BASE + 0x008)
#define WWDT_TV				MMIO32(WWDT_BASE + 0x00c)
#define WWDT_WARNINT			MMIO32(WWDT_BASE + 0x014)
#define WWDT_WINDOW			MMIO32(WWDT_BASE + 0x018)

/* --- WWDT_MOD values ----------------------------------------------------- */

#define WWDT_MOD_LOCK			(1 << 5)
#define WWDT_MOD_WDPROTECT		(1 << 4)
#define WWDT_MOD_WDINT			(1 << 3)
#define WWDT_MOD_WDTOF			(1 << 2)
#define WWDT_MOD_WDRESET		(1 << 1)
#define WWDT_MOD_WDEN			(1 << 0)

/* --- WWDT_TC values ------------------------------------------------------ */

/* WWDT_TC[23:0]: COUNT */

/* --- WWDT_FEED values --------------------------------------------------- */

/* WWDT_FEED[7:0]: FEED (0xaa, 0x55) */

/* --- WWDT_TV values ------------------------------------------------------ */

/* WWDT_TV[23:0]: COUNT */

/* --- WWDT_WARNINT values ------------------------------------------------- */

/* WWDT_WARNINT[9:0]: WARNINT */

/* --- WWDT_WINDOW values -------------------------------------------------- */

/* WWDT_WINDOW[23:0]: WINDOW */

/* --- Function prototypes ------------------------------------------------- */

/* Mode */
enum {
	WWDT_RESET = (1 << 1),
	WWDT_PROTECT = (1 << 4),
	WWDT_LOCK = (1 << 5)
};

/* Status */
enum {
	WWDT_TIME_OUT = (1 << 2),
	WWDT_WARNING = (1 << 3)
};

void wwdt_start(int mode, int tc, int window);
void wwdt_set_time_out(int count);
void wwdt_set_window(int window);
void wwdt_restart(void);
int wwdt_get_timer_value(void);
void wwdt_set_warning_interrupt(int warnint);
int wwdt_get_status(int status);
void wwdt_clear_status(int status);
