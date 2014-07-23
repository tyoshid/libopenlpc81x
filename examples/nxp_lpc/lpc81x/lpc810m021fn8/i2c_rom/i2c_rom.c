/*
 * i2c_rom - Write/read a serial EEPROM.
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
#include <usart.h>
#include <i2c.h>

/*
 * Microchip 24FC64
 * 64Kbit I2C Serial EEPROM
 */
#define ROM_SIZE	8192	/* 8KByte */
#define PAGE_SIZE	32	/* 32Byte */
#define ROM_SLA		0xa0	/* |1|0|1|0|A2|A1|A0|R/W| */

#define FASTMODE

/* USART clock frequency */
#define U_PCLK	(16 * 115200)

int status;
int exp_state;

/* Set LPC810 to 24 MHz. */
static void clock_setup(void)
{
	/* Set up PLL (Fclkin = 12MHz (IRC), Fclkout (Main clock) = 24MHz) */
	syscon_enable_pll(SYSCON_IRC, 2, 4);

	/* Select PLL as system clock source. */
	syscon_set_system_clock(SYSCON_PLL_OUT, 1);

	/* Enable USART clock (U_PCLK). */
	syscon_set_usart_clock(24000000, U_PCLK);

#ifdef FASTMODE
	/* Set IOCON glitch filter clock divider. */
	syscon_set_ioconclkdiv(0, 1);
#endif
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

#ifndef FASTMODE
	/* Set PIO0_2 to 'I2C SCL'. */
	gpio_config(GPIO_I2C_SCL, GPIO_OPENDRAIN | GPIO_HYST, PIO0_2);

	/* Set PIO0_3 to 'I2C SDA'. */
	gpio_config(GPIO_I2C_SDA, GPIO_OPENDRAIN | GPIO_HYST, PIO0_3);
#else
	/* Set PIO0_2 to 'I2C SCL'. */
	gpio_config(GPIO_I2C_SCL, GPIO_OPENDRAIN | GPIO_HYST |
		    GPIO_IOCONCLKDIV0 | GPIO_FILTER_2CLOCK, PIO0_2);

	/* Set PIO0_3 to 'I2C SDA'. */
	gpio_config(GPIO_I2C_SDA, GPIO_OPENDRAIN | GPIO_HYST |
		    GPIO_IOCONCLKDIV0 | GPIO_FILTER_2CLOCK, PIO0_3);
#endif

	/* Set PIO0_4 to 'USART0 TXD'. */
	gpio_config(GPIO_U0_TXD, GPIO_HYST, PIO0_4);
}

static void usart_setup(void)
{
	/* Enable USART0 clock. */
	syscon_enable_clock(SYSCON_UART0);

	/* Set up USART0. */
	usart_init(USART0, U_PCLK, 115200, 8, 1, USART_PARITY_NONE,
		   USART_FLOW_NONE);
}

static void i2c_setup(void)
{
	/* Enable I2C clock. */
	syscon_enable_clock(SYSCON_I2C);

#ifndef FASTMODE
	/* Set fSCL to 100kHz (tHIGH = 5 usec, tLOW = 5 usec). */
	/* Standard-mode */
	i2c_set_scl(24, 5, 5);
#else
	/* Set fSCL to 400kHz (tHIGH = 1 usec, tLOW = 1.5 usec). */
	/* Fast-mode */
	i2c_set_scl(12, 2, 3);
#endif

	/* Enable I2C master function */
	i2c_enable_master(false);
}

static void print_hex(int c)
{
	int d;

	d = (c >> 4) & 0xf;
	if (d > 9)
		d += 'a' - 10;
	else
		d += '0';
	usart_send_blocking(USART0, d);
	d = c & 0xf;
	if (d > 9)
		d += 'a' - 10;
	else
		d += '0';
	usart_send_blocking(USART0, d);
}

static void print_hex_half_word(int w)
{
	print_hex((w >> 8) & 0xff);
	print_hex(w & 0xff);
}

static void print_hex_word(int w)
{
	print_hex((w >> 24) & 0xff);
	print_hex((w >> 16) & 0xff);
	print_hex((w >> 8) & 0xff);
	print_hex(w & 0xff);
}

static void print_int(int d)
{
	bool zerosup;
	int i;

	if (d < 0) {
		usart_send_blocking(USART0, '-');
		d = -d;
	}
	zerosup = true;
	for (i = 1000000000; i > 0; i /= 10) {
		if (d / i || i == 1 || !zerosup) {
			usart_send_blocking(USART0, d / i + '0');
			d %= i;
			zerosup = false;
		}
	}
}

static void xprintf(char *p, int arg0, int arg1, int arg2)
{
	int arg[3];
	int i;

	arg[0] = arg0;
	arg[1] = arg1;
	arg[2] = arg2;
	i = 0;
	while (*p != '\0') {
		if (*p == '%') {
			switch (*++p) {
			case '%':
				usart_send_blocking(USART0, *p++);
				break;
			case 'x':
				if (i < 3)
					print_hex(arg[i++]);
				p++;
				break;
			case 'h':
				if (i < 3)
					print_hex_half_word(arg[i++]);
				p++;
				break;
			case 'w':
				if (i < 3)
					print_hex_word(arg[i++]);
				p++;
				break;
			case 'd':
				if (i < 3)
					print_int(arg[i++]);
				p++;
				break;
			default:
				usart_send_blocking(USART0, '?');
				p++;
				break;
			}
		} else {
			usart_send_blocking(USART0, *p++);
		}
	}
}

static void xputs(char *p)
{
	while (*p != '\0')
		usart_send_blocking(USART0, *p++);
	usart_send_blocking(USART0, '\r');
	usart_send_blocking(USART0, '\n');
}

static int wait_i2c_state(int state)
{
	exp_state = state;
	do {
		status = i2c_get_status(I2C_MSTPENDING | I2C_MSTSTATE);
	} while (!(status & I2C_MSTPENDING));

	if (state != (status & I2C_MSTSTATE))
		return status & I2C_MSTSTATE;

	return 0;
}

/*
 * Byte Write 
 *
 * |S|10100000|A|xxx.....|A|........|A|........|A|P|
 *    Slave      Address    Address    Data
 *    Address    High Byte  Low Byte
 */
static int rom_write_byte(int addr, char data)
{
	int r;

	/* Idle */
	if ((r = wait_i2c_state(I2C_MST_IDLE)))
		return r;

	do {
		/* Send START + I2C Slave address. */
		i2c_master_send_addr(ROM_SLA);

		/* Receive ACK. */
		if ((r = wait_i2c_state(I2C_MST_TX_RDY))) {
			if (r != I2C_MST_NACK_ADDR)
				return r;
		}
	} while (r == I2C_MST_NACK_ADDR);

	/* Send ROM address high byte. */
	i2c_master_send(addr >> 8);

	/* Receive ACK. */
	if ((r = wait_i2c_state(I2C_MST_TX_RDY)))
		return r;

	/* Send ROM address low byte. */
	i2c_master_send(addr & 0xff);

	/* Receive ACK. */
	if ((r = wait_i2c_state(I2C_MST_TX_RDY)))
		return r;

	/* Send data. */
	i2c_master_send(data);

	/* Receive ACK. */
	if ((r = wait_i2c_state(I2C_MST_TX_RDY)))
		return r;

	/* Send STOP. */
	i2c_stop();

	/* Idle */
	if ((r = wait_i2c_state(I2C_MST_IDLE)))
		return r;

	return 0;
}

/*
 * Page Write 
 *
 * |S|10100000|A|xxx.....|A|........|A|........|A|~|........|A|P|
 *    Slave      Address    Address    Data         Data
 *    Address    High Byte  Low Byte   Byte 0       Byte 31
 */
static int rom_write_page(int addr, char *data)
{
	int r;
	int i;

	/* Idle */
	if ((r = wait_i2c_state(I2C_MST_IDLE)))
		return r;

	do {
		/* Send START + I2C Slave address. */
		i2c_master_send_addr(ROM_SLA);

		/* Receive ACK. */
		if ((r = wait_i2c_state(I2C_MST_TX_RDY))) {
			if (r != I2C_MST_NACK_ADDR)
				return r;
		}
	} while (r == I2C_MST_NACK_ADDR);

	/* Send ROM address high byte. */
	i2c_master_send(addr >> 8);

	/* Receive ACK. */
	if ((r = wait_i2c_state(I2C_MST_TX_RDY)))
		return r;

	/* Send ROM address low byte. */
	i2c_master_send(addr & 0xff);

	for (i = 0; i < PAGE_SIZE; i++) {
		/* Receive ACK. */
		if ((r = wait_i2c_state(I2C_MST_TX_RDY)))
			return r;

		/* Send data. */
		i2c_master_send(*data++);
	}

	/* Receive ACK. */
	if ((r = wait_i2c_state(I2C_MST_TX_RDY)))
		return r;

	/* Send STOP. */
	i2c_stop();

	/* Idle */
	if ((r = wait_i2c_state(I2C_MST_IDLE)))
		return r;

	return 0;
}

/*
 * Random Read / Sequential Read
 *
 * |S|10100000|A|xxx.....|A|........|A|S|10100001|A|........|A|~|........|N|P|
 *    Slave      Address    Address      Slave      Data n       Data n + x
 *    Address    High Byte  Low Byte     Address
 */
static int rom_read(int addr, char *data, int nbyte)
{
	int r;
	int i;

	/* Idle */
	if ((r = wait_i2c_state(I2C_MST_IDLE)))
		return r;

	do {
		/* Send START + I2C Slave address (Write). */
		i2c_master_send_addr(ROM_SLA);

		/* Receive ACK. */
		if ((r = wait_i2c_state(I2C_MST_TX_RDY))) {
			if (r != I2C_MST_NACK_ADDR)
				return r;
		}
	} while (r == I2C_MST_NACK_ADDR);

	/* Send ROM address high byte. */
	i2c_master_send(addr >> 8);

	/* Receive ACK. */
	if ((r = wait_i2c_state(I2C_MST_TX_RDY)))
		return r;

	/* Send ROM address low byte. */
	i2c_master_send(addr & 0xff);

	/* Receive ACK. */
	if ((r = wait_i2c_state(I2C_MST_TX_RDY)))
		return r;

	/* Send START + I2C Slave address (Read). */
	i2c_master_send_addr(ROM_SLA | 1);

	for (i = 0; i < nbyte - 1; i++) {
		/* Receive data. */
		if ((r = wait_i2c_state(I2C_MST_RX_RDY)))
			return r;

		/* Get data and send ACK. */
		*data++ = i2c_master_recv();
	}

	/* Receive data. */
	if ((r = wait_i2c_state(I2C_MST_RX_RDY)))
		return r;

	/* Get data. */
	*data = i2c_master_recv_last();

	/* Send NACK + STOP. */
	i2c_stop();

	/* Idle */
	if ((r = wait_i2c_state(I2C_MST_IDLE)))
		return r;

	return 0;
}

int main(void)
{
	char start_data;
	int addr;
	char data;
	int i;
	int j;
	char buf[256];
	int n;
	int m;

	clock_setup();
	gpio_setup();
	usart_setup();
	i2c_setup();

	xputs("i2c_rom");

	xputs("Random read");
	if (rom_read(0, &start_data, 1)) {
		xputs("ROM read error");
		goto i2c_error;
	}
	start_data++;
	xprintf("data = 0x%x\r\n", start_data, 0, 0);

	/* Write data. */
	addr = 0;
	data = start_data;
	/* Byte */
	xputs("Byte Write");
	for (i = 0; i < PAGE_SIZE; i++) {
		if (rom_write_byte(addr++, data++)) {
			xputs("ROM write error");
			goto i2c_error;
		}
	}
	/* Page */
	xputs("Page Write");
	for (i = 0; i < ROM_SIZE / PAGE_SIZE - 1; i++) {
		for (j = 0; j < PAGE_SIZE; j++)
			buf[j] = data++;
		if (rom_write_page(addr, buf)) {
			xputs("ROM write error");
			goto i2c_error;
		}
		addr += PAGE_SIZE;
	}

	/* Read and verify data. */
	xputs("Sequential Read");
	while (1) {
		for (n = 1; n < (int)sizeof(buf); n++) {
			xprintf("size = %d\r\n", n, 0, 0);

			addr = 0;
			data = start_data;
			for (i = 0; i < ROM_SIZE / n; i++) {
				if (rom_read(addr, buf, n)) {
					xputs("ROM read error");
					goto i2c_error;
				}
				for (j = 0; j < n; j++) {
					if (buf[j] != data)
						goto data_error;
					data++;
				}
				addr += n;
			}

			m = ROM_SIZE % n;
			if (m) {
				if (rom_read(addr, buf, m)) {
					xputs("ROM read error");
					goto i2c_error;
				}
				for (j = 0; j < m; j++) {
					if (buf[j] != data)
						goto data_error;
					data++;
				}
			}
		}
	}

i2c_error:
	xprintf("I2C error: status = 0x%w  expected state = 0x%x\r\n",
		status, exp_state, 0);
	while (1)
		;

data_error:
	xprintf("data error: addr = 0x%h  data = 0x%x (0x%x)\r\n",
		addr + j, buf[j], data);
	while (1)
		;

	return 0;
}
