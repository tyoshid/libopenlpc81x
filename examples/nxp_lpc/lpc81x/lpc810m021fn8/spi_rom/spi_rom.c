/*
 * spi_rom - Write/read a serial EEPROM.
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
#include <mrt.h>
#include <spi.h>

/*
 * Microchip 25LC640A
 * 64Kbit SPI Serial EEPROM
 *
 * Vcc = 3.3V
 * FCLKmax	5MHz 		Clock frequency
 * TCSSmin	100nsec		CS setup time
 * TCSHmin	200nsec		CS hold time
 * TCSDmin	 50nsec		CS disable time
 */
#define ROM_SIZE	8192	/* 8KByte */
#define PAGE_SIZE	32	/* 32Byte */
#define COMMAND_READ	0x03
#define COMMAND_WRITE	0x02
#define COMMAND_WRDI	0x04
#define COMMAND_WREN	0x06
#define COMMAND_RDSR	0x05
#define COMMAND_WRSR	0x01
#define TWC		5	/* Internal write cycle time: 5 msec */

/* USART clock frequency */
#define U_PCLK	(16 * 115200)

/* Set LPC810 to 24 MHz. */
static void clock_setup(void)
{
	/* Set up PLL (Fclkin = 12MHz (IRC), Fclkout (Main clock) = 24MHz) */
	syscon_enable_pll(SYSCON_IRC, 2, 4);

	/* Select PLL as system clock source. */
	syscon_set_system_clock(SYSCON_PLL_OUT, 1);

	/* Enable USART clock (U_PCLK). */
	syscon_set_usart_clock(24000000, U_PCLK);
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

	/* Set PIO0_2 to 'SPI0 SCK'. */
	gpio_config(GPIO_SPI0_SCK, GPIO_HYST, PIO0_2);

	/* Set PIO0_3 to 'SPIO MOSI'. */
	gpio_config(GPIO_SPI0_MOSI, GPIO_HYST, PIO0_3);

	/* Set PIO0_5 to 'SPIO MISO'. */
	gpio_config(GPIO_SPI0_MISO, GPIO_HYST, PIO0_5);

	/* Set PIO0_0 to 'SPIO SSEL'. */
	gpio_config(GPIO_SPI0_SSEL, GPIO_HYST, PIO0_0);

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

static void mrt_setup(void)
{
	/* Enable MRT clock. */
	syscon_enable_clock(SYSCON_MRT);

	/* Set channel 0 to 'one-shot interrupt mode'. */
	mrt_set_mode(MRT0, MRT_ONE_SHOT);
}

static void spi_setup(void)
{
	/* Enable SPI0 clock. */
	syscon_enable_clock(SYSCON_SPI0);

	/* Enable SPI0. */
	/* 24MHz / 6 = 4MHz */
	spi_init_master(SPI0, SPI_MODE0, 6, 0, 0, 0, 0);
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

static void delay_ms(int ms)
{
	mrt_set_interval(MRT0, 24000000 / 1000 * ms);

	while (!mrt_get_channel_status(MRT0, MRT_INT))
		;

	mrt_clear_channel_status(MRT0, MRT_INT);
}

/* Read */
static void rom_read(int addr, char *data, int nbyte)
{
	int i;

	/* Instruction */
	spi_send_control_blocking(SPI0, COMMAND_READ, 8, SPI_RXIGNORE);

	/* Address */
       	spi_send_control_blocking(SPI0, addr, 16, SPI_RXIGNORE);

	/* Data */
	spi_set_tx_control(SPI0, 8, 0);
	for (i = 0; i < nbyte; i++) {
		if (i == nbyte - 1)
			spi_set_tx_control(SPI0, 8, SPI_EOT);
		spi_send_blocking(SPI0, 0);
		*data++ = spi_recv_blocking(SPI0);
	}
}

/* Byte Write */
static void rom_write_byte(int addr, int data)
{
	/* Write Enable */
	spi_send_control_blocking(SPI0, COMMAND_WREN, 8,
				  SPI_RXIGNORE | SPI_EOT);

	/* Instruction */
	spi_send_control_blocking(SPI0, COMMAND_WRITE, 8, SPI_RXIGNORE);

	/* Address */
       	spi_send_control_blocking(SPI0, addr, 16, SPI_RXIGNORE);

	/* Data */
	spi_send_control_blocking(SPI0, data, 8, SPI_RXIGNORE | SPI_EOT);
}

/* Page Write */
static void rom_write_page(int addr, char *data)
{
	int i;

	/* Write Enable */
	spi_send_control_blocking(SPI0, COMMAND_WREN, 8,
				  SPI_RXIGNORE | SPI_EOT);

	/* Instruction */
	spi_send_control_blocking(SPI0, COMMAND_WRITE, 8, SPI_RXIGNORE);

	/* Address */
       	spi_send_control_blocking(SPI0, addr, 16, SPI_RXIGNORE);

	/* Data */
	spi_set_tx_control(SPI0, 8, SPI_RXIGNORE);
	for (i = 0; i < PAGE_SIZE; i++) {
		if (i == PAGE_SIZE - 1)
			spi_set_tx_control(SPI0, 8, SPI_RXIGNORE | SPI_EOT);
		spi_send_blocking(SPI0, *data++);
	}
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
	mrt_setup();
	spi_setup();

	xputs("spi_rom");

	xputs("Read 1 byte");
	rom_read(0, &start_data, 1);
	start_data++;
	xprintf("data = 0x%x\r\n", start_data, 0, 0);

	/* Write data. */
	addr = 0;
	data = start_data;
	/* Byte */
	xputs("Byte Write");
	for (i = 0; i < PAGE_SIZE; i++) {
		rom_write_byte(addr++, data++);

		/* internal write cycle time */
		delay_ms(TWC);
	}

	/* Page */
	xputs("Page Write");
	for (i = 0; i < ROM_SIZE / PAGE_SIZE - 1; i++) {
		for (j = 0; j < PAGE_SIZE; j++)
			buf[j] = data++;
		rom_write_page(addr, buf);
		addr += PAGE_SIZE;

		/* internal write cycle time */
		delay_ms(TWC);
	}

	/* Read and verify data. */
	xputs("Sequential Read");
	while (1) {
		for (n = 1; n < (int)sizeof(buf); n++) {
			xprintf("size = %d\r\n", n, 0, 0);

			addr = 0;
			data = start_data;
			for (i = 0; i < ROM_SIZE / n; i++) {
				rom_read(addr, buf, n);
				for (j = 0; j < n; j++) {
					if (buf[j] != data)
						goto data_error;
					data++;
				}
				addr += n;
			}

			m = ROM_SIZE % n;
			if (m) {
				rom_read(addr, buf, m);
				for (j = 0; j < m; j++) {
					if (buf[j] != data)
						goto data_error;
					data++;
				}
			}
		}
	}

data_error:
	xprintf("data error: addr = 0x%h  data = 0x%x (0x%x)\r\n",
		addr + j, buf[j], data);
	while (1)
		;

	return 0;
}
