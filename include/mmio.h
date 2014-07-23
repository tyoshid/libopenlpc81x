/*
 * Memory mapped I/O
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

#ifndef MMIO_H
#define MMIO_H

#include <stdint.h>
#include <stdbool.h>

typedef	int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;

/* Memory mapped I/O */
#define MMIO8(addr)	(*(volatile u8 *)(addr))
#define MMIO16(addr)	(*(volatile u16 *)(addr))
#define MMIO32(addr)	(*(volatile u32 *)(addr))

#endif
