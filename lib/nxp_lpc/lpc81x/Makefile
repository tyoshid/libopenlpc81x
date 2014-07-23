# Makefile for liblpc81x.a

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

LIB		= liblpc81x.a
OBJS		= vector.o syscon.o pmu.o gpio.o nvic.o pinint.o usart.o \
                  sct.o mrt.o wwdt.o scb.o wkt.o systick.o i2c.o spi.o \
                  acmp.o crc.o flashcon.o

CC		= arm-none-eabi-gcc
AR		= arm-none-eabi-ar
CFLAGS		= -MMD -Os \
		  -Wall -Wextra -Wimplicit-function-declaration \
		  -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
		  -Wundef -Wshadow \
		  -I../../../include -I../../../include/nxp_lpc/lpc81x \
		  -ffreestanding -fno-common \
		  -ffunction-sections -fdata-sections \
		  -mthumb -mcpu=cortex-m0plus
ARFLAGS		= rcs

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJS)
	echo "  $(@F)"
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	echo "  $(<F)"
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(LIB) $(OBJS) $(OBJS:.o=.d)

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJS:.o=.d)
endif
