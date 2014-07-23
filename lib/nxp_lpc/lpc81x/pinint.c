/*
 * Pin interrupt/pattern match engine functions
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

#include <pinint.h>

void pinint_set_interrupt_mode(pinint_mode_t mode, int pinints)
{
	switch (mode) {
	case PININT_DISABLE:
		PININT_CIENR = pinints;
		PININT_CIENF = pinints;
		break;
	case PININT_RISING:
		PININT_ISEL &= ~pinints;
		PININT_SIENR = pinints;
		PININT_CIENF = pinints;
		break;
	case PININT_FALLING:
		PININT_ISEL &= ~pinints;
		PININT_CIENR = pinints;
		PININT_SIENF = pinints;
		break;
	case PININT_BOTH:
		PININT_ISEL &= ~pinints;
		PININT_SIENR = pinints;
		PININT_SIENF = pinints;
		break;
	case PININT_LOW:
		PININT_ISEL |= pinints;
		PININT_CIENF = pinints;
		PININT_SIENR = pinints;
		break;
	case PININT_HIGH:
		PININT_ISEL |= pinints;
		PININT_SIENF = pinints;
		PININT_SIENR = pinints;
		break;
	default:
		break;
	}
}

pinint_mode_t pinint_get_interrupt_mode(int pinints)
{
	if ((int)(PININT_ISEL & pinints) == pinints) {
		/* Level sensitive */
		if ((int)(PININT_IENR & pinints) == pinints) {
			if ((int)(PININT_IENF & pinints) == pinints)
				return PININT_HIGH;
			else if ((PININT_IENF & pinints) == 0)
				return PININT_LOW;
			return PININT_DISABLE;
		} else if ((PININT_IENR & pinints) == 0) {
			return PININT_DISABLE;
		}
		return PININT_UNKNOWN;
	} else if ((PININT_ISEL & pinints) == 0) {
		/* Edge sensitive */
		if ((int)(PININT_IENR & pinints) == pinints) {
			if ((int)(PININT_IENF & pinints) == pinints)
				return PININT_BOTH;
			else
				return PININT_RISING;
		} else if ((PININT_IENR & pinints) == 0) {
			if ((int)(PININT_IENF & pinints) == pinints)
				return PININT_FALLING;
			else
				return PININT_DISABLE;
		}
	}

	return PININT_UNKNOWN;
}

int pinint_get_interrupt_status(int pinints)
{
	return PININT_IST & pinints;
}

void pinint_clear_interrupt(int pinints)
{
	PININT_IST = pinints;
}

int pinint_rising_edge_detected(int pinints)
{
	return PININT_RISE & pinints;
}

int pinint_falling_edge_detected(int pinints)
{
	return PININT_FALL & pinints;
}

void pinint_clear_rising_edge(int pinints)
{
	PININT_RISE = pinints;
}

void pinint_clear_falling_edge(int pinints)
{
	PININT_FALL = pinints;
}

void pinint_set_bit_slice(int bitslice, int pinint, pinint_match_t match,
			  bool endpoint)
{
	int r;

	bitslice &= 7;

	r = PININT_PMSRC;
	r &= ~(7 << (bitslice * 3 + 8));
	PININT_PMSRC = r | ((pinint & 7) << (bitslice * 3 + 8));

	r = PININT_PMCFG;
	r &= ~(7 << (bitslice * 3 + 8));
	if (bitslice < 7) {
		if (endpoint)
			r |= 1 << bitslice;
		else
			r &= ~(1 << bitslice);
	}
	PININT_PMCFG = r | (match << (bitslice * 3 + 8));
}

void pinint_enable_pattern_match(void)
{
	PININT_PMCTRL |= PININT_PMCTRL_SEL_PMATCH;
}

void pinint_disable_pattern_match(void)
{
	PININT_PMCTRL &= ~PININT_PMCTRL_SEL_PMATCH;
}

bool pinint_pattern_match_enabled(void)
{
	return PININT_PMCTRL & PININT_PMCTRL_SEL_PMATCH;
}

void pinint_enable_rxev(void)
{
	PININT_PMCTRL |= PININT_PMCTRL_ENA_RXEV;
}

void pinint_disable_rxev(void)
{
	PININT_PMCTRL &= ~PININT_PMCTRL_ENA_RXEV;
}

int pinint_get_pattern_match_state(int pinints)
{
	return (PININT_PMCTRL >> 24) & pinints;
}
