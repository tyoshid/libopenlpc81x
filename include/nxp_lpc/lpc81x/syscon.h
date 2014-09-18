/*
 * System configuration (SYSCON)
 *
 * Copyright 2014 Toshiaki Yoshiada <yoshida@mpc.net>
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
 * Chapter 4: LPC81x System configuration (SYSCON)
 */

#include <mmio.h>
#include <memorymap.h>

/* --- SYSCON registers ---------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	SYSCON_SYSMEMREMAP	System memory remap
 * 0x004	SYSCON_PRESETCTRL	Peripheral reset control
 * 0x008	SYSCON_SYSPLLCTRL	System PLL control
 * 0x00c	SYSCON_SYSPLLSTAT	System PLL status
 * 0x020	SYSCON_SYSOSCCTRL	System oscillator control
 * 0x024	SYSCON_WDTOSCCTRL	Watchdog oscillator control
 * 0x030	SYSCON_SYSRSTSTAT	System reset status register
 * 0x040	SYSCON_SYSPLLCLKSEL	System PLL clock source select
 * 0x044	SYSCON_SYSPLLCLKUEN	System PLL clock source update enable
 * 0x070	SYSCON_MAINCLKSEL	Main clock source select
 * 0x074	SYSCON_MAINCLKUEN	Main clock source update enable
 * 0x078	SYSCON_SYSAHBCLKDIV	System clock divider
 * 0x080	SYSCON_SYSAHBCLKCTRL	System clock control
 * 0x094	SYSCON_UARTCLKDIV	USART clock divider
 * 0x0e0	SYSCON_CLKOUTSEL	CLKOUT clock source select
 * 0x0e4	SYSCON_CLKOUTUEN	CLKOUT clock source update enable
 * 0x0e8	SYSCON_CLKOUTDIV	CLKOUT clock divider
 * 0x0f0	SYSCON_UARTFRGDIV	USART1 to USART4 common fractional
 *					generator divider value
 * 0x0f4	SYSCON_UARTFRGMULT	USART1 to USART4 common fractional
 *					generator multiplier value
 * 0x0fc	SYSCON_EXTTRACECMD	External trace buffer command register
 * 0x100	SYSCON_PIOPORCAP0	POR captured PIO status 0
 * 0x134	SYSCON_IOCONCLKDIV6	Peripheral clock 6 to the IOCON block
 *					for programmable glitch filter
 * 0x138	SYSCON_IOCONCLKDIV5	Peripheral clock 5 to the IOCON block
 *					for programmable glitch filter
 * 0x13c	SYSCON_IOCONCLKDIV4	Peripheral clock 4 to the IOCON block
 *					for programmable glitch filter
 * 0x140	SYSCON_IOCONCLKDIV3	Peripheral clock 3 to the IOCON block
 *					for programmable glitch filter
 * 0x144	SYSCON_IOCONCLKDIV2	Peripheral clock 2 to the IOCON block
 *					for programmable glitch filter
 * 0x148	SYSCON_IOCONCLKDIV1	Peripheral clock 1 to the IOCON block
 *					for programmable glitch filter
 * 0x14c	SYSCON_IOCONCLKDIV0	Peripheral clock 0 to the IOCON block
 *					for programmable glitch filter
 * 0x150	SYSCON_BODCTRL		Brown-Out Detect
 * 0x154	SYSCON_SYSTCKCAL	System tick counter calibration
 * 0x170	SYSCON_IRQLATENCY	IRQ delay
 * 0x174	SYSCON_NMISRC		NMI Source Control
 * 0x178	SYSCON_PINTSEL0		GPIO Pin Interrupt Select register 0
 * 0x17c	SYSCON_PINTSEL1		GPIO Pin Interrupt Select register 1
 * 0x180	SYSCON_PINTSEL2		GPIO Pin Interrupt Select register 2
 * 0x184	SYSCON_PINTSEL3		GPIO Pin Interrupt Select register 3
 * 0x188	SYSCON_PINTSEL4		GPIO Pin Interrupt Select register 4
 * 0x18c	SYSCON_PINTSEL5		GPIO Pin Interrupt Select register 5
 * 0x190	SYSCON_PINTSEL6		GPIO Pin Interrupt Select register 6
 * 0x194	SYSCON_PINTSEL7		GPIO Pin Interrupt Select register 7
 * 0x204	SYSCON_STARTERP0	Start logic 0 pin wake-up enable
 *					register
 * 0x214	SYSCON_STARTERP1	Start logic 1 interrupt wake-up enable
 *					register
 * 0x230	SYSCON_PDSLEEPCFG	Power-down states in deep-sleep mode
 * 0x234	SYSCON_PDAWAKECFG	Power-down states for wake-up from
 *					deep-sleep
 * 0x238	SYSCON_PDRUNCFG		Power-down configuration register
 * 0x3f8	SYSCON_DEVICE_ID	Device ID
 */

#define SYSCON_SYSMEMREMAP		MMIO32(SYSCON_BASE + 0x000)
#define SYSCON_PRESETCTRL		MMIO32(SYSCON_BASE + 0x004)
#define SYSCON_SYSPLLCTRL		MMIO32(SYSCON_BASE + 0x008)
#define SYSCON_SYSPLLSTAT		MMIO32(SYSCON_BASE + 0x00c)
#define SYSCON_SYSOSCCTRL		MMIO32(SYSCON_BASE + 0x020)
#define SYSCON_WDTOSCCTRL		MMIO32(SYSCON_BASE + 0x024)
#define SYSCON_SYSRSTSTAT		MMIO32(SYSCON_BASE + 0x030)
#define SYSCON_SYSPLLCLKSEL		MMIO32(SYSCON_BASE + 0x040)
#define SYSCON_SYSPLLCLKUEN		MMIO32(SYSCON_BASE + 0x044)
#define SYSCON_MAINCLKSEL		MMIO32(SYSCON_BASE + 0x070)
#define SYSCON_MAINCLKUEN		MMIO32(SYSCON_BASE + 0x074)
#define SYSCON_SYSAHBCLKDIV		MMIO32(SYSCON_BASE + 0x078)
#define SYSCON_SYSAHBCLKCTRL		MMIO32(SYSCON_BASE + 0x080)
#define SYSCON_UARTCLKDIV		MMIO32(SYSCON_BASE + 0x094)
#define SYSCON_CLKOUTSEL		MMIO32(SYSCON_BASE + 0x0e0)
#define SYSCON_CLKOUTUEN		MMIO32(SYSCON_BASE + 0x0e4)
#define SYSCON_CLKOUTDIV		MMIO32(SYSCON_BASE + 0x0e8)
#define SYSCON_UARTFRGDIV		MMIO32(SYSCON_BASE + 0x0f0)
#define SYSCON_UARTFRGMULT		MMIO32(SYSCON_BASE + 0x0f4)
#define SYSCON_EXTTRACECMD		MMIO32(SYSCON_BASE + 0x0fc)
#define SYSCON_PIOPORCAP0		MMIO32(SYSCON_BASE + 0x100)
#define SYSCON_IOCONCLKDIV6		MMIO32(SYSCON_BASE + 0x134)
#define SYSCON_IOCONCLKDIV5		MMIO32(SYSCON_BASE + 0x138)
#define SYSCON_IOCONCLKDIV4		MMIO32(SYSCON_BASE + 0x13c)
#define SYSCON_IOCONCLKDIV3		MMIO32(SYSCON_BASE + 0x140)
#define SYSCON_IOCONCLKDIV2		MMIO32(SYSCON_BASE + 0x144)
#define SYSCON_IOCONCLKDIV1		MMIO32(SYSCON_BASE + 0x148)
#define SYSCON_IOCONCLKDIV0		MMIO32(SYSCON_BASE + 0x14c)
#define SYSCON_BODCTRL			MMIO32(SYSCON_BASE + 0x150)
#define SYSCON_SYSTCKCAL		MMIO32(SYSCON_BASE + 0x154)
#define SYSCON_IRQLATENCY		MMIO32(SYSCON_BASE + 0x170)
#define SYSCON_NMISRC			MMIO32(SYSCON_BASE + 0x174)
#define SYSCON_PINTSEL0			MMIO32(SYSCON_BASE + 0x178)
#define SYSCON_PINTSEL1			MMIO32(SYSCON_BASE + 0x17c)
#define SYSCON_PINTSEL2			MMIO32(SYSCON_BASE + 0x180)
#define SYSCON_PINTSEL3			MMIO32(SYSCON_BASE + 0x184)
#define SYSCON_PINTSEL4			MMIO32(SYSCON_BASE + 0x188)
#define SYSCON_PINTSEL5			MMIO32(SYSCON_BASE + 0x18c)
#define SYSCON_PINTSEL6			MMIO32(SYSCON_BASE + 0x190)
#define SYSCON_PINTSEL7			MMIO32(SYSCON_BASE + 0x194)
#define SYSCON_STARTERP0		MMIO32(SYSCON_BASE + 0x204)
#define SYSCON_STARTERP1		MMIO32(SYSCON_BASE + 0x214)
#define SYSCON_PDSLEEPCFG		MMIO32(SYSCON_BASE + 0x230)
#define SYSCON_PDAWAKECFG		MMIO32(SYSCON_BASE + 0x234)
#define SYSCON_PDRUNCFG			MMIO32(SYSCON_BASE + 0x238)
#define SYSCON_DEVICE_ID		MMIO32(SYSCON_BASE + 0x3f8)

/* --- SYSCON_SYSMEMREMAP values ------------------------------------------- */

#define SYSCON_SYSMEMREAMP_MAP1		(1 << 1)
#define SYSCON_SYSMEMREAMP_MAP0		(1 << 0)

/* MAP */
#define SYSCON_SYSMEMREAMP_MAP_BOOT	0
#define SYSCON_SYSMEMREAMP_MAP_RAM	1
#define SYSCON_SYSMEMREAMP_MAP_FLASH	2

/* --- SYSCON_PRESETCTRL values -------------------------------------------- */

#define SYSCON_PRESETCTRL_ACMP_RST_N	(1 << 12)
#define SYSCON_PRESETCTRL_FLASH_RST_N	(1 << 11)
#define SYSCON_PRESETCTRL_GPIO_RST_N	(1 << 10)
#define SYSCON_PRESETCTRL_WKT_RST_N	(1 << 9)
#define SYSCON_PRESETCTRL_SCT_RST_N	(1 << 8)
#define SYSCON_PRESETCTRL_MRT_RST_N	(1 << 7)
#define SYSCON_PRESETCTRL_I2C_RST_N	(1 << 6)
#define SYSCON_PRESETCTRL_UART2_RST_N	(1 << 5)
#define SYSCON_PRESETCTRL_UART1_RST_N	(1 << 4)
#define SYSCON_PRESETCTRL_UART0_RST_N	(1 << 3)
#define SYSCON_PRESETCTRL_UARTFRG_RST_N	(1 << 2)
#define SYSCON_PRESETCTRL_SPI1_RST_N	(1 << 1)
#define SYSCON_PRESETCTRL_SPI0_RST_N	(1 << 0)

/* --- SYSCON_SYSPLLCTRL values -------------------------------------------- */

/* SYSCON_SYSPLLCTRL[6:5]: PSEL */
#define SYSCON_SYSPLLCTRL_PSEL_1	(0 << 5)
#define SYSCON_SYSPLLCTRL_PSEL_2	(1 << 5)
#define SYSCON_SYSPLLCTRL_PSEL_4	(2 << 5)
#define SYSCON_SYSPLLCTRL_PSEL_8	(3 << 5)

/* SYSCON_SYSPLLCTRL[4:0]: MSEL */
#define SYSCON_SYSPLLCTRL_MSEL(M)	(((M) - 1) << 0)

/* --- SYSCON_SYSPLLSTAT values -------------------------------------------- */

#define SYSCON_SYSPLLSTAT_LOCK		(1 << 0)

/* --- SYSCON_SYSOSCCTRL values -------------------------------------------- */

#define SYSCON_SYSOSCCTRL_FREQRANGE	(1 << 1)
#define SYSCON_SYSOSCCTRL_BYPASS	(1 << 0)

/* FREQRANGE */
#define SYSCON_SYSOSCCTRL_FREQRANGE_1_20MHZ	(0 << 1)
#define SYSCON_SYSOSCCTRL_FREQRANGE_15_25MHZ	(1 << 1)

/* --- SYSCON_WDTOSCCTRL values -------------------------------------------- */

/* SYSCON_WDTOSCCTRL[4:0]: DIVSEL wdt_osc_clk = Fclkana / (2 * (1 + DIVSEL)) */

#define SYSCON_FREQSEL_0_6MHZ		(1 << 5)
#define SYSCON_FREQSEL_1_05MHZ		(2 << 5)
#define SYSCON_FREQSEL_1_4MHZ		(3 << 5)
#define SYSCON_FREQSEL_1_75MHZ		(4 << 5)
#define SYSCON_FREQSEL_2_1MHZ		(5 << 5)
#define SYSCON_FREQSEL_2_4MHZ		(6 << 5)
#define SYSCON_FREQSEL_2_7MHZ		(7 << 5)
#define SYSCON_FREQSEL_3_0MHZ		(8 << 5)
#define SYSCON_FREQSEL_3_25MHZ		(9 << 5)
#define SYSCON_FREQSEL_3_5MHZ		(10 << 5)
#define SYSCON_FREQSEL_3_75MHZ		(11 << 5)
#define SYSCON_FREQSEL_4_0MHZ		(12 << 5)
#define SYSCON_FREQSEL_4_2MHZ		(13 << 5)
#define SYSCON_FREQSEL_4_4MHZ		(14 << 5)
#define SYSCON_FREQSEL_4_6MHZ		(15 << 5)

/* --- SYSCON_SYSRSTSTAT values -------------------------------------------- */

#define SYSCON_SYSRSTSTAT_SYSRST	(1 << 4)
#define SYSCON_SYSRSTSTAT_BOD		(1 << 3)
#define SYSCON_SYSRSTSTAT_WDT		(1 << 2)
#define SYSCON_SYSRSTSTAT_EXTRST	(1 << 1)
#define SYSCON_SYSRSTSTAT_POR		(1 << 0)

/* --- SYSCON_SYSPLLCLKSEL values ------------------------------------------ */

#define SYSCON_SYSPLLCLKSEL_SEL_IRC	(0 << 0)
#define SYSCON_SYSPLLCLKSEL_SEL_SYSOSC	(1 << 0)
#define SYSCON_SYSPLLCLKSEL_SEL_CLKIN	(3 << 0)

/* --- SYSCON_SYSPLLCLKUEN values ------------------------------------------ */

#define SYSCON_SYSPLLCLKUEN_ENA		(1 << 0)

/* --- SYSCON_MAINCLKSEL values -------------------------------------------- */

#define SYSCON_MAINCLKSEL_SEL_IRC	(0 << 0)
#define SYSCON_MAINCLKSEL_SEL_PLL_IN	(1 << 0)
#define SYSCON_MAINCLKSEL_SEL_WDT_OSC	(2 << 0)
#define SYSCON_MAINCLKSEL_SEL_PLL_OUT	(3 << 0)

/* --- SYSCON_MAINCLKUEN values -------------------------------------------- */

#define SYSCON_MAINCLKUEN_ENA		(1 << 0)

/* --- SYSCON_SYSAHBCLKDIV values ------------------------------------------ */

/* SYSCON_SYSAHBCLKDIV[7:0]: DIV */
#define SYSCON_SYSAHBCLKDIV_DIV_DISABLE	(0 << 0)

/* --- SYSCON_SYSAHBCLKCTRL values ----------------------------------------- */

#define SYSCON_SYSAHBCLKCTRL_ACMP	(1 << 19)
#define SYSCON_SYSAHBCLKCTRL_IOCON	(1 << 18)
#define SYSCON_SYSAHBCLKCTRL_WWDT	(1 << 17)
#define SYSCON_SYSAHBCLKCTRL_UART2	(1 << 16)
#define SYSCON_SYSAHBCLKCTRL_UART1	(1 << 15)
#define SYSCON_SYSAHBCLKCTRL_UART0	(1 << 14)
#define SYSCON_SYSAHBCLKCTRL_CRC	(1 << 13)
#define SYSCON_SYSAHBCLKCTRL_SPI1	(1 << 12)
#define SYSCON_SYSAHBCLKCTRL_SPI0	(1 << 11)
#define SYSCON_SYSAHBCLKCTRL_MRT	(1 << 10)
#define SYSCON_SYSAHBCLKCTRL_WKT	(1 << 9)
#define SYSCON_SYSAHBCLKCTRL_SCT	(1 << 8)
#define SYSCON_SYSAHBCLKCTRL_SWM	(1 << 7)
#define SYSCON_SYSAHBCLKCTRL_GPIO	(1 << 6)
#define SYSCON_SYSAHBCLKCTRL_I2C	(1 << 5)
#define SYSCON_SYSAHBCLKCTRL_FLASH	(1 << 4)
#define SYSCON_SYSAHBCLKCTRL_FLASHREG	(1 << 3)
#define SYSCON_SYSAHBCLKCTRL_RAM	(1 << 2)
#define SYSCON_SYSAHBCLKCTRL_ROM	(1 << 1)
#define SYSCON_SYSAHBCLKCTRL_SYS	(1 << 0)

/* --- SYSCON_UARTCLKDIV values -------------------------------------------- */

/* SYSCON_UARTCLKDIV[7:0]: DIV */
#define SYSCON_UARTCLKDIV_DIV_DISABLE	(0 << 0)

/* --- SYSCON_CLKOUTSEL values --------------------------------------------- */

#define SYSCON_CLKOUTSEL_SEL_IRC	(0 << 0)
#define SYSCON_CLKOUTSEL_SEL_SYSOSC	(1 << 0)
#define SYSCON_CLKOUTSEL_SEL_WDT_OSC	(2 << 0)
#define SYSCON_CLKOUTSEL_SEL_MAIN	(3 << 0)

/* --- SYSCON_CLKOUTUEN values --------------------------------------------- */

#define SYSCON_CLKOUTUEN_ENA		(1 << 0)

/* --- SYSCON_CLKOUTDIV values --------------------------------------------- */

/* SYSCON_CLKOUTDIV[7:0]: DIV */
#define SYSCON_CLKOUTDIV_DIV_DISABLE	(0 << 0)

/* --- SYSCON_UARTFRGDIV values -------------------------------------------- */

/* SYSCON_UARTFRGDIV[7:0]: DIV */

/* --- SYSCON_UARTFRGMULT values ------------------------------------------- */

/* SYSCON_UARTFRGMULT[7:0]: MULT */

/* --- SYSCON_EXTTRACECMD values ------------------------------------------- */

#define SYSCON_EXTTRACECMD_START	(1 << 0)
#define SYSCON_EXTTRACECMD_STOP		(1 << 1)

/* --- SYSCON_PIOPORCAP0 values -------------------------------------------- */

/* SYSCON_PIOPORCAP0[17:0]: PIOSTAT */

/* --- SYSCON_IOCONCLKDIV[6:0] values -------------------------------------- */

/* SYSCON_IOCONCLKDIVx[7:0]: DIV */
#define SYSCON_IOCONCLKDIV_DIV_DISABLE	(0 << 0)

/* --- SYSCON_BODCTRL values ----------------------------------------------- */

#define SYSCON_BODCTRL_BODRSTENA	(1 << 4)
/* SYSCON_BODCTRL[3:2]: BODINTVAL */
#define SYSCON_BODCTRL_BODINTVAL_1	(1 << 2)
#define SYSCON_BODCTRL_BODINTVAL_2	(2 << 2)
#define SYSCON_BODCTRL_BODINTVAL_3	(3 << 2)
/* SYSCON_BODCTRL[1:0]: BODRSTLEV */
#define SYSCON_BODCTRL_BODRSTLEV_1	(1 << 0)
#define SYSCON_BODCTRL_BODRSTLEV_2	(2 << 0)
#define SYSCON_BODCTRL_BODRSTLEV_3	(3 << 0)

/* --- SYSCON_TCKCAL values ------------------------------------------------ */

/* SYSCON_TCKCAL[25:0]: CAL */

/* --- SYSCON_IRQLATENCY values -------------------------------------------- */

/* SYSCON_IRQLATENCY[7:0]: LATENCY */

/* --- SYSCON_NMISRC values ------------------------------------------------ */

#define SYSCON_NMISRC_NMIEN	(1 << 31)
/* SYSCON_NMISRC[4:0]: IRQN */

/* --- SYSCON_PINTSEL[0:7] values ------------------------------------------ */

/* SYSCON_PINTSELx[5:0]: INTPIN */

/* --- SYSCON_STARTERP0 values --------------------------------------------- */

#define SYSCON_STARTERP0_PINT7	(1 << 7)
#define SYSCON_STARTERP0_PINT6	(1 << 6)
#define SYSCON_STARTERP0_PINT5	(1 << 5)
#define SYSCON_STARTERP0_PINT4	(1 << 4)
#define SYSCON_STARTERP0_PINT3	(1 << 3)
#define SYSCON_STARTERP0_PINT2	(1 << 2)
#define SYSCON_STARTERP0_PINT1	(1 << 1)
#define SYSCON_STARTERP0_PINT0	(1 << 0)

/* --- SYSCON_STARTERP1 values --------------------------------------------- */

#define SYSCON_STARTERP1_WKT	(1 << 15)
#define SYSCON_STARTERP1_BOD	(1 << 13)
#define SYSCON_STARTERP1_WWDT	(1 << 12)
#define SYSCON_STARTERP1_I2C	(1 << 8)
#define SYSCON_STARTERP1_USART2	(1 << 5)
#define SYSCON_STARTERP1_USART1	(1 << 4)
#define SYSCON_STARTERP1_USART0	(1 << 3)
#define SYSCON_STARTERP1_SPI1	(1 << 1)
#define SYSCON_STARTERP1_SPI0	(1 << 0)

/* --- SYSCON_PDSLEEPCFG values -------------------------------------------- */

#define SYSCON_PDSLEEPCFG_WDTOSC_PD	(1 << 6)
#define SYSCON_PDSLEEPCFG_BOD_PD	(1 << 3)

/* --- SYSCON_PDAWAKECFG values -------------------------------------------- */

#define SYSCON_PDAWAKECFG_ACMP		(1 << 15)
#define SYSCON_PDAWAKECFG_SYSPLL_PD	(1 << 7)
#define SYSCON_PDAWAKECFG_WDTOSC_PD	(1 << 6)
#define SYSCON_PDAWAKECFG_SYSOSC_PD	(1 << 5)
#define SYSCON_PDAWAKECFG_BOD_PD	(1 << 3)
#define SYSCON_PDAWAKECFG_FLASH_PD	(1 << 2)
#define SYSCON_PDAWAKECFG_IRC_PD	(1 << 1)
#define SYSCON_PDAWAKECFG_IRCOUT_PD	(1 << 0)

/* --- SYSCON_PDRUNCFG values ---------------------------------------------- */

#define SYSCON_PDRUNCFG_ACMP		(1 << 15)
#define SYSCON_PDRUNCFG_SYSPLL_PD	(1 << 7)
#define SYSCON_PDRUNCFG_WDTOSC_PD	(1 << 6)
#define SYSCON_PDRUNCFG_SYSOSC_PD	(1 << 5)
#define SYSCON_PDRUNCFG_BOD_PD		(1 << 3)
#define SYSCON_PDRUNCFG_FLASH_PD	(1 << 2)
#define SYSCON_PDRUNCFG_IRC_PD		(1 << 1)
#define SYSCON_PDRUNCFG_IRCOUT_PD	(1 << 0)

/* --- SYSCON_DEVICE_ID values --------------------------------------------- */

/* SYSCON_DEVICE_ID[31:0]: DEVICEID */
#define SYSCON_DEVICE_ID_LPC810M021FN8		0x00008100
#define SYSCON_DEVICE_ID_LPC811M001JDH16	0x00008110
#define SYSCON_DEVICE_ID_LPC812M101JDH16	0x00008120
#define SYSCON_DEVICE_ID_LPC812M101JD20		0x00008121
#define SYSCON_DEVICE_ID_LPC812M101JDH20	0x00008122
#define SYSCON_DEVICE_ID_LPC812M101JTB16	0x00008122

/* --- Function prototypes ------------------------------------------------- */

/* System memory remap */
typedef enum {
	SYSCON_BOOT_LOADER_MODE,
	SYSCON_USER_RAM_MODE,
	SYSCON_USER_FLASH_MODE
} syscon_map_t;

/* System oscillator frequency range */
typedef enum {
	SYSCON_1_20MHZ = 0,
	SYSCON_15_25MHZ = 2
} syscon_range_t;

/* Watchdog oscillator analog output frequency */
typedef enum {
	SYSCON_0_6MHZ = (1 << 5),
	SYSCON_1_05MHZ = (2 << 5),
	SYSCON_1_4MHZ = (3 << 5),
	SYSCON_1_75MHZ = (4 << 5),
	SYSCON_2_1MHZ = (5 << 5),
	SYSCON_2_4MHZ = (6 << 5),
	SYSCON_2_7MHZ = (7 << 5),
	SYSCON_3_0MHZ = (8 << 5),
	SYSCON_3_25MHZ = (9 << 5),
	SYSCON_3_5MHZ = (10 << 5),
	SYSCON_3_75MHZ = (11 << 5),
	SYSCON_4_0MHZ = (12 << 5),
	SYSCON_4_2MHZ = (13 << 5),
	SYSCON_4_4MHZ = (14 << 5),
	SYSCON_4_6MHZ = (15 << 5)
} syscon_fclkana_t;

/* Reset */
enum {
	SYSCON_POR = (1 << 0),
	SYSCON_EXTRST = (1 << 1),
	SYSCON_WDT_RST = (1 << 2),
	SYSCON_BOD = (1 << 3),
	SYSCON_SYSRST = (1 << 4)
};

/* Oscillator */
typedef enum {
	SYSCON_IRC,
	SYSCON_XTAL,
	SYSCON_CLKIN,
	SYSCON_WDT_OSC,
	SYSCON_PLL_IN,
	SYSCON_PLL_OUT,
	SYSCON_MAIN
} syscon_osc_t;

/* Peripheral */
enum {
	SYSCON_UARTFRG = (1 << 20),
	SYSCON_ACMP = (1 << 19),
	SYSCON_IOCON = (1 << 18),
	SYSCON_WWDT = (1 << 17),
	SYSCON_UART2 = (1 << 16),
	SYSCON_UART1 = (1 << 15),
	SYSCON_UART0 = (1 << 14),
	SYSCON_CRC = (1 << 13),
	SYSCON_SPI1 = (1 << 12),
	SYSCON_SPI0 = (1 << 11),
	SYSCON_MRT = (1 << 10),
	SYSCON_WKT = (1 << 9),
	SYSCON_SCT = (1 << 8),
	SYSCON_SWM = (1 << 7),
	SYSCON_GPIO = (1 << 6),
	SYSCON_I2C = (1 << 5),
	SYSCON_FLASH = (1 << 4),
	SYSCON_FLASHREG = (1 << 3),
	SYSCON_RAM = (1 << 2),
	SYSCON_ROM = (1 << 1),
	SYSCON_SYS = (1 << 0)
};

/* External trace buffer command */
typedef enum {
	SYSCON_TSTART = (1 << 0),
	SYSCON_TSTOP = (1 << 1)
} syscon_exttracecmd_t;

/* Interrupt */
enum {
	SYSCON_PINT0 = (1 << 0),
	SYSCON_PINT1 = (1 << 1),
	SYSCON_PINT2 = (1 << 2),
	SYSCON_PINT3 = (1 << 3),
	SYSCON_PINT4 = (1 << 4),
	SYSCON_PINT5 = (1 << 5),
	SYSCON_PINT6 = (1 << 6),
	SYSCON_PINT7 = (1 << 7),

	SYSCON_SPI0_IRQ = (1 << 8),
	SYSCON_SPI1_IRQ = (1 << 9),
	SYSCON_USART0_IRQ = (1 << 11),
	SYSCON_USART1_IRQ = (1 << 12),
	SYSCON_USART2_IRQ = (1 << 13),
	SYSCON_I2C_IRQ = (1 << 16),
	SYSCON_WWDT_IRQ = (1 << 20),
	SYSCON_BOD_IRQ = (1 << 21),
	SYSCON_WKT_IRQ = (1 << 23)
};

/* Power down */
typedef enum {
	SYSCON_IRCOUT_PD = (1 << 0),
	SYSCIN_IRC_PD = (1 << 1),
	SYSCON_FLASH_PD = (1 << 2),
	SYSCON_BOD_PD = (1 << 3),
	SYSCON_SYSOSC_PD = (1 << 5),
	SYSCON_WDTOSC_PD = (1 << 6),
	SYSCON_SYSPLL_PD = (1 << 7),
	SYSCON_ACMP_PD = (1 << 15)
} syscon_power_down_t;

void syscon_set_system_memory_remap(syscon_map_t map);
syscon_map_t syscon_get_system_memory_remap(void);
void syscon_disable_reset(int peripheral);
void syscon_enable_reset(int peripheral);
void syscon_enable_pll(syscon_osc_t source, int m, int p);
void syscon_disable_pll(void);
void syscon_enable_sys_osc(bool bypass, syscon_range_t freq);
void syscon_disable_sys_osc(void);
void syscon_enable_wdt_osc(syscon_fclkana_t fclkana, int divsel);
void syscon_disable_wdt_osc(void);
int syscon_get_reset_status(int reset);
void syscon_clear_reset_status(int reset);
void syscon_set_system_clock(syscon_osc_t source, int div);
void syscon_enable_clock(int peripheral);
void syscon_disable_clock(int peripheral);
void syscon_set_usart_clock(int main_clock, int u_pclk);
void syscon_set_clockout(syscon_osc_t source, int div);
void syscon_set_external_trace_buffer_command(syscon_exttracecmd_t command);
int syscon_get_gpio_status_at_por(void);
void syscon_set_ioconclkdiv(int num, int div);
void syscon_set_bod_interrupt(int level);
void syscon_enable_bod_reset(int level);
void syscon_disable_bod_reset(void);
int syscon_get_systick_calibration(void);
void syscon_set_irq_latency(int latency);
void syscon_enable_nmi(int source);
void syscon_disable_nmi(void);
void syscon_select_pins(int offset, int pins);
void syscon_set_wakeup_interrupt(int interrupt);
void syscon_disable_deep_sleep_power_down(syscon_power_down_t pd);
void syscon_enable_deep_sleep_power_down(syscon_power_down_t pd);
void syscon_disable_wakeup_power_down(syscon_power_down_t pd);
void syscon_enable_wakeup_power_down(syscon_power_down_t pd);
void syscon_disable_power_down(syscon_power_down_t pd);
void syscon_enable_power_down(syscon_power_down_t pd);
int syscon_get_device_id(void);
