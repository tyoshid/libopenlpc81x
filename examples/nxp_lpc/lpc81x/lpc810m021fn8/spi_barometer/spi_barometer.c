/*
 * spi_barometer - Read a barometer.
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
#include <nvic.h>
#include <spi.h>

/*
 * Freescale MPL115A1
 * Miniature SPI Digital Barometer
 *
 * Ofmax	8MHz		Operating Frequency
 * tSCSmin	125nsec		Enable lead time
 * tHCSmin	125nsec		Enable lag time
 * tWCSmin	 30nsec		Width CS High
 */
#define START_CONVERSIONS	0x24
#define READ_PRESSURE_MSB	0x80
#define READ_PRESSURE_LSB	0x82
#define READ_TEMPERATURE_MSB	0x84
#define READ_TEMPERATURE_LSB	0x86
#define READ_A0_MSB		0x88
#define READ_A0_LSB		0x8a
#define READ_B1_MSB		0x8c
#define READ_B1_LSB		0x8e
#define READ_B2_MSB		0x90
#define READ_B2_LSB		0x92
#define READ_C12_MSB		0x94
#define READ_C12_LSB		0x96
#define TC			3	/* Conversion Time: 3 msec */
#define TW			5	/* Wakeup Time: 5 msec */

/* USART clock frequency */
#define U_PCLK	(16 * 115200)

volatile int sndbuflen;
volatile const int *sndbuf;
volatile int rcvbuflen;
volatile int *rcvbuf;

volatile unsigned int unexpected_rxrdy;

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

	/* Enable SPI0 interrupt. */
	nvic_enable_irq(NVIC_SPI0);

	/* Enable SPI0. */
	/* 24MHz / 3 = 8MHz */
	spi_init_master(SPI0, SPI_MODE0, 3, 0, 0, 0, 0);

	/* Enable RXRDY interrupt. */
	spi_enable_interrupt(SPI0, SPI_RXRDY);
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

void spi0_isr(void)
{
	int m;
	int s;

	/* Get interrupt mask. */
	m = spi_get_interrupt_mask(SPI0, SPI_TXRDY | SPI_RXRDY);

	/* Get interrupt status. */
	s = spi_get_interrupt_status(SPI0, SPI_TXRDY | SPI_RXRDY);

	/* TXRDY */
	if (m & s & SPI_TXRDY) {
		if (sndbuflen) {
			/* Send data. */
			spi_send_control(SPI0, *sndbuf, 8, *sndbuf);
			sndbuf++;
			sndbuflen--;
		} else {
			/* Disable TXRDY interrupt. */
			spi_disable_interrupt(SPI0, SPI_TXRDY);
		}
	}

	/* RXRDY */
	if (m & s & SPI_RXRDY) {
		if (rcvbuflen) {
			/* Receive data. */
			*rcvbuf++ = spi_recv(SPI0);
			rcvbuflen--;
		} else {
			/* Discard data. */
			spi_recv(SPI0);
			unexpected_rxrdy++;
		}
	}
}

/* Calculate the compensated pressure PComp value */
static short calc_pcomp(unsigned short padc, unsigned short tadc,
			short a0, short b1, short b2, short c12)
{
	int c12x2;
	int a1;
	int a1x1;
	int y1;
	int a2x2;
	int pcomp;

	/* PComp = a0 + (b1 + c12 * Tadc) * Padc + b2 * Tadc */

	/*
	 * c12 Signed, Integer Bits = 0, Fractional Bits = 13,
	 * dec pt zero pad = 9
	 */
	/* c12x2 = c12 * Tadc */
	c12x2 = (c12 * tadc) >> 11;

	/* b1 Signed, Integer Bits = 2, Fractional Bits = 13 */
	/* a1 = b1 + c12x2 */
	a1 = b1 + c12x2;

	/* a1x1 = a1 * Padc */
	a1x1 = a1 * padc;

	/* a0 Signed, Integer Bits = 12, Fractional Bits = 3 */
	/* y1 = a0 + a1x1 */
	y1 = (a0 << 10) + a1x1;

	/* b2 Signed, Integer Bits = 1, Fractional Bits = 14 */
	/* a2x2 = b2 * Tadc */
	a2x2 = (b2 * tadc) >> 1;

	/* Pcomp = y1 + a2x2 */
	/* Fractional Bits = 4 */
	pcomp = (y1 + a2x2) >> 9;

	return pcomp;
}

static void print_pressure(int pressure)
{
	int f;
	int i;
	static const char frac[16] = {0, 1, 1, 2, 3, 3, 4, 4,
				      5, 6, 6, 7, 8, 8, 9, 9};

	f = pressure & 0xf;
	i = pressure >> 4;
	xprintf("%d.%d kPa\r\n", i, frac[f], 0);
}

int main(void)
{
	int buf[8];
	unsigned short padc;
	unsigned short tadc;
	short a0;
	short b1;
	short b2;
	short c12;
	short pcomp;
	int pressure;
	static const int coeff[] = {
		READ_A0_MSB | SPI_RXIGNORE, 0,
		READ_A0_LSB | SPI_RXIGNORE, 0,
		READ_B1_MSB | SPI_RXIGNORE, 0,
		READ_B1_LSB | SPI_RXIGNORE, 0,
		READ_B2_MSB | SPI_RXIGNORE, 0,
		READ_B2_LSB | SPI_RXIGNORE, 0,
		READ_C12_MSB | SPI_RXIGNORE, 0,
		READ_C12_LSB | SPI_RXIGNORE, 0,
		0 | SPI_RXIGNORE | SPI_EOT
	};
	static const int conv[] = {
		START_CONVERSIONS | SPI_RXIGNORE,
		0 | SPI_RXIGNORE | SPI_EOT
	};
	static const int press[] = {
		READ_PRESSURE_MSB | SPI_RXIGNORE, 0,
		READ_PRESSURE_LSB | SPI_RXIGNORE, 0,
		READ_TEMPERATURE_MSB | SPI_RXIGNORE, 0,
		READ_TEMPERATURE_LSB | SPI_RXIGNORE, 0,
		0 | SPI_RXIGNORE | SPI_EOT
	};

	clock_setup();
	gpio_setup();
	usart_setup();
	mrt_setup();
	spi_setup();

	xputs("spi_barometer");

	/* Wakeup Time */
	delay_ms(TW);

	/* Read Coefficients. */
	sndbuf = coeff;
	sndbuflen = 17;
	rcvbuf = buf;
	rcvbuflen = 8;
	spi_enable_interrupt(SPI0, SPI_TXRDY);
	while (rcvbuflen || sndbuflen)
		;

	xprintf("a0=0x%x%x ", buf[0], buf[1], 0);
	xprintf("b1=0x%x%x ", buf[2], buf[3], 0);
	xprintf("b2=0x%x%x ", buf[4], buf[5], 0);
	xprintf("c12=0x%x%x\r\n", buf[6], buf[7], 0);

	a0 = ((buf[0] & 0xff) << 8) | (buf[1] & 0xff);
	b1 = ((buf[2] & 0xff) << 8) | (buf[3] & 0xff);
	b2 = ((buf[4] & 0xff) << 8) | (buf[5] & 0xff);
	c12 = ((buf[6] & 0xff) << 8) | (buf[7] & 0xff);

	while (1) {
		/* Start Pressure and Temperature Conversion. */
		sndbuf = conv;
		sndbuflen = 2;
		rcvbuflen = 0;
		spi_enable_interrupt(SPI0, SPI_TXRDY);
		while (sndbuflen)
			;

		/* Conversion Time. */
		delay_ms(TC);

		/* Read raw Pressure. */
		sndbuf = press;
		sndbuflen = 9;
		rcvbuf = buf;
		rcvbuflen = 4;
		spi_enable_interrupt(SPI0, SPI_TXRDY);
		while (rcvbuflen || sndbuflen)
			;

		padc = ((buf[0] & 0xff) << 8) | (buf[1] & 0xff);
		padc >>= 6;
		tadc = ((buf[2] & 0xff) << 8) | (buf[3] & 0xff);
		tadc >>= 6;

		xprintf("padc=%d tadc=%d\r\n", padc, tadc, 0);

		/* compensated pressure */
		pcomp = calc_pcomp(padc, tadc, a0, b1, b2, c12);

		/* Pressure = PComp * ((115.0 - 50.0) / 1023.0) + 50 */
		/* Pressure = ((PComp * 1041) >> 14) + 50 */
		/* Fractional Bits = 4 */
		pressure = ((pcomp * 1041) >> 14) + 800;

		/* Print the result. */
		print_pressure(pressure);

		/* Wait 1 sec. */
		delay_ms(1000);
	}

	return 0;
}
