/*
 * sct - Blink two LEDs.
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
#include <sct.h>

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

	/* Set PIO0_2 and PIO0_3 to 'output push-pull' (LED). */
	gpio_config(GPIO_OUTPUT, 0, PIO0(2, 3));
}

static void sct_setup(void)
{
	/* Enable SCT clock. */
	syscon_enable_clock(SYSCON_SCT);

	/* Enable the SCT interrupt. */
	nvic_enable_irq(NVIC_SCT);

	/* Set counter to 16-bit. */
	sct_config(SCT_CLOCK_BUS, 0, SCT_NORELOAD_L | SCT_NORELOAD_H |
		   SCT_AUTOLIMIT_L | SCT_AUTOLIMIT_H);

	/* Match0: limit */
	/* 2Hz: 100kHz / 2Hz = 50000 (< 65535) */
	sct_set_match_l(0, 50000 - 1);
	/* 3Hz: 120kHz / 3Hz = 40000 (< 65535) */
	sct_set_match_h(0, 40000 - 1);

	/* Event0: match0 low -> interrupt */
	sct_setup_event(0, 0, 0, SCT_STATE0 | SCT_MATCH_ONLY | SCT_REG_L,
			SCT_EV_INT, 0);

	/* Event1: match0 high -> interrupt */
	sct_setup_event(1, 0, 0, SCT_STATE0 | SCT_MATCH_ONLY | SCT_REG_H,
			SCT_EV_INT, 0);

	/* Set prescaler value. */
	/* 100kHz: 24MHz / 100kHz = 240 (< 256) */
	sct_set_prescaler_l(240);
	/* 120kHz: 24MHz / 120kHz = 200 (< 256) */
	sct_set_prescaler_h(200);


	/* Start timer. */
	sct_start_counter_l();
	sct_start_counter_h();
}

void sct_isr(void)
{
	int m;
	int s;

	/* Get interrupt mask. */
	m = sct_get_interrupt_mask(SCT_EVENT0 | SCT_EVENT1);

	/* Get interrupt status. */
	s = sct_get_interrupt_status(SCT_EVENT0 | SCT_EVENT1);

	/* Event0 */
	if (m & s & SCT_EVENT0) {
		/* LED1 on/off */
		gpio_toggle(PIO0_2);

		/* Clear interrupt. */
		sct_clear_interrupt(SCT_EVENT0);
	}

	/* Event1 */
	if (m & s & SCT_EVENT1) {
		/* LED2 on/off */
		gpio_toggle(PIO0_3);

		/* Clear interrupt. */
		sct_clear_interrupt(SCT_EVENT1);
	}
}

int main(void)
{
	clock_setup();
	gpio_setup();
	sct_setup();

	while (1)
		;

	return 0;
}
