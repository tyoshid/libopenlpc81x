/*
 * PMU functions
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

#include <pmu.h>

void pmu_set_power_mode(pmu_power_mode_t pm)
{
	int r;

	r = PMU_PCON;
	r &= ~3;
	PMU_PCON = r | pm;
}

pmu_power_mode_t pmu_get_power_mode(void)
{
	return PMU_PCON & 3;
}

void pmu_prevent_deep_power_down(void)
{
	PMU_PCON |= PMU_PCON_NODPD;
}

int pmu_get_flag(int flag)
{
	return PMU_PCON & flag;
}

void pmu_clear_flag(int flag)
{
	PMU_PCON |= flag & (PMU_PCON_SLEEPFLAG | PMU_PCON_DPDFLAG);
}

void pmu_enable_low_power_osc(bool deep_power_down)
{
	int r;

	r = PMU_DPDCTRL;
	if (deep_power_down)
		r |= PMU_DPDCTRL_LPOSCDPDEN;
	else
		r &= ~PMU_DPDCTRL_LPOSCDPDEN;
	PMU_DPDCTRL = r | PMU_DPDCTRL_LPOSCEN;
}

void pmu_disable_low_power_osc(void)
{
	PMU_DPDCTRL &= ~(PMU_DPDCTRL_LPOSCEN | PMU_DPDCTRL_LPOSCDPDEN);
}

void pmu_disable_wakeup_pin(void)
{
	int r;

	r = PMU_DPDCTRL;
	r &= ~PMU_DPDCTRL_WAKEUPHYS;
	PMU_DPDCTRL = r | PMU_DPDCTRL_WAKEPAD_DISABLE;
}

void pmu_enable_wakeup_pin(bool hysteresis)
{
	int r;

	r = PMU_DPDCTRL;
	if (hysteresis)
		r |= PMU_DPDCTRL_WAKEUPHYS;
	else
		r &= ~PMU_DPDCTRL_WAKEUPHYS;
	PMU_DPDCTRL = r & ~PMU_DPDCTRL_WAKEPAD_DISABLE;
}
