# Makefile for libopenlpc81x/include/*.html

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

TREETOP		= ..
RELPATH		= $(TREETOP)/$(LIBDIR)
SRCDIR		= $(RELPATH)/include
SRCS		= $(wildcard $(SRCDIR)/*.h)
HTMLS		= $(sort $(patsubst %.h,%.h.html,$(notdir $(SRCS))))

.PHONY: all clean

all: $(HTMLS) include.html

%.h.html: $(SRCDIR)/%.h
	echo "  $(<F)"
	expand $< > $(<F)
	source-highlight --no-doc -n --gen-references=inline \
	--css=default.css -i $(<F) -o $@
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

include.html:
	echo -e "<!DOCTYPE html>\n<html>\n<head>" > $@
	echo "<title>include/</title>" >> $@
	echo -n "<link href=\"$(TREETOP)/stylesheets/stylesheet.css\" " >> $@
	echo "rel=\"stylesheet\" type=\"text/css\">" >> $@
	cat $(TREETOP)/page_header >> $@
	$(TREETOP)/make_header >> $@
	echo "<pre>" >> $@
	for i in $(HTMLS); \
	do \
		echo "|-- <a href=\"$$i\">$${i%.*}</a>" >> $@; \
	done
	echo "|" >> $@
	echo -n "\`-- nxp_lpc/ --- " >> $@
	echo "<a href=\"nxp_lpc/lpc81x/lpc81x.html\">lpc81x/</a>" >> $@
	echo "</pre>" >> $@
	cat $(TREETOP)/page_footer >> $@

clean:
	rm -f $(HTMLS) include.html tmp tags $(HTMLS:.h.html=.h)
