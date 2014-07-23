# Makefile for libopenlpc81x

# Copyright 2014 Toshiaki Yoshida <yoshida@mpc.net>

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

LIBS		= lib/nxp_lpc/lpc81x
TOOLS		= tools/nxp_lpc/lpc81x/usart-util

EXAMPLES	= examples/nxp_lpc/lpc81x/lpc810m021fn8/miniblink \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/fancyblink \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/button \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/pinint \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/usart \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/usart_irq \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/sct \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/sct_pwm \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/piezo \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/rc_servo \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/mrt \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/wwdt \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/wkt \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/systick \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/i2c_rom \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/i2c_proximity \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/spi_rom \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/spi_barometer \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/acmp \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/crc \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/flashcon \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/flash_iap \
		  examples/nxp_lpc/lpc81x/lpc810m021fn8/rom_api

ifeq ($(MAKECMDGOALS), clean_example)
EXAMPLEGOAL	= clean
endif

.PHONY: all clean $(LIBS) $(TOOLS)

all clean: $(LIBS) $(TOOLS)

$(LIBS) $(TOOLS):
	@echo "-- $@ --"
	@$(MAKE) -C $@ -s $(MAKECMDGOALS)

.PHONY: example clean_example $(EXAMPLES)

example clean_example: $(EXAMPLES)

$(EXAMPLES):
	@echo "-- $@ --"
	@$(MAKE) -C $@ -s $(EXAMPLEGOAL)
