/*
 * WWDT functions
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

#include <wwdt.h>

void wwdt_start(int mode, int tc, int window)
{
	WWDT_MOD = (mode & (WWDT_MOD_WDRESET | WWDT_MOD_WDPROTECT |
			    WWDT_MOD_LOCK)) | WWDT_MOD_WDEN;
	WWDT_TC = tc & 0xffffff;
	WWDT_WINDOW = window & 0xffffff;
	WWDT_FEED = 0xaa;
	WWDT_FEED = 0x55;
}

void wwdt_set_time_out(int count)
{
	WWDT_TC = count & 0xffffff;
}

void wwdt_set_window(int window)
{
	WWDT_WINDOW = window & 0xffffff;
}

void wwdt_restart(void)
{
	WWDT_FEED = 0xaa;
	WWDT_FEED = 0x55;
}

int wwdt_get_timer_value(void)
{
	return WWDT_TV & 0xffffff;
}

void wwdt_set_warning_interrupt(int warnint)
{
	WWDT_WARNINT = warnint & 0x3ff;
}

int wwdt_get_status(int status)
{
	return WWDT_MOD & status;
}

void wwdt_clear_status(int status)
{
	WWDT_MOD &= ~status & 0x3f;
}
