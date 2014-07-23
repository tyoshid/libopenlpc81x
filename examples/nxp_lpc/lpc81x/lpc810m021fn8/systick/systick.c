/*
 * systick - Blink an LED.
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

#include <syscon.h>
#include <gpio.h>
#include <systick.h>

/* Set LPC810 to 24 MHz. */
void clock_setup(void)
{
	/* Set up PLL (Fclkin = 12MHz (IRC), Fclkout (Main clock) = 24MHz) */
	syscon_enable_pll(SYSCON_IRC, 2, 4);

	/* Select PLL as system clock source. */
	syscon_set_system_clock(SYSCON_PLL_OUT, 1);
}

static void gpio_setup(void)
{
	/* Enable GPIO clock. */
	// syscon_enable_clock(SYSCON_GPIO);

	/* Enable switch matrix clock. */
	// syscon_enable_clock(SYSCON_SWM);

	/* Enable IOCON clock. */
	syscon_enable_clock(SYSCON_IOCON);

	/* Prevent the I2C pins from internally floating. */
	gpio_config(GPIO_OUTPUT, GPIO_IO, PIO0_10 | PIO0_11);
	gpio_clear(PIO0_10 | PIO0_11);

	/* Set PIO0_2 to 'output push-pull' (LED). */
	gpio_config(GPIO_OUTPUT, 0, PIO0_2);
}

void systick_isr(void)
{
	if (systick_get_status(SYST_INT_ENABLE | SYST_COUNTFLAG) ==
	    (SYST_INT_ENABLE | SYST_COUNTFLAG)) {
		/* LED on/off */
		gpio_toggle(PIO0_2);
	}
}

int main(void)
{
	clock_setup();
	gpio_setup();

	systick_enable_timer(12000000 - 1, SYST_INT_ENABLE);

	while (1)
		;

	return 0;
}
