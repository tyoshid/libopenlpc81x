# Makefile for libopenlpc81x/lib/nxp_lpc/lpc81x/*.html

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
LDSCRIPT	?= ldscripts/lpc810.x

TREETOP		= ../../..
RELPATH		= $(TREETOP)/$(LIBDIR)
SRCDIR		= $(RELPATH)/lib/nxp_lpc/lpc81x
SRCS		= $(wildcard $(SRCDIR)/*.c)
HTMLS		= $(sort $(patsubst %.c,%.c.html,$(notdir $(SRCS))))

CPP		= gcc -E
CPPFLAGS	= -I$(TREETOP)/include -I$(TREETOP)/include/nxp_lpc/lpc81x -MM

.PHONY: all clean

all: Makefile.html $(HTMLS) lpc81x.html

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

vector.c.html: $(SRCDIR)/vector.c $(SRCDIR)/$(LDSCRIPT)
	echo "  $(<F)"
	(cd $(SRCDIR); ctags -n -f - \
	--langdef=ldscript --langmap=ldscript:.x \
	--regex-ldscript="/([a-zA-Z_.][a-zA-Z0-9_.-]*)\s*\
	(\([RWXAILrwxail\!]*\))?\s*:\s*(ORIGIN|org|o|LENGTH|len|l)\s*=\
	/\1/d,definition/" \
	--regex-ldscript="/REGION_ALIAS\s*\(\s*\"([a-zA-Z0-9_.-]+)\"\
	/\1/d,definition/" \
	--regex-ldscript="/(^|;|\{|\}|\()\s*((\.[a-zA-Z0-9_.-]+)\
	|([a-zA-Z_][a-zA-Z0-9_.-]*))\s*(=|\+=|-=|\*=|\/=|<<=|>>=|&=|\|=)\
	/\2/v,variable/" $(LDSCRIPT) > $(CURDIR)/tags; \
	ctags -n -f - $(<F) >> $(CURDIR)/tags;)
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

vpath %.c $(SRCDIR)
vpath %.h $(SRCDIR)

%.c.html: %.d
	echo "  $(@:%.c.html=%.c)"
	(cd $(SRCDIR); ctags -n -f - \
	$(subst $(SRCDIR)/,,$(filter-out $<,$^)) > $(CURDIR)/tags;)
	expand $(SRCDIR)/$(@:%.c.html=%.c) > $(@:%.c.html=%.c)
	source-highlight --no-doc -n --gen-references=inline --ctags= \
	--css=default.css -i $(@:%.c.html=%.c) -o $@
	mv $@ tmp
	rm $(@:%.c.html=%.c)
	rm tags
	echo -e "<!DOCTYPE html>\n<html>\n<head>" > $@
	echo "<title>$(@:%.c.html=%.c)</title>" >> $@
	echo -n "<link href=\"$(TREETOP)/stylesheets/stylesheet.css\" " >> $@
	echo "rel=\"stylesheet\" type=\"text/css\">" >> $@
	echo -n "<link href=\"$(TREETOP)/stylesheets/" >> $@
	echo -n "source-highlight.css\" " >> $@
	echo "rel=\"stylesheet\" type=\"text/css\">" >> $@
	cat $(TREETOP)/page_header >> $@
	$(TREETOP)/make_header $(@:%.c.html=%.c) >> $@
	cat tmp >> $@
	rm tmp
	cat $(TREETOP)/page_footer >> $@

%.d: %.c
	(cd $(SRCDIR); $(CPP) $(CPPFLAGS) $(<F) | \
	sed 's/$(<F:%.c=%.o)/$(<F:%.c=%.c.html)/' > $(CURDIR)/$@;)

lpc81x.html:
	echo -e "<!DOCTYPE html>\n<html>\n<head>" > $@
	echo "<title>lpc81x/</title>" >> $@
	echo -n "<link href=\"$(TREETOP)/stylesheets/stylesheet.css\" " >> $@
	echo "rel=\"stylesheet\" type=\"text/css\">" >> $@
	cat $(TREETOP)/page_header >> $@
	$(TREETOP)/make_header >> $@
	echo "<pre>" >> $@
	echo "|-- <a href=\"Makefile.html\">Makefile</a>" >> $@
	for i in $(HTMLS); \
	do \
		echo "|-- <a href=\"$$i\">$${i%.*}</a>" >> $@; \
	done
	echo -e -n "|\n\`-- " >> $@
	echo "<a href=\"ldscripts/ldscripts.html\">ldscripts/</a>" >> $@
	echo "</pre>" >> $@
	cat $(TREETOP)/page_footer >> $@

clean:
	rm -f Makefile.html $(HTMLS) lpc81x.html tmp tags makefile.lang \
	mkfile $(HTMLS:.c.html=.c) $(HTMLS:.c.html=.d)

ifneq ($(MAKECMDGOALS),clean)
-include $(HTMLS:.c.html=.d)
endif
