/*
 * SYSCON functions
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

void syscon_set_system_memory_remap(syscon_map_t map)
{
	SYSCON_SYSMEMREMAP = map;
}

syscon_map_t syscon_get_system_memory_remap(void)
{
	return SYSCON_SYSMEMREMAP & 3;
}

static int peripheral_to_reset(int peripheral)
{
	int reset;
	int i;
	static const int peripheral_reset[] = {
		0,
		0,
		0,
		0,
		SYSCON_PRESETCTRL_FLASH_RST_N,
		SYSCON_PRESETCTRL_I2C_RST_N,
		SYSCON_PRESETCTRL_GPIO_RST_N,
		0,
		SYSCON_PRESETCTRL_SCT_RST_N,
		SYSCON_PRESETCTRL_WKT_RST_N,
		SYSCON_PRESETCTRL_MRT_RST_N,
		SYSCON_PRESETCTRL_SPI0_RST_N,
		SYSCON_PRESETCTRL_SPI1_RST_N,
		0,
		SYSCON_PRESETCTRL_UART0_RST_N,
		SYSCON_PRESETCTRL_UART1_RST_N,
		SYSCON_PRESETCTRL_UART2_RST_N,
		0,
		0,
		SYSCON_PRESETCTRL_ACMP_RST_N,
		SYSCON_PRESETCTRL_UARTFRG_RST_N
	};

	reset = 0;
	for (i = 0; i < (int)(sizeof(peripheral_reset) / sizeof(int)); i++) {
		if (peripheral & (1 << i))
			reset |= peripheral_reset[i];
	}
	return reset;
}

void syscon_disable_reset(int peripheral)
{
	SYSCON_PRESETCTRL |= peripheral_to_reset(peripheral);
}

void syscon_enable_reset(int peripheral)
{
	SYSCON_PRESETCTRL &= ~peripheral_to_reset(peripheral);
}

void syscon_enable_pll(syscon_osc_t source, int m, int p)
{
	int osc;
	int psel;
	int msel;

	switch (source) {
	case SYSCON_IRC:
		osc = SYSCON_SYSPLLCLKSEL_SEL_IRC;
		break;
	case SYSCON_XTAL:
		osc = SYSCON_SYSPLLCLKSEL_SEL_SYSOSC;
		break;
	case SYSCON_CLKIN:
		osc = SYSCON_SYSPLLCLKSEL_SEL_CLKIN;
		break;
	default:
		return;
	}

	for (psel = 0; psel < 4 && p != 1 << psel; psel++)
		;
	if (psel >= 4)
		return;

	if (m < 1 || m > 32)
		return;
	msel = m - 1;

	SYSCON_PDRUNCFG &= ~SYSCON_PDRUNCFG_SYSPLL_PD;

	SYSCON_SYSPLLCLKSEL = osc;

	SYSCON_SYSPLLCLKUEN = 0;
	SYSCON_SYSPLLCLKUEN = SYSCON_SYSPLLCLKUEN_ENA;

	SYSCON_SYSPLLCTRL = psel << 5 | msel << 0;

	while (!(SYSCON_SYSPLLSTAT & SYSCON_SYSPLLSTAT_LOCK))
		;
}

void syscon_disable_pll(void)
{
	SYSCON_SYSPLLCTRL = 0;
	SYSCON_PDRUNCFG |= SYSCON_PDRUNCFG_SYSPLL_PD;
}

void syscon_enable_sys_osc(bool bypass, syscon_range_t freq)
{
	SYSCON_PDRUNCFG &= ~SYSCON_PDRUNCFG_SYSOSC_PD;
	SYSCON_SYSOSCCTRL = freq | (bypass ? 1 : 0);
}

void syscon_disable_sys_osc(void)
{
	SYSCON_PDRUNCFG |= SYSCON_PDRUNCFG_SYSOSC_PD;
}

void syscon_enable_wdt_osc(syscon_fclkana_t fclkana, int divsel)
{
	SYSCON_PDRUNCFG &= ~SYSCON_PDRUNCFG_WDTOSC_PD;

	/* wdt_osc_clk = Fclkana / (2 * (1 + DIVSEL)) */
	SYSCON_WDTOSCCTRL = fclkana | (divsel & 0x1f);
}

void syscon_disable_wdt_osc(void)
{
	SYSCON_WDTOSCCTRL = 0;
	SYSCON_PDRUNCFG |= SYSCON_PDRUNCFG_WDTOSC_PD;
}

int syscon_get_reset_status(int reset)
{
	return SYSCON_SYSRSTSTAT & reset;
}

void syscon_clear_reset_status(int reset)
{
	SYSCON_SYSRSTSTAT = reset;
}

void syscon_set_system_clock(syscon_osc_t source, int div)
{
	int osc;

	switch (source) {
	case SYSCON_IRC:
		osc = SYSCON_MAINCLKSEL_SEL_IRC;
		break;
	case SYSCON_PLL_IN:
		osc = SYSCON_MAINCLKSEL_SEL_PLL_IN;
		break;
	case SYSCON_WDT_OSC:
		osc = SYSCON_MAINCLKSEL_SEL_WDT_OSC;
		break;
	case SYSCON_PLL_OUT:
		osc = SYSCON_MAINCLKSEL_SEL_PLL_OUT;
		break;
	default:
		return;
	}

	if (div < 0 || div > 255)
		return;

	SYSCON_MAINCLKSEL = osc;

	SYSCON_MAINCLKUEN = 0;
	SYSCON_MAINCLKUEN = SYSCON_MAINCLKUEN_ENA;

	SYSCON_SYSAHBCLKDIV = div;
}

void syscon_enable_clock(int peripheral)
{
	SYSCON_SYSAHBCLKCTRL |= (peripheral & 0xfffff);
}

void syscon_disable_clock(int peripheral)
{
	SYSCON_SYSAHBCLKCTRL &= ~(peripheral & 0xfffff);
}

void syscon_set_usart_clock(int main_clock, int u_pclk)
{
	int n;
	int t;
	int mult;

	/*
	 * U_PCLK = main clock / UARTCLKDIV / (1 + MULT / 256)
	 * MULT = 256 * (main clock / (UARTCLKDIV * U_PCLK) - 1)
	 */
	if (u_pclk <= 0)
		return;
	n = main_clock / u_pclk;
	if (n > 255 || n <= 0)
		return;
	t = u_pclk * n;
	mult = 256 * (main_clock - t) / t;

	SYSCON_UARTCLKDIV = n;
	if (mult) {
		SYSCON_UARTFRGDIV = 0xff;
		SYSCON_UARTFRGMULT = mult;
	} else {
		SYSCON_UARTFRGDIV = 0;
		SYSCON_UARTFRGMULT = 0;
	}
}

void syscon_set_clockout(syscon_osc_t source, int div)
{
	int osc;

	switch (source) {
	case SYSCON_IRC:
		osc = SYSCON_CLKOUTSEL_SEL_IRC;
		break;
	case SYSCON_XTAL:
		osc = SYSCON_CLKOUTSEL_SEL_SYSOSC;
		break;
	case SYSCON_WDT_OSC:
		osc = SYSCON_CLKOUTSEL_SEL_WDT_OSC;
		break;
	case SYSCON_MAIN:
		osc = SYSCON_CLKOUTSEL_SEL_MAIN;
		break;
	default:
		return;
	}

	if (div < 0 || div > 255)
		return;

	SYSCON_CLKOUTSEL = osc;

	SYSCON_CLKOUTUEN = 0;
	SYSCON_CLKOUTUEN = SYSCON_CLKOUTUEN_ENA;

	SYSCON_CLKOUTDIV = div;
}

void syscon_set_external_trace_buffer_command(syscon_exttracecmd_t command)
{
	SYSCON_EXTTRACECMD = command;
}

int syscon_get_gpio_status_at_por(void)
{
	return SYSCON_PIOPORCAP0 & 0x3ffff;
}

void syscon_set_ioconclkdiv(int num, int div)
{
	static volatile u32 * const reg[] = {
		&SYSCON_IOCONCLKDIV0,
		&SYSCON_IOCONCLKDIV1,
		&SYSCON_IOCONCLKDIV2,
		&SYSCON_IOCONCLKDIV3,
		&SYSCON_IOCONCLKDIV4,
		&SYSCON_IOCONCLKDIV5,
		&SYSCON_IOCONCLKDIV6
	};

	if (num > 6 || num < 0)
		return;
	*reg[num] = div;
}

void syscon_set_bod_interrupt(int level)
{
	int r;

	r = SYSCON_BODCTRL;
	r &= ~0xc;
	SYSCON_BODCTRL = r | ((level & 3) << 2);
}

void syscon_enable_bod_reset(int level)
{
	int r;

	r = SYSCON_BODCTRL;
	r &= ~(SYSCON_BODCTRL_BODRSTENA | 3);
	SYSCON_BODCTRL = r | SYSCON_BODCTRL_BODRSTENA | (level & 3);
}

void syscon_disable_bod_reset(void)
{
	SYSCON_BODCTRL &= ~SYSCON_BODCTRL_BODRSTENA;
}

int syscon_get_systick_calibration(void)
{
	return SYSCON_SYSTCKCAL & 0x03ffffff;
}

void syscon_set_irq_latency(int latency)
{
	SYSCON_IRQLATENCY = latency & 0xff;
}

void syscon_enable_nmi(int source)
{
	SYSCON_NMISRC = SYSCON_NMISRC_NMIEN | (source & 0x1f);
}

void syscon_disable_nmi(void)
{
	SYSCON_NMISRC &= ~SYSCON_NMISRC_NMIEN;
}

void syscon_select_pins(int offset, int pins)
{
	int i;
	int j;
	static volatile u32 * const reg[] = {
		&SYSCON_PINTSEL0,
		&SYSCON_PINTSEL1,
		&SYSCON_PINTSEL2,
		&SYSCON_PINTSEL3,
		&SYSCON_PINTSEL4,
		&SYSCON_PINTSEL5,
		&SYSCON_PINTSEL6,
		&SYSCON_PINTSEL7
	};

	j = offset;
	if (j >= 8)
		return;
	for (i = 0; i < 18; i++) {
		if (pins & (1 << i)) {
			*reg[j++] = i;
			if (j >= 8)
				break;
		}
	}
}

void syscon_set_wakeup_interrupt(int interrupt)
{
	SYSCON_STARTERP0 = interrupt & 0xff;
	SYSCON_STARTERP1 = (interrupt >> 8) & 0xb13b;
}

void syscon_disable_deep_sleep_power_down(syscon_power_down_t pd)
{
	SYSCON_PDSLEEPCFG &= ~(pd & (SYSCON_PDSLEEPCFG_BOD_PD |
				     SYSCON_PDSLEEPCFG_WDTOSC_PD));
}

void syscon_enable_deep_sleep_power_down(syscon_power_down_t pd)
{
	SYSCON_PDSLEEPCFG |= pd & (SYSCON_PDSLEEPCFG_BOD_PD |
				   SYSCON_PDSLEEPCFG_WDTOSC_PD);
}

void syscon_disable_wakeup_power_down(syscon_power_down_t pd)
{
	SYSCON_PDAWAKECFG &= ~pd;
}

void syscon_enable_wakeup_power_down(syscon_power_down_t pd)
{
	SYSCON_PDAWAKECFG |= pd;
}

void syscon_disable_power_down(syscon_power_down_t pd)
{
	SYSCON_PDRUNCFG &= ~pd;
}

void syscon_enable_power_down(syscon_power_down_t pd)
{
	SYSCON_PDRUNCFG |= pd;
}

int syscon_get_device_id(void)
{
	return SYSCON_DEVICE_ID;
}
