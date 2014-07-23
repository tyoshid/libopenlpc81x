/*
 * Memory mapping
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
 * Chapter 2: LPC81x Memory mapping
 */

#ifndef MEMORYMAP_H
#define MEMORYMAP_H

/* --- LPC81x memory map ------------------------------------------------ */

#define FLASH_MEMORY_BASE		0x00000000
#define SRAM_BASE			0x10000000
#define MTB_BASE			0x14000000
#define BOOT_ROM_BASE			0x1fff0000
#define PERIPH_BASE			0x40000000
#define CRC_BASE			0x50000000
#define SCT_BASE			0x50004000
#define GPIO_BASE			0xa0000000
#define PININT_BASE			0xa0004000

#define PPB_BASE			0xe0000000

/* --- APB Peripherals ----------------------------------------------------- */

#define WWDT_BASE			(PERIPH_BASE + 0x00000)
#define MRT_BASE			(PERIPH_BASE + 0x04000)
#define WKT_BASE			(PERIPH_BASE + 0x08000)
#define SWM_BASE			(PERIPH_BASE + 0x0c000)
#define PMU_BASE			(PERIPH_BASE + 0x20000)
#define ACMP_BASE			(PERIPH_BASE + 0x24000)
#define FLASHCON_BASE			(PERIPH_BASE + 0x40000)
#define IOCON_BASE			(PERIPH_BASE + 0x44000)
#define SYSCON_BASE			(PERIPH_BASE + 0x48000)
#define I2C_BASE			(PERIPH_BASE + 0x50000)
#define SPI0_BASE			(PERIPH_BASE + 0x58000)
#define SPI1_BASE			(PERIPH_BASE + 0x5c000)
#define USART0_BASE			(PERIPH_BASE + 0x64000)
#define USART1_BASE			(PERIPH_BASE + 0x68000)
#define USART2_BASE			(PERIPH_BASE + 0x6c000)

/* --- ARM Cortex-M0+ specific definitions --------------------------------- */

/* Private peripheral bus */
#define SCS_BASE			(PPB_BASE + 0xe000)

/* System Control Space */
#define STK_BASE			(SCS_BASE + 0x010)
#define NVIC_BASE			(SCS_BASE + 0x100)
#define SCB_BASE			(SCS_BASE + 0xd00)

#endif
