# Makefile for gen_test_pattern

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

PROG	= gen_test_pattern

OBJS	= gen_test_pattern.o crc-ccitt.o crc16.o crc32.o

CC	= gcc
CFLAGS	= -MMD -O2 -Wall

.PHONY: all clean

all: $(PROG)

%.o: %.c
	echo "  $<"
	$(CC) $(CFLAGS) -c $<

$(PROG): $(OBJS)
	echo "  $@"
	$(CC) -o $(PROG) $(OBJS)

clean:
	rm -f $(PROG) $(OBJS) $(OBJS:.o=.d)

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJS:.o=.d)
endif
