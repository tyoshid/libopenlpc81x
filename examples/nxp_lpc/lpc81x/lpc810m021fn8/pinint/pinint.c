/*
 * pinint - Count the number of button pushes.
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
#include <pinint.h>

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

	/* Set PIO0_3 to 'input float' (SW). */
	gpio_config(GPIO_INPUT, GPIO_HYST, PIO0_3);
}

static void pinint_setup(void)
{
	/* Set PININT0 to PIO0_3. */
	syscon_select_pins(0, PIO0_3);

	/* Enable interrupt. */
	/* PININT0: Pin interrupt */
	/* PININT1: Pattern match */
	nvic_enable_irq(NVIC_PININT0 | NVIC_PININT1);

	/* Clear interrupt */
	pinint_clear_interrupt(PININT0);

	/* Enable falling edge interrupt. */
	pinint_set_interrupt_mode(PININT_FALLING, PININT0);
}

/*
 * Change the interrupt mode when the button is pushed and released 3 times.
 * 1) falling edge (pin interrupt)
 * 2) rising edge (pin interrupt)
 * 3) both edge (pin interrupt)
 * 4) falling edge (pattern match)
 * 5) rising edge (pattern match)
 * 6) both edge (pattern match)
 */

/* Pin interrupt */
void pinint0_isr(void)
{
	pinint_mode_t m;
	static int count;

	/* Not pattern match! */
	if (pinint_pattern_match_enabled())
		return;

	m = pinint_get_interrupt_mode(PININT0);
	if (m == PININT_DISABLE || !pinint_get_interrupt_status(PININT0))
		return;

	switch (m) {
	case PININT_FALLING:
		if (!pinint_falling_edge_detected(PININT0))
			break;

		/* Blink the LED (PIO0_2). */
		gpio_toggle(PIO0_2);

		if (++count >= 3) {
			count = -1;

			/* Enable rising edge interrupt. */
			pinint_set_interrupt_mode(PININT_RISING, PININT0);
		}
		break;
	case PININT_RISING:
		if (!pinint_rising_edge_detected(PININT0))
			break;

		if (count >= 0)
			/* Blink the LED (PIO0_2). */
			gpio_toggle(PIO0_2);

		if (++count >= 3) {
			count = 0;

			/* Enable both edge interrupt. */
			pinint_set_interrupt_mode(PININT_BOTH, PININT0);
		}
		break;
	case PININT_BOTH:
		if (!pinint_rising_edge_detected(PININT0) &&
		    !pinint_falling_edge_detected(PININT0))
			break;

		/* Blink the LED (PIO0_2). */
		gpio_toggle(PIO0_2);

		if (++count >= 6) {
			count = 0;

			/* Pattern match (falling edge) */
			pinint_set_bit_slice(0, 0, PININT_CONST_HIGH, false);
			pinint_set_bit_slice(1, 0, PININT_STICKY_FALLING,
					     true);

			/* Enable patten match. */
			pinint_enable_pattern_match();
		}
		break;
	default:
		break;
	}

	/* Clear interrupt. */
	pinint_clear_interrupt(PININT0);
}

/* Pattern match */
void pinint1_isr(void)
{
	static int count;

	if (pinint_pattern_match_enabled()) {
		if (count != 3)
			/* Blink the LED (PIO0_2). */
			gpio_toggle(PIO0_2);

		count++;
		if (count < 3) {
			/* Falling edge */
			pinint_set_bit_slice(1, 0, PININT_STICKY_FALLING,
					     true);
		} else if (count < 7) {
			/* Rising edge */
			pinint_set_bit_slice(1, 0, PININT_STICKY_RISING, true);
		} else if (count < 13) {
			/* Both edge */
			pinint_set_bit_slice(1, 0, PININT_STICKY_BOTH, true);
		} else {
			count = 0;

			/* Disable patten match. */
			pinint_disable_pattern_match();

			/* Clear interrupt */
			pinint_clear_interrupt(PININT0);

			/* Enable falling edge interrupt. */
			pinint_set_interrupt_mode(PININT_FALLING, PININT0);
		}
	}
}

int main(void)
{
	clock_setup();
	gpio_setup();
	pinint_setup();

	while (1)
		;

	return 0;
}
