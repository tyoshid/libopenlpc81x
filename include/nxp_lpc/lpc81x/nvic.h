/*
 * Nested Vectored Interrupt Controller (NVIC)
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
 * Chapter 3: LPC81x Nested Vectored Interrupt Controller (NVIC)
 */

#include <mmio.h>
#include <memorymap.h>

/* --- NVIC registers ------------------------------------------------------ */
/*
 * Offset	Register
 * 0x000	NVIC_ISER0		Interrupt Set Enable Register 0
 * 0x080	NVIC_ICER0		Interrupt Clear Enable Register 0
 * 0x100	NVIC_ISPR0		Interrupt Set Pending Register 0
 * 0x180	NVIC_ICPR0		Interrupt Clear Pending Register 0
 * 0x200	NVIC_IABR0		Interrupt Active Bit Register 0
 * 0x300	NVIC_IPR0		Interrupt Priority Registers 0
 * 0x304	NVIC_IPR1		Interrupt Priority Registers 1
 * 0x308	NVIC_IPR2		Interrupt Priority Registers 2
 * 0x30c	NVIC_IPR3		Interrupt Priority Registers 3
 * 0x318	NVIC_IPR6		Interrupt Priority Registers 6
 * 0x31c	NVIC_IPR7		Interrupt Priority Registers 7
 */

#define NVIC_ISER0			MMIO32(NVIC_BASE + 0x000)
#define NVIC_ICER0			MMIO32(NVIC_BASE + 0x080)
#define NVIC_ISPR0			MMIO32(NVIC_BASE + 0x100)
#define NVIC_ICPR0			MMIO32(NVIC_BASE + 0x180)
#define NVIC_IABR0			MMIO32(NVIC_BASE + 0x200)

#define NVIC_IPR(x)			MMIO32(NVIC_BASE + 0x300 + (x) * 4)
#define NVIC_IPR0			NVIC_IPR(0)
#define NVIC_IPR1			NVIC_IPR(1)
#define NVIC_IPR2			NVIC_IPR(2)
#define NVIC_IPR3			NVIC_IPR(3)
#define NVIC_IPR6			NVIC_IPR(6)
#define NVIC_IPR7			NVIC_IPR(7)

/* --- NVIC_ISER0 values --------------------------------------------------- */

#define NVIC_ISER0_ISE_PININT7		(1 << 31)
#define NVIC_ISER0_ISE_PININT6		(1 << 30)
#define NVIC_ISER0_ISE_PININT5		(1 << 29)
#define NVIC_ISER0_ISE_PININT4		(1 << 28)
#define NVIC_ISER0_ISE_PININT3		(1 << 27)
#define NVIC_ISER0_ISE_PININT2		(1 << 26)
#define NVIC_ISER0_ISE_PININT1		(1 << 25)
#define NVIC_ISER0_ISE_PININT0		(1 << 24)
#define NVIC_ISER0_ISE_WKT		(1 << 15)
#define NVIC_ISER0_ISE_FLASH		(1 << 14)
#define NVIC_ISER0_ISE_BOD		(1 << 13)
#define NVIC_ISER0_ISE_WDT		(1 << 12)
#define NVIC_ISER0_ISE_CMP		(1 << 11)
#define NVIC_ISER0_ISE_MRT		(1 << 10)
#define NVIC_ISER0_ISE_SCT		(1 << 9)
#define NVIC_ISER0_ISE_I2C		(1 << 8)
#define NVIC_ISER0_ISE_UART2		(1 << 5)
#define NVIC_ISER0_ISE_UART1		(1 << 4)
#define NVIC_ISER0_ISE_UART0		(1 << 3)
#define NVIC_ISER0_ISE_SPI1		(1 << 1)
#define NVIC_ISER0_ISE_SPI0		(1 << 0)

/* --- NVIC_ICER0 values --------------------------------------------------- */

#define NVIC_ICER0_ICE_PININT7		(1 << 31)
#define NVIC_ICER0_ICE_PININT6		(1 << 30)
#define NVIC_ICER0_ICE_PININT5		(1 << 29)
#define NVIC_ICER0_ICE_PININT4		(1 << 28)
#define NVIC_ICER0_ICE_PININT3		(1 << 27)
#define NVIC_ICER0_ICE_PININT2		(1 << 26)
#define NVIC_ICER0_ICE_PININT1		(1 << 25)
#define NVIC_ICER0_ICE_PININT0		(1 << 24)
#define NVIC_ICER0_ICE_WKT		(1 << 15)
#define NVIC_ICER0_ICE_FLASH		(1 << 14)
#define NVIC_ICER0_ICE_BOD		(1 << 13)
#define NVIC_ICER0_ICE_WDT		(1 << 12)
#define NVIC_ICER0_ICE_CMP		(1 << 11)
#define NVIC_ICER0_ICE_MRT		(1 << 10)
#define NVIC_ICER0_ICE_SCT		(1 << 9)
#define NVIC_ICER0_ICE_I2C		(1 << 8)
#define NVIC_ICER0_ICE_UART2		(1 << 5)
#define NVIC_ICER0_ICE_UART1		(1 << 4)
#define NVIC_ICER0_ICE_UART0		(1 << 3)
#define NVIC_ICER0_ICE_SPI1		(1 << 1)
#define NVIC_ICER0_ICE_SPI0		(1 << 0)

/* --- NVIC_ISPR0 values --------------------------------------------------- */

#define NVIC_ISPR0_ISP_PININT7		(1 << 31)
#define NVIC_ISPR0_ISP_PININT6		(1 << 30)
#define NVIC_ISPR0_ISP_PININT5		(1 << 29)
#define NVIC_ISPR0_ISP_PININT4		(1 << 28)
#define NVIC_ISPR0_ISP_PININT3		(1 << 27)
#define NVIC_ISPR0_ISP_PININT2		(1 << 26)
#define NVIC_ISPR0_ISP_PININT1		(1 << 25)
#define NVIC_ISPR0_ISP_PININT0		(1 << 24)
#define NVIC_ISPR0_ISP_WKT		(1 << 15)
#define NVIC_ISPR0_ISP_FLASH		(1 << 14)
#define NVIC_ISPR0_ISP_BOD		(1 << 13)
#define NVIC_ISPR0_ISP_WDT		(1 << 12)
#define NVIC_ISPR0_ISP_CMP		(1 << 11)
#define NVIC_ISPR0_ISP_MRT		(1 << 10)
#define NVIC_ISPR0_ISP_SCT		(1 << 9)
#define NVIC_ISPR0_ISP_I2C		(1 << 8)
#define NVIC_ISPR0_ISP_UART2		(1 << 5)
#define NVIC_ISPR0_ISP_UART1		(1 << 4)
#define NVIC_ISPR0_ISP_UART0		(1 << 3)
#define NVIC_ISPR0_ISP_SPI1		(1 << 1)
#define NVIC_ISPR0_ISP_SPI0		(1 << 0)

/* --- NVIC_ICPR0 values --------------------------------------------------- */

#define NVIC_ICPR0_ICP_PININT7		(1 << 31)
#define NVIC_ICPR0_ICP_PININT6		(1 << 30)
#define NVIC_ICPR0_ICP_PININT5		(1 << 29)
#define NVIC_ICPR0_ICP_PININT4		(1 << 28)
#define NVIC_ICPR0_ICP_PININT3		(1 << 27)
#define NVIC_ICPR0_ICP_PININT2		(1 << 26)
#define NVIC_ICPR0_ICP_PININT1		(1 << 25)
#define NVIC_ICPR0_ICP_PININT0		(1 << 24)
#define NVIC_ICPR0_ICP_WKT		(1 << 15)
#define NVIC_ICPR0_ICP_FLASH		(1 << 14)
#define NVIC_ICPR0_ICP_BOD		(1 << 13)
#define NVIC_ICPR0_ICP_WDT		(1 << 12)
#define NVIC_ICPR0_ICP_CMP		(1 << 11)
#define NVIC_ICPR0_ICP_MRT		(1 << 10)
#define NVIC_ICPR0_ICP_SCT		(1 << 9)
#define NVIC_ICPR0_ICP_I2C		(1 << 8)
#define NVIC_ICPR0_ICP_UART2		(1 << 5)
#define NVIC_ICPR0_ICP_UART1		(1 << 4)
#define NVIC_ICPR0_ICP_UART0		(1 << 3)
#define NVIC_ICPR0_ICP_SPI1		(1 << 1)
#define NVIC_ICPR0_ICP_SPI0		(1 << 0)

/* --- NVIC_IABR0 values --------------------------------------------------- */

#define NVIC_IABR0_IAB_PININT7		(1 << 31)
#define NVIC_IABR0_IAB_PININT6		(1 << 30)
#define NVIC_IABR0_IAB_PININT5		(1 << 29)
#define NVIC_IABR0_IAB_PININT4		(1 << 28)
#define NVIC_IABR0_IAB_PININT3		(1 << 27)
#define NVIC_IABR0_IAB_PININT2		(1 << 26)
#define NVIC_IABR0_IAB_PININT1		(1 << 25)
#define NVIC_IABR0_IAB_PININT0		(1 << 24)
#define NVIC_IABR0_IAB_WKT		(1 << 15)
#define NVIC_IABR0_IAB_FLASH		(1 << 14)
#define NVIC_IABR0_IAB_BOD		(1 << 13)
#define NVIC_IABR0_IAB_WDT		(1 << 12)
#define NVIC_IABR0_IAB_CMP		(1 << 11)
#define NVIC_IABR0_IAB_MRT		(1 << 10)
#define NVIC_IABR0_IAB_SCT		(1 << 9)
#define NVIC_IABR0_IAB_I2C		(1 << 8)
#define NVIC_IABR0_IAB_UART2		(1 << 5)
#define NVIC_IABR0_IAB_UART1		(1 << 4)
#define NVIC_IABR0_IAB_UART0		(1 << 3)
#define NVIC_IABR0_IAB_SPI1		(1 << 1)
#define NVIC_IABR0_IAB_SPI0		(1 << 0)

/* --- NVIC_IPRx values ---------------------------------------------------- */

/* NVIC_IPRx[31:24]: IP[4x + 3]: Priority, byte offset 3 */
/* NVIC_IPRx[23:16]: IP[4x + 2]: Priority, byte offset 2 */
/* NVIC_IPRx[15:8]:  IP[4x + 1]: Priority, byte offset 1 */
/* NVIC_IPRx[7:0]:   IP[4x]:     Priority, byte offset 0 */

/* --- IRQ channel numbers ------------------------------------------------- */

#define NVIC_SPI0_IRQ			0
#define NVIC_SPI1_IRQ			1
#define NVIC_UART0_IRQ			3
#define NVIC_UART1_IRQ			4
#define NVIC_UART2_IRQ			5
#define NVIC_I2C_IRQ			8
#define NVIC_SCT_IRQ			9
#define NVIC_MRT_IRQ			10
#define NVIC_CMP_IRQ			11
#define NVIC_WDT_IRQ			12
#define NVIC_BOD_IRQ			13
#define NVIC_FLASH_IRQ			14
#define NVIC_WKT_IRQ			15
#define NVIC_PININT0_IRQ		24
#define NVIC_PININT1_IRQ		25
#define NVIC_PININT2_IRQ		26
#define NVIC_PININT3_IRQ		27
#define NVIC_PININT4_IRQ		28
#define NVIC_PININT5_IRQ		29
#define NVIC_PININT6_IRQ		30
#define NVIC_PININT7_IRQ		31

void nmi(void)		__attribute__ ((weak));
void hardfault(void)	__attribute__ ((weak));
void svcall(void)	__attribute__ ((weak));
void pendsv(void)	__attribute__ ((weak));
void systick_isr(void)	__attribute__ ((weak));

void spi0_isr(void)	__attribute__ ((weak));
void spi1_isr(void)	__attribute__ ((weak));
void uart0_isr(void)	__attribute__ ((weak));
void uart1_isr(void)	__attribute__ ((weak));
void uart2_isr(void)	__attribute__ ((weak));
void i2c_isr(void)	__attribute__ ((weak));
void sct_isr(void)	__attribute__ ((weak));
void mrt_isr(void)	__attribute__ ((weak));
void cmp_isr(void)	__attribute__ ((weak));
void wdt_isr(void)	__attribute__ ((weak));
void bod_isr(void)	__attribute__ ((weak));
void flash_isr(void)	__attribute__ ((weak));
void wkt_isr(void)	__attribute__ ((weak));
void pinint0_isr(void)	__attribute__ ((weak));
void pinint1_isr(void)	__attribute__ ((weak));
void pinint2_isr(void)	__attribute__ ((weak));
void pinint3_isr(void)	__attribute__ ((weak));
void pinint4_isr(void)	__attribute__ ((weak));
void pinint5_isr(void)	__attribute__ ((weak));
void pinint6_isr(void)	__attribute__ ((weak));
void pinint7_isr(void)	__attribute__ ((weak));

/* --- Function prototypes ------------------------------------------------- */

/* Interrupts */
enum {
	NVIC_SPI0 = (1 << 0),
	NVIC_SPI1 = (1 << 1),
	NVIC_UART0 = (1 << 3),
	NVIC_UART1 = (1 << 4),
	NVIC_UART2 = (1 << 5),
	NVIC_I2C = (1 << 8),
	NVIC_SCT = (1 << 9),
	NVIC_MRT = (1 << 10),
	NVIC_CMP = (1 << 11),
	NVIC_WDT = (1 << 12),
	NVIC_BOD = (1 << 13),
	NVIC_FLASH = (1 << 14),
	NVIC_WKT = (1 << 15),
	NVIC_PININT0 = (1 << 24),
	NVIC_PININT1 = (1 << 25),
	NVIC_PININT2 = (1 << 26),
	NVIC_PININT3 = (1 << 27),
	NVIC_PININT4 = (1 << 28),
	NVIC_PININT5 = (1 << 29),
	NVIC_PININT6 = (1 << 30),
	NVIC_PININT7 = (1 << 31)
};

/* Priority */
enum {
	NVIC_PRIO_0 = 0x00,	/* highest priority */
	NVIC_PRIO_1 = 0x40,
	NVIC_PRIO_2 = 0x80,
	NVIC_PRIO_3 = 0xc0	/* lowest priority */
};

int nvic_get_enabled_irq(int interrupts);
void nvic_enable_irq(int interrupts);
void nvic_disable_irq(int interrupts);
int nvic_get_pending_irq(int interrupts);
void nvic_set_pending_irq(int interrupts);
void nvic_clear_pending_irq(int interrupts);
int nvic_get_active_irq(int interrupts);
void nvic_set_priority(int irqn, int priority);
int nvic_get_priority(int irqn);
