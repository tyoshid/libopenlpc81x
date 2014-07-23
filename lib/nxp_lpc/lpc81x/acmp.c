/*
 * Analog comparator functions
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

#include <acmp.h>

void acmp_init(acmp_input_t p, acmp_input_t n, acmp_hys_t hys, bool sync,
	       acmp_edge_t edge)
{
	ACMP_CTRL = (hys << 25) | (n << 11) | (p << 8) |
		(sync ? ACMP_CTRL_COMPSA : 0) | (edge << 3);
}

void acmp_enable_ladder(bool vddcmp)
{
	ACMP_LAD = (vddcmp ? ACMP_LAD_LADREF : 0) | ACMP_LAD_LADEN;
}

void acmp_disable_ladder(void)
{
	ACMP_LAD = 0;
}

void acmp_set_ladder(int ladder)
{
	int r;

	r = ACMP_LAD;
	r &= ~(0x1f << 1);
	ACMP_LAD = r | ((ladder & 0x1f) << 1);
}

int acmp_get_status(int status)
{
	return ACMP_CTRL & status;
}

void acmp_clear_interrupt(void)
{
	int r;

	r = ACMP_CTRL;
	ACMP_CTRL = r | ACMP_CTRL_EDGECLR;
	ACMP_CTRL = r & ~ACMP_CTRL_EDGECLR;
}
