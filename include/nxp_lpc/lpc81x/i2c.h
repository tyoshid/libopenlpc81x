/*
 * I2C-bus interface
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
 * Chapter 16: LPC81x I2C-bus interface
 */

#include <mmio.h>
#include <memorymap.h>

/* --- I2C registers ------------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	I2C_CFG			Configuration register
 * 0x004	I2C_STAT		Status register
 * 0x008	I2C_INTENSET		Interrupt Enable Set and read register
 * 0x00c	I2C_INTENCLR		Interrupt Enable Clear register
 * 0x010	I2C_TIMEOUT		Time-out value register
 * 0x014	I2C_CLKDIV		Clock Divider register
 * 0x018	I2C_INTSTAT		Interrupt Status register
 * 0x020	I2C_MSTCTL		Master Control register
 * 0x024	I2C_MSTTIME		Master Time register
 * 0x028	I2C_MSTDAT		Master Data register
 * 0x040	I2C_SLVCTL		Slave Control register
 * 0x044	I2C_SLVDAT		Slave Data register
 * 0x048	I2C_SLVADR0		Slave Address register 0
 * 0x04c	I2C_SLVADR1		Slave Address register 1
 * 0x050	I2C_SLVADR2		Slave Address register 2
 * 0x054	I2C_SLVADR3		Slave Address register 3
 * 0x058	I2C_SLVQUAL0		Slave address Qualifier 0 register
 * 0x080	I2C_MONRXDAT		Monitor receiver data register
 */

#define I2C_CFG				MMIO32(I2C_BASE + 0x000)
#define I2C_STAT			MMIO32(I2C_BASE + 0x004)
#define I2C_INTENSET			MMIO32(I2C_BASE + 0x008)
#define I2C_INTENCLR			MMIO32(I2C_BASE + 0x00c)
#define I2C_TIMEOUT			MMIO32(I2C_BASE + 0x010)
#define I2C_CLKDIV			MMIO32(I2C_BASE + 0x014)
#define I2C_INTSTAT			MMIO32(I2C_BASE + 0x018)
#define I2C_MSTCTL			MMIO32(I2C_BASE + 0x020)
#define I2C_MSTTIME			MMIO32(I2C_BASE + 0x024)
#define I2C_MSTDAT			MMIO32(I2C_BASE + 0x028)
#define I2C_SLVCTL			MMIO32(I2C_BASE + 0x040)
#define I2C_SLVDAT			MMIO32(I2C_BASE + 0x044)
#define I2C_SLVADR(n)			MMIO32(I2C_BASE + 0x048 + (n) * 4)
#define I2C_SLVADR0			I2C_SLVADR(0)
#define I2C_SLVADR1			I2C_SLVADR(1)
#define I2C_SLVADR2			I2C_SLVADR(2)
#define I2C_SLVADR3			I2C_SLVADR(3)
#define I2C_SLVQUAL0			MMIO32(I2C_BASE + 0x058)
#define I2C_MONRXDAT			MMIO32(I2C_BASE + 0x080)

/* --- I2C_CFG values ------------------------------------------------------ */

#define I2C_CFG_MONCLKSTR		(1 << 4)
#define I2C_CFG_TIMEOUTEN		(1 << 3)
#define I2C_CFG_MONEN			(1 << 2)
#define I2C_CFG_SLVEN			(1 << 1)
#define I2C_CFG_MSTEN			(1 << 0)

/* --- I2C_STAT values ----------------------------------------------------- */

#define I2C_STAT_SCLTIMEOUT		(1 << 25)
#define I2C_STAT_EVENTTIMEOUT		(1 << 24)
#define I2C_STAT_MONIDLE		(1 << 19)
#define I2C_STAT_MONACTIVE		(1 << 18)
#define I2C_STAT_MONOV			(1 << 17)
#define I2C_STAT_MONRDY			(1 << 16)
#define I2C_STAT_SLVDESEL		(1 << 15)
#define I2C_STAT_SLVSEL			(1 << 14)
#define I2C_STAT_SLVIDX1		(1 << 13)
#define I2C_STAT_SLVIDX0		(1 << 12)
#define I2C_STAT_SLVNOTSTR		(1 << 11)
#define I2C_STAT_SLVSTATE1		(1 << 10)
#define I2C_STAT_SLVSTATE0		(1 << 9)
#define I2C_STAT_SLVPENDING		(1 << 8)
#define I2C_STAT_MSTSTSTPERR		(1 << 6)
#define I2C_STAT_MSTARBLOSS		(1 << 4)
#define I2C_STAT_MSTSTATE2		(1 << 3)
#define I2C_STAT_MSTSTATE1		(1 << 2)
#define I2C_STAT_MSTSTATE0		(1 << 1)
#define I2C_STAT_MSTPENDING		(1 << 0)

/* Slave address match index */
#define I2C_STAT_SLVIDX_ADDR0		(0 << 12)
#define I2C_STAT_SLVIDX_ADDR1		(1 << 12)
#define I2C_STAT_SLVIDX_ADDR2		(2 << 12)
#define I2C_STAT_SLVIDX_ADDR3		(3 << 12)

/* Slave State code */
#define I2C_STAT_SLVSTATE_ADDR		(0 << 9)
#define I2C_STAT_SLVSTATE_RX		(1 << 9)
#define I2C_STAT_SLVSTATE_TX		(2 << 9)

/* Master State code */
#define I2C_STAT_MSTSTATE_IDLE		(0 << 1)
#define I2C_STAT_MSTSTATE_RX_READY	(1 << 1)
#define I2C_STAT_MSTSTATE_TX_READY	(2 << 1)
#define I2C_STAT_MSTSTATE_NACK_ADDR	(3 << 1)
#define I2C_STAT_MSTSTATE_NACK_DATA	(4 << 1)

/* --- I2C_INTENSET values ------------------------------------------------- */

#define I2C_INTENSET_SCLTIMEOUTEN	(1 << 25)
#define I2C_INTENSET_EVENTTIMEOUTEN	(1 << 24)
#define I2C_INTENSET_MONIDLEEN		(1 << 19)
#define I2C_INTENSET_MONOVEN		(1 << 17)
#define I2C_INTENSET_MONRDYEN		(1 << 16)
#define I2C_INTENSET_SLVDESELEN		(1 << 15)
#define I2C_INTENSET_SLVNOTSTREN	(1 << 11)
#define I2C_INTENSET_SLVPENDINGEN	(1 << 8)
#define I2C_INTENSET_MSTSTSTPERREN	(1 << 6)
#define I2C_INTENSET_MSTARBLOSSEN	(1 << 4)
#define I2C_INTENSET_MSTPENDINGEN	(1 << 0)

/* --- I2C_INTENCLR values ------------------------------------------------- */

#define I2C_INTENCLR_SCLTIMEOUTCLR	(1 << 25)
#define I2C_INTENCLR_EVENTTIMEOUTCLR	(1 << 24)
#define I2C_INTENCLR_MONIDLECLR		(1 << 19)
#define I2C_INTENCLR_MONOVCLR		(1 << 17)
#define I2C_INTENCLR_MONRDYCLR		(1 << 16)
#define I2C_INTENCLR_SLVDESELCLR	(1 << 15)
#define I2C_INTENCLR_SLVNOTSTRCLR	(1 << 11)
#define I2C_INTENCLR_SLVPENDINGCLR	(1 << 8)
#define I2C_INTENCLR_MSTSTSTPERRCLR	(1 << 6)
#define I2C_INTENCLR_MSTARBLOSSCLR	(1 << 4)
#define I2C_INTENCLR_MSTPENDINGCLR	(1 << 0)

/* --- I2C_TIMEOUT values -------------------------------------------------- */

/* I2C_TIMEOUT[15:4]: TO */
/* I2C_TIMEOUT[3:0]: 0xf */

/* --- I2C_CLKDIV values --------------------------------------------------- */

/* I2C_CLKDIV[15:0]: DIVVAL */

/* --- I2C_INTSTAT values -------------------------------------------------- */

#define I2C_INTSTAT_SCLTIMEOUT		(1 << 25)
#define I2C_INTSTAT_EVENTTIMEOUT	(1 << 24)
#define I2C_INTSTAT_MONIDLE		(1 << 19)
#define I2C_INTSTAT_MONOV		(1 << 17)
#define I2C_INTSTAT_MONRDY		(1 << 16)
#define I2C_INTSTAT_SLVDESEL		(1 << 15)
#define I2C_INTSTAT_SLVNOTSTR		(1 << 11)
#define I2C_INTSTAT_SLVPENDING		(1 << 8)
#define I2C_INTSTAT_MSTSTSTPERR		(1 << 6)
#define I2C_INTSTAT_MSTARBLOSS		(1 << 4)
#define I2C_INTSTAT_MSTPENDING		(1 << 0)

/* --- I2C_MSTCTL values --------------------------------------------------- */

#define I2C_MSTCTL_MSTSTOP		(1 << 2)
#define I2C_MSTCTL_MSTSTART		(1 << 1)
#define I2C_MSTCTL_MSTCONTINUE		(1 << 0)

/* --- I2C_MSTTIME values -------------------------------------------------- */

/* I2C_MSTTIME[6:4]: MSTSCLHIGH */
#define I2C_MSTTIME_MSTSCLHIGH_CLOCK_2	(0 << 4)
#define I2C_MSTTIME_MSTSCLHIGH_CLOCK_3	(1 << 4)
#define I2C_MSTTIME_MSTSCLHIGH_CLOCK_4	(2 << 4)
#define I2C_MSTTIME_MSTSCLHIGH_CLOCK_5	(3 << 4)
#define I2C_MSTTIME_MSTSCLHIGH_CLOCK_6	(4 << 4)
#define I2C_MSTTIME_MSTSCLHIGH_CLOCK_7	(5 << 4)
#define I2C_MSTTIME_MSTSCLHIGH_CLOCK_8	(6 << 4)
#define I2C_MSTTIME_MSTSCLHIGH_CLOCK_9	(7 << 4)

/* I2C_MSTTIME[2:0]: MSTSCLLOW */
#define I2C_MSTTIME_MSTSCLLOW_CLOCK_2	(0 << 0)
#define I2C_MSTTIME_MSTSCLLOW_CLOCK_3	(1 << 0)
#define I2C_MSTTIME_MSTSCLLOW_CLOCK_4	(2 << 0)
#define I2C_MSTTIME_MSTSCLLOW_CLOCK_5	(3 << 0)
#define I2C_MSTTIME_MSTSCLLOW_CLOCK_6	(4 << 0)
#define I2C_MSTTIME_MSTSCLLOW_CLOCK_7	(5 << 0)
#define I2C_MSTTIME_MSTSCLLOW_CLOCK_8	(6 << 0)
#define I2C_MSTTIME_MSTSCLLOW_CLOCK_9	(7 << 0)

/* --- I2C_MSTDAT values --------------------------------------------------- */

/* I2C_MSTDAT[7:0]: DATA */

/* --- I2C_SLVCTL values --------------------------------------------------- */

#define I2C_SLVCTL_SLVNACK		(1 << 1)
#define I2C_SLVCTL_SLVCONTINUE		(1 << 0)

/* --- I2C_SLVDAT values --------------------------------------------------- */

/* I2C_SLVDAT[7:0]: DATA */

/* --- I2C_SLVADRx values -------------------------------------------------- */

/* I2C_SLVADRx[7:1]: SLVADR */
#define I2C_SLVADR_SADISABLE		(1 << 0)

/* --- I2C_SLVQUAL0 values ------------------------------------------------- */

/* I2C_SLVQUAL0[7:1]: SLVQUAL0 */
#define I2C_SLVQUAL0_QUALMODE0		(1 << 0)

/* --- I2C_MONRXDAT values ------------------------------------------------- */

#define I2C_MONRXDAT_MONNACK		(1 << 10)
#define I2C_MONRXDAT_MONRESTART		(1 << 9)
#define I2C_MONRXDAT_MONSTART		(1 << 8)
/* I2C_MONRXDAT[7:0]: MONRXDAT */

/* --- Function prototypes ------------------------------------------------- */

/* Interrupt and status */
enum {
	I2C_MSTPENDING = (1 << 0),
	I2C_MSTARBLOSS = (1 << 4),
	I2C_MSTSTSTPERR = (1 << 6),
	I2C_SLVPENDING = (1 << 8),
	I2C_SLVNOTSTR = (1 << 11),
	I2C_SLVDESEL = (1 << 15),
	I2C_MONRDY = (1 << 16),
	I2C_MONOV = (1 << 17),
	I2C_MONIDLE = (1 << 19),
	I2C_EVENTTIMEOUT = (1 << 24),
	I2C_SCLTIMEOUT= (1 << 25),

	I2C_MSTSTATE = (7 << 1),
	I2C_MST_IDLE = (0 << 1),
	I2C_MST_RX_RDY = (1 << 1),
	I2C_MST_TX_RDY = (2 << 1),
	I2C_MST_NACK_ADDR = (3 << 1),
	I2C_MST_NACK_DATA = (4 << 1),

	I2C_SLVSTATE = (3 << 9),
	I2C_SLV_ADDR = (0 << 9),
	I2C_SLV_RX = (1 << 9),
	I2C_SLV_TX = (2 << 9),

	I2C_SLVIDX = (3 << 12),
	I2C_SLV_MATCH0 = (0 << 12),
	I2C_SLV_MATCH1 = (1 << 12),
	I2C_SLV_MATCH2 = (2 << 12),
	I2C_SLV_MATCH3 = (3 << 12),
	I2C_SLVSEL = (1 << 14),

	I2C_MONACTIVE = (1 << 18)
};

void i2c_set_scl(int div, int high, int low);
void i2c_set_time_out(int to);
void i2c_enable_master(bool timeout);
void i2c_disable_master(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_master_continue(void);
void i2c_master_send_addr(int addr);
void i2c_master_send(int data);
int i2c_master_recv(void);
int i2c_master_recv_last(void);
void i2c_enable_slave(bool timeout);
void i2c_disable_slave(void);
void i2c_nack(void);
void i2c_slave_continue(void);
void i2c_slave_send(int data);
int i2c_slave_recv(void);
void i2c_set_slave_address(int num, int addr);
void i2c_set_slave_address_qualifier0(int qual);
void i2c_enable_monitor(bool timeout, bool clkstr);
void i2c_disable_monitor(void);
int i2c_get_monitor_data(void);
int i2c_get_status(int status);
void i2c_clear_status(int status);
void i2c_enable_interrupt(int interrupt);
void i2c_disable_interrupt(int interrupt);
int i2c_get_interrupt_mask(int interrupt);
int i2c_get_interrupt_status(int interrupt);
