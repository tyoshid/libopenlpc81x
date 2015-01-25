# Makefile for libopenlpc81x/lib/nxp_lpc/lpc81x/ldscripts/*.html

# Copyright 2015 Toshiaki Yoshida <yoshida@mpc.net>

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

LIBDIR		?= ../../libopenlpc81x

TREETOP		= ../../../..
RELPATH		= $(TREETOP)/$(LIBDIR)
SRCDIR		= $(RELPATH)/lib/nxp_lpc/lpc81x/ldscripts
SRCS		= $(wildcard $(SRCDIR)/*.x)
HTMLS		= $(sort $(patsubst %.x,%.x.html,$(notdir $(SRCS))))
CURDIR		= $(shell pwd)

.PHONY: all clean

all: $(HTMLS) ldscripts.html

%.x.html: $(SRCDIR)/%.x $(SRCDIR)/../vector.c
	echo "  $(<F)"
	(cd $(SRCDIR); ctags -n -f - ../vector.c > $(CURDIR)/tags;)
	expand $< > $(<F)
	ctags -a -n --langdef=ldscript --langmap=ldscript:.x \
	--regex-ldscript="/([a-zA-Z_.][a-zA-Z0-9_.-]*)\s*\
	(\([RWXAILrwxail\!]*\))?\s*:\s*(ORIGIN|org|o|LENGTH|len|l)\s*=\
	/\1/d,definition/" \
	--regex-ldscript="/REGION_ALIAS\s*\(\s*\"([a-zA-Z0-9_.-]+)\"\
	/\1/d,definition/" \
	--regex-ldscript="/(^|;|\{|\}|\()\s*((\.[a-zA-Z0-9_.-]+)\
	|([a-zA-Z_][a-zA-Z0-9_.-]*))\s*(=|\+=|-=|\*=|\/=|<<=|>>=|&=|\|=)\
	/\2/v,variable/" $(<F)
	source-highlight --no-doc -n --gen-references=inline --ctags= \
	--lang-def=ldscript.lang --css=default.css -i $(<F) -o $@
	mv $@ tmp
	rm tags
	rm $(<F)
	echo -e "<!DOCTYPE html>\n<html>\n<head>" > $@
	echo "<title>$(<F)</title>" >> $@
	echo -n "<link href=\"$(TREETOP)/stylesheets/stylesheet.css\" " >> $@
	echo "rel=\"stylesheet\" type=\"text/css\">" >> $@
	echo -n "<link href=\"$(TREETOP)/stylesheets/" >> $@
	echo -n "source-highlight.css\" " >> $@
	echo "rel=\"stylesheet\" type=\"text/css\">" >> $@
	cat $(TREETOP)/page_header >> $@
	$(TREETOP)/make_header $(<F) >> $@
	cat tmp >> $@
	rm tmp
	cat $(TREETOP)/page_footer >> $@

ldscripts.html:
	echo -e "<!DOCTYPE html>\n<html>\n<head>" > $@
	echo "<title>ldscripts/</title>" >> $@
	echo -n "<link href=\"$(TREETOP)/stylesheets/stylesheet.css\" " >> $@
	echo "rel=\"stylesheet\" type=\"text/css\">" >> $@
	cat $(TREETOP)/page_header >> $@
	$(TREETOP)/make_header >> $@
	echo "<pre>" >> $@
	for i in $(HTMLS); \
	do \
		if [ "$${i}" = $(lastword $(HTMLS)) ] ; then \
			echo "\`-- <a href=\"$$i\">$${i%.*}</a>" >> $@; \
		else \
			echo "|-- <a href=\"$$i\">$${i%.*}</a>" >> $@; \
		fi \
	done
	echo "</pre>" >> $@
	cat $(TREETOP)/page_footer >> $@

clean:
	rm -f $(HTMLS) ldscripts.html tmp tags $(HTMLS:.x.html=.x)
