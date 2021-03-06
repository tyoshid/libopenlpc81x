/*
 * USART functions
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

#include <usart.h>

static int base_addr(enum usart usart)
{
	switch (usart) {
	case USART0:
		return USART0_BASE;
	case USART1:
		return USART1_BASE;
	case USART2:
		return USART2_BASE;
	default:
		break;
	}
	return 0;
}

void usart_set_baudrate(enum usart usart, int u_pclk, int baud)
{
	USART_BRG(base_addr(usart)) = (u_pclk / (16 * baud) - 1) &
		USART_BRG_MASK;
}

void usart_set_databits(enum usart usart, int bits)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CFG(base);
	if (bits == 9) {
		r |= USART_CFG_DATALEN1;
		r &= ~USART_CFG_DATALEN0;
	} else if (bits == 8) {
		r &= ~USART_CFG_DATALEN1;
		r |= USART_CFG_DATALEN0;
	} else {
		r &= ~(USART_CFG_DATALEN1 | USART_CFG_DATALEN0);
	}
	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_set_stopbits(enum usart usart, int bits)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CFG(base);
	if (bits != 1)
		r |= USART_CFG_STOPLEN;
	else
		r &= ~USART_CFG_STOPLEN;
	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_set_parity(enum usart usart, enum usart_parity parity)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CFG(base);
	if (parity == USART_EVEN) {
		r |= USART_CFG_PARITYSEL1;
		r &= ~USART_CFG_PARITYSEL0;
	} else if (parity == USART_ODD) {
		r |= USART_CFG_PARITYSEL1 | USART_CFG_PARITYSEL0;
	} else {
		r &= ~(USART_CFG_PARITYSEL1 | USART_CFG_PARITYSEL0);
	}
	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_set_flow_control(enum usart usart,
			    enum usart_flowcontrol flowcontrol)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CFG(base);
	if (flowcontrol != USART_FLOW_NONE)
		r |= USART_CFG_CTSEN;
	else
		r &= ~USART_CFG_CTSEN;
	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_init(enum usart usart, int u_pclk, int baud, int databits,
		int stopbits, enum usart_parity parity,
		enum usart_flowcontrol flowcontrol)
{
	int base;
	int r;

	base = base_addr(usart);

	/* Baud rate */
	USART_BRG(base) = (u_pclk / (16 * baud) - 1) & USART_BRG_MASK;

	r = USART_CFG(base);

	/* Data bits */
	if (databits == 9) {
		r |= USART_CFG_DATALEN1;
		r &= ~USART_CFG_DATALEN0;
	} else if (databits == 8) {
		r &= ~USART_CFG_DATALEN1;
		r |= USART_CFG_DATALEN0;
	} else {
		r &= ~(USART_CFG_DATALEN1 | USART_CFG_DATALEN0);
	}

	/* Stop bits */
	if (stopbits != 1)
		r |= USART_CFG_STOPLEN;
	else
		r &= ~USART_CFG_STOPLEN;

	/* Parity */
	if (parity == USART_EVEN) {
		r |= USART_CFG_PARITYSEL1;
		r &= ~USART_CFG_PARITYSEL0;
	} else if (parity == USART_ODD) {
		r |= USART_CFG_PARITYSEL1 | USART_CFG_PARITYSEL0;
	} else {
		r &= ~(USART_CFG_PARITYSEL1 | USART_CFG_PARITYSEL0);
	}

	/* Flow control */
	if (flowcontrol != USART_FLOW_NONE)
		r |= USART_CFG_CTSEN;
	else
		r &= ~USART_CFG_CTSEN;

	/* Enable USART */
	r |= USART_CFG_ENABLE;

	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_enable(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CFG(base);
	r |= USART_CFG_ENABLE;
	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_disable(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CFG(base);
	r &= ~USART_CFG_ENABLE;
	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_enable_sync_mode(enum usart usart, bool master, bool rising_edge)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CFG(base);
	r |= USART_CFG_SYNCEN | (master ? USART_CFG_SYNCMST : 0) |
		(rising_edge ? USART_CFG_CLKPOL : 0);
	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_disable_sync_mode(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CFG(base);
	r &= ~USART_CFG_SYNCEN;
	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_send(enum usart usart, int data)
{
	USART_TXDAT(base_addr(usart)) = data & USART_TXDAT_MASK;
}

int usart_recv(enum usart usart)
{
	return USART_RXDAT(base_addr(usart)) & USART_RXDAT_MASK;
}

void usart_send_blocking(enum usart usart, int data)
{
	int base;

	base = base_addr(usart);
	while (!(USART_STAT(base) & USART_STAT_TXRDY))
		;
	USART_TXDAT(base) = data & USART_TXDAT_MASK;
}

int usart_recv_blocking(enum usart usart)
{
	int base;

	base = base_addr(usart);
	while (!(USART_STAT(base) & USART_STAT_RXRDY))
		;
	return USART_RXDAT(base) & USART_RXDAT_MASK;
}

void usart_enable_loopback(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CFG(base);
	r |= USART_CFG_LOOP;
	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_disable_loopback(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CFG(base);
	r &= ~USART_CFG_LOOP;
	USART_CFG(base) = r & USART_CFG_MASK;
}

void usart_enable_break(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CTL(base);
	r |= USART_CTL_TXBRKEN;
	USART_CTL(base) = r & USART_CTL_MASK;
}

void usart_disable_break(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CTL(base);
	r &= ~USART_CTL_TXBRKEN;
	USART_CTL(base) = r & USART_CTL_MASK;
}

void usart_enable_address_detect_mode(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CTL(base);
	r |= USART_CTL_ADDRDET;
	USART_CTL(base) = r & USART_CTL_MASK;
}

void usart_disable_address_detect_mode(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CTL(base);
	r &= ~USART_CTL_ADDRDET;
	USART_CTL(base) = r & USART_CTL_MASK;
}

void usart_disable_tx(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CTL(base);
	r |= USART_CTL_TXDIS;
	USART_CTL(base) = r & USART_CTL_MASK;
}

void usart_enable_tx(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CTL(base);
	r &= ~USART_CTL_TXDIS;
	USART_CTL(base) = r & USART_CTL_MASK;
}

void usart_enable_continuous_clock(enum usart usart, bool auto_clear)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CTL(base);
	r |= USART_CTL_CC | (auto_clear ? USART_CTL_CLRCC : 0);
	USART_CTL(base) = r & USART_CTL_MASK;
}

void usart_disable_continuous_clock(enum usart usart)
{
	int base;
	int r;

	base = base_addr(usart);
	r = USART_CTL(base);
	r &= ~USART_CTL_CC;
	USART_CTL(base) = r & USART_CTL_MASK;
}

void usart_enable_interrupt(enum usart usart, int interrupt)
{
	USART_INTENSET(base_addr(usart)) = interrupt & USART_INTENSET_MASK;
}

void usart_disable_interrupt(enum usart usart, int interrupt)
{
	USART_INTENCLR(base_addr(usart)) = interrupt & USART_INTENCLR_MASK;
}

int usart_get_interrupt_mask(enum usart usart, int interrupt)
{
	return USART_INTENSET(base_addr(usart)) & interrupt;
}

int usart_get_interrupt_status(enum usart usart, int interrupt)
{
	return USART_INTSTAT(base_addr(usart)) & interrupt;
}

void usart_clear_interrupt(enum usart usart, int interrupt)
{
	USART_STAT(base_addr(usart)) = interrupt & USART_STAT_MASK;
}
