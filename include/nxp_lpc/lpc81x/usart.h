/*
 * USART
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
 * Chapter 15: LPC81x USART0/1/2
 */

#include <mmio.h>
#include <memorymap.h>

/* --- USART registers ----------------------------------------------------- */
/*
 * Offset	Register
 * 0x000	USART_CFG		USART Configuration register
 * 0x004	USART_CTL		USART Control register
 * 0x008	USART_STAT		USART Status register
 * 0x00c	USART_INTENSET		Interrupt Enable read and Set register
 * 0x010	USART_INTENCLR		Interrupt Enable Clear register
 * 0x014	USART_RXDAT		Receiver Data register
 * 0x018	USART_RXDATSTAT		Receiver Data with Status register
 * 0x01c	USART_TXDAT		Transmit Data register
 * 0x020	USART_BRG		Baud Rate Generator register
 * 0x024	USART_INTSTAT		Interrupt status register
 */

#define USART_CFG(usart_base)		MMIO32((usart_base) + 0x000)
#define USART0_CFG			USART_CFG(USART0_BASE)
#define USART1_CFG			USART_CFG(USART1_BASE)
#define USART2_CFG			USART_CFG(USART2_BASE)

#define USART_CTL(usart_base)		MMIO32((usart_base) + 0x004)
#define USART0_CTL			USART_CTL(USART0_BASE)
#define USART1_CTL			USART_CTL(USART1_BASE)
#define USART2_CTL			USART_CTL(USART2_BASE)

#define USART_STAT(usart_base)		MMIO32((usart_base) + 0x008)
#define USART0_STAT			USART_STAT(USART0_BASE)
#define USART1_STAT			USART_STAT(USART1_BASE)
#define USART2_STAT			USART_STAT(USART2_BASE)

#define USART_INTENSET(usart_base)	MMIO32((usart_base) + 0x00c)
#define USART0_INTENSET			USART_INTENSET(USART0_BASE)
#define USART1_INTENSET			USART_INTENSET(USART1_BASE)
#define USART2_INTENSET			USART_INTENSET(USART2_BASE)

#define USART_INTENCLR(usart_base)	MMIO32((usart_base) + 0x010)
#define USART0_INTENCLR			USART_INTENCLR(USART0_BASE)
#define USART1_INTENCLR			USART_INTENCLR(USART1_BASE)
#define USART2_INTENCLR			USART_INTENCLR(USART2_BASE)

#define USART_RXDAT(usart_base)		MMIO32((usart_base) + 0x014)
#define USART0_RXDAT			USART_RXDAT(USART0_BASE)
#define USART1_RXDAT			USART_RXDAT(USART1_BASE)
#define USART2_RXDAT			USART_RXDAT(USART2_BASE)

#define USART_RXDATSTAT(usart_base)	MMIO32((usart_base) + 0x018)
#define USART0_RXDATSTAT		USART_RXDATSTAT(USART0_BASE)
#define USART1_RXDATSTAT		USART_RXDATSTAT(USART1_BASE)
#define USART2_RXDATSTAT		USART_RXDATSTAT(USART2_BASE)

#define USART_TXDAT(usart_base)		MMIO32((usart_base) + 0x01c)
#define USART0_TXDAT			USART_TXDAT(USART0_BASE)
#define USART1_TXDAT			USART_TXDAT(USART1_BASE)
#define USART2_TXDAT			USART_TXDAT(USART2_BASE)

#define USART_BRG(usart_base)		MMIO32((usart_base) + 0x020)
#define USART0_BRG			USART_BRG(USART0_BASE)
#define USART1_BRG			USART_BRG(USART1_BASE)
#define USART2_BRG			USART_BRG(USART2_BASE)

#define USART_INTSTAT(usart_base)	MMIO32((usart_base) + 0x024)
#define USART0_INTSTAT			USART_INTSTAT(USART0_BASE)
#define USART1_INTSTAT			USART_INTSTAT(USART1_BASE)
#define USART2_INTSTAT			USART_INTSTAT(USART2_BASE)

/* --- USART_CFG values ---------------------------------------------------- */

#define USART_CFG_LOOP			(1 << 15)
#define USART_CFG_SYNCMST		(1 << 14)
#define USART_CFG_CLKPOL		(1 << 12)
#define USART_CFG_SYNCEN		(1 << 11)
#define USART_CFG_CTSEN			(1 << 9)
#define USART_CFG_STOPLEN		(1 << 6)
#define USART_CFG_PARITYSEL1		(1 << 5)
#define USART_CFG_PARITYSEL0		(1 << 4)
#define USART_CFG_DATALEN1		(1 << 3)
#define USART_CFG_DATALEN0		(1 << 2)
#define USART_CFG_ENABLE		(1 << 0)

/* PARITYSEL */
#define USART_CFG_PARITYSEL_NONE	(0 << 4)
#define USART_CFG_PARITYSEL_EVEN	(2 << 4)
#define USART_CFG_PARITYSEL_ODD		(3 << 4)

/* DATALEN */
#define USART_CFG_DATALEN_7BIT		(0 << 2)
#define USART_CFG_DATALEN_8BIT		(1 << 2)
#define USART_CFG_DATALEN_9BIT		(2 << 2)

#define USART_CFG_MASK			0xda7d

/* --- USART_CTL values ---------------------------------------------------- */

#define USART_CTL_CLRCC			(1 << 9)
#define USART_CTL_CC			(1 << 8)
#define USART_CTL_TXDIS			(1 << 6)
#define USART_CTL_ADDRDET		(1 << 2)
#define USART_CTL_TXBRKEN		(1 << 1)

#define USART_CTL_MASK			0x346

/* --- USART_STAT values --------------------------------------------------- */

#define USART_STAT_RXNOISEINT		(1 << 15)
#define USART_STAT_PARITYERRINT		(1 << 14)
#define USART_STAT_FRAMERRINT		(1 << 13)
#define USART_STAT_START		(1 << 12)
#define USART_STAT_DELTARXBRK		(1 << 11)
#define USART_STAT_RXBRK		(1 << 10)
#define USART_STAT_OVERRUNINT		(1 << 8)
#define USART_STAT_TXDISINT		(1 << 6)
#define USART_STAT_DELTACTS		(1 << 5)
#define USART_STAT_CTS			(1 << 4)
#define USART_STAT_TXIDLE		(1 << 3)
#define USART_STAT_TXRDY		(1 << 2)
#define USART_STAT_RXIDLE		(1 << 1)
#define USART_STAT_RXRDY		(1 << 0)

/* --- USART_INTENSET values ----------------------------------------------- */

#define USART_INTENSET_RXNOISEEN	(1 << 15)
#define USART_INTENSET_PARITYERREN	(1 << 14)
#define USART_INTENSET_FRAMERREN	(1 << 13)
#define USART_INTENSET_STARTEN		(1 << 12)
#define USART_INTENSET_DELTARXBRKEN	(1 << 11)
#define USART_INTENSET_OVERRUNEN	(1 << 8)
#define USART_INTENSET_TXDISINTEN	(1 << 6)
#define USART_INTENSET_DELTACTSEN	(1 << 5)
#define USART_INTENSET_TXRDYEN		(1 << 2)
#define USART_INTENSET_RXRDYEN		(1 << 0)

/* --- USART_INTENCLR values ----------------------------------------------- */

#define USART_INTENCLR_RXNOISECLR	(1 << 15)
#define USART_INTENCLR_PARITYERRCLR	(1 << 14)
#define USART_INTENCLR_FRAMERRCLR	(1 << 13)
#define USART_INTENCLR_STARTCLR		(1 << 12)
#define USART_INTENCLR_DELTARXBRKCLR	(1 << 11)
#define USART_INTENCLR_OVERRUNCLR	(1 << 8)
#define USART_INTENCLR_TXDISINTCLR	(1 << 6)
#define USART_INTENCLR_DELTACTSCLR	(1 << 5)
#define USART_INTENCLR_TXRDYCLR		(1 << 2)
#define USART_INTENCLR_RXRDYCLR		(1 << 0)

/* --- USART_RXDAT values -------------------------------------------------- */

/* USART_RXDAT[8:0]: RXDATA */

#define USART_RXDAT_MASK		0x1ff

/* --- USART_RXDATSTAT values ---------------------------------------------- */

#define USART_RXDATSTAT_RXNOISE		(1 << 15)
#define USART_RXDATSTAT_PARITYERR	(1 << 14)
#define USART_RXDATSTAT_FRAMERR		(1 << 13)

/* USART_RXDATSTAT[8:0]: RXDATA */

#define USART_RXDATSTAT_MASK		0xe1ff

/* --- USART_TXDAT values -------------------------------------------------- */

/* USART_TXDAT[8:0]: TXDATA */

#define USART_TXDAT_MASK		0x1ff

/* --- USART_BRG values ---------------------------------------------------- */

/* USART_BRG[15:0]: BRGVAL */

/* --- USART_INTSTAT values ------------------------------------------------ */

#define USART_INTSTAT_RXNOISEINT	(1 << 15)
#define USART_INTSTAT_PARITYERRINT	(1 << 14)
#define USART_INTSTAT_FRAMERRINT	(1 << 13)
#define USART_INTSTAT_START		(1 << 12)
#define USART_INTSTAT_DELTARXBRK	(1 << 11)
#define USART_INTSTAT_OVERRUNINT	(1 << 8)
#define USART_INTSTAT_TXDISINT		(1 << 6)
#define USART_INTSTAT_DELTACTS		(1 << 5)
#define USART_INTSTAT_TXRDY		(1 << 2)
#define USART_INTSTAT_RXRDY		(1 << 0)

/* --- Function prototypes ------------------------------------------------- */

/* Device number */
typedef enum {
	USART0,
	USART1,
	USART2
} usart_t;

/* Parity */
typedef enum {
	USART_PARITY_NONE,
	USART_ODD,
	USART_EVEN
} usart_parity_t;

/* Flow control */
typedef enum {
	USART_FLOW_NONE,
	USART_RTS_CTS
} usart_flowcontrol_t;

/* Interrupt */
enum {
	USART_RXRDY = (1 << 0),
	USART_TXRDY = (1 << 2),
	USART_CTS = (1 << 5),
	USART_TXDIS = (1 << 6),
	USART_OVERRUN = (1 << 8),
	USART_RX_BREAK = (1 << 11),
	USART_START = (1 << 12),
	USART_FRAMERR = (1 << 13),
	USART_PARITYERR = (1 << 14),
	USART_RX_NOISE = (1 << 15)
};

void usart_set_baudrate(usart_t usart, int u_pclk, int baud);
void usart_set_databits(usart_t usart, int bits);
void usart_set_stopbits(usart_t usart, int bits);
void usart_set_parity(usart_t usart, usart_parity_t parity);
void usart_set_flow_control(usart_t usart, usart_flowcontrol_t flowcontrol);
void usart_init(usart_t usart, int u_pclk, int baud, int databits,
		int stopbits, usart_parity_t parity,
		usart_flowcontrol_t flowcontrol);
void usart_enable(usart_t usart);
void usart_disable(usart_t usart);
void usart_enable_sync_mode(usart_t usart, bool master, bool rising_edge);
void usart_disable_sync_mode(usart_t usart);
void usart_send(usart_t usart, int data);
int usart_recv(usart_t usart);
void usart_send_blocking(usart_t usart, int data);
int usart_recv_blocking(usart_t usart);
void usart_enable_loopback(usart_t usart);
void usart_disable_loopback(usart_t usart);
void usart_enable_break(usart_t usart);
void usart_disable_break(usart_t usart);
void usart_enable_address_detect_mode(usart_t usart);
void usart_disable_address_detect_mode(usart_t usart);
void usart_disable_tx(usart_t usart);
void usart_enable_tx(usart_t usart);
void usart_enable_continuous_clock(usart_t usart, bool auto_clear);
void usart_disable_continuous_clock(usart_t usart);
void usart_enable_interrupt(usart_t usart, int interrupt);
void usart_disable_interrupt(usart_t usart, int interrupt);
int usart_get_interrupt_mask(usart_t usart, int interrupt);
int usart_get_interrupt_status(usart_t usart, int interrupt);
void usart_clear_interrupt(usart_t usart, int interrupt);
