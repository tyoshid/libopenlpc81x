
LIBDIR		?= ../../libopenlpc81x

TREETOP		= ../../../../..
RELPATH		= $(TREETOP)/$(LIBDIR)
SRCDIR		= $(RELPATH)/examples/nxp_lpc/lpc81x/lpc810m021fn8/$(NAME)
SRCS		= $(SRCDIR)/$(NAME).c
HTMLS		= $(NAME).c.html

ifeq ($(strip $(REFERENCE)),)
PDF		= $(NAME).pdf
SCHEMATIC	= schematic.jpg
LAYOUT		= layout.jpg
else
PDF		= ../$(REFERENCE)/$(REFERENCE).pdf
SCHEMATIC	= ../$(REFERENCE)/schematic.jpg
LAYOUT		= ../$(REFERENCE)/layout.jpg
endif

CPP		= gcc -E
CPPFLAGS	= -I$(TREETOP)/include -I$(TREETOP)/include/nxp_lpc/lpc81x -MM

.PHONY: all clean

all: $(NAME).html

$(NAME).html: src.html $(PDF) $(SCHEMATIC) $(LAYOUT) $(HTMLS) Makefile.html
	echo "  $@"
	awk '{ print } /<!-- header -->/ { \
	  system("$(TREETOP)/make_header"); \
	}' $< | \
	awk '{ print } /<!-- schematic -->/ { \
	  pdf = "$(PDF)"; \
	  jpg = "$(SCHEMATIC)"; \
	  "identify -format \"%w\" " jpg | getline width; \
	  "identify -format \"%h\" " jpg | getline height; \
	  printf("<h3><a href=\"%s\">Schematic</a></h3>\n", pdf); \
	  printf("<p class=\"picture\"><a href=\"%s\">", pdf); \
	  printf("<img src=\"%s\" width=\"%d\" ", jpg, width); \
	  printf("height=\"%d\" alt=\"$(NAME) schematic\"></a></p>\n",height); \
	}' | \
	awk '{ print } /<!-- layout -->/ { \
	  jpg = "$(LAYOUT)"; \
	  "identify -format \"%w\" " jpg | getline width; \
	  "identify -format \"%h\" " jpg | getline height; \
	  height = height / (width / 320.0) + 0.5; \
	  width = 320.0; \
	  printf("<h3><a href=\"%s\">Layout</a></h3>\n", jpg); \
	  printf("<p class=\"picture\"><a href=\"%s\">", jpg); \
	  printf("<img src=\"%s\" width=\"%d\" ", jpg, width); \
	  printf("height=\"%d\" alt=\"$(NAME) layout\"></a></p>\n",height); \
	}' | \
	awk '{ print } /<!-- source -->/ { \
	  printf("<h3><a href=\"$(NAME).c.html\">$(NAME).c</a></h3>\n"); \
	  while (getline x < "$(NAME).c.html" > 0) { \
	    if (flag == 0 && match(x, "<pre>")) \
	      flag = 1; \
	    if (flag == 1) \
	      print x; \
	    if (flag == 1 && match(x, "</pre>")) \
	      flag = 0; \
	  } \
	  printf("<h3><a href=\"Makefile.html\">Makefile</a></h3>\n"); \
	}' > $@

$(NAME).pdf: $(SRCDIR)/hardware/$(NAME).pdf
	echo "  $@"
	cp $< $@

schematic.jpg: $(NAME).pdf
	echo "  $@"
	convert -density 600 $< $@
	convert $@ -scale 640 $@

vpath %.c $(SRCDIR)
vpath %.h $(SRCDIR)

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
	sed 's/$(<F:.c=.o)/$(<F:.c=.c.html)/' > $(CURDIR)/$@;)
	for i in `cat $@`; \
	do \
	  if [ $${i##*.} = h ] ; \
	  then \
	    f=$${i##*/}; \
	    libfile=$(TREETOP)/lib/nxp_lpc/lpc81x/$${f%.h}.c; \
	    if [ -e $(SRCDIR)/$${libfile} ] ; \
	    then \
	      sed -i '$$ s/$$/ \\/' $@; \
	      echo " $${libfile}" >> $@; \
	    fi; \
	  fi; \
	done

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

clean:
	rm -f $(NAME).html $(NAME).pdf schematic.jpg $(HTMLS) Makefile.html \
	$(HTMLS:.c.html=.c) $(HTMLS:.c.html=.d)	mkfile \
	makefile.lang tmp tags

ifneq ($(MAKECMDGOALS),clean)
-include $(HTMLS:.c.html=.d)
endif
