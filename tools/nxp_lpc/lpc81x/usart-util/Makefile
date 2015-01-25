# Makefile for libopenlpc81x/tools/nxp_lpc/lpc81x/usart-util/*.html

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

LIBRARYDIR	?= ../../libopenlpc81x

TREETOP		= ../../../..
RELPATH		= $(TREETOP)/$(LIBRARYDIR)
SRCDIR		= $(RELPATH)/tools/nxp_lpc/lpc81x/usart-util
SRCS		= $(wildcard $(SRCDIR)/*.c)
HDRS		= $(wildcard $(SRCDIR)/*.h)

HTMLS		= $(sort $(notdir $(patsubst %.c,%.c.html,$(SRCS)) \
		  $(patsubst %.h,%.h.html,$(HDRS))))

CURDIR		= $(shell pwd)

CPP		= gcc -E
CPPFLAGS	= -MM

.PHONY: all clean

all: Makefile.html $(HTMLS) usart-util.html

Makefile.html: $(SRCDIR)/Makefile
	echo "  $(<F)"
	expand $< > mkfile
	ln -f -s $(TREETOP)/makefile.lang
	ctags -n --language-force=Make mkfile
	source-highlight --no-doc -n --gen-references=inline --ctags= \
	--css=default.css --src-lang=makefile -i mkfile -o $@
	mv $@ tmp
	rm tags
	rm makefile.lang
	rm mkfile
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

vpath %.h $(SRCDIR)
vpath %.c $(SRCDIR)

%.h.html: %.h
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

%.c.html: %.c
	echo "  $(<F)"
	(cd $(SRCDIR); ctags -n -f - \
	$(subst $(SRCDIR)/,,$^) > $(CURDIR)/tags;)
	expand $< > $(<F)
	source-highlight --no-doc -n --gen-references=inline --ctags= \
	--css=default.css -i $(<F) -o $@
	mv $@ tmp
	rm $(<F)
	rm tags
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

%.d: %.c
	(cd $(SRCDIR); $(CPP) $(CPPFLAGS) $(<F) | \
	sed 's/$(<F:%.c=%.o)/$(<F:%.c=%.c.html)/' > $(CURDIR)/$@;)
	for i in `cat $@`; \
	do \
	  if [ $${i##*.} = h ] ; \
	  then \
	    f=$${i%.h}.c; \
	    if [ "$${f}" != "$(<F)" ] ; \
	    then \
	      sed -i '$$ s/$$/ \\/' $@; \
	      echo " $${f}" >> $@; \
	    fi; \
	  fi; \
	done

usart-util.html:
	echo -e "<!DOCTYPE html>\n<html>\n<head>" > $@
	echo "<title>usart-util/</title>" >> $@
	echo -n "<link href=\"$(TREETOP)/stylesheets/stylesheet.css\" " >> $@
	echo "rel=\"stylesheet\" type=\"text/css\">" >> $@
	cat $(TREETOP)/page_header >> $@
	$(TREETOP)/make_header >> $@
	echo "<pre>" >> $@
	echo "|-- <a href=\"Makefile.html\">Makefile</a>" >> $@
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
	rm -f Makefile.html $(HTMLS) usart-util.html \
	tmp tags makefile.lang \
	mkfile $(notdir $(HDRS)) $(notdir $(SRCS)) $(notdir $(SRCS:.c=.d))

ifneq ($(MAKECMDGOALS),clean)
-include $(notdir $(SRCS:.c=.d))
endif
