/*
 * rc_servo - Control the shaft of a servo motor with a switch.
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
#include <sct.h>

#define PCLK	24000000

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

	/* Set PIO0_2 to 'SCT output 0'. */
	gpio_config(GPIO_CTOUT0, 0, PIO0_2);

	/* Set PIO0_3 to 'SCT input 0'. */
	gpio_config(GPIO_CTIN0, GPIO_HYST, PIO0_3);
}

static void sct_setup(void)
{
	/* Enable SCT clock. */
	syscon_enable_clock(SYSCON_SCT);

	/* Set counter to 32-bit. */
	sct_config(SCT_CLOCK_BUS, 0, SCT_UNIFY | SCT_AUTOLIMIT);

	/* Match0: limit */
	/* 50Hz */
	sct_set_match_and_reload(0, PCLK / 50 - 1);

	/* Match1: PWM low threshold (state 0) */
	sct_set_match_and_reload(1, 0);

	/* Match2: PWM high threshold (state 0) */
	/* pulse width: 1 msec */
	sct_set_match_and_reload(2, PCLK / 1000 - 1);

	/* Match3: PWM low threshold (state 1) */
	sct_set_match_and_reload(3, 0);

	/* Match4: PWM high threshold (state 1) */
	/* pulse width: 2 msec */
	sct_set_match_and_reload(4, PCLK / 500 - 1);

	/* Event0: match1 -> output0 low (state 0) */
	sct_setup_event(0, 1, 0, SCT_STATE0 | SCT_MATCH_ONLY,
			SCT_EV_OUT0_CLR, 0);

	/* Event1: match2 -> output0 high (state 0) */
	sct_setup_event(1, 2, 0, SCT_STATE0 | SCT_MATCH_ONLY,
			SCT_EV_OUT0_SET, 0);

	/* Event2: input0 (falling edge) -> state 1 */
	sct_setup_event(2, 0, 0, SCT_STATE0 | SCT_IO_ONLY | SCT_FALL,
			SCT_EV_STATE_LOAD, 1);

	/* Event3: match3 -> output0 low (state 1) */
	sct_setup_event(3, 3, 0, SCT_STATE1 | SCT_MATCH_ONLY,
			SCT_EV_OUT0_CLR, 0);

	/* Event4: match4 -> output0 high (state 1) */
	sct_setup_event(4, 4, 0, SCT_STATE1 | SCT_MATCH_ONLY,
			SCT_EV_OUT0_SET, 0);

	/* Event5: input0 (falling edge) -> state 0 */
	sct_setup_event(5, 0, 0, SCT_STATE1 | SCT_IO_ONLY | SCT_FALL,
			SCT_EV_STATE_LOAD, 0);

	/* Start timer. */
	sct_start_counter();
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
