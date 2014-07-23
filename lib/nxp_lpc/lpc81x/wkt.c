/*
 * WKT functions
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

#include <wkt.h>

void wkt_set_clock(wkt_clock_t clock)
{
	if (clock == WKT_IRC)
		WKT_CTRL &= ~WKT_CTRL_CLKSEL;
	else if (clock == WKT_LOW_POWER_CLOCK)
		WKT_CTRL |= WKT_CTRL_CLKSEL;
}

int wkt_get_status(int status)
{
	return WKT_CTRL & status;
}

void wkt_clear_status(int status)
{
	WKT_CTRL |= status;
}

void wkt_start_counter(int count)
{
	WKT_COUNT = count;
}

void wkt_stop_counter(void)
{
	WKT_CTRL |= WKT_CTRL_CLEARCTR;
}
