/*
 * SysTick functions
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

#include <systick.h>

void systick_enable_timer(int reload, int option)
{
	SYST_RVR = reload & 0x00ffffff;
	SYST_CVR = reload & 0x00ffffff;
	SYST_CSR = SYST_CSR_ENABLE | (option & 6);
}

void systick_disable_timer(void)
{
	SYST_CSR = 0;
}

int systick_get_status(int status)
{
	return SYST_CSR & status;
}

int systick_get_timer(void)
{
	return SYST_CVR & 0x00ffffff;
}

void systick_clear_timer(void)
{
	SYST_CVR = 0;
}

void systick_set_reload(int reload)
{
	SYST_RVR = reload & 0x00ffffff;
}

int systick_get_calibration(void)
{
	return SYST_CALIB & 0xc0ffffff;
}
