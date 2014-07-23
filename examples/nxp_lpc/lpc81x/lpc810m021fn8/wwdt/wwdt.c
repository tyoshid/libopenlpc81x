/*
 * wwdt - Test watchdog reset.
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
#include <mrt.h>
#include <wwdt.h>
#include <nvic.h>
#include <scb.h>

extern char end;
static char *heap = &end;

#define TESTNUM		(*heap)
#define STATE		(*(heap + 1))
#define INTSTATUS	(*(heap + 2))

/* Set LPC810 to 24 MHz (+/- 1.5%). */
static void clock_setup(void)
{
	/* Set up PLL (Fclkin = 12MHz (IRC), Fclkout (Main clock) = 24MHz) */
	syscon_enable_pll(SYSCON_IRC, 2, 4);

	/* Select PLL as system clock source. */
	syscon_set_system_clock(SYSCON_PLL_OUT, 1);

	/* wdt_osc_clk = Fclkana / (2 * (1 + DIVSEL)) */
	if (TESTNUM == 4 - 1)
		/* 0.6MHz / (2 * (1 + 31)) = 9.3kHz (+/- 40%) */
		syscon_enable_wdt_osc(SYSCON_0_6MHZ, 31);
	else
		/* 4.6MHz / (2 * (1 + 0)) = 2.3MHz (+/- 40%) */
		syscon_enable_wdt_osc(SYSCON_4_6MHZ, 0);
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

	/* Set channel 0 to 'bus-stall mode'. */
	mrt_set_mode(MRT0, MRT_BUS_STALL);

	/* Set channel 1 to 'one-shot interrupt mode'. */
	mrt_set_mode(MRT1, MRT_ONE_SHOT);
}

static void wwdt_setup(void)
{
	/* Enable WWDT clock. */
	syscon_enable_clock(SYSCON_WWDT);
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

static void blink_led(int n)
{
	int i;

	for (i = 0; i < n; i++) {
		/* LED2 on */
		gpio_set(PIO0_3);

		/* Wait a bit */
		delay_ms(250);

		/* LED2 off */
		gpio_clear(PIO0_3);

		/* Wait a bit */
		delay_ms(250);
	}

	/* Wait a bit */
	delay_ms(250);
}

void wdt_isr(void)
{
	/* Get interrupt status. */
	INTSTATUS = wwdt_get_status(WWDT_TIME_OUT | WWDT_WARNING);

	/* We can't clear interrupt(?). */
	/* software system reset */
	scb_reset_system();
}

/*
 * TEST 0 
 * Time-out = 0xff
 * min = 1 / 2.3MHz * 4 * (0xff + 1) * (1 - 40%) = 267 usec
 * max = 1 / 2.3MHz * 4 * (0xff + 1) * (1 + 40%) = 624 usec
 */
static void test0(void)
{
	int i;

	/* STATE 0 */
	STATE = 0;

	/* LED1 on */
	gpio_set(PIO0_2);

	/* Blink LED2. */
	blink_led(TESTNUM + 1);

	/* Start watchdog timer. */
	wwdt_start(WWDT_RESET, 0xff, 0xffffff);

	for (i = 0; i < 5; i++) {
		/* 266 usec * (1 - 1.5%) = 263 usec */
		delay_us(260);

		/* Restart watchdog timer. */
		wwdt_restart();
	}

	/* STATE 1 */
	STATE = 1;

	/* LED1 off */
	gpio_clear(PIO0_2);

	/* 624 usec * (1 + 1.5%) = 633 usec */
	delay_us(640);

	/* STATE 2 */
	STATE = 2;

	/* LED1 on */
	gpio_set(PIO0_2);
}

/*
 * TEST 1
 * Time-out = 0x2000 (Window = 0x1400)
 * min = 1 / 2.3MHz * 4 * (0x2000 + 1) * (1 - 40%) = 8.5 msec
 * max = 1 / 2.3MHz * 4 * (0x2000 + 1) * (1 + 40%) = 19.9 msec
 */
static void test1(void)
{
	int i;

	/* STATE 0 */
	STATE = 0;

	/* LED1 on */
	gpio_set(PIO0_2);

	/* Blink LED2. */
	blink_led(TESTNUM + 1);

	/* Start watchdog timer. */
	wwdt_start(WWDT_RESET, 0x2000, 0x1400);

	for (i = 0; i < 5; i++) {
		/* 8.5 msec * (1 - 1.5%) = 8.4 msec */
		delay_ms(8);

		/* Restart watchdog timer. */
		wwdt_restart();
	}

	/* STATE 1 */
	STATE = 1;

	/* LED1 off */
	gpio_clear(PIO0_2);

	/* 19.9 msec * (1 + 1.5%) = 20.2 msec */
	delay_ms(21);

	/* STATE 2 */
	STATE = 2;

	/* LED1 on */
	gpio_set(PIO0_2);
}

/*
 * TEST 2
 * Window = 0x1400 (Time-out = 0x2000)
 * min = 1 / 2.3MHz * 4 * (0x2000 - 0x1400) * (1 - 40%) = 3.2 msec
 * max = 1 / 2.3MHz * 4 * (0x2000 - 0x1400) * (1 + 40%) = 7.5 msec
 */
static void test2(void)
{
	int i;

	/* STATE 0 */
	STATE = 0;

	/* LED1 on */
	gpio_set(PIO0_2);

	/* Blink LED2. */
	blink_led(TESTNUM + 1);

	/* Start watchdog timer. */
	wwdt_start(WWDT_RESET, 0x2000, 0x1400);

	for (i = 0; i < 5; i++) {
		/* 7.5 msec * (1 + 1.5%) = 7.6 msec */
		delay_ms(8);

		/* Restart watchdog timer. */
		wwdt_restart();
	}

	/* STATE 1 */
	STATE = 1;

	/* LED1 off */
	gpio_clear(PIO0_2);

	/* 3.2 msec * (1 - 1.5%) = 3.2 msec */
	delay_ms(3);

	/* Restart watchdog timer. */
	/* Early Feed Event */
	wwdt_restart();

	/* Wait a bit. */
	delay_ms(1);

	/* STATE 2 */
	STATE = 2;

	/* LED1 on */
	gpio_set(PIO0_2);
}

/*
 * TEST 3
 * Time-out = 0xffffff
 * min = 1 / 2.3MHz * 4 * (0xffffff + 1) * (1 - 40%) = 17.5 sec
 * max = 1 / 2.3MHz * 4 * (0xffffff + 1) * (1 + 40%) = 40.8 sec
 */
static void test3(void)
{
	int i;

	/* STATE 0 */
	STATE = 0;

	/* LED1 on */
	gpio_set(PIO0_2);

	/* Blink LED2. */
	blink_led(TESTNUM + 1);

	/* Start watchdog timer. */
	wwdt_start(WWDT_RESET, 0xffffff, 0xffffff);

	/* 17.5 sec * (1 - 1.5%) = 17.2 usec */
	for (i = 0; i < 17 * 2; i++) {
		delay_ms(500);

		/* LED1 on/off */
		gpio_toggle(PIO0_2);
	}

	/* Restart watchdog timer. */
	wwdt_restart();

	/* STATE 1 */
	STATE = 1;

	/* LED1 off */
	gpio_clear(PIO0_2);

	/* 40.8 sec * (1 + 1.5%) = 41.5 sec */
	for (i = 0; i < 42 * 2; i++) {
		delay_ms(500);

		/* LED1 on/off */
		gpio_toggle(PIO0_2);
	}

	/* STATE 2 */
	STATE = 2;

	/* LED1 on */
	gpio_set(PIO0_2);
}

/*
 * TEST 4
 * Time-out = 0xff
 * min = 1 / 9.3kHz * 4 * (0xff + 1) * (1 - 40%) = 65.5 msec
 * max = 1 / 9.3kHz * 4 * (0xff + 1) * (1 + 40%) = 153 msec
 */
static void test4(void)
{
	int i;

	/* STATE 0 */
	STATE = 0;

	/* LED1 on */
	gpio_set(PIO0_2);

	/* Blink LED2. */
	blink_led(TESTNUM + 1);

	/* Start watchdog timer. */
	wwdt_start(WWDT_RESET, 0xff, 0xffffff);

	for (i = 0; i < 5; i++) {
		/* 65.5 msec * (1 - 1.5%) = 64.6 msec */
		delay_ms(64);

		/* Restart watchdog timer. */
		wwdt_restart();
	}

	/* STATE 1 */
	STATE = 1;

	/* LED1 off */
	gpio_clear(PIO0_2);

	/* 153 msec * (1 + 1.5%) = 155 msec */
	delay_ms(156);

	/* STATE 2 */
	STATE = 2;

	/* LED1 on */
	gpio_set(PIO0_2);
}

/*
 * TEST 5
 * Interrupt
 * Time-out = 0x2000 (Window = 0x1400)
 */
static void test5(void)
{
	int i;

	/* STATE 0 */
	STATE = 0;

	/* LED1 on */
	gpio_set(PIO0_2);

	/* Blink LED2. */
	blink_led(TESTNUM + 1);

	/* Clear status. */
	wwdt_clear_status(WWDT_TIME_OUT);

	/* Enable watchdog interrupt. */
	nvic_enable_irq(NVIC_WDT);

	/* Start watchdog timer. */
	wwdt_start(0, 0x2000, 0x1400);

	for (i = 0; i < 5; i++) {
		/* 8.5 msec * (1 - 1.5%) = 8.4 msec */
		delay_ms(8);

		/* Restart watchdog timer. */
		wwdt_restart();
	}

	/* STATE 1 */
	STATE = 1;

	/* LED1 off */
	gpio_clear(PIO0_2);

	/* 19.9 msec * (1 + 1.5%) = 20.2 msec */
	delay_ms(21);

	/* STATE 2 */
	STATE = 2;

	/* LED1 on */
	gpio_set(PIO0_2);
}


int main(void)
{
	int s;

	/* Get system reset status. */
	s = syscon_get_reset_status(SYSCON_WDT_RST | SYSCON_SYSRST);

	/* Clear system reset status. */
	syscon_clear_reset_status(SYSCON_WDT_RST | SYSCON_SYSRST);
	
	clock_setup();
	gpio_setup();
	mrt_setup();
	wwdt_setup();

	if (s & SYSCON_WDT_RST) {
		/* watchdog reset */
		if (STATE == 0)
			goto wwdt_error0;
		if (STATE == 2)
			goto wwdt_error2;
		if (TESTNUM == 5)
			goto wwdt_error0;

		/* next test */
		if (TESTNUM >= 5)
			TESTNUM = 0;
		else
			TESTNUM++;
	} else if (s & SYSCON_SYSRST) {
		/* software system reset */
		if (TESTNUM != 5)
			goto wwdt_error0;
		if (STATE == 0)
			goto wwdt_error0;
		if (STATE == 2)
			goto wwdt_error2;
		if (INTSTATUS != (WWDT_TIME_OUT | WWDT_WARNING))
			goto wwdt_error2;

		INTSTATUS = 0;
		TESTNUM = 0;
	} else {
		/* power-on reset */
		TESTNUM = 0;
	}

	switch (TESTNUM) {
	case 0:
		test0();
		break;
	case 1:
		test1();
		break;
	case 2:
		test2();
		break;
	case 3:
		test3();
		break;
	case 4:
		test4();
		break;
	case 5:
		test5();
		break;
	default:
		break;
	}

wwdt_error2:
	while (1) {
		/* Blink LED2 slowly. */
		gpio_toggle(PIO0_3);
		delay_ms(2000);
	}

wwdt_error0:
	while (1) {
		/* Blink LED2 quickly. */
		gpio_toggle(PIO0_3);
		delay_ms(500);
	}

	return 0;
}
