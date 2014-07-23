/*
 * Self wake-up timer (WKT)
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
 * Chapter 13: LPC81x Self wake-up timer (WKT)
 */

#include <mmio.h>
#include <memorymap.h>

/* --- WKT registers ------------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	WKT_CTRL		Self wake-up timer control register
 * 0x00c	WKT_COUNT		Counter register
 */

#define WKT_CTRL			MMIO32(WKT_BASE + 0x000)
#define WKT_COUNT			MMIO32(WKT_BASE + 0x00c)

/* --- WKT_CTRL values ----------------------------------------------------- */

#define WKT_CTRL_CLEARCTR		(1 << 2)
#define WKT_CTRL_ALARMFLAG		(1 << 1)
#define WKT_CTRL_CLKSEL			(1 << 0)

/* --- WKT_COUNT values ---------------------------------------------------- */

/* WKT_COUNT[31:0]: VALUE */

/* --- Function prototypes ------------------------------------------------- */

/* Clock */
typedef enum {
	WKT_IRC = 0,
	WKT_LOW_POWER_CLOCK = 1,
	WKT_750KHZ = 0,
	WKT_10KHZ = 1
} wkt_clock_t;

/* Status */
enum {
	WKT_ALARM = (1 << 1)
};

void wkt_set_clock(wkt_clock_t clock);
int wkt_get_status(int status);
void wkt_clear_status(int status);
void wkt_start_counter(int count);
void wkt_stop_counter(void);
