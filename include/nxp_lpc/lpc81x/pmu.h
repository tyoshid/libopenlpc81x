/*
 * Power Management Unit (PMU)
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
 * Chapter 5: LPC81x Reduced power modes and Power Management Unit (PMU)
 */

#include <mmio.h>
#include <memorymap.h>

/* --- PMU registers ------------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	PMU_PCON		Power control register
 * 0x004	PMU_GPREG0		General purpose register 0
 * 0x008	PMU_GPREG1		General purpose register 1
 * 0x00c	PMU_GPREG2		General purpose register 2
 * 0x010	PMU_GPREG3		General purpose register 3
 * 0x014	PMU_DPDCTRL		Deep power-down control register
 */

#define PMU_PCON			MMIO32(PMU_BASE + 0x000)

#define PMU_GPREG(n)			MMIO32(PMU_BASE + 0x004 + (n) * 4)
#define PMU_GPREG0			PMU_GPREG(0)
#define PMU_GPREG1			PMU_GPREG(1)
#define PMU_GPREG2			PMU_GPREG(2)
#define PMU_GPREG3			PMU_GPREG(3)

#define PMU_DPDCTRL			MMIO32(PMU_BASE + 0x014)

/* --- PMU_PCON values ----------------------------------------------------- */

#define PMU_PCON_DPDFLAG		(1 << 11)
#define PMU_PCON_SLEEPFLAG		(1 << 8)
#define PMU_PCON_NODPD			(1 << 3)
#define PMU_PCON_PM2			(1 << 2)
#define PMU_PCON_PM1			(1 << 1)
#define PMU_PCON_PM0			(1 << 0)

/* Power mode */
#define PMU_PCON_PM_DEFAULT		(0 << 0)
#define PMU_PCON_PM_DEEP_SLEEP		(1 << 0)
#define PMU_PCON_PM_POWER_DOWN		(2 << 0)
#define PMU_PCON_PM_DEEP_POWER_DOWN	(3 << 0)

/* --- PMU_GPREGx values --------------------------------------------------- */

/* PMU_GPREGx[31:0]: GPDATA */

/* --- PMU_DPDCTRL values -------------------------------------------------- */

#define PMU_DPDCTRL_LPOSCDPDEN		(1 << 3)
#define PMU_DPDCTRL_LPOSCEN		(1 << 2)
#define PMU_DPDCTRL_WAKEPAD_DISABLE	(1 << 1)
#define PMU_DPDCTRL_WAKEUPHYS		(1 << 0)

/* --- Function prototypes ------------------------------------------------- */

/* Power mode */
typedef enum {
	PMU_DEFAULT,
	PMU_DEEP_SLEEP,
	PMU_POWER_DOWN,
	PMU_DEEP_POWER_DOWN
} pmu_power_mode_t;

/* Flag */
enum {
	PMU_SLEEPFLAG = (1 << 8),
	PMU_DPDFLAG = (1 << 11)
};

void pmu_set_power_mode(pmu_power_mode_t pm);
pmu_power_mode_t pmu_get_power_mode(void);
void pmu_prevent_deep_power_down(void);
int pmu_get_flag(int flag);
void pmu_clear_flag(int flag);
void pmu_enable_low_power_osc(bool deep_power_down);
void pmu_disable_low_power_osc(void);
void pmu_disable_wakeup_pin(void);
void pmu_enable_wakeup_pin(bool hysteresis);
