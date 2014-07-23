/*
 * acmp - Use a potentiometer to fade an LED.
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
#include <mrt.h>
#include <acmp.h>

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

	/* Prevent the pins from internally floating. */
	gpio_config(GPIO_OUTPUT, GPIO_IO, PIO0_10 | PIO0_11);
	gpio_clear(PIO0_10 | PIO0_11);

	/* Set PIO0_2 to 'SCT output 0'. */
	gpio_config(GPIO_CTOUT0, 0, PIO0_2);

	/* Set PIO0_0 to 'ACMP input 1'. */
	gpio_config(GPIO_ACMP_I1, 0, PIO0_0);
}

static void sct_setup(void)
{
	/* Enable SCT clock. */
	syscon_enable_clock(SYSCON_SCT);

	/* Set counter to 32-bit. */
	sct_config(SCT_CLOCK_BUS, 0, SCT_UNIFY | SCT_AUTOLIMIT);

	/* Match0: limit */
	/* 100Hz */
	sct_set_match_and_reload(0, 24000000 / 100 - 1);

	/* Match1: PWM low threshold */
	sct_set_match_and_reload(1, 0);

	/* Match2: PWM high threshold */
	sct_set_match_and_reload(2, 24000000 / 100 / 32);

	/* Event0: match1 -> output0 high */
	sct_setup_event(0, 1, 0, SCT_STATE0 | SCT_MATCH_ONLY,
			SCT_EV_OUT0_SET, 0);

	/* Event1: match2 -> output0 low */
	sct_setup_event(1, 2, 0, SCT_STATE0 | SCT_MATCH_ONLY,
			SCT_EV_OUT0_CLR, 0);

	/* Start timer. */
	sct_start_counter();
}

static void mrt_setup(void)
{
	/* Enable MRT clock. */
	syscon_enable_clock(SYSCON_MRT);

	/* Set channel 0 to 'bus-stall mode'. */
	mrt_set_mode(MRT0, MRT_BUS_STALL);

	/* Set channel 1 to 'one-shot interrupt mode'. */
	mrt_set_mode(MRT1, MRT_ONE_SHOT);
}

static void acmp_setup(void)
{
	/* Enable ACMP power. */
	syscon_disable_power_down(SYSCON_ACMP_PD);

	/* Enable ACMP clock. */
	syscon_enable_clock(SYSCON_ACMP);

	/* Set up comparator. */
	acmp_init(ACMP_I1, ACMP_LADDER, ACMP_NONE, false, 0);

	/* Enable voltage ladder. */
	acmp_enable_ladder(false);
}

static void delay_us(int us)
{
	mrt_set_interval(MRT0, 24000000 / 1000000 * us);
}

static void delay_ms(int ms)
{
	mrt_set_interval(MRT1, 24000000 / 1000 * ms);

	while (!mrt_get_channel_status(MRT1, MRT_INT))
		;

	mrt_clear_channel_status(MRT1, MRT_INT);
}

int main(void)
{
	int i;

	clock_setup();
	gpio_setup();
	sct_setup();
	mrt_setup();
	acmp_setup();

	/* voltage ladder power-up settling time */
	delay_us(ACMP_TS_PU);

	while (1) {
		for (i = 0; i < 32; i++) {
			/* Switch voltage ladder. */
			acmp_set_ladder(i);

			/* switching settling time + propagation delay */
			delay_us(ACMP_TS_SW + 1);

			if (!acmp_get_status(ACMP_COMPSTAT))
				break;
		}

		sct_set_match_reload(2, 24000000 / 100 / 32 * i);

		delay_ms(100);
	}

	return 0;
}
