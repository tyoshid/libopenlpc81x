/*
 * Flash controller
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
 * Chapter 20: LPC81x Flash controller
 */

#include <mmio.h>
#include <memorymap.h>

/* --- FLASHCON registers -------------------------------------------------- */
/*
 * Offset	Register
 * 0x010	FLASHCON_FLASHCFG	Flash configuration register
 * 0x020	FLASHCON_FMSSTART	Signature start address register
 * 0x024	FLASHCON_FMSSTOP	Signature stop address register
 * 0x02c	FLASHCON_FMSW0		Signature word
 */

#define FLASHCON_FLASHCFG		MMIO32(FLASHCON_BASE + 0x010)
#define FLASHCON_FMSSTART		MMIO32(FLASHCON_BASE + 0x020)
#define FLASHCON_FMSSTOP		MMIO32(FLASHCON_BASE + 0x024)
#define FLASHCON_FMSW0			MMIO32(FLASHCON_BASE + 0x02c)

/* --- FLASHCON_FLASHCFG values -------------------------------------------- */

#define FLASHCON_FLASHCFG_FLASHTIM1	(1 << 1)
#define FLASHCON_FLASHCFG_FLASHTIM0	(1 << 0)

/* --- FLASHCON_FMSSTART values -------------------------------------------- */

/* FLASHCON_FMSSTART[16:0]: START */

/* --- FLASHCON_FMSSTOP values --------------------------------------------- */

#define FLASHCON_FMSSTOP_STRTBIST	(1 << 31)

/* FLASHCON_FMSSTOP[16:0]: STOPA */

/* --- FLASHCON_FMSW0 values ----------------------------------------------- */

/* FLASHCON_FMSW0[31:0]: SIG */

/* --- Function prototypes ------------------------------------------------- */

void flashcon_set_flash_access_time(int clock);
int flashcon_generate_signature(int start, int stop);
