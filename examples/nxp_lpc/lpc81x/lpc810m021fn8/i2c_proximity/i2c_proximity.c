/*
 * i2c_proximity - Detect objects with a proximity sensor.
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
#include <i2c.h>

/*
 * Vishay VCNL4000
 * Fully Integrated Proximity and Ambient Light Sensor with Infrared Emitter
 * and I2C Interface
 *
 * VCNL4000 Infrared Proximity Breakout
 * BOB-10901
 * SparkFun Electronics
 */
#define VCNL4000_SLA	0x26	/* |0|0|1|0|0|1|1|R/W| */

#define FASTMODE

#define PROX_THRESH	25

/* USART clock frequency */
#define U_PCLK	(16 * 115200)

volatile int state;
volatile int next_state;
volatile int sndbuflen;
volatile int *sndbuf;
volatile int rcvbuflen;
volatile int *rcvbuf;
volatile bool busy;

volatile unsigned int start_stop_error;
volatile unsigned int arbitration_loss;

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

	/* Prevent the pins from internally floating. */
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

static void mrt_setup(void)
{
	/* Enable MRT clock. */
	syscon_enable_clock(SYSCON_MRT);

	/* Set channel 0 to 'bus-stall mode'. */
	mrt_set_mode(MRT0, MRT_BUS_STALL);

	/* Set channel 1 to 'one-shot interrupt mode'. */
	mrt_set_mode(MRT1, MRT_ONE_SHOT);
}

static void i2c_setup(void)
{
	/* Enable I2C clock. */
	syscon_enable_clock(SYSCON_I2C);

	/* Enable I2C interrupt. */
	nvic_enable_irq(NVIC_I2C);

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

void i2c_isr(void)
{
	int m;
	int s;

	/* Get interrupt mask. */
	m = i2c_get_interrupt_mask(I2C_MSTSTSTPERR | I2C_MSTARBLOSS |
				   I2C_MSTPENDING);

	/* Get interrupt status. */
	s = i2c_get_interrupt_status(I2C_MSTSTSTPERR | I2C_MSTARBLOSS |
				     I2C_MSTPENDING);

	/* Master Pending */
	if (m & s & I2C_MSTPENDING) {
		/* Get Master State code. */
		state = i2c_get_status(I2C_MSTSTATE);

		/* unexpected state */
		if (state != next_state) {
			/* Disable interrupt. */
			i2c_disable_interrupt(I2C_MSTPENDING);
				
			/* Clear flag. */
			busy = false;
		}

		switch (state) {
		case I2C_MST_IDLE:
			if (sndbuflen) {
				/* Send START + I2C Slave address (W). */
				i2c_master_send_addr(VCNL4000_SLA);

				next_state = I2C_MST_TX_RDY;
			} else if (rcvbuflen) {
				/* Send START + I2C Slave address (R). */
				i2c_master_send_addr(VCNL4000_SLA | 1);

				next_state = I2C_MST_RX_RDY;
			} else {
				/* transfer end */

				/* Disable interrupt. */
				i2c_disable_interrupt(I2C_MSTPENDING);
				
				/* Clear flag. */
				busy = false;
			}
			break;
		case I2C_MST_RX_RDY:
			if (rcvbuflen > 1) {
				/* Get data and send ACK. */
				*rcvbuf++ = i2c_master_recv();
				rcvbuflen--;

				next_state = I2C_MST_RX_RDY;
			} else {
				/* Get data. */
				*rcvbuf = i2c_master_recv_last();
				rcvbuflen--;

				/* Send NACK + STOP. */
				i2c_stop();

				next_state = I2C_MST_IDLE;
			}
		case I2C_MST_TX_RDY:
			if (sndbuflen) {
				/* Send data. */
				i2c_master_send(*sndbuf++);
				sndbuflen--;

				next_state = I2C_MST_TX_RDY;
			} else {
				/* Send STOP. */
				i2c_stop();

				next_state = I2C_MST_IDLE;
			}
			break;
		default:
			break;
		}
	}

	/* Master Start/Stop Error */
	if (m & s & I2C_MSTSTSTPERR) {
		start_stop_error++;
		i2c_clear_status(I2C_MSTSTSTPERR);
	}

	/* Master Arbitration Loss */
	if (m & s & I2C_MSTARBLOSS) {
		arbitration_loss++;
		i2c_clear_status(I2C_MSTARBLOSS);
	}
}
		
/* Write command */
static int vcnl_write_command(int addr, int data)
{
	int buf[2];

	/* Write register address and data. */
	buf[0] = addr;
	buf[1] = data;
	sndbuf = buf;
	sndbuflen = 2;

	/* nothing to read */
	rcvbuflen = 0;

	/* Set flag. */
	busy = true;

	/* Idle */
	next_state = I2C_MST_IDLE;

	/* Enable interrupt. */
	i2c_enable_interrupt(I2C_MSTPENDING);

	/* Wait until the completion of the transfer. */
	while (busy)
		;

	/* Error */
	if (state != next_state)
		return -1;

	return 0;
}

/* Read word */
static int vcnl_read_word(int addr, int *data)
{
	int h;

	/* Write register address. */
	sndbuf = &addr;
	sndbuflen = 1;

	/* Read high byte. */
	rcvbuf = &h;
	rcvbuflen = 1;

	/* Set flag. */
	busy = true;

	/* Idle */
	next_state = I2C_MST_IDLE;

	/* Enable interrupt. */
	i2c_enable_interrupt(I2C_MSTPENDING);

	/* Wait until the completion of the transfer. */
	while (busy)
		;

	/* Error */
	if (state != next_state)
		return -1;

	/* nothing to write */
	sndbuflen = 0;

	/* Read low byte. */
	rcvbuf = data;
	rcvbuflen = 1;

	/* Set flag. */
	busy = true;

	/* Enable interrupt. */
	i2c_enable_interrupt(I2C_MSTPENDING);

	/* Wait until the completion of the transfer. */
	while (busy)
		;

	/* Error */
	if (state != next_state)
		return -1;

	/* 16bit data */
	*data |= h << 8;

	return 0;
}

int main(void)
{
	int d;
	int i;
	int prox_min;
	int prox_max;
	int total;
	int prox_off;
	int n;
	int x;
	int p;

	clock_setup();
	gpio_setup();
	usart_setup();
	mrt_setup();
	i2c_setup();

	xputs("i2c_proximity");

	/* Power Up */
	delay_us(400);

	/* Initialize */
	/* Set IR LED current to 100mA. */
	if (vcnl_write_command(0x83, 10)) {
		xputs("register write error");
		goto i2c_error;
	}
	/* Set proximity IR test signal frequency to 781.25 kHz. */
	if (vcnl_write_command(0x89, 2)) {
		xputs("register write error");
		goto i2c_error;
	}
	/* Modulation delay time = 4 and Modulation dead time = 1 */
	if (vcnl_write_command(0x8a, 129)) {
		xputs("register write error");
		goto i2c_error;
	}

	/* Proximity offset */
	for (i = 0; i < 5; i++) {
		/* Start measurement. */
		if (vcnl_write_command(0x80, 8)) {
			xputs("register write error");
			goto i2c_error;
		}

		delay_us(i == 0 ? 600 : 200);

		/* Get measurement result */
		if (vcnl_read_word(0x87, &d)) {
			xputs("register read error");
			goto i2c_error;
		}
		if (i == 0) {
			prox_min = d;
			prox_max = d;
			total = d;
		} else {
			if (d < prox_min)
				prox_min = d;
			if (d > prox_max)
				prox_max = d;
			total += d;
		}

		delay_ms(10);
	}
	prox_off = total / 5;
	xprintf("proximity offset = %d (%d - %d)\r\n",
		prox_off, prox_min, prox_max);

	/* Ambient Light */
	/* Start measurement. */
	if (vcnl_write_command(0x80, 0x10)) {
		xputs("register write error");
		goto i2c_error;
	}

	delay_ms(400);

	/* Get measurement result */
	if (vcnl_read_word(0x85, &d)) {
		xputs("register read error");
		goto i2c_error;
	}
	xprintf("Ambient Light = %d lx\r\n", d / 4, 0, 0);

	while (1) {
		n = 0;
		x = 0;
		p = 0;
		while (n < 9 && x < 10) {
			/* Start measurement. */
			if (vcnl_write_command(0x80, 8)) {
				xputs("register write error");
				goto i2c_error;
			}

			delay_us(200);

			/* Get measurement result */
			if (vcnl_read_word(0x87, &d)) {
				xputs("register read error");
				goto i2c_error;
			}

			if (d > prox_off + PROX_THRESH) {
				p += d;
				if (++x == 10) {
					xprintf("Proximity detected %d\r\n",
						p / 10, 0, 0);
				}
			} else {
				n++;
			}

			delay_ms(10);
		}
	}

i2c_error:
	xprintf("I2C error: state = 0x%x  expected state = 0x%x\r\n",
		state, next_state, 0);
	while (1)
		;

	return 0;
}
