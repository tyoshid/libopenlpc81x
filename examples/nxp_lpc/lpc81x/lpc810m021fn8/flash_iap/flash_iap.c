/*
 * flash_iap - Write the flash memory.
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
#include <flash_iap.h>

#define TEST_FLASH_ADDRESS	0x00000c00
#define TEST_SECTOR		3
#define SECTOR_SIZE		1024
#define TEST_DATA_SIZE		256

/* USART clock frequency */
#define U_PCLK	(16 * 115200)

volatile int dont_delete_loop;

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

	/* Set PIO0_2 to 'output push-pull' (LED). */
	gpio_config(GPIO_OUTPUT, 0, PIO0_2);

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

int main(void)
{
	IAP iap_entry = (IAP)IAP_LOCATION;
	unsigned int command_param[5];
	unsigned int status_result[4];
	char *p;
	char start_data;
	char data;
	int i;
	int j;
	char buf[TEST_DATA_SIZE] __attribute__ ((aligned(4)));

	clock_setup();
	gpio_setup();
	usart_setup();

	xputs("Flash IAP");

	/* Read Part Identification number */
	command_param[0] = IAP_READ_PART_ID;
	iap_entry(command_param, status_result);
	xprintf("Part ID = 0x%w\r\n", status_result[1], 0, 0);

	/* Read Boot code version number */
	command_param[0] = IAP_READ_BOOT_CODE_VERSION;
	iap_entry(command_param, status_result);
	xprintf("Boot code version = %d.%d\r\n",
		(status_result[1] >> 8) & 0xff, status_result[1] & 0xff, 0);

	p = (char *)TEST_FLASH_ADDRESS;
	start_data = *p + 1;
	data = start_data;

	xputs("Blank check sector");
	command_param[0] = IAP_BLANK_CHECK_SECTOR;
	command_param[1] = TEST_SECTOR;
	command_param[2] = TEST_SECTOR;
	iap_entry(command_param, status_result);
	if (status_result[0] == IAP_SECTOR_NOT_BLANK) {
		xputs("Prepare sector for write operation");
		command_param[0] = IAP_PREPARE_SECTOR_FOR_WRITE_OPERATION;
		command_param[1] = TEST_SECTOR;
		command_param[2] = TEST_SECTOR;
		iap_entry(command_param, status_result);
		if (status_result[0] != IAP_CMD_SUCCESS) {
			xprintf("IAP Prepare sector(s) for write operation "
				"command failed: %d\r\n", 
				status_result[0], 0, 0);
			goto iap_error;
		}

		xputs("Erase sector");
		command_param[0] = IAP_ERASE_SECTOR;
		command_param[1] = TEST_SECTOR;
		command_param[2] = TEST_SECTOR;
		command_param[3] = 24000;
		iap_entry(command_param, status_result);
		if (status_result[0] != IAP_CMD_SUCCESS) {
			xprintf("IAP Erase sector(s) command failed: %d\r\n", 
				status_result[0], 0, 0);
			goto iap_error;
		}

		xputs("Blank check sector");
		command_param[0] = IAP_BLANK_CHECK_SECTOR;
		command_param[1] = TEST_SECTOR;
		command_param[2] = TEST_SECTOR;
		iap_entry(command_param, status_result);
		if (status_result[0] != IAP_CMD_SUCCESS) {
			xprintf("IAP Blank check sector(s) command failed: "
				"%d\r\n", 
				status_result[0], 0, 0);
			goto iap_error;
		}
	} else if (status_result[0] != IAP_CMD_SUCCESS) {
		xprintf("IAP Blank check sector(s) command failed: %d\r\n", 
			status_result[0], 0, 0);
		goto iap_error;
	}

	for (i = 0; i < SECTOR_SIZE / TEST_DATA_SIZE; i++) {
		/* Generate test data. */
		for (j = 0; j < TEST_DATA_SIZE; j++)
			buf[j] = data++;

		xputs("Prepare sector for write operation");
		command_param[0] = IAP_PREPARE_SECTOR_FOR_WRITE_OPERATION;
		command_param[1] = TEST_SECTOR;
		command_param[2] = TEST_SECTOR;
		iap_entry(command_param, status_result);
		if (status_result[0] != IAP_CMD_SUCCESS) {
			xprintf("IAP Prepare sector(s) for write operation "
				"command failed: %d\r\n", 
				status_result[0], 0, 0);
			goto iap_error;
		}

		xputs("Copy RAM to flash");
		command_param[0] = IAP_COPY_RAM_TO_FLASH;
		command_param[1] = TEST_FLASH_ADDRESS + TEST_DATA_SIZE * i;
		command_param[2] = (unsigned int)buf;
		command_param[3] = TEST_DATA_SIZE;
		command_param[4] = 24000;
		iap_entry(command_param, status_result);
		if (status_result[0] != IAP_CMD_SUCCESS) {
			xprintf("IAP Prepare sector(s) for write operation "
				"command failed: %d\r\n", 
				status_result[0], 0, 0);
			goto iap_error;
		}

		xputs("Compare");
		command_param[0] = IAP_COMPARE;
		command_param[1] = TEST_FLASH_ADDRESS + TEST_DATA_SIZE * i;
		command_param[2] = (unsigned int)buf;
		command_param[3] = TEST_DATA_SIZE;
		iap_entry(command_param, status_result);
		if (status_result[0] != IAP_CMD_SUCCESS) {
			xprintf("IAP Compare command failed: %d\r\n", 
				status_result[0], 0, 0);
			goto iap_error;
		}
	}

	p = (char *)TEST_FLASH_ADDRESS;
	data = start_data;
	for (i = 0; i < SECTOR_SIZE; i++) {
		if (*p != data) {
			xprintf("Verify error: 0x%w 0x%x (0x%x)\r\n",
				(int)p, *p, data);
			goto iap_error;
		}
		p++;
		data++;
	}

	xputs("Reinvoke ISP");
	/* LED on */
	gpio_set(PIO0_2);
	command_param[0] = IAP_REINVOKE_ISP;
	iap_entry(command_param, status_result);

iap_error:
	while (1) {
		/* LED on/off */
		gpio_toggle(PIO0_2);

		/* Wait a bit. */
		for (i = 0; i < 1000000; i++)
			dont_delete_loop;
	}

	return 0;
}
