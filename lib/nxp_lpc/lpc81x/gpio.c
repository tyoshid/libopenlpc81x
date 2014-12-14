/*
 * GPIO functions
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

#include <swm.h>
#include <iocon.h>
#include <gpio.h>

static const int pinenable_bits[] = {
	(1 << 0),
	(1 << 1) | (1 << 7),
	(1 << 3),
	(1 << 2),
	0,
	(1 << 6),
	(1 << 8),
	0,
	(1 << 4),
	(1 << 5),
	0, 0, 0, 0, 0, 0, 0, 0
};

static volatile u32 * const ioconp[] = {
	&IOCON_PIO0_0,
	&IOCON_PIO0_1,
	&IOCON_PIO0_2,
	&IOCON_PIO0_3,
	&IOCON_PIO0_4,
	&IOCON_PIO0_5,
	&IOCON_PIO0_6,
	&IOCON_PIO0_7,
	&IOCON_PIO0_8,
	&IOCON_PIO0_9,
	&IOCON_PIO0_10,
	&IOCON_PIO0_11,
	&IOCON_PIO0_12,
	&IOCON_PIO0_13,
	&IOCON_PIO0_14,
	&IOCON_PIO0_15,
	&IOCON_PIO0_16,
	&IOCON_PIO0_17
};

static int swm_enable_fixed_pin_function(enum gpio_func func)
{
	int pin;

	switch (func) {
	case GPIO_ACMP_I1:
		SWM_PINENABLE0 &= ~SWM_PINENABLE0_ACMP_I1_EN;
		pin = PIO0_0;
		break;
	case GPIO_ACMP_I2:
		SWM_PINENABLE0 &= ~SWM_PINENABLE0_ACMP_I2_EN;
		pin = PIO0_1;
		break;
	case GPIO_SWCLK:
		SWM_PINENABLE0 &= ~SWM_PINENABLE0_SWCLK_EN;
		pin = PIO0_3;
		break;
	case GPIO_SWDIO:
		SWM_PINENABLE0 &= ~SWM_PINENABLE0_SWDIO_EN;
		pin = PIO0_2;
		break;
	case GPIO_XTALIN:
		SWM_PINENABLE0 &= ~SWM_PINENABLE0_XTALIN_EN;
		pin = PIO0_8;
		break;
	case GPIO_XTALOUT:
		SWM_PINENABLE0 &= ~SWM_PINENABLE0_XTALOUT_EN;
		pin = PIO0_9;
		break;
	case GPIO_RESET:
		SWM_PINENABLE0 &= ~SWM_PINENABLE0_RESET_EN;
		pin = PIO0_5;
		break;
	case GPIO_CLKIN:
		SWM_PINENABLE0 &= ~SWM_PINENABLE0_CLKIN;
		pin = PIO0_1;
		break;
	case GPIO_VDDCMP:
		SWM_PINENABLE0 &= ~SWM_PINENABLE0_VDDCMP;
		pin = PIO0_6;
		break;
	default:
		pin = 0;
		break;
	}
	return pin;
}

static void swm_disable_fixed_pin_function(int pins)
{
	int d;
	int p;
	int r;

	d = 0;
	for (p = 0; p < GPIO_MAXPIN; p++) {
		if (pins & 1 << p)
			d |= pinenable_bits[p];
	}
	r = SWM_PINENABLE0;
	if (r != (r | d))
		SWM_PINENABLE0 = r | d;
}

static int swm_enable_movable_function(enum gpio_func func, int pins)
{
	int p;
	int r;

	for (p = 0; p < GPIO_MAXPIN && !(pins & 1 << p); p++)
		;
	if (p >= GPIO_MAXPIN)
		return 0;

	if (func >= SWM_PINASSIGN_REGNUM * 4)
		return 0;
	r = SWM_PINASSIGN(func / 4);
	r &= ~(0xff << func % 4 * 8);
	SWM_PINASSIGN(func / 4) = r | p << func % 4 * 8;
	
	return 1 << p;
}

static void swm_disable_movable_function(int pins)
{
	int i;
	int r;
	int d;
	int j;
	int a;
	int k;

	for (i = 0; i < SWM_PINASSIGN_REGNUM; i++) {
		r = SWM_PINASSIGN(i);
		if (r == (int)0xffffffff)
			continue;
		d = 0;
		for (j = 0; j < 4; j++) {
			a = r >> (j * 8) & 0xff;
			if (a >= GPIO_MAXPIN)
				continue;
			for (k = 0; k < GPIO_MAXPIN; k++) {
				if (pins & 1 << k && k == a) {
					d |= 0xff << (j * 8);
					break;
				}
			}
		}
		if (d)
			SWM_PINASSIGN(i) = r | d;
	}
}

static void iocon_set_iocon(int iocon, int pins)
{
	int p;

	for (p = 0; p < GPIO_MAXPIN; p++) {
		if (pins & 1 << p)
			*ioconp[p] = iocon;
	}
}

void gpio_config(enum gpio_func func, int iocon, int pins)
{
	switch (func) {
	case GPIO_OUTPUT:
	case GPIO_INPUT:
		/* Standard I/O */
		swm_disable_fixed_pin_function(pins);
		iocon_set_iocon(iocon, pins);
		if (func == GPIO_OUTPUT)
			GPIO_DIR0 |= pins;
		break;
	case GPIO_ACMP_I1:
	case GPIO_ACMP_I2:
	case GPIO_SWCLK:
	case GPIO_SWDIO:
	case GPIO_XTALIN:
	case GPIO_XTALOUT:
	case GPIO_RESET:
	case GPIO_CLKIN:
	case GPIO_VDDCMP:
		/* Fixed-pin function */
		pins = swm_enable_fixed_pin_function(func);
		iocon_set_iocon(iocon, pins);
		break;
	default:
		/* Movable function */
		pins = swm_enable_movable_function(func, pins);
		swm_disable_fixed_pin_function(pins);
		iocon_set_iocon(iocon, pins);
		break;
	}
}

void gpio_reconfig(enum gpio_func func, int iocon, int pins)
{
	switch (func) {
	case GPIO_OUTPUT:
	case GPIO_INPUT:
		/* Standard I/O */
		swm_disable_fixed_pin_function(pins);
		swm_disable_movable_function(pins);
		iocon_set_iocon(iocon, pins);
		if (func == GPIO_OUTPUT)
			GPIO_DIR0 |= pins;
		else
			GPIO_DIR0 &= ~pins;
		break;
	case GPIO_ACMP_I1:
	case GPIO_ACMP_I2:
	case GPIO_SWCLK:
	case GPIO_SWDIO:
	case GPIO_XTALIN:
	case GPIO_XTALOUT:
	case GPIO_RESET:
	case GPIO_CLKIN:
	case GPIO_VDDCMP:
		/* Fixed-pin function */
		pins = swm_enable_fixed_pin_function(func);
		swm_disable_movable_function(pins);
		iocon_set_iocon(iocon, pins);
		break;
	default:
		/* Movable function */
		swm_disable_movable_function(pins);
		pins = swm_enable_movable_function(func, pins);
		swm_disable_fixed_pin_function(pins);
		iocon_set_iocon(iocon, pins);
		break;
	}
}

void gpio_set(int pins)
{
	GPIO_SET0 = pins & 0x3ffff;
}

void gpio_clear(int pins)
{
	GPIO_CLR0 = pins & 0x3ffff;
}

int gpio_get(int pins)
{
	return GPIO_PIN0 & pins;
}

void gpio_toggle(int pins)
{
	GPIO_NOT0 = pins & 0x3ffff;
}

int gpio_read_port(void)
{
	return GPIO_PIN0;
}

void gpio_write_port(int data)
{
	GPIO_PIN0 = data & 0x3ffff;
}

void gpio_set_mask(int mask)
{
	GPIO_MASK0 = mask & 0x3ffff;
}

int gpio_read_masked_port(void)
{
	return GPIO_MPIN0;
}

void gpio_write_maksed_port(int data)
{
	GPIO_MPIN0 = data & 0x3ffff;
}

int gpio_read_pin_byte(int pin)
{
	if (pin < 0 || pin >= GPIO_MAXPIN)
		return 0;

	return GPIO_B(pin);
}

void gpio_write_pin_byte(int pin, int data)
{
	if (pin < 0 || pin >= GPIO_MAXPIN)
		return;

	GPIO_B(pin) = data;
}

int gpio_read_pin_word(int pin)
{
	if (pin < 0 || pin >= GPIO_MAXPIN)
		return 0;

	return GPIO_W(pin);
}

void gpio_write_pin_word(int pin, int data)
{
	if (pin < 0 || pin >= GPIO_MAXPIN)
		return;

	GPIO_W(pin) = data;
}
