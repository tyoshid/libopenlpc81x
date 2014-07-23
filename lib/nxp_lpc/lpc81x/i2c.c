/*
 * I2C functions
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

#include <i2c.h>

void i2c_set_scl(int div, int high, int low)
{
	I2C_CLKDIV = (div - 1) & 0xffff;
	I2C_MSTTIME = ((high - 2) & 7) << 4 | ((low - 2) & 7);
}

void i2c_set_time_out(int to)
{
	I2C_TIMEOUT = (to - 1) & 0xffff;
}

void i2c_enable_master(bool timeout)
{
	int r;

	r = I2C_CFG;
	if (timeout)
		r |= I2C_CFG_TIMEOUTEN;
	else
		r &= ~I2C_CFG_TIMEOUTEN;
	I2C_CFG = r | I2C_CFG_MSTEN;
}

void i2c_disable_master(void)
{
	I2C_CFG &= ~I2C_CFG_MSTEN;
}

void i2c_start(void)
{
	I2C_MSTCTL = I2C_MSTCTL_MSTSTART;
}

void i2c_stop(void)
{
	I2C_MSTCTL = I2C_MSTCTL_MSTSTOP;
}

void i2c_master_continue(void)
{
	I2C_MSTCTL = I2C_MSTCTL_MSTCONTINUE;
}

void i2c_master_send_addr(int addr)
{
	I2C_MSTDAT = addr & 0xff;
	I2C_MSTCTL = I2C_MSTCTL_MSTSTART;
}

void i2c_master_send(int data)
{
	I2C_MSTDAT = data & 0xff;
	I2C_MSTCTL = I2C_MSTCTL_MSTCONTINUE;
}

int i2c_master_recv(void)
{
	int r;

	r = I2C_MSTDAT;
	I2C_MSTCTL = I2C_MSTCTL_MSTCONTINUE;
	return r & 0xff;
}

int i2c_master_recv_last(void)
{
	return I2C_MSTDAT & 0xff;
}

void i2c_enable_slave(bool timeout)
{
	int r;

	r = I2C_CFG;
	if (timeout)
		r |= I2C_CFG_TIMEOUTEN;
	else
		r &= ~I2C_CFG_TIMEOUTEN;
	I2C_CFG = r | I2C_CFG_SLVEN;
}

void i2c_disable_slave(void)
{
	I2C_CFG &= ~I2C_CFG_SLVEN;
}

void i2c_nack(void)
{
	I2C_SLVCTL = I2C_SLVCTL_SLVNACK;
}

void i2c_slave_continue(void)
{
	I2C_SLVCTL = I2C_SLVCTL_SLVCONTINUE;
}

void i2c_slave_send(int data)
{
	I2C_SLVDAT = data & 0xff;
	I2C_SLVCTL = I2C_SLVCTL_SLVCONTINUE;
}

int i2c_slave_recv(void)
{
	return I2C_SLVDAT & 0xff;
}

void i2c_set_slave_address(int num, int addr)
{
	I2C_SLVADR(num) = addr & 0xff;
}

void i2c_set_slave_address_qualifier0(int qual)
{
	I2C_SLVQUAL0 = qual & 0xff;
}

void i2c_enable_monitor(bool timeout, bool clkstr)
{
	int r;

	r = I2C_CFG;
	if (timeout)
		r |= I2C_CFG_TIMEOUTEN;
	else
		r &= ~I2C_CFG_TIMEOUTEN;
	if (clkstr)
		r |= I2C_CFG_MONCLKSTR;
	else
		r &= ~I2C_CFG_MONCLKSTR;
	I2C_CFG = r | I2C_CFG_MONEN;
}

void i2c_disable_monitor(void)
{
	I2C_CFG &= ~I2C_CFG_MONEN;
}

int i2c_get_monitor_data(void)
{
	return I2C_MONRXDAT & 0x7ff;
}

int i2c_get_status(int status)
{
	return I2C_STAT & status;
}

void i2c_clear_status(int status)
{
	I2C_STAT = status & (I2C_STAT_SCLTIMEOUT | I2C_STAT_EVENTTIMEOUT |
			     I2C_STAT_MONIDLE | I2C_STAT_MONOV |
			     I2C_STAT_SLVDESEL | I2C_STAT_MSTSTSTPERR |
			     I2C_STAT_MSTARBLOSS);
}

void i2c_enable_interrupt(int interrupt)
{
	I2C_INTENSET = interrupt & 0x30b8951;

}

void i2c_disable_interrupt(int interrupt)
{
	I2C_INTENCLR = interrupt & 0x30b8951;

}

int i2c_get_interrupt_mask(int interrupt)
{
	return I2C_INTENSET & interrupt;
}

int i2c_get_interrupt_status(int interrupt)
{
	return I2C_INTSTAT & interrupt;
}
