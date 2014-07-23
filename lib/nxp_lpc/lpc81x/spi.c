/*
 * SPI functions
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

#include <spi.h>

static int base_addr(spi_t spi)
{
	switch (spi) {
	case SPI0:
		return SPI0_BASE;
	case SPI1:
		return SPI1_BASE;
	default:
		break;
	}
	return 0;
}

void spi_init_master(spi_t spi, int config, int clkdiv, int pre_delay,
		     int post_delay, int frame_delay, int transfer_delay)
{
	int base;

	base = base_addr(spi);
	SPI_DIV(base) = (clkdiv - 1) & 0xffff;
	SPI_DLY(base) = (pre_delay & 0xf) | ((post_delay & 0xf) << 4) |
		((frame_delay & 0xf) << 8) | ((transfer_delay & 0xf) << 12);
	SPI_CFG(base) = (config & 0x1b8) | SPI_CFG_MASTER | SPI_CFG_ENABLE;
}

void spi_init_slave(spi_t spi, int config)
{
	int base;

	base = base_addr(spi);
	SPI_CFG(base) = (config & 0x1b8) | SPI_CFG_ENABLE;
}

void spi_enable(spi_t spi)
{
	int base;
	int r;

	base = base_addr(spi);
	r = SPI_CFG(base);
	SPI_CFG(base) = (r & 0x1bc) | SPI_CFG_ENABLE;
}

void spi_disable(spi_t spi)
{
	int base;
	int r;

	base = base_addr(spi);
	r = SPI_CFG(base);
	SPI_CFG(base) = r & 0x1bc;
}

void spi_enable_loop_back(spi_t spi)
{
	int base;
	int r;

	base = base_addr(spi);
	r = SPI_CFG(base);
	SPI_CFG(base) = (r & 0x13d) | SPI_CFG_LOOP;
}

void spi_disable_loop_back(spi_t spi)
{
	int base;
	int r;

	base = base_addr(spi);
	r = SPI_CFG(base);
	SPI_CFG(base) = r & 0x13d;
}

void spi_send(spi_t spi, int data)
{
	SPI_TXDAT(base_addr(spi)) = data & 0xffff;
}

void spi_set_tx_control(spi_t spi, int len, int control)
{
	SPI_TXCTL(base_addr(spi)) = (((len - 1) & 0xf) << 24) |
		(control & 0x710000);
}

void spi_send_control(spi_t spi, int data, int len, int control)
{
	SPI_TXDATCTL(base_addr(spi)) = (((len - 1) & 0xf) << 24) |
		(control & 0x710000) | (data & 0xffff);
}

int spi_recv(spi_t spi)
{
	return SPI_RXDAT(base_addr(spi)) & 0x11ffff;
}

void spi_send_blocking(spi_t spi, int data)
{
	int base;

	base = base_addr(spi);
	while (!(SPI_STAT(base) & SPI_STAT_TXRDY))
		;
	SPI_TXDAT(base) = data & 0xffff;
}

void spi_send_control_blocking(spi_t spi, int data, int len, int control)
{
	int base;

	base = base_addr(spi);
	while (!(SPI_STAT(base) & SPI_STAT_TXRDY))
		;
	SPI_TXDATCTL(base) = (((len - 1) & 0xf) << 24) |
		(control & 0x710000) | (data & 0xffff);
}

int spi_recv_blocking(spi_t spi)
{
	int base;

	base = base_addr(spi);
	while (!(SPI_STAT(base) & SPI_STAT_RXRDY))
		;
	return SPI_RXDAT(base) & 0x11ffff;
}

void spi_enable_interrupt(spi_t spi, int interrupt)
{
	SPI_INTENSET(base_addr(spi)) = interrupt & 0x3f;
}

void spi_disable_interrupt(spi_t spi, int interrupt)
{
	SPI_INTENCLR(base_addr(spi)) = interrupt & 0x3f;
}

int spi_get_interrupt_mask(spi_t spi, int interrupt)
{
	return 	SPI_INTENSET(base_addr(spi)) & interrupt;
}

int spi_get_interrupt_status(spi_t spi, int interrupt)
{
	return 	SPI_INTSTAT(base_addr(spi)) & interrupt;
}

int spi_get_status(spi_t spi, int status)
{
	return 	SPI_STAT(base_addr(spi)) & status;
}

void spi_clear_status(spi_t spi, int status)
{
	SPI_STAT(base_addr(spi)) = status & 0x1ff;
}
