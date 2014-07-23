/*
 * usart - Transmit characters.
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
#include <usart.h>

volatile int dont_delete_loop;

/* USART clock frequency */
#define U_PCLK	(16 * 115200)

/* Set LPC810 to 24 MHz. */
static void clock_setup(void)
{
	/* Set up PLL (Fclkin = 12MHz (IRC), Fclkout (Main clock) = 24MHz) */
	syscon_enable_pll(SYSCON_IRC, 2, 4);

	/* Select PLL as system clock source. */
	syscon_set_system_clock(SYSCON_PLL_OUT, 1);

	/* Enable USART clock (U_PCLK). */
	syscon_set_usart_clock(24000000, U_PCLK);
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

	/* Set PIO0_4 to 'USART0 TXD'. */
	gpio_config(GPIO_U0_TXD, GPIO_HYST, PIO0_4);
}

static void usart_setup(void)
{
	/* Enable USART0 clock. */
	syscon_enable_clock(SYSCON_UART0);

	/* Set up USART0. */
	usart_init(USART0, U_PCLK, 9600, 8, 1, USART_PARITY_NONE,
		   USART_FLOW_NONE);
}

int main(void)
{
	char c = '0';
	int n = 0;
	int i;

	clock_setup();
	gpio_setup();
	usart_setup();


	/* Blink the LED (PIO0_2) with every transmitted byte. */
	while (1) {
		/* LED on/off */
		gpio_toggle(PIO0_2);

		/* Send digit. */
		usart_send_blocking(USART0, c++);
		if (c > '9')
			c = '0';

		/* Send 'CR' and 'NL' once every 80 times. */
		if (++n >= 80) {
			usart_send_blocking(USART0, '\r');
			usart_send_blocking(USART0, '\n');
			n = 0;
		}

		/* Wait a bit. */
		for (i = 0; i < 1000000; i++)
			dont_delete_loop;
	}

	return 0;
}
