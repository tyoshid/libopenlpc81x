/*
 * miniblink - Blink an LED.
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
// #include <swm.h>
// #include <iocon.h>
#include <gpio.h>

volatile int dont_delete_loop;

static void gpio_setup(void)
{
	/* Manually: */
	/* Enable IOCON clock. */
	// SYSCON_SYSAHBCLKCTRL |= SYSCON_SYSAHBCLKCTRL_IOCON;
	/* Using API functions: */
	syscon_enable_clock(SYSCON_IOCON);

	/* Prevent the I2C pins from internally floating. */
	// IOCON_PIO0_10 = IOCON_I2CMODE_IO;
	// IOCON_PIO0_11 = IOCON_I2CMODE_IO;
	// GPIO_DIR0 |= GPIO_DIR0_OUTPUT << 10 | GPIO_DIR0_OUTPUT << 11;
	// GPIO_CLR0 |= 1 << 10 | 1 << 11;
	gpio_config(GPIO_OUTPUT, GPIO_IO, PIO0_10 | PIO0_11);
	gpio_clear(PIO0_10 | PIO0_11);

	/* Manually: */
	/* Disable SWDIO. */
	// SWM_PINENABLE0 |= SWM_PINENABLE0_SWDIO_EN;
	/* Set IOCON to 'output pushpull'. */
	// IOCON_PIO0_2 = 0;
	/* Set PIO0_2 to output. */
	// GPIO_DIR0 = GPIO_DIR0_OUTPUT << 2;
	/* Using API functions: */
	gpio_config(GPIO_OUTPUT, 0, PIO0_2);
}

int main(void)
{
	int i;

	gpio_setup();

	/* Blink the LED (PIO0_2). */
	while (1) {
		/* Manually: */
		// GPIO_B2 = 1;			/* LED on */
		// for (i = 0; i < 500000; i++)	/* Wait a bit. */
		//	dont_delete_loop;
		// GPIO_B2 = 0;			/* LED off */
		// for (i = 0; i < 500000; i++)	/* Wait a bit. */
		//	dont_delete_loop;

		/* Using API functions gpio_set()/gpio_clear(): */
		// gpio_set(PIO0_2);		/* LED on */
		// for (i = 0; i < 500000; i++)	/* Wait a bit. */
		//	dont_delete_loop;
		// gpio_clear(PIO0_2);		/* LED off */
		// for (i = 0; i < 500000; i++)	/* Wait a bit. */
		//	dont_delete_loop;

		/* Using API function gpio_toggle(): */
		gpio_toggle(PIO0_2);		/* LED on/off */
		for (i = 0; i < 500000; i++)	/* Wait a bit. */
			dont_delete_loop;
	}

	return 0;
}
