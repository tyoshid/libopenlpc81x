/*
 * NVIC functions
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

#include <nvic.h>

int nvic_get_enabled_irq(int interrupts)
{
	return NVIC_ISER0 & interrupts;
}

void nvic_enable_irq(int interrupts)
{
	NVIC_ISER0 = interrupts;
}

void nvic_disable_irq(int interrupts)
{
	NVIC_ICER0 = interrupts;
}

int nvic_get_pending_irq(int interrupts)
{
	return NVIC_ISPR0 & interrupts;
}

void nvic_set_pending_irq(int interrupts)
{
	NVIC_ISPR0 = interrupts;
}

void nvic_clear_pending_irq(int interrupts)
{
	NVIC_ICPR0 = interrupts;
}

int nvic_get_active_irq(int interrupts)
{
	return NVIC_IABR0 & interrupts;
}

void nvic_set_priority(int irqn, int priority)
{
	int r;

	r = NVIC_IPR(irqn / 4);
	r &= ~(NVIC_PRIO_3 << irqn % 4 * 8);
	NVIC_IPR(irqn / 4) = r | priority << irqn % 4 * 8;
}

int nvic_get_priority(int irqn)
{
	int r;

	r = NVIC_IPR(irqn / 4);
	return (r >> (irqn % 4 * 8)) & 0xff;
}
