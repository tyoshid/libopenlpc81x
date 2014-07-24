/*
 * usart_irq - Receive and transmit (echo back) characters.
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
#include <nvic.h>
#include <usart.h>

/* USART clock frequency */
#define U_PCLK	(16 * 115200)

/* Queue size */
#define QUEUESIZE	8

/* Set LPC810 to 30 MHz. */
static void clock_setup(void)
{
	/*
	 * Select IRC (12MHz) as PLL source (Fclkin).
	 * Main clock (Fclkout) is 60MHz (< 100MHz).
	 * M = Fclkout / Fclkin = 5
	 * FCCO = 2 * P * Fclkout (156MHz <= FCCO <= 320MHz)
	 * P = 2 (FCCO = 240MHz)
	 */
	syscon_enable_pll(SYSCON_IRC, 5, 2);

	/*
	 * Select PLL as system clock source.
	 * Main clock:	 60MHz
	 * System clock: 30MHz
	 */
	syscon_set_system_clock(SYSCON_PLL_OUT, 2);

	/* Enable USART clock (U_PCLK). */
	syscon_set_usart_clock(60000000, U_PCLK);
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

	/* Set PIO0_2 and PIO0_3 to 'output push-pull'. */
	gpio_config(GPIO_OUTPUT, 0, PIO0_2 | PIO0_3);

	/* Set PIO0_4 to 'USART0 TXD'. */
	gpio_config(GPIO_U0_TXD, GPIO_HYST, PIO0_4);

	/* Set PIO0_0 to 'USART0 RXD'. */
	gpio_config(GPIO_U0_RXD, GPIO_HYST, PIO0_0);
}

static void usart_setup(void)
{
	/* Enable USART0 clock. */
	syscon_enable_clock(SYSCON_UART0);

	/* Enable USART0 interrupt. */
	nvic_enable_irq(NVIC_UART0);

	/* Enable USART0 Receive interrupt. */
	usart_enable_interrupt(USART0, USART_RXRDY);

	/* Set up USART0. */
	usart_init(USART0, U_PCLK, 115200, 8, 1, USART_PARITY_NONE,
		   USART_FLOW_NONE);
}

void uart0_isr(void)
{
	int m;
	int s;

	/* Rx, Tx queue */
	static int queue[QUEUESIZE];
	static int head;
	static int tail;

	/* Get interrupt mask. */
	m = usart_get_interrupt_mask(USART0, USART_RXRDY | USART_TXRDY);

	/* Get interrupt status. */
	s = usart_get_interrupt_status(USART0, USART_RXRDY | USART_TXRDY);

	/* RxRDY */
	if (m & s & USART_RXRDY) {
		if ((tail + 1) % QUEUESIZE != head) {
			/* LED1 on/off */
			gpio_toggle(PIO0_2);

			/* Receive data. */
			queue[tail++] = usart_recv(USART0);
			tail %= QUEUESIZE;

			/* Enable transmit interrupt. */
			if (!(m & USART_TXRDY))
				usart_enable_interrupt(USART0, USART_TXRDY);
		} else {
			/* Overflow */
			/* Clear interrupt. */
			usart_recv(USART0);
		}

	}

	/* TxRDY */
	if (m & s & USART_TXRDY) {
		if (head != tail) {
			/* LED2 on/off */
			gpio_toggle(PIO0_3);

			/* Send data. */
			usart_send(USART0, queue[head++]);
			head %= QUEUESIZE;
		} else {
			/* No more data */
			/* Disable transmit interrupt. */
			usart_disable_interrupt(USART0, USART_TXRDY);
		}
	}
}

int main(void)
{
	clock_setup();
	gpio_setup();
	usart_setup();

	while (1)
		;

	return 0;
}
