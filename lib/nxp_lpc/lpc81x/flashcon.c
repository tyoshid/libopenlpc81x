/*
 * Flash controller functions
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

#include <flashcon.h>

void flashcon_set_flash_access_time(int clock)
{
	int r;

	r = FLASHCON_FLASHCFG;
	r &= ~3;
	FLASHCON_FLASHCFG = r | ((clock - 1) & 3);
}

int flashcon_generate_signature(int start, int stop)
{
	int r;

	FLASHCON_FMSSTART = (start >> 2) & 0xffff;
	FLASHCON_FMSSTOP = ((stop >> 2) & 0xffff) | FLASHCON_FMSSTOP_STRTBIST;
	r = FLASHCON_FMSW0;
	return r;
}
