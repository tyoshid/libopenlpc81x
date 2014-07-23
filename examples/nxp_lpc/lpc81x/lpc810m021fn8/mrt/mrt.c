/*
 * mrt - Blink an LED.
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
#include <mrt.h>

volatile int count;

/* Set LPC810 to 24 MHz. */
static void clock_setup(void)
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

static void mrt_setup(void)
{
	/* Enable MRT clock. */
	syscon_enable_clock(SYSCON_MRT);

	/* Enable MRT interrupt. */
	nvic_enable_irq(NVIC_MRT);

	/* Set channel 0 to 'repeat interrupt mode'. */
	mrt_set_mode(MRT0, MRT_REPEAT);

	/* Enable channel 0 interrupt. */
	mrt_enable_interrupt(MRT_INT0);

	/* Set channel 1 to 'one-shot interrupt mode'. */
	mrt_set_mode(MRT1, MRT_ONE_SHOT);

	/* Set channel 2 to 'bus-stall mode'. */
	mrt_set_mode(MRT2, MRT_BUS_STALL);
}

void mrt_isr(void)
{
	if (mrt_get_interrupt_mask(MRT_INT0) &&
	    mrt_get_interrupt_status(MRT_INT0)) {
		/* LED on/off */
		gpio_toggle(PIO0_2);

		if (++count == 9)
			/* Stop timer. */
			mrt_set_interval(MRT0, 0);

		/* Clear status. */
		mrt_clear_channel_status(MRT0, MRT_INT);

		/* Clear interrupt. */
		mrt_clear_interrupt(MRT_INT0);
	}
}

static void delay_ms(int ms)
{
	mrt_set_interval(MRT1, 24000000 / 1000 * ms);

	while (!mrt_get_channel_status(MRT1, MRT_INT))
		;

	mrt_clear_channel_status(MRT1, MRT_INT);
}

static void delay_us(int us)
{
	mrt_set_interval(MRT2, 24000000 / 1000000 * us);
}

int main(void)
{
	int i;
	int j;

	clock_setup();
	gpio_setup();
	mrt_setup();

	while (1) {
		/* Repeat interrupt mode */
		mrt_set_interval(MRT0, 24000000 / 2);
		while (count < 10)
			;
		count = 0;

		/* One-shot interrupt mode */
		for (i = 0; i < 10; i++) {
			delay_ms(1000);

			/* LED on/off */
			gpio_toggle(PIO0_2);
		}

		/* Bus-stall mode */
		for (i = 0; i < 10; i++) {
			for (j = 0; j < 250; j++)
				delay_us(1000);

			/* LED on/off */
			gpio_toggle(PIO0_2);
		}
	}

	return 0;
}
