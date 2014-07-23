/*
 * command.h
 *
 * Copyright 2014 Toshiaki Yoshida <yoshida@mpc.net>
 *
 * This file is part of usart-util.
 *
 * usart-util is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * usart-util is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

/* UART ISP commands */
#define COM_UNLOCK					'U'
#define COM_SET_BAUD_RATE				'B'
#define COM_ECHO					'A'
#define COM_WRITE_RAM					'W'
#define COM_READ_MEMORY					'R'
#define COM_PREPARE_SECTORS_FOR_WRITE_OPERATION		'P'
#define COM_COPY_RAM_TO_FLASH				'C'
#define COM_GO						'G'
#define COM_ERASE_SECTORS				'E'
#define COM_BLANK_CHECK_SECTORS				'I'
#define COM_READ_PART_ID				'J'
#define COM_READ_BOOT_CODE_VERSION			'K'
#define COM_COMPARE					'M'
#define COM_RAED_UID					'N'
#define COM_READ_CRC_CHECKSUM				'S'

/* UART ISP Return Codes */
#define RESULT_CMD_SUCCESS				0
#define RESULT_INVALID_COMMAND				1
#define RESULT_SRC_ADDR_ERROR				2
#define RESULT_DST_ADDR_ERROR				3
#define RESULT_SRC_ADDR_NOT_MAPPED			4
#define RESULT_DST_ADDR_NOT_MAPPED			5
#define RESULT_COUNT_ERROR				6
#define RESULT_INVALID_SECTOR				7
#define RESULT_SECTOR_NOT_BLANK				8
#define RESULT_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION	9
#define RESULT_COMPARE_ERROR				10
#define RESULT_BUSY					11
#define RESULT_PARAM_ERROR				12
#define RESULT_ADDR_ERROR				13
#define RESULT_ADDR_NOT_MAPPED				14
#define RESULT_CMD_LOCKED				15
#define RESULT_INVALID_CODE				16
#define RESULT_INVALID_BAUD_RATE			17
#define RESULT_INVALID_STOP_BIT				18
#define RESULT_CODE_READ_PROTECTION_ENABLED		19

#define ERROR_TIMEOUT					32
#define ERROR_INVALID_VALUE				33
#define ERROR_SIZE					34

/* Part identification numbers */
#define PID_LPC810M021FN8	0x8100
#define PID_LPC811M001JDH16	0x8110
#define PID_LPC812M101JDH16	0x8120
#define PID_LPC812M101JD20	0x8121
#define PID_LPC812M101JDH20	0x8122
#define PID_LPC812M101JTB16	0x8122

#define PAGE_SIZE		64
#define SECTOR_SIZE		1024

int isp_init(int fd, int retry);
int unlock(int fd);
int set_baud_rate(int fd, char *baud, int stop);
int echo(int fd, int setting);
int write_to_ram(int fd, uint32_t addr, int bytes, uint8_t *data);
int read_memory(int fd, uint32_t addr, int bytes, uint8_t *data);
int prepare_sectors(int fd, int start, int end);
int copy_ram_to_flash(int fd, uint32_t flash, uint32_t ram, int bytes);
int go(int fd, uint32_t addr);
int erase_sectors(int fd, int start, int end);
int blank_check_sectors(int fd, int start, int end);
int read_boot_code_version(int fd, uint8_t *version);
int read_part_id(int fd, uint32_t *pid);
int compare(int fd, uint32_t addr1, uint32_t addr2, int bytes);
int read_uid(int fd, uint32_t *uid);
int read_crc_checksum(int fd, uint32_t addr, int bytes, uint32_t *checksum);
