/*
 * fancyblink - Blink two LEDs.
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

volatile int dont_delete_loop;

/* Set LPC810 to 24 MHz. */
static void clock_setup(void)
{
	/*
	 * Select IRC (12MHz) as PLL source (Fclkin).
	 * Main clock (Fclkout) is 24MHz (< 100MHz).
	 * M = Fclkout / Fclkin = 2
	 * FCCO = 2 * P * Fclkout (156MHz <= FCCO <= 320MHz)
	 * P = 4 (FCCO = 192MHz)
	 */
	syscon_enable_pll(SYSCON_IRC, 2, 4);

	/*
	 * Select PLL as system clock source.
	 * Main clock:	 24MHz
	 * System clock: 24MHz
	 */
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

	/* Set PIO0_2 and PIO0_3 to 'output push-pull'. */
	gpio_config(GPIO_OUTPUT, 0, PIO0_2 | PIO0_3);

	/* LED1 on */
	gpio_set(PIO0_2);

	/* LED2 off */
	gpio_clear(PIO0_3);
}

int main(void)
{
	int i;

	clock_setup();
	gpio_setup();

	/* Blink the LEDs (PIO0_2 and PIO0_3). */
	while (1) {
		/* Wait a bit. */
		for (i = 0; i < 500000; i++)
			dont_delete_loop;

		/* LED on/off */
		gpio_toggle(PIO0(2, 3));
	}

	return 0;
}
