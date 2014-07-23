/*
 * rom_api - Read a thermometer.
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
#include <rom_api.h>

/*
 * TI TMP102
 * Low Power Digital Temperature Sensor
 * With SMBus/Two-Wire Serial Interface in SOT563
 *
 * Digital Temperature Sensor Breakout - TMP102
 * SEN-09418
 * SparkFun Electronics
 */
#define TMP102_SLA	0x90	/* ADD0 -> GND */
#define CONVERSION_TIME	35	/* MAX 35msec */

/* USART clock frequency */
#define U_PCLK		(16 * 115200)

#define MAX_STACK_SIZE	512

int system_clock;
int uart_ramsize_in_bytes;
api_uart_handle_t uart_handle;
int i2c_ramsize_in_bytes;
api_i2c_handle_t i2c_handle;

static void *xmalloc(int nbytes)
{
	extern char end;
	extern char _stack;
	static char *heap_end;
	char *p;

	if (heap_end == 0)
		heap_end = &end;

	if (heap_end + nbytes + MAX_STACK_SIZE >= &_stack)
		return 0;

	p = heap_end;
	heap_end += (nbytes + 3) / 4 * 4; /* word boundary */

	return p;
}

static int api_clock_setup(void)
{
	unsigned int command[4];
	unsigned int result[2];

	/* Power profile API */
	/* set_pll */
	command[0] = 12000;	/* 12MHz (IRC) */
	command[1] = 24000;	/* 24MHz */
	command[2] = API_CPU_FREQ_EQU;
	command[3] = 0;		/* time-out */
	LPC_PWRD_API->set_pll(command, result);
	if (result[0] != API_OK)
		return result[0];

	/* system clock (in kHz) */
	system_clock = result[1];

	/* set_power */
	command[0] = 24;	/* main clock 24MHz */
	command[1] = API_PWR_CPU_PERFORMANCE;
	command[2] = 24;	/* system clcok 24MHz */
	LPC_PWRD_API->set_power(command, result);
	if (result[0] != API_OK)
		return result[0];

	/* Enable USART clock (U_PCLK). */
	syscon_set_usart_clock(24000000, U_PCLK);

	/* Set IOCON glitch filter clock divider. */
	syscon_set_ioconclkdiv(0, 1);

	return 0;
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

	/* Set PIO0_2 to 'I2C SCL'. */
	gpio_config(GPIO_I2C_SCL, GPIO_OPENDRAIN | GPIO_HYST |
		    GPIO_IOCONCLKDIV0 | GPIO_FILTER_2CLOCK, PIO0_2);

	/* Set PIO0_3 to 'I2C SDA'. */
	gpio_config(GPIO_I2C_SDA, GPIO_OPENDRAIN | GPIO_HYST |
		    GPIO_IOCONCLKDIV0 | GPIO_FILTER_2CLOCK, PIO0_3);

	/* Set PIO0_4 to 'USART0 TXD'. */
	gpio_config(GPIO_U0_TXD, GPIO_HYST, PIO0_4);
}

static int api_usart_setup(void)
{
	api_uart_config_t uart_config;
	uint8_t *m;

	/* Enable USART0 clock. */
	syscon_enable_clock(SYSCON_UART0);

	/* USART API */
	/* uart_get_mem_size */
	uart_ramsize_in_bytes = LPC_UARTD_API->uart_get_mem_size();
	m = xmalloc(uart_ramsize_in_bytes);
	if (m == 0)
		return -1;

	/* uart_setup */
	uart_handle = LPC_UARTD_API->uart_setup(USART0_BASE, m);

	/* uart_init */
	uart_config.sys_clk_in_hz = U_PCLK;
	uart_config.baudrate_in_hz = 115200;
	uart_config.config = 1;	/* 1 Stop bit, No Parity, 8 bits length */
	uart_config.sync_mode = 0; /* Async mode */
	uart_config.error_en = 0;
	LPC_UARTD_API->uart_init(uart_handle, &uart_config);

	return 0;
}

static int api_i2c_setup(void)
{
	uint32_t *m;
	int r;

	/* Enable I2C clock. */
	syscon_enable_clock(SYSCON_I2C);

	/* I2C API */
	/* i2c_get_mem_size */
	i2c_ramsize_in_bytes = LPC_I2CD_API->i2c_get_mem_size();
	m = xmalloc(i2c_ramsize_in_bytes);
	if (m == 0)
		return -1;

	/* i2c_setup */
	i2c_handle = LPC_I2CD_API->i2c_setup(I2C_BASE, m);

	/* i2c_set_bitrate */
	r = LPC_I2CD_API->i2c_set_bitrate(i2c_handle, 24000000, 400000);
	if (r != API_OK)
		return r;

	return 0;
}

static void mrt_setup(void)
{
	/* Enable MRT clock. */
	syscon_enable_clock(SYSCON_MRT);

	/* Set channel 0 to 'one-shot interrupt mode'. */
	mrt_set_mode(MRT0, MRT_ONE_SHOT);
}

static void delay_ms(int ms)
{
	mrt_set_interval(MRT0, 24000000 / 1000 * ms);

	while (!mrt_get_channel_status(MRT0, MRT_INT))
		;

	mrt_clear_channel_status(MRT0, MRT_INT);
}

static void xputchar(char c)
{
	LPC_UARTD_API->uart_put_char(uart_handle, c);
}

static void print_hex(int c)
{
	int d;

	d = (c >> 4) & 0xf;
	if (d > 9)
		d += 'a' - 10;
	else
		d += '0';
	xputchar(d);
	d = c & 0xf;
	if (d > 9)
		d += 'a' - 10;
	else
		d += '0';
	xputchar(d);
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
		xputchar('-');
		d = -d;
	}
	zerosup = true;
	for (i = 1000000000; i > 0; i /= 10) {
		if (d / i || i == 1 || !zerosup) {
			xputchar(d / i + '0');
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
				xputchar(*p++);
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
				xputchar('?');
				p++;
				break;
			}
		} else {
			xputchar(*p++);
		}
	}
}

static void xputs(char *p)
{
	api_uart_param_t uart_param;

	uart_param.buffer = (uint8_t *)p;
	uart_param.size = 0;
	uart_param.transfer_mode = 1; /* '\0' termination, add <CR><LF> */
	uart_param.driver_mode = 0;   /* Polling mode */
	uart_param.callback_func_pt = 0;
	LPC_UARTD_API->uart_put_line(uart_handle, &uart_param);
}

static int temp_read(uint8_t *buf)
{
	api_i2c_param_t i2c_param;
	api_i2c_result_t i2c_result;
	uint8_t sndbuf[4];
	uint8_t rcvbuf[4];
	int r;

	/* i2c_master_receive_poll */
	i2c_param.num_bytes_send = 0;
	i2c_param.num_bytes_rec = 3;
	rcvbuf[0] = TMP102_SLA | 1;
	i2c_param.buffer_ptr_send = sndbuf;
	i2c_param.buffer_ptr_rec = rcvbuf;
	i2c_param.func_pt = 0;
	i2c_param.stop_flag = -1;
	i2c_result.n_bytes_sent = 0;
	i2c_result.n_bytes_recd = 0;
	r = LPC_I2CD_API->i2c_master_receive_poll(i2c_handle, &i2c_param,
						  &i2c_result);
	if (r != API_OK) {
		xprintf("i2c_master_receive_poll failed: 0x%w\r\n", r, 0, 0);
		return r;
	}

	/*
	xprintf("%d bytes sent, %d bytes received\r\n",
		i2c_result.n_bytes_sent, i2c_result.n_bytes_recd, 0);
	xprintf("%x %x %x\r\n", rcvbuf[0], rcvbuf[1], rcvbuf[2]);
	*/

	buf[0] = rcvbuf[1];
	buf[1] = rcvbuf[2];

	return 0;
}

static void temp_display(uint8_t *buf)
{
	int16_t t;
	int i;
	int f;
	static const char frac[16] = {0, 1, 1, 2, 3, 3, 4, 4,
				      5, 6, 6, 7, 8, 8, 9, 9};

	t = (buf[0] << 8) | buf[1];
	t >>= 4;
	if (t < 0) {
		t = -t;
		xputchar('-');
	} else {
		xputchar(' ');
	}
	f = t & 0xf;
	i = t >> 4;
	xprintf("%d.%d degrees Celsius\r\n", i, frac[f], 0);
}

int main(void)
{
	uint32_t v;
	uint8_t buf[2] = {0, 0};

	if (api_clock_setup())
		goto api_error;
	gpio_setup();
	if (api_usart_setup())
		goto api_error;
	if (api_i2c_setup())
		goto api_error;
	mrt_setup();

	xputs("rom_api");
	xprintf("system clock = %dkHz\r\n", system_clock, 0, 0);
	xprintf("USART memory = %d bytes\r\n", uart_ramsize_in_bytes, 0, 0);
	xprintf("I2C memory = %d bytes\r\n", i2c_ramsize_in_bytes, 0, 0);

	/* i2c_get_firmware_version */
	v = LPC_I2CD_API->i2c_get_firmware_version();
	xprintf("I2C firmware version = 0x%w\r\n", v, 0, 0);

	while (1) {
		delay_ms(1000);

		if (temp_read(buf))
			goto api_error;

		temp_display(buf);
	}

api_error:
	while (1)
		;

	return 0;
}
