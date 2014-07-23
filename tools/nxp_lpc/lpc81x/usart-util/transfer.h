/*
 * transfer.h
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

#define FLASH_ADDRESS	0x00000000
#define SRAM_ADDRESS	0x10000000
#define RESERVE_SIZE	0x00000300
#define CRP		0x000002fc

int upload(int fd, FILE *stream, int bytes);
int download(int fd, FILE *stream);
