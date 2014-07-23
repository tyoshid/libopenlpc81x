/*
 * SCB functions
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

#include <scb.h>

void scb_reset_system(void)
{
	SCB_AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
}

void scb_set_sleep(int sleep)
{
	SCB_SCR = sleep & 0x16;
}

int scb_get_sleep(int sleep)
{
	return SCB_SCR & sleep;
}

void scb_set_vector_table_offset(int tbloff)
{
	SCB_VTOR = tbloff & 0xffffff80;
}
