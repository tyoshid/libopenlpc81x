/*
 * MRT functions
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

#include <mrt.h>

void mrt_set_mode(mrt_channel_t mrt, mrt_mode_t mode)
{
	MRT_CTRL(mrt) = mode;
}

void mrt_set_interval(mrt_channel_t mrt, int ivalue)
{
	MRT_INTVAL(mrt) = ivalue;
}

int mrt_get_timer(mrt_channel_t mrt)
{
	return MRT_TIMER(mrt);
}

void mrt_enable_interrupt(int interrupt)
{
	int i;

	for (i = 0; i < 4; i++) {
		if (interrupt & (1 << i))
			MRT_CTRL(i) |= MRT_CTRL_INTEN;
	}
}

void mrt_disable_interrupt(int interrupt)
{
	int i;

	for (i = 0; i < 4; i++) {
		if (interrupt & (1 << i))
			MRT_CTRL(i) &= ~MRT_CTRL_INTEN;
	}
}

int mrt_get_interrupt_mask(int interrupt)
{
	int i;
	int r = 0;

	for (i = 0; i < 4; i++) {
		if (interrupt & (1 << i)) {
			if (MRT_CTRL(i) & MRT_CTRL_INTEN)
				r |= 1 << i;
		}
	}
	return r;
}

int mrt_get_interrupt_status(int interrupt)
{
	return MRT_IRQ_FLAG & interrupt;
}

void mrt_clear_interrupt(int interrupt)
{
	MRT_IRQ_FLAG = interrupt;
}

int mrt_get_channel_status(mrt_channel_t mrt, int status)
{
	return MRT_STAT(mrt) & status;
}

void mrt_clear_channel_status(mrt_channel_t mrt, int status)
{
	MRT_STAT(mrt) = status;
}

int mrt_get_idle_channel(void)
{
	return MRT_IDLE_CH >> 4;
}
