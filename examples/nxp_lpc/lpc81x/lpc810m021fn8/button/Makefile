# Makefile for button

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

NAME		= button
OBJS		= button.o
OUTFILES	= $(NAME).bin $(NAME).list
LDSCRIPT	= ldscripts/lpc810.x
LDSPECS		=
LIBDIR		?= ../../../../..

CC		= arm-none-eabi-gcc
LD		= arm-none-eabi-gcc
OBJCOPY		= arm-none-eabi-objcopy
OBJDUMP		= arm-none-eabi-objdump
SIZE		= arm-none-eabi-size

ARCHFLAGS	= -mthumb -mcpu=cortex-m0plus
CFLAGS		+= -MMD -Os \
		   -Wall -Wextra -Wimplicit-function-declaration \
		   -Wredundant-decls -Wstrict-prototypes -Wundef \
		   -I$(LIBDIR)/include -I$(LIBDIR)/include/nxp_lpc/lpc81x \
		   -fno-common -fstack-usage $(ARCHFLAGS)
LDFLAGS		+= -L$(LIBDIR)/lib/nxp_lpc/lpc81x -llpc81x \
		   -T $(LDSCRIPT) -nostartfiles $(LDSPECS) \
	           -Wl,--gc-sections -Wl,-Map=$(NAME).map -Wl,--cref \
		   $(ARCHFLAGS)

USARTUTIL	?= $(LIBDIR)/tools/nxp_lpc/lpc81x/usart-util/usart-util
ifneq ($(strip &(USARTUTIL_DEVICE)),)
USARTUTIL	+= -d $(USARTUTIL_DEVICE)
endif

.PHONY: all clean flash

all: $(OUTFILES)

%.bin: %.elf
	echo "  $@"
	$(OBJCOPY) -O binary $< $@

%.hex: %.elf
	echo "  $@"
	$(OBJCOPY) -O ihex $< $@

%.srec: %.elf
	echo "  $@"
	$(OBJCOPY) -O srec $< $@

%.list: %.elf
	echo "  $@"
	$(OBJDUMP) -d $< > $@
	$(OBJDUMP) -t $< >> $@

$(NAME).elf: $(OBJS)
	echo "  $@"
	$(LD) -o $@ $(OBJS) $(LDFLAGS)
	$(SIZE) $(NAME).elf

%.o: %.c
	echo "  $<"
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OUTFILES) $(NAME).elf $(OBJS) $(OBJS:.o=.d) $(OBJS:.o=.su) \
	$(NAME).map

flash: $(NAME).bin
	$(USARTUTIL) -D $<
#	@$(USARTUTIL) -D $< 2>&1 | grep --color=never wrote

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJS:.o=.d)
endif
