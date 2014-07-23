/*
 * SCT functions
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

#include <sct.h>

void sct_config(sct_clock_t clkmode, sct_input_t cksel, int config)
{
	SCT_CONFIG = (clkmode << 1) | (cksel << 3) | config;
}

void sct_clear_counter(void)
{
	SCT_CTRL |= SCT_CTRL_CLRCTR_L;
}

void sct_clear_counter_l(void)
{
	SCT_CTRL_L |= SCT_CTRL_CLRCTR_L;
}

void sct_clear_counter_h(void)
{
	SCT_CTRL_H |= SCT_CTRL_CLRCTR_L;
}

void sct_set_prescaler(int prescaler)
{
	int r;

	r = SCT_CTRL;
	r &= ~(0xff << 5);
	SCT_CTRL = r | ((prescaler - 1) & 0xff) << 5;
}

void sct_set_prescaler_l(int prescaler)
{
	int r;

	r = SCT_CTRL_L;
	r &= ~(0xff << 5);
	SCT_CTRL_L = r | ((prescaler - 1) & 0xff) << 5;
}

void sct_set_prescaler_h(int prescaler)
{
	int r;

	r = SCT_CTRL_H;
	r &= ~(0xff << 5);
	SCT_CTRL_H = r | ((prescaler - 1) & 0xff) << 5;
}

void sct_start_counter(void)
{
	SCT_CTRL &= ~(SCT_CTRL_STOP_L | SCT_CTRL_HALT_L);
}

void sct_start_counter_l(void)
{
	SCT_CTRL_L &= ~(SCT_CTRL_STOP_L | SCT_CTRL_HALT_L);
}

void sct_start_counter_h(void)
{
	SCT_CTRL_H &= ~(SCT_CTRL_STOP_L | SCT_CTRL_HALT_L);
}

void sct_stop_counter(void)
{
	int r;

	r = SCT_CTRL;
	r &= ~SCT_CTRL_HALT_L;
	SCT_CTRL = r | SCT_CTRL_STOP_L;
}

void sct_stop_counter_l(void)
{
	int r;

	r = SCT_CTRL_L;
	r &= ~SCT_CTRL_HALT_L;
	SCT_CTRL_L = r | SCT_CTRL_STOP_L;
}

void sct_stop_counter_h(void)
{
	int r;

	r = SCT_CTRL_H;
	r &= ~SCT_CTRL_HALT_L;
	SCT_CTRL_H = r | SCT_CTRL_STOP_L;
}

void sct_halt_counter(void)
{
	int r;

	r = SCT_CTRL;
	r &= ~SCT_CTRL_STOP_L;
	SCT_CTRL = r | SCT_CTRL_HALT_L;
}

void sct_halt_counter_l(void)
{
	int r;

	r = SCT_CTRL_L;
	r &= ~SCT_CTRL_STOP_L;
	SCT_CTRL_L = r | SCT_CTRL_HALT_L;
}

void sct_halt_counter_h(void)
{
	int r;

	r = SCT_CTRL_H;
	r &= ~SCT_CTRL_STOP_L;
	SCT_CTRL_H = r | SCT_CTRL_HALT_L;
}

void sct_set_counter_mode(sct_counter_t mode)
{
	int r;

	r = SCT_CTRL;
	switch (mode) {
	case SCT_COUNT_UP:
		r &= ~(SCT_CTRL_BIDIR_L | SCT_CTRL_DOWN_L);
		break;
	case SCT_COUNT_BIDIR_UP:
		r &= ~SCT_CTRL_DOWN_L;
		r |= SCT_CTRL_BIDIR_L;
		break;
	case SCT_COUNT_BIDIR_DOWN:
		r |= SCT_CTRL_BIDIR_L | SCT_CTRL_DOWN_L;
		break;
	default:
		break;
	}
	SCT_CTRL = r;
}

void sct_set_counter_mode_l(sct_counter_t mode)
{
	int r;

	r = SCT_CTRL_L;
	switch (mode) {
	case SCT_COUNT_UP:
		r &= ~(SCT_CTRL_BIDIR_L | SCT_CTRL_DOWN_L);
		break;
	case SCT_COUNT_BIDIR_UP:
		r &= ~SCT_CTRL_DOWN_L;
		r |= SCT_CTRL_BIDIR_L;
		break;
	case SCT_COUNT_BIDIR_DOWN:
		r |= SCT_CTRL_BIDIR_L | SCT_CTRL_DOWN_L;
		break;
	default:
		break;
	}
	SCT_CTRL_L = r;
}

void sct_set_counter_mode_h(sct_counter_t mode)
{
	int r;

	r = SCT_CTRL_H;
	switch (mode) {
	case SCT_COUNT_UP:
		r &= ~(SCT_CTRL_BIDIR_L | SCT_CTRL_DOWN_L);
		break;
	case SCT_COUNT_BIDIR_UP:
		r &= ~SCT_CTRL_DOWN_L;
		r |= SCT_CTRL_BIDIR_L;
		break;
	case SCT_COUNT_BIDIR_DOWN:
		r |= SCT_CTRL_BIDIR_L | SCT_CTRL_DOWN_L;
		break;
	default:
		break;
	}
	SCT_CTRL_H = r;
}

void sct_set_limit(int event)
{
	SCT_LIMIT = event;
}

void sct_set_limit_l(int event)
{
	SCT_LIMIT_L = event;
}

void sct_set_limit_h(int event)
{
	SCT_LIMIT_H = event;
}

void sct_set_halt(int event)
{
	SCT_HALT = event;
}

void sct_set_halt_l(int event)
{
	SCT_HALT_L = event;
}

void sct_set_halt_h(int event)
{
	SCT_HALT_H = event;
}

void sct_set_stop(int event)
{
	SCT_STOP = event;
}

void sct_set_stop_l(int event)
{
	SCT_STOP_L = event;
}

void sct_set_stop_h(int event)
{
	SCT_STOP_H = event;
}

void sct_set_start(int event)
{
	SCT_START = event;
}

void sct_set_start_l(int event)
{
	SCT_START_L = event;
}

void sct_set_start_h(int event)
{
	SCT_START_H = event;
}

void sct_set_counter(int value)
{
	SCT_COUNT = value;
}

void sct_set_counter_l(int value)
{
	SCT_COUNT_L = value;
}

void sct_set_counter_h(int value)
{
	SCT_COUNT_H = value;
}

int sct_get_counter(void)
{
	return SCT_COUNT;
}

int sct_get_counter_l(void)
{
	return SCT_COUNT_L;
}

int sct_get_counter_h(void)
{
	return SCT_COUNT_H;
}

void sct_set_state(int state)
{
	SCT_STATE = state;
}

void sct_set_state_l(int state)
{
	SCT_STATE_L = state;
}

void sct_set_state_h(int state)
{
	SCT_STATE_H = state;
}

int sct_get_state(void)
{
	return SCT_STATE & 0x001f001f;
}

int sct_get_state_l(void)
{
	return SCT_STATE_L & 0x001f;
}

int sct_get_state_h(void)
{
	return SCT_STATE_H & 0x001f;
}

int sct_get_input(void)
{
	return SCT_INPUT & 0x000f000f;
}

void sct_enable_capture(int registers)
{
	SCT_REGMODE |= registers;
}

void sct_enable_capture_l(int registers)
{
	SCT_REGMODE_L |= registers;
}

void sct_enable_capture_h(int registers)
{
	SCT_REGMODE_H |= registers;
}

void sct_disable_capture(int registers)
{
	SCT_REGMODE &= ~registers;
}

void sct_disable_capture_l(int registers)
{
	SCT_REGMODE_L &= ~registers;
}

void sct_disable_capture_h(int registers)
{
	SCT_REGMODE_H &= ~registers;
}

void sct_set_output(int output)
{
	SCT_OUTPUT = output;
}

int sct_get_output(void)
{
	return SCT_OUTPUT & 0xf;
}

void sct_enable_output_counter_control(int output)
{
	int i;
	int r;

	r = SCT_OUTPUTDIRCTRL;
	for (i = 0; i < 4; i++) {
		if (output & (1 << i)) {
			r |= 1 << i * 2;
			r &= ~(2 << i * 2);
		}
	}
	SCT_OUTPUTDIRCTRL = r;
}

void sct_enable_output_counter_h_control(int output)
{
	int i;
	int r;

	r = SCT_OUTPUTDIRCTRL;
	for (i = 0; i < 4; i++) {
		if (output & (1 << i)) {
			r |= 2 << i * 2;
			r &= ~(1 << i * 2);
		}
	}
	SCT_OUTPUTDIRCTRL = r;
}

void sct_disable_output_counter_control(int output)
{
	int i;
	int r;

	r = SCT_OUTPUTDIRCTRL;
	for (i = 0; i < 4; i++) {
		if (output & (1 << i))
			r &= ~(3 << i * 2);
	}
	SCT_OUTPUTDIRCTRL = r;
}

void sct_set_conflict_resolution(sct_res_t res, int output)
{
	int i;
	int r;

	r = SCT_RES;
	for (i = 0; i < 4; i++) {
		if (output & (1 << i)) {
			r &= ~(3 << i * 2);
			r |= res << i * 2;
		}
	}
	SCT_OUTPUTDIRCTRL = r;
}

void sct_enable_interrupt(int interrupt)
{
	if (interrupt & 0x3f)
		SCT_EVEN |= interrupt & 0x3f;
	if (interrupt & 0xf00)
		SCT_CONEN |= (interrupt >> 8) & 0xf;
}

void sct_disable_interrupt(int interrupt)
{
	if (interrupt & 0x3f)
		SCT_EVEN &= ~(interrupt & 0x3f);
	if (interrupt & 0xf00)
		SCT_CONEN &= ~((interrupt >> 8) & 0xf);
}

int sct_get_interrupt_mask(int interrupt)
{
	int r = 0;

	if (interrupt & 0x3f)
		r = SCT_EVEN & (interrupt & 0x3f);
	if (interrupt & 0xf00)
		r |= (SCT_CONEN << 8) & (interrupt & 0xf00);
	return r;
}

int sct_get_interrupt_status(int interrupt)
{
	int r = 0;

	if (interrupt & 0x3f)
		r = SCT_EVFLAG & (interrupt & 0x3f);
	if (interrupt & 0xf00)
		r |= (SCT_CONFLAG << 8) & (interrupt & 0xf00);
	return r;
}

void sct_clear_interrupt(int interrupt)
{
	if (interrupt & 0x3f)
		SCT_EVFLAG = interrupt & 0x3f;
	if (interrupt & 0xf00)
		SCT_CONFLAG = (interrupt >> 8) & 0xf;
}

void sct_set_match(int match, int value)
{
	SCT_MATCH(match) = value;
}

void sct_set_match_l(int match, int value)
{
	SCT_MATCH_L(match) = value;
}

void sct_set_match_h(int match, int value)
{
	SCT_MATCH_H(match) = value;
}

void sct_set_match_reload(int match, int value)
{
	SCT_MATCHREL(match) = value;
}

void sct_set_match_reload_l(int match, int value)
{
	SCT_MATCHREL_L(match) = value;
}

void sct_set_match_reload_h(int match, int value)
{
	SCT_MATCHREL_H(match) = value;
}

void sct_set_match_and_reload(int match, int value)
{
	SCT_MATCH(match) = value;
	SCT_MATCHREL(match) = value;
}

void sct_set_match_and_reload_l(int match, int value)
{
	SCT_MATCH_L(match) = value;
	SCT_MATCHREL_L(match) = value;
}

void sct_set_match_and_reload_h(int match, int value)
{
	SCT_MATCH_H(match) = value;
	SCT_MATCHREL_H(match) = value;
}

int sct_get_capture(int cap)
{
	return SCT_CAP(cap);
}

int sct_get_capture_l(int cap)
{
	return SCT_CAP_L(cap);
}

int sct_get_capture_h(int cap)
{
	return SCT_CAP_H(cap);
}

void sct_set_capture(int cap, int event)
{
	SCT_CAPCTRL_L(cap) = event;
}

void sct_set_capture_h(int cap, int event)
{
	SCT_CAPCTRL_H(cap) = event;
}

void sct_setup_event(int ev, int match, int io, int op, int action, int state)
{
	if (action & SCT_EV_LIMIT)
		SCT_LIMIT |= 1 << ev;
	if (action & SCT_EV_LIMIT_H)
		SCT_LIMIT_H |= 1 << ev;

	if (action & SCT_EV_HALT)
		SCT_HALT |= 1 << ev;
	if (action & SCT_EV_HALT_H)
		SCT_HALT_H |= 1 << ev;

	if (action & SCT_EV_STOP)
		SCT_STOP |= 1 << ev;
	if (action & SCT_EV_STOP_H)
		SCT_STOP_H |= 1 << ev;

	if (action & SCT_EV_START)
		SCT_START |= 1 << ev;
	if (action & SCT_EV_START_H)
		SCT_START_H |= 1 << ev;

	if (action & SCT_EV_INT)
		SCT_EVEN |= 1 << ev;

	if (action & SCT_EV_OUT0_SET)
		SCT_OUT0_SET |= 1 << ev;
	if (action & SCT_EV_OUT0_CLR)
		SCT_OUT0_CLR |= 1 << ev;
	if (action & SCT_EV_OUT1_SET)
		SCT_OUT1_SET |= 1 << ev;
	if (action & SCT_EV_OUT1_CLR)
		SCT_OUT1_CLR |= 1 << ev;
	if (action & SCT_EV_OUT2_SET)
		SCT_OUT2_SET |= 1 << ev;
	if (action & SCT_EV_OUT2_CLR)
		SCT_OUT2_CLR |= 1 << ev;
	if (action & SCT_EV_OUT3_SET)
		SCT_OUT3_SET |= 1 << ev;
	if (action & SCT_EV_OUT3_CLR)
		SCT_OUT3_CLR |= 1 << ev;

	if (action & SCT_EV_CAP0)
		SCT_CAP0 |= 1 << ev;
	if (action & SCT_EV_CAP0_H)
		SCT_CAP0_H |= 1 << ev;
	if (action & SCT_EV_CAP1)
		SCT_CAP1 |= 1 << ev;
	if (action & SCT_EV_CAP1_H)
		SCT_CAP1_H |= 1 << ev;
	if (action & SCT_EV_CAP2)
		SCT_CAP2 |= 1 << ev;
	if (action & SCT_EV_CAP2_H)
		SCT_CAP2_H |= 1 << ev;
	if (action & SCT_EV_CAP3)
		SCT_CAP3 |= 1 << ev;
	if (action & SCT_EV_CAP3_H)
		SCT_CAP3_H |= 1 << ev;
	if (action & SCT_EV_CAP4)
		SCT_CAP4 |= 1 << ev;
	if (action & SCT_EV_CAP4_H)
		SCT_CAP4_H |= 1 << ev;

	SCT_EV_CTRL(ev) = (match & 0xf) | (io & 0xf) << 6 |
		(state & 0x1f) << 15 |
		((action & SCT_EV_STATE_LOAD) ? SCT_EV_CTRL_STATELD : 0) |
		(op & ~0xfc3cf);
	SCT_EV_STATE(ev) = op & 3;
}

void sct_set_output_set(int out, int event)
{
	SCT_OUT_SET(out) = event;
}

void sct_set_output_clear(int out, int event)
{
	SCT_OUT_CLR(out) = event;
}
