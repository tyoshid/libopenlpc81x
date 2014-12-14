/*
 * GPIO port
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

/*
 * NXP Semiconductors(www.nxp.com)
 *
 * UM10601: LPC81x User manual
 * (Rev.1.6 - 2 April 2014)
 *
 * Chapter 7: LPC81x GPIO port
 */

#include <mmio.h>
#include <memorymap.h>

/* --- GPIO registers ------------------------------------------------------ */
/*
 * Offset	Register
 * 0x0000	GPIO_B0			Byte pin register port 0; PIO0_0
 * 0x0001	GPIO_B1			Byte pin register port 0; PIO0_1
 * 0x0002	GPIO_B2			Byte pin register port 0; PIO0_2
 * 0x0003	GPIO_B3			Byte pin register port 0; PIO0_3
 * 0x0004	GPIO_B4			Byte pin register port 0; PIO0_4
 * 0x0005	GPIO_B5			Byte pin register port 0; PIO0_5
 * 0x0006	GPIO_B6			Byte pin register port 0; PIO0_6
 * 0x0007	GPIO_B7			Byte pin register port 0; PIO0_7
 * 0x0008	GPIO_B8			Byte pin register port 0; PIO0_8
 * 0x0009	GPIO_B9			Byte pin register port 0; PIO0_9
 * 0x000a	GPIO_B10		Byte pin register port 0; PIO0_10
 * 0x000b	GPIO_B11		Byte pin register port 0; PIO0_11
 * 0x000c	GPIO_B12		Byte pin register port 0; PIO0_12
 * 0x000d	GPIO_B13		Byte pin register port 0; PIO0_13
 * 0x000e	GPIO_B14		Byte pin register port 0; PIO0_14
 * 0x000f	GPIO_B15		Byte pin register port 0; PIO0_15
 * 0x0010	GPIO_B16		Byte pin register port 0; PIO0_16
 * 0x0011	GPIO_B17		Byte pin register port 0; PIO0_17
 * 0x1000	GPIO_W0			Word pin register port 0; PIO0_0
 * 0x1004	GPIO_W1			Word pin register port 0; PIO0_1
 * 0x1008	GPIO_W2			Word pin register port 0; PIO0_2
 * 0x100c	GPIO_W3			Word pin register port 0; PIO0_3
 * 0x1010	GPIO_W4			Word pin register port 0; PIO0_4
 * 0x1014	GPIO_W5			Word pin register port 0; PIO0_5
 * 0x1018	GPIO_W6			Word pin register port 0; PIO0_6
 * 0x101c	GPIO_W7			Word pin register port 0; PIO0_7
 * 0x1020	GPIO_W8			Word pin register port 0; PIO0_8
 * 0x1024	GPIO_W9			Word pin register port 0; PIO0_9
 * 0x1028	GPIO_W10		Word pin register port 0; PIO0_10
 * 0x102c	GPIO_W11		Word pin register port 0; PIO0_11
 * 0x1030	GPIO_W12		Word pin register port 0; PIO0_12
 * 0x1034	GPIO_W13		Word pin register port 0; PIO0_13
 * 0x1038	GPIO_W14		Word pin register port 0; PIO0_14
 * 0x103c	GPIO_W15		Word pin register port 0; PIO0_15
 * 0x1040	GPIO_W16		Word pin register port 0; PIO0_16
 * 0x1044	GPIO_W17		Word pin register port 0; PIO0_17
 * 0x2000	GPIO_DIR0		Direction register port 0
 * 0x2080	GPIO_MASK0		Mask register port 0
 * 0x2100	GPIO_PIN0		Port pin register port 0
 * 0x2180	GPIO_MPIN0		Masked port register port 0
 * 0x2200	GPIO_SET0		Write: Set register for port 0
 *					Read: Output bits for port 0
 * 0x2280	GPIO_CLR0		Clear port 0
 * 0x2300	GPIO_NOT0		Toggle port 0
 */

#define GPIO_B(pin)			MMIO8(GPIO_BASE + (pin))
#define GPIO_B0				GPIO_B(0)
#define GPIO_B1				GPIO_B(1)
#define GPIO_B2				GPIO_B(2)
#define GPIO_B3				GPIO_B(3)
#define GPIO_B4				GPIO_B(4)
#define GPIO_B5				GPIO_B(5)
#define GPIO_B6				GPIO_B(6)
#define GPIO_B7				GPIO_B(7)
#define GPIO_B8				GPIO_B(8)
#define GPIO_B9				GPIO_B(9)
#define GPIO_B10			GPIO_B(10)
#define GPIO_B11			GPIO_B(11)
#define GPIO_B12			GPIO_B(12)
#define GPIO_B13			GPIO_B(13)
#define GPIO_B14			GPIO_B(14)
#define GPIO_B15			GPIO_B(15)
#define GPIO_B16			GPIO_B(16)
#define GPIO_B17			GPIO_B(17)

/* GPIO_Wx */
#define GPIO_W(pin)			MMIO32(GPIO_BASE + 0x1000 + (pin) * 4)
#define GPIO_W0				GPIO_W(0)
#define GPIO_W1				GPIO_W(1)
#define GPIO_W2				GPIO_W(2)
#define GPIO_W3				GPIO_W(3)
#define GPIO_W4				GPIO_W(4)
#define GPIO_W5				GPIO_W(5)
#define GPIO_W6				GPIO_W(6)
#define GPIO_W7				GPIO_W(7)
#define GPIO_W8				GPIO_W(8)
#define GPIO_W9				GPIO_W(9)
#define GPIO_W10			GPIO_W(10)
#define GPIO_W11			GPIO_W(11)
#define GPIO_W12			GPIO_W(12)
#define GPIO_W13			GPIO_W(13)
#define GPIO_W14			GPIO_W(14)
#define GPIO_W15			GPIO_W(15)
#define GPIO_W16			GPIO_W(16)
#define GPIO_W17			GPIO_W(17)

#define GPIO_DIR0			MMIO32(GPIO_BASE + 0x2000)
#define GPIO_MASK0			MMIO32(GPIO_BASE + 0x2080)
#define GPIO_PIN0			MMIO32(GPIO_BASE + 0x2100)
#define GPIO_MPIN0			MMIO32(GPIO_BASE + 0x2180)
#define GPIO_SET0			MMIO32(GPIO_BASE + 0x2200)
#define GPIO_CLR0			MMIO32(GPIO_BASE + 0x2280)
#define GPIO_NOT0			MMIO32(GPIO_BASE + 0x2300)


/* --- GPIO_Bx values ------------------------------------------------------ */

#define GPIO_B_PBYTE_LOW		0
#define GPIO_B_PBYTE_HIGH		1

/* --- GPIO_Wx values ------------------------------------------------------ */

#define GPIO_W_PWORD_LOW		0
#define GPIO_W_PWORD_HIGH		0xffffffff

/* --- GPIO_DIR0 values ---------------------------------------------------- */

/* GPIO_DIR0[17:0]: DIRP0 */
#define GPIO_DIR0_INPUT			0
#define GPIO_DIR0_OUTPUT		1

/* --- GPIO_MASK0 values --------------------------------------------------- */

/* GPIO_MASK0[17:0]: MASKP0 */

/* --- GPIO_PIN0 values ---------------------------------------------------- */

/* GPIO_PIN0[17:0]: PORT0 */

/* --- GPIO_MPIN0 values --------------------------------------------------- */

/* GPIO_MPIN0[17:0]: MPORTP0 */

/* --- GPIO_SET0 values ---------------------------------------------------- */

/* GPIO_SET0[17:0]: SETP0 */

/* --- GPIO_CLR0 values ---------------------------------------------------- */

/* GPIO_CLR0[17:0]: CLRP0 */

/* --- GPIO_NOT0 values ---------------------------------------------------- */

/* GPIO_NOT0[17:0]: NOTP0 */

/* --- GPIO pins definition ------------------------------------------------ */

#define GPIO_MAXPIN			18

enum {
	PIO0_0 = (1 << 0),
	PIO0_1 = (1 << 1),
	PIO0_2 = (1 << 2),
	PIO0_3 = (1 << 3),
	PIO0_4 = (1 << 4),
	PIO0_5 = (1 << 5),
	PIO0_6 = (1 << 6),
	PIO0_7 = (1 << 7),
	PIO0_8 = (1 << 8),
	PIO0_9 = (1 << 9),
	PIO0_10 = (1 << 10),
	PIO0_11 = (1 << 11),
	PIO0_12 = (1 << 12),
	PIO0_13 = (1 << 13),
	PIO0_14 = (1 << 14),
	PIO0_15 = (1 << 15),
	PIO0_16 = (1 << 16),
	PIO0_17 = (1 << 17),
	PIO0_ALL = 0x3ffff,
	PIO0_NONE = 0
};

#define GPIO_OR(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, \
		p14, p15, p16, p17, ...) \
	(PIO0_##p0 | PIO0_##p1 | PIO0_##p2 | PIO0_##p3 | PIO0_##p4 | \
	 PIO0_##p5 | PIO0_##p6 | PIO0_##p7 | PIO0_##p8 | PIO0_##p9 | \
	 PIO0_##p10 | PIO0_##p11 | PIO0_##p12 | PIO0_##p13 | PIO0_##p14 | \
	 PIO0_##p15 | PIO0_##p16 | PIO0_##p17)

#define PIO0(...)	GPIO_OR(__VA_ARGS__, NONE, NONE, NONE, NONE, NONE, \
				NONE, NONE, NONE, NONE, NONE, NONE, NONE, \
				NONE, NONE, NONE, NONE, NONE, NONE)

/* --- Function prototypes ------------------------------------------------- */

/* IOCON */
enum {
	GPIO_IOCONCLKDIV0 = (0 << 13),
	GPIO_IOCONCLKDIV1 = (1 << 13),
	GPIO_IOCONCLKDIV2 = (2 << 13),
	GPIO_IOCONCLKDIV3 = (3 << 13),
	GPIO_IOCONCLKDIV4 = (4 << 13),
	GPIO_IOCONCLKDIV5 = (5 << 13),
	GPIO_IOCONCLKDIV6 = (6 << 13),
	GPIO_FILTER_BYPASS = (0 << 11),
	GPIO_FILTER_1CLOCK = (1 << 11),
	GPIO_FILTER_2CLOCK = (2 << 11),
	GPIO_FILTER_3CLOCK = (3 << 11),
	GPIO_OPENDRAIN = (1 << 10),
	GPIO_PUSHPULL = 0,

	GPIO_I2C = 0,
	GPIO_IO = (1 << 8),
	GPIO_I2C_FAST_PLUS = (2 << 8),

	GPIO_INVERT = (1 << 6),
	GPIO_HYST =(1 << 5),
	GPIO_PULLDOWN = (1 << 3),
	GPIO_PULLUP = (2 << 3),
	GPIO_REPEATER = (3 << 3),
	GPIO_NOPUPD = 0,
	GPIO_FLOAT = 0
};

enum  gpio_func {
	/* Movable functions */
	GPIO_U0_TXD,
	GPIO_U0_RXD,
	GPIO_U0_RTS,
	GPIO_U0_CTS,
	GPIO_U0_SCLK,
	GPIO_U1_TXD,
	GPIO_U1_RXD,
	GPIO_U1_RTS,
	GPIO_U1_CTS,
	GPIO_U1_SCLK,
	GPIO_U2_TXD,
	GPIO_U2_RXD,
	GPIO_U2_RTS,
	GPIO_U2_CTS,
	GPIO_U2_SCLK,
	GPIO_SPI0_SCK,
	GPIO_SPI0_MOSI,
	GPIO_SPI0_MISO,
	GPIO_SPI0_SSEL,
	GPIO_SPI1_SCK,
	GPIO_SPI1_MOSI,
	GPIO_SPI1_MISO,
	GPIO_SPI1_SSEL,
	GPIO_CTIN0,
	GPIO_CTIN1,
	GPIO_CTIN2,
	GPIO_CTIN3,
	GPIO_CTOUT0,
	GPIO_CTOUT1,
	GPIO_CTOUT2,
	GPIO_CTOUT3,
	GPIO_I2C_SDA,
	GPIO_I2C_SCL,
	GPIO_ACMP_O,
	GPIO_CLKOUT,
	GPIO_INT_BMAT,

	/* Fixed-pin functions */
	GPIO_ACMP_I1,
	GPIO_ACMP_I2,
	GPIO_SWCLK,
	GPIO_SWDIO,
	GPIO_XTALIN,
	GPIO_XTALOUT,
	GPIO_RESET,
	GPIO_CLKIN,
	GPIO_VDDCMP,

	/* Standard I/O */
	GPIO_OUTPUT,
	GPIO_INPUT
};

void gpio_config(enum gpio_func func, int iocon, int pins);
void gpio_reconfig(enum gpio_func func, int iocon, int pins);
void gpio_set(int pins);
void gpio_clear(int pins);
int gpio_get(int pins);
void gpio_toggle(int pins);
int gpio_read_port(void);
void gpio_write_port(int data);
void gpio_set_mask(int mask);
int gpio_read_masked_port(void);
void gpio_write_maksed_port(int data);
int gpio_read_pin_byte(int pin);
void gpio_write_pin_byte(int pin, int data);
int gpio_read_pin_word(int pin);
void gpio_write_pin_word(int pin, int data);
