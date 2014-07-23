/*
 * flashcon - Calculate the flash signatures.
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
#include <flashcon.h>

#define FLASHSIZE	4096

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

	/* Prevent the pins from internally floating. */
	gpio_config(GPIO_OUTPUT, GPIO_IO, PIO0_10 | PIO0_11);
	gpio_clear(PIO0_10 | PIO0_11);

	/* Set PIO0_2 and PIO0_3 to 'output push-pull' (LED). */
	gpio_config(GPIO_OUTPUT, 0, PIO0_2 | PIO0_3);

	/* Set PIO0_4 to 'USART0 TXD'. */
	gpio_config(GPIO_U0_TXD, GPIO_HYST, PIO0_4);
}

static void usart_setup(void)
{
	/* Enable USART0 clock. */
	syscon_enable_clock(SYSCON_UART0);

	/* Set up USART0. */
	usart_init(USART0, U_PCLK, 115200, 8, 1, USART_PARITY_NONE,
		   USART_FLOW_NONE);
}

static void print_hex(int c)
{
	int d;

	d = (c >> 4) & 0xf;
	if (d > 9)
		d += 'a' - 10;
	else
		d += '0';
	usart_send_blocking(USART0, d);
	d = c & 0xf;
	if (d > 9)
		d += 'a' - 10;
	else
		d += '0';
	usart_send_blocking(USART0, d);
}

static void print_hex_half_word(int w)
{
	print_hex((w >> 8) & 0xff);
	print_hex(w & 0xff);
}

static void print_hex_word(int w)
{
	print_hex((w >> 24) & 0xff);
	print_hex((w >> 16) & 0xff);
	print_hex((w >> 8) & 0xff);
	print_hex(w & 0xff);
}

static void print_int(int d)
{
	bool zerosup;
	int i;

	if (d < 0) {
		usart_send_blocking(USART0, '-');
		d = -d;
	}
	zerosup = true;
	for (i = 1000000000; i > 0; i /= 10) {
		if (d / i || i == 1 || !zerosup) {
			usart_send_blocking(USART0, d / i + '0');
			d %= i;
			zerosup = false;
		}
	}
}

static void xprintf(char *p, int arg0, int arg1, int arg2)
{
	int arg[3];
	int i;

	arg[0] = arg0;
	arg[1] = arg1;
	arg[2] = arg2;
	i = 0;
	while (*p != '\0') {
		if (*p == '%') {
			switch (*++p) {
			case '%':
				usart_send_blocking(USART0, *p++);
				break;
			case 'x':
				if (i < 3)
					print_hex(arg[i++]);
				p++;
				break;
			case 'h':
				if (i < 3)
					print_hex_half_word(arg[i++]);
				p++;
				break;
			case 'w':
				if (i < 3)
					print_hex_word(arg[i++]);
				p++;
				break;
			case 'd':
				if (i < 3)
					print_int(arg[i++]);
				p++;
				break;
			default:
				usart_send_blocking(USART0, '?');
				p++;
				break;
			}
		} else {
			usart_send_blocking(USART0, *p++);
		}
	}
}

static void xputs(char *p)
{
	while (*p != '\0')
		usart_send_blocking(USART0, *p++);
	usart_send_blocking(USART0, '\r');
	usart_send_blocking(USART0, '\n');
}

static int signature(int start, int stop)
{
	unsigned int *p;
	unsigned int s;
	int i;
	unsigned int n;

	p = (unsigned int *)start;
	s = 0;
	for (i = start; i < stop; i += 4) {
		n = *p++ ^ (s >> 1);
		if (s & (1 << 0))
			n ^= 0x80000000;
		if (s & (1 << 10))
			n ^= 0x80000000;
		if (s & (1 << 30))
			n ^= 0x80000000;
		if (s & (1 << 31))
			n ^= 0x80000000;
		s = n;
	}
	return s;
}

int main(void)
{
	int i;
	int j;
	int r;
	int s;

	clock_setup();
	gpio_setup();
	usart_setup();

	xputs("Flash controller");

	while (1) {
		for (i = 0; i < FLASHSIZE; i += 4) {
			/* LED1 on/off */
			gpio_toggle(PIO0_2);

			for (j = i + 3; j < FLASHSIZE; j += 4) {
				r = flashcon_generate_signature(i, j);
				s = signature(i, j);
				if (r != s) {
					xprintf("Fail %w %w ", i, j, 0);
					xprintf("%w (%w)\r\n", r, s, 0);
					/* LED2 on */
					gpio_set(PIO0_3);
				}
			}
		}
	}

	return 0;
}
