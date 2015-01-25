# Makefile for libopenlpc81x/include/nxp_lpc/lpc81x/*.html

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

TREETOP		= ../../..
RELPATH		= $(TREETOP)/$(LIBDIR)
SRCDIR		= $(RELPATH)/include/nxp_lpc/lpc81x
SRCS		= $(wildcard $(SRCDIR)/*.h)
HTMLS		= $(sort $(patsubst %.h,%.h.html,$(notdir $(SRCS))))
CURDIR		= $(shell pwd)

CPP		= gcc -E
CPPFLAGS	= -I../.. -I. -MM

.PHONY: all clean

all: $(HTMLS) lpc81x.html

vpath %.h $(SRCDIR)

%.h.html: %.d
	echo "  $(@:%.h.html=%.h)"
	(cd $(SRCDIR); ctags -n -f - \
	$(subst $(SRCDIR)/,,$(filter-out $<,$^)) > $(CURDIR)/tags;)
	expand $(SRCDIR)/$(@:%.h.html=%.h) > $(@:%.h.html=%.h)
	source-highlight --no-doc -n --gen-references=inline --ctags= \
	--css=default.css -i $(@:%.h.html=%.h) -o $@
	mv $@ tmp
	rm tags
	rm $(@:%.h.html=%.h)
	echo -e "<!DOCTYPE html>\n<html>\n<head>" > $@
	echo "<title>$(@:%.h.html=%.h)</title>" >> $@
	echo -n "<link href=\"$(TREETOP)/stylesheets/stylesheet.css\" " >> $@
	echo "rel=\"stylesheet\" type=\"text/css\">" >> $@
	echo -n "<link href=\"$(TREETOP)/stylesheets/" >> $@
	echo -n "source-highlight.css\" " >> $@
	echo "rel=\"stylesheet\" type=\"text/css\">" >> $@
	cat $(TREETOP)/page_header >> $@
	$(TREETOP)/make_header $(@:%.h.html=%.h) >> $@
	cat tmp >> $@
	rm tmp
	cat $(TREETOP)/page_footer >> $@

%.d: %.h
	(cd $(SRCDIR); $(CPP) $(CPPFLAGS) $(<F) | \
	sed 's/$(<F:%.h=%.o)/$(<F:%.h=%.h.html)/' > $(CURDIR)/$@;)

lpc81x.html:
	echo -e "<!DOCTYPE html>\n<html>\n<head>" > $@
	echo "<title>lpc81x/</title>" >> $@
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
	rm -f $(HTMLS) lpc81x.html tmp tags $(HTMLS:.h.html=.h) \
	$(HTMLS:.h.html=.d)

ifneq ($(MAKECMDGOALS),clean)
-include $(HTMLS:.h.html=.d)
endif
