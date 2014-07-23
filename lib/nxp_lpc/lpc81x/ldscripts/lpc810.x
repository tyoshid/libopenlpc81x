/*
 * Default LPC810x linker script
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

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm");
OUTPUT_ARCH(arm);

MEMORY
{
	ROM : ORIGIN = 0x00000000, LENGTH = 4K
	RAM : ORIGIN = 0x10000000, LENGTH = 1K
}

REGION_ALIAS("REGION_TEXT", ROM);
REGION_ALIAS("REGION_RODATA", ROM);
REGION_ALIAS("REGION_DATA", RAM);
REGION_ALIAS("REGION_BSS", RAM);

_stack = ORIGIN(RAM) + LENGTH(RAM);

EXTERN(_vector);
/* EXTERN(_crp); */

SECTIONS
{
	.text :
	{
		*(.vector)
		*(.startup)
/*
		. = 0x000002fc;
		*(.crp)
*/
		*(.text*)
	} > REGION_TEXT
	.rodata :
	{
		*(.rodata*)
	} > REGION_RODATA
	. = .;			/* Orphan sections */
	_rodata_end = .;
	.data : AT (_rodata_end)
	{
		_data_start = .;
		*(.data*)
	} > REGION_DATA
	_data_size = SIZEOF(.data);
	_data_load_start = LOADADDR(.data);
	.bss :
	{
		_bss_start = .;
		*(.bss*)
		*(COMMON)
	} > REGION_BSS
	. = .;
	PROVIDE(end = ALIGN(4));
	_bss_size = SIZEOF(.bss);
}
