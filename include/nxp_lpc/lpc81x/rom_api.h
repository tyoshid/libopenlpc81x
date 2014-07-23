/*
 * ROM API
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
 * Chapter 23: LPC81x Power profile API ROM driver
 * Chapter 24: LPC81x I2C-bus ROM API
 * Chapter 25: LPC81x USART API ROM driver routines
 */

#include <mmio.h>
#include <memorymap.h>

#define ROM_DRIVER_BASE	(BOOT_ROM_BASE + 0x1ff8)

#define LPC_PWRD_API	((*(rom_api_t **)(ROM_DRIVER_BASE))->p_pwrd)
#define LPC_I2CD_API	((*(rom_api_t **)(ROM_DRIVER_BASE))->p_i2cd)
#define LPC_UARTD_API	((*(rom_api_t **)(ROM_DRIVER_BASE))->p_uartd)

/* --- Error codes --------------------------------------------------------- */
enum {
	API_OK = 0,
	API_ERROR = 1,

	API_ERR_PLL_INVALID_FREQ = 1,
	API_ERR_PLL_INVALID_MODE = 2,
	API_ERR_PLL_FREQ_NOT_FOUND = 3,
	API_ERR_PLL_NOT_LOCKED = 4,

	API_ERR_PWR_INVALID_FREQ = 1,
	API_ERR_PWR_INVALID_MODE = 2,

	API_ERR_I2C_NAK = 0x60001,
	API_ERR_I2C_BUFFER_OVERFLOW = 0x60002,
	API_ERR_I2C_BYTE_COUNT_ERR = 0x60003,
	API_ERR_I2C_LOSS_OF_ARBITRATION = 0x60004,
	API_ERR_I2C_SLAVE_NOT_ADDRESSED = 0x60005,
	API_ERR_I2C_LOSS_OF_ARBITRATION_NAK_BIT = 0x60006,
	API_ERR_I2C_GENERAL_FAILURE = 0x60007,
	API_ERR_I2C_REGS_SET_TO_DEFAULT = 0x60008,

	API_ERR_UART_RXD_BUSY = 0x80001,
	API_ERR_UART_TXD_BUSY = 0x80002,
	API_ERR_UART_OVERRUN_FRAME_PARITY_NOISE = 0x80003,
	API_ERR_UART_UNDERRUN = 0x80004,
	API_ERR_UART_RARAM = 0x80005,
};

/* --- Power profile API --------------------------------------------------- */

/* set_pll mode options */
enum {
	API_CPU_FREQ_EQU,
	API_CPU_FREQ_LTE,
	API_CPU_FREQ_GTE,
	API_CPU_FREQ_APPROX
};

/* set_power mode options */
enum {
	API_PWR_DEFAULT,
	API_PWR_CPU_PERFORMANCE,
	API_PWR_EFFICIENCY,
	API_PWR_LOW_CURRENT
};

typedef struct {
	void (*set_pll)(unsigned int cmd[], unsigned int resp[]);
	void (*set_power)(unsigned int cmd[], unsigned int resp[]);
} pwrd_api_t;

/* --- I2C API ------------------------------------------------------------- */

typedef void *api_i2c_handle_t;

typedef void (*api_i2c_callbk_t)(uint32_t err_code, uint32_t n);

typedef struct {
	uint32_t num_bytes_send;
	uint32_t num_bytes_rec;
	uint8_t *buffer_ptr_send;
	uint8_t *buffer_ptr_rec;
	api_i2c_callbk_t func_pt;
	uint8_t stop_flag;
	uint8_t dummy[3];
} api_i2c_param_t;

typedef struct {
	uint32_t n_bytes_sent;
	uint32_t n_bytes_recd;
} api_i2c_result_t;

typedef enum {
	API_I2C_IDLE,
	API_I2C_MASTER_SEND,
	API_I2C_MASTER_RECEIVE,
	API_I2C_SLAVE_SEND,
	API_I2C_SLAVE_RECEIVE
} api_i2c_mode_t;

typedef struct {
	void (*i2c_isr_handler)(api_i2c_handle_t h_i2c);
	uint32_t (*i2c_master_transmit_poll)(api_i2c_handle_t h_i2c,
					     api_i2c_param_t *ptp,
					     api_i2c_result_t *ptr);
	uint32_t (*i2c_master_receive_poll)(api_i2c_handle_t h_i2c,
					    api_i2c_param_t *ptp,
					    api_i2c_result_t *ptr);
	uint32_t (*i2c_master_tx_rx_poll)(api_i2c_handle_t h_i2c,
					  api_i2c_param_t *ptp,
					  api_i2c_result_t *ptr);
	uint32_t (*i2c_master_transmit_intr)(api_i2c_handle_t h_i2c,
					     api_i2c_param_t *ptp,
					     api_i2c_result_t *ptr);
	uint32_t (*i2c_master_receive_intr)(api_i2c_handle_t h_i2c,
					    api_i2c_param_t *ptp,
					    api_i2c_result_t *ptr);
	uint32_t (*i2c_master_tx_rx_intr)(api_i2c_handle_t h_i2c,
					  api_i2c_param_t *ptp,
					  api_i2c_result_t *ptr);
	uint32_t (*i2c_slave_receive_poll)(api_i2c_handle_t h_i2c,
					   api_i2c_param_t *ptp,
					   api_i2c_result_t *ptr);
	uint32_t (*i2c_slave_transmit_poll)(api_i2c_handle_t h_i2c,
					    api_i2c_param_t *ptp,
					    api_i2c_result_t *ptr);
	uint32_t (*i2c_slave_receive_intr)(api_i2c_handle_t h_i2c,
					   api_i2c_param_t *ptp,
					   api_i2c_result_t *ptr);
	uint32_t (*i2c_slave_transmit_intr)(api_i2c_handle_t h_i2c,
					    api_i2c_param_t *ptp,
					    api_i2c_result_t *ptr);
	uint32_t (*i2c_set_slave_addr)(api_i2c_handle_t h_i2c,
				       uint32_t slave_addr_0_3,
				       uint32_t slave_mask_0_3);
	uint32_t (*i2c_get_mem_size)(void);
	api_i2c_handle_t (*i2c_setup)(uint32_t i2c_base_addr,
				      uint32_t *start_of_ram);
	uint32_t (*i2c_set_bitrate)(api_i2c_handle_t h_i2c,
				    uint32_t p_clk_in_hz,
				    uint32_t bitrate_in_bps);
	uint32_t (*i2c_get_firmware_version)(void);
	api_i2c_mode_t (*i2c_get_status)(api_i2c_handle_t h_i2c);
} i2cd_api_t;

/* --- UART API ------------------------------------------------------------ */

typedef void *api_uart_handle_t;

typedef struct {
	uint32_t sys_clk_in_hz;
	uint32_t baudrate_in_hz;
	uint8_t config;
	uint8_t sync_mode;
	uint16_t error_en;
} api_uart_config_t;

typedef void (*api_uart_callbk_t)(uint32_t err_code, uint32_t n);

typedef struct {
	uint8_t *buffer;
	uint32_t size;
	uint16_t transfer_mode;
	uint16_t driver_mode;
	api_uart_callbk_t callback_func_pt;
} api_uart_param_t;

typedef struct {
	uint32_t (*uart_get_mem_size)(void);
	api_uart_handle_t (*uart_setup)(uint32_t base_addr, uint8_t *ram);
	uint32_t (*uart_init)(api_uart_handle_t handle,
			      api_uart_config_t *set);
	uint8_t (*uart_get_char)(api_uart_handle_t handle);
	void (*uart_put_char)(api_uart_handle_t handle, uint8_t data);
	uint32_t (*uart_get_line)(api_uart_handle_t handle,
				  api_uart_param_t *param);
	uint32_t (*uart_put_line)(api_uart_handle_t handle,
				  api_uart_param_t *param);
	void (*uart_isr)(api_uart_handle_t handle);
} uartd_api_t;

/* --- ROM API ------------------------------------------------------------- */

typedef struct {
	uint32_t unused[3];
	pwrd_api_t *p_pwrd;
	uint32_t p_dev1;
	i2cd_api_t *p_i2cd;
	uint32_t p_dev3;
	uint32_t p_dev4;
	uint32_t p_dev5;
	uartd_api_t *p_uartd;
} rom_api_t;
