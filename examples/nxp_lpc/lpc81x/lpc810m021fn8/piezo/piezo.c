/*
 * piezo - Play tones on a piezo sounder.
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

#define PCLK	24000000

const int freq[] = {
	1047,
	1175,
	1319,
	1397,
	1568,
	1760,
	1976,
	2093
};

volatile int findex;
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

	/* Set PIO0_2 to 'SCT output 0'. */
	gpio_config(GPIO_CTOUT0, 0, PIO0_2);

	/* Set PIO0_3 to 'SCT output 1'. */
	gpio_config(GPIO_CTOUT1, 0, PIO0_3);
}

static void sct_setup(void)
{
	/* Enable SCT clock. */
	syscon_enable_clock(SYSCON_SCT);

	/* Enable the SCT interrupt. */
	nvic_enable_irq(NVIC_SCT);

	/* Set counter to 32-bit. */
	sct_config(SCT_CLOCK_BUS, 0, SCT_UNIFY | SCT_AUTOLIMIT);

	/* Match0: limit */
	sct_set_match_and_reload(0, PCLK / freq[findex] - 1);

	/* Duration (1 sec)*/
	count = freq[findex];

	/* Match1: low threshold */
	sct_set_match_and_reload(1, 0);

	/* Match2: high threshold (duty 50%) */
	sct_set_match_and_reload(2, PCLK / freq[findex] / 2);

	/* Event0: match1 -> output0 high */
	sct_setup_event(0, 1, 0, SCT_STATE0 | SCT_MATCH_ONLY,
			SCT_EV_OUT0_SET, 0);

	/* Event1: match2 -> output0 low */
	sct_setup_event(1, 2, 0, SCT_STATE0 | SCT_MATCH_ONLY,
			SCT_EV_OUT0_CLR, 0);

	/* Event2: match1 -> output1 low */
	sct_setup_event(2, 1, 0, SCT_STATE0 | SCT_MATCH_ONLY,
			SCT_EV_OUT1_CLR, 0);

	/* Event3: match2 -> output1 high */
	sct_setup_event(3, 2, 0, SCT_STATE0 | SCT_MATCH_ONLY,
			SCT_EV_OUT1_SET, 0);

	/* Event4: match1 -> interrupt */
	sct_setup_event(4, 1, 0, SCT_STATE0 | SCT_MATCH_ONLY, SCT_EV_INT, 0);

	/* Start timer. */
	sct_start_counter();
}

void sct_isr(void)
{
	if (sct_get_interrupt_mask(SCT_EVENT4) &&
	    sct_get_interrupt_status(SCT_EVENT4)) {
		if (--count == 0) {
			if (++findex == sizeof(freq) / sizeof(int))
				findex = 0;

			/* Match0: limit */
			sct_set_match_reload(0, PCLK / freq[findex] - 1);

			/* Duration (1 sec)*/
			count = freq[findex];

			/* Match2: high threshold (duty 50%) */
			sct_set_match_reload(2, PCLK / freq[findex] / 2);
		}

		/* Clear interrupt. */
		sct_clear_interrupt(SCT_EVENT4);
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
