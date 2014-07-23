/*
 * wkt - Wake up from deep power-down mode.
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
#include <pmu.h>
#include <gpio.h>
#include <mrt.h>
#include <nvic.h>
#include <wkt.h>
#include <scb.h>

volatile int count;

/* Set LPC810 to 24 MHz. */
static void clock_setup(void)
{
	/* Set up PLL (Fclkin = 12MHz (IRC), Fclkout (Main clock) = 24MHz) */
	syscon_enable_pll(SYSCON_IRC, 2, 4);

	/* Select PLL as system clock source. */
	syscon_set_system_clock(SYSCON_PLL_OUT, 1);

	/* Enable low-power oscillator. */
	pmu_enable_low_power_osc(true);
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

static void mrt_setup(void)
{
	/* Enable MRT clock. */
	syscon_enable_clock(SYSCON_MRT);

	/* Set channel 0 to 'one-shot interrupt mode'. */
	mrt_set_mode(MRT0, MRT_ONE_SHOT);
}

static void wkt_setup(void)
{
	/* Enable WKT clock. */
	syscon_enable_clock(SYSCON_WKT);

	/* Clear interrupt. */
	wkt_clear_status(WKT_ALARM);

	/* Enable WKT interrupt. */
	nvic_enable_irq(NVIC_WKT);

	/* Set clock to IRC. */
	wkt_set_clock(WKT_IRC);
}

void wkt_isr(void)
{
	if (wkt_get_status(WKT_ALARM)) {
		/* LED1 on/off */
		gpio_toggle(PIO0_2);

		count++;
		if (count < 10) {
			/* Reload counter. (IRC)(1 sec) */
			wkt_start_counter(750000 - 1);
		} else if (count == 10) {
			/* Set clock to low-power clock. */
			wkt_set_clock(WKT_LOW_POWER_CLOCK);

			/* Start counter. (1 sec) */
			wkt_start_counter(10000 - 1);
		} else if (count < 20) {
			/* Reload counter. */
			wkt_start_counter(10000 - 1);
		}

		/* Clear interrupt. */
		wkt_clear_status(WKT_ALARM);
	}
}

static void delay_ms(int ms)
{
	/* Start timer. */
	mrt_set_interval(MRT0, 24000000 / 1000 * ms);

	/* Wait for time-out. */
	while (!mrt_get_channel_status(MRT0, MRT_INT))
		;

	/* Clear status */
	mrt_clear_channel_status(MRT0, MRT_INT);
}

int main(void)
{
	int flag;

	clock_setup();
	gpio_setup();
	mrt_setup();
	wkt_setup();

	/* Get deep power-down flag. */
	flag = pmu_get_flag(PMU_DPDFLAG);
	if (flag) {
		/* Clear deep power-down flag. */
		pmu_clear_flag(PMU_DPDFLAG);

		/* Verify flag. */
		if (!PMU_GPREG0)
			goto wkt_error;

		/* Clear flag. */
		PMU_GPREG0 = 0;
	} else {
		/* Verify flag. */
		if (PMU_GPREG0)
			goto wkt_error;
	}

	/* Start counter. (IRC)(1 sec) */
	wkt_start_counter(750000 - 1);
	
	/* Wait for interrupts. */
	while (count < 20)
		;

	/* Deep power-down mode */
	pmu_set_power_mode(PMU_DEEP_POWER_DOWN);

	/* Set flag. */
	PMU_GPREG0 = -1;

	/* Set SLEEPDEEP bit. */
	scb_set_sleep(SCB_SLEEPDEEP);

	/* Start counter.  (low-power clock)(3 sec) */
	wkt_start_counter(10000 * 3 - 1);

	/* Sleep. */
	__asm__ ("wfi");

wkt_error:
	while (1) {
		/* LED2 on/off */
		gpio_toggle(PIO0_3);

		/* Wait a bit. */
		delay_ms(500);
	}

	return 0;
}
