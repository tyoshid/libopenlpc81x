/*
 * SPI
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
 * Chapter 17: LPC81x SPI0/1
 */

#include <mmio.h>
#include <memorymap.h>

/* --- SPIx registers ------------------------------------------------------ */
/*
 * Offset	Register
 * 0x000	SPIx_CFG		SPI Configuration register
 * 0x004	SPIx_DLY		SPI Delay register
 * 0x008	SPIx_STAT		SPI Status register
 * 0x00c	SPIx_INTENSET		SPI Interrupt Enable read Set register
 * 0x010	SPIx_INTENCLR		SPI Interrupt Enable Clear register
 * 0x014	SPIx_RXDAT		SPI Receiver Data register
 * 0x018	SPIx_TXDATCTL		SPI Transmitter Data and Control
 *					register
 * 0x01c	SPIx_TXDAT		SPI Transmitter Data register
 * 0x020	SPIx_TXCTL		SPI Transmitter Control register
 * 0x024	SPIx_DIV		SPI Divider register
 * 0x028	SPIx_INTSTAT		SPI Interrupt Status register
 */

#define SPI_CFG(base)			MMIO32((base) + 0x000)
#define SPI0_CFG			SPI_CFG(SPI0_BASE)
#define SPI1_CFG			SPI_CFG(SPI1_BASE)

#define SPI_DLY(base)			MMIO32((base) + 0x004)
#define SPI0_DLY			SPI_DLY(SPI0_BASE)
#define SPI1_DLY			SPI_DLY(SPI1_BASE)

#define SPI_STAT(base)			MMIO32((base) + 0x008)
#define SPI0_STAT			SPI_STAT(SPI0_BASE)
#define SPI1_STAT			SPI_STAT(SPI1_BASE)

#define SPI_INTENSET(base)		MMIO32((base) + 0x00c)
#define SPI0_INTENSET			SPI_INTENSET(SPI0_BASE)
#define SPI1_INTENSET			SPI_INTENSET(SPI1_BASE)

#define SPI_INTENCLR(base)		MMIO32((base) + 0x010)
#define SPI0_INTENCLR			SPI_INTENCLR(SPI0_BASE)
#define SPI1_INTENCLR			SPI_INTENCLR(SPI1_BASE)

#define SPI_RXDAT(base)			MMIO32((base) + 0x014)
#define SPI0_RXDAT			SPI_RXDAT(SPI0_BASE)
#define SPI1_RXDAT			SPI_RXDAT(SPI1_BASE)

#define SPI_TXDATCTL(base)		MMIO32((base) + 0x018)
#define SPI0_TXDATCTL			SPI_TXDATCTL(SPI0_BASE)
#define SPI1_TXDATCTL			SPI_TXDATCTL(SPI1_BASE)

#define SPI_TXDAT(base)			MMIO32((base) + 0x01c)
#define SPI0_TXDAT			SPI_TXDAT(SPI0_BASE)
#define SPI1_TXDAT			SPI_TXDAT(SPI1_BASE)

#define SPI_TXCTL(base)			MMIO32((base) + 0x020)
#define SPI0_TXCTL			SPI_TXCTL(SPI0_BASE)
#define SPI1_TXCTL			SPI_TXCTL(SPI1_BASE)

#define SPI_DIV(base)			MMIO32((base) + 0x024)
#define SPI0_DIV			SPI_DIV(SPI0_BASE)
#define SPI1_DIV			SPI_DIV(SPI1_BASE)

#define SPI_INTSTAT(base)		MMIO32((base) + 0x028)
#define SPI0_INTSTAT			SPI_INTSTAT(SPI0_BASE)
#define SPI1_INTSTAT			SPI_INTSTAT(SPI1_BASE)

/* --- SPIx_CFG values ----------------------------------------------------- */

#define SPI_CFG_SPOL			(1 << 8)
#define SPI_CFG_LOOP			(1 << 7)
#define SPI_CFG_CPOL			(1 << 5)
#define SPI_CFG_CPHA			(1 << 4)
#define SPI_CFG_LSBF			(1 << 3)
#define SPI_CFG_MASTER			(1 << 2)
#define SPI_CFG_ENABLE			(1 << 0)

/* --- SPIx_DLY values ----------------------------------------------------- */

/* SPIx_DLY[15:12]: TRANSFER_DELAY */
/* SPIx_DLY[11:8]: FRAME_DELAY */
/* SPIx_DLY[7:4]: POST_DELAY */
/* SPIx_DLY[3:0]: PRE_DELAY */

/* --- SPIx_STAT values ---------------------------------------------------- */

#define SPI_STAT_MSTIDLE		(1 << 8)
#define SPI_STAT_ENDTRANSFER		(1 << 7)
#define SPI_STAT_STALLED		(1 << 6)
#define SPI_STAT_SSD			(1 << 5)
#define SPI_STAT_SSA			(1 << 4)
#define SPI_STAT_TXUR			(1 << 3)
#define SPI_STAT_RXOV			(1 << 2)
#define SPI_STAT_TXRDY			(1 << 1)
#define SPI_STAT_RXRDY			(1 << 0)

/* --- SPIx_INTENSET values ------------------------------------------------ */

#define SPI_INTENSET_SSDEN		(1 << 5)
#define SPI_INTENSET_SSAEN		(1 << 4)
#define SPI_INTENSET_TXUREN		(1 << 3)
#define SPI_INTENSET_RXOVEN		(1 << 2)
#define SPI_INTENSET_TXRDYEN		(1 << 1)
#define SPI_INTENSET_RXRDYEN		(1 << 0)

/* --- SPIx_INTENCLR values ------------------------------------------------ */

#define SPI_INTENCLR_SSDEN		(1 << 5)
#define SPI_INTENCLR_SSAEN		(1 << 4)
#define SPI_INTENCLR_TXUREN		(1 << 3)
#define SPI_INTENCLR_RXOVEN		(1 << 2)
#define SPI_INTENCLR_TXRDYEN		(1 << 1)
#define SPI_INTENCLR_RXRDYEN		(1 << 0)

/* --- SPIx_RXDAT values --------------------------------------------------- */

#define SPI_RXDAT_SOT			(1 << 20)
#define SPI_RXDAT_RXSSEL_N		(1 << 16)
/* SPIx_RXDAT[15:0]: RXDAT */

/* --- SPIx_TXDATCTL values ------------------------------------------------ */

/* SPIx_TXDATCTL[27:24]: LEN */
#define SPI_TXDATCTL_RXIGNORE		(1 << 22)
#define SPI_TXDATCTL_EOF		(1 << 21)
#define SPI_TXDATCTL_EOT		(1 << 20)
#define SPI_TXDATCTL_TXSSEL_N		(1 << 16)
/* SPIx_TXDATCTL[15:0]: TXDAT */

/* --- SPIx_TXDAT values --------------------------------------------------- */

/* SPIx_TXDAT[15:0]: DATA */

/* --- SPIx_TXCTL values --------------------------------------------------- */

/* SPIx_TXCTL[27:24]: LEN */
#define SPI_TXCTL_RXIGNORE		(1 << 22)
#define SPI_TXCTL_EOF			(1 << 21)
#define SPI_TXCTL_EOT			(1 << 20)
#define SPI_TXCTL_TXSSEL_N		(1 << 16)

/* --- SPIx_DIV values ----------------------------------------------------- */

/* SPIx_DIV[15:0]: DIVVAL */

/* --- SPIx_INTSTAT values ------------------------------------------------- */

#define SPI_INTSTAT_SSD			(1 << 5)
#define SPI_INTSTAT_SSA			(1 << 4)
#define SPI_INTSTAT_TXUR		(1 << 3)
#define SPI_INTSTAT_RXOV		(1 << 2)
#define SPI_INTSTAT_TXRDY		(1 << 1)
#define SPI_INTSTAT_RXRDY		(1 << 0)

/* --- Function prototypes ------------------------------------------------- */

/* Device number */
typedef enum {
	SPI0,
	SPI1
} spi_t;

/* Configuration */
enum {
	SPI_LSBF = (1 << 3),

	SPI_CPHA = (1 << 4),
	SPI_CPOL = (1 << 5),
	SPI_MODE0 = (0 << 4),
	SPI_MODE1 = (1 << 4),
	SPI_MODE2 = (2 << 4),
	SPI_MODE3 = (3 << 4),

	SPI_LOOP = (1 << 7),
	SPI_SPOL = (1 << 8)
};

/* Transfer control and status */
enum {
	SPI_NO_SSEL = (1 << 16),
	SPI_SOT = (1 << 20),
	SPI_EOT = (1 << 20),
	SPI_EOF = (1 << 21),
	SPI_RXIGNORE = (1 << 22)
};

/* Interrupt and status */
enum {
	SPI_RXRDY = (1 << 0),
	SPI_TXRDY = (1 << 1),
	SPI_RXOV = (1 << 2),
	SPI_TXUR = (1 << 3),
	SPI_SSA = (1 << 4),
	SPI_SSD = (1 << 5),

	SPI_STALLED = (1 << 6),
	SPI_ENDTRANSFER = (1 << 7),
	SPI_MSTIDLE = (1 << 8)
};

void spi_init_master(spi_t spi, int config, int clkdiv, int pre_delay,
		     int post_delay, int frame_delay, int transfer_delay);
void spi_init_slave(spi_t spi, int config);
void spi_enable(spi_t spi);
void spi_disable(spi_t spi);
void spi_enable_loop_back(spi_t spi);
void spi_disable_loop_back(spi_t spi);
void spi_send(spi_t spi, int data);
void spi_set_tx_control(spi_t spi, int len, int control);
void spi_send_control(spi_t spi, int data, int len, int control);
int spi_recv(spi_t spi);
void spi_send_blocking(spi_t spi, int data);
void spi_send_control_blocking(spi_t spi, int data, int len, int control);
int spi_recv_blocking(spi_t spi);
void spi_enable_interrupt(spi_t spi, int interrupt);
void spi_disable_interrupt(spi_t spi, int interrupt);
int spi_get_interrupt_mask(spi_t spi, int interrupt);
int spi_get_interrupt_status(spi_t spi, int interrupt);
int spi_get_status(spi_t spi, int status);
void spi_clear_status(spi_t spi, int status);
