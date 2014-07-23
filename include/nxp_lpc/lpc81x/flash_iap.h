/*
 * Flash IAP
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
 * Chapter 21: LPC81x Boot ROM
 * Chapter 22: LPC81x Flash ISP and IAP programming
 */

#include <mmio.h>
#include <memorymap.h>

/* Code Read Protection */
#define CRP_NO_ISP				0x4e697370
#define CRP1					0x12345678
#define CRP2					0x87654321
#define CRP3					0x43218765

/*
 * e.g.
 * const int _crp __attribute__ ((section (".crp"))) = CRP1;
 */

#define IAP_LOCATION				(BOOT_ROM_BASE + 0x1ff1)

/* IAP Command */
#define IAP_PREPARE_SECTOR_FOR_WRITE_OPERATION	50
#define IAP_COPY_RAM_TO_FLASH			51
#define IAP_ERASE_SECTOR			52
#define IAP_BLANK_CHECK_SECTOR			53
#define IAP_READ_PART_ID			54
#define IAP_READ_BOOT_CODE_VERSION		55
#define IAP_COMPARE				56
#define IAP_REINVOKE_ISP			57
#define IAP_READ_UID				58
#define IAP_ERASE_PAGE				59

/* IAP Status codes */
#define IAP_CMD_SUCCESS				0
#define IAP_INVALID_COMMAND			1
#define IAP_SRC_ADDR_ERROR			2
#define IAP_DST_ADDR_ERROR			3
#define IAP_SRC_ADDR_NOT_MAPPED			4
#define IAP_DST_ADDR_NOT_MAPPED			5
#define IAP_COUNT_ERROR				6
#define IAP_INVALID_SECTOR			7
#define IAP_SECTOR_NOT_BLANK			8
#define IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OP	9
#define IAP_COMPARE_ERROR			10
#define IAP_BUSY				11

typedef void (*IAP)(unsigned int cmd[], unsigned int resp[]);
