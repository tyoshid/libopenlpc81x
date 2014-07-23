/*
 * Vector table
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

extern char _stack[];

extern void nmi(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void hardfault(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void svcall(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void pendsv(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void systick_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void spi0_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void spi1_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void uart0_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void uart1_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void uart2_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void i2c_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void sct_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void mrt_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void cmp_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void wdt_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void bod_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void flash_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void wkt_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void pinint0_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void pinint1_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void pinint2_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void pinint3_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void pinint4_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void pinint5_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void pinint6_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));
extern void pinint7_isr(void)	__attribute__ ((weak, alias ("_dummy_isr")));

void _reset(void)	__attribute__ ((naked, section (".startup")));
void _dummy_isr(void)	__attribute__ ((section (".startup")));

void _reset(void)
{
	int i;
	extern char _data_start[];
	extern char _data_load_start[];
	extern char _data_size[];
	extern char _bss_start[];
	extern char _bss_size[];
	extern int main(void);

	/* Move initialized data (Flash) to RAM */
	for (i = 0; i < (int)_data_size; i++)
		_data_start[i] = _data_load_start[i];

	/* Clear BSS. */
	for (i = 0; i < (int)_bss_size; i++)
		_bss_start[i] = 0;

	/* Go to user application. */
	main();

	while (1)
		;
}

void _dummy_isr(void)
{
	while (1)
		;
}

void (* const _vector[]) (void) __attribute__ ((section (".vector"))) = {
	(void *)_stack,
	_reset,
	nmi,
	hardfault,
	0, 0, 0, 0, 0, 0, 0,
	svcall,
	0, 0,
	pendsv,
	systick_isr,
	spi0_isr,
	spi1_isr,
	0,
	uart0_isr,
	uart1_isr,
	uart2_isr,
	0, 0,
	i2c_isr,
	sct_isr,
	mrt_isr,
	cmp_isr,
	wdt_isr,
	bod_isr,
	flash_isr,
	wkt_isr,
	0, 0, 0, 0, 0, 0, 0, 0,
	pinint0_isr,
	pinint1_isr,
	pinint2_isr,
	pinint3_isr,
	pinint4_isr,
	pinint5_isr,
	pinint6_isr,
	pinint7_isr
};
