## Process this file with automake to produce Makefile.in
# $Id: Makefile.am,v 1.26 2007-09-15 18:02:36 ahuillet Exp $

SUBDIRS = manual tools

# Special construct to enable recursive call of doc ;
#   1/ A suffix is added to each dir in SUBDIRS (see DOCSUBDIRS)
#   2/ This new list is used as a target, to call a rule in loop on each
#      suffixed dir
#   3/ In this rule, the suffix is removed to extract the subdir name
#      (see the rule of the $(DOCSUBDIRS) target)
#
# Should be replaced by AM_EXTRA_RECURSIVE_TARGETS in top
# configure.ac, but this needs automake 1.13
# Inspired by http://lackof.org/taggart/hacking/make-example/

DOCSUBDIRS = $(SUBDIRS:%=%-doc)

doc: local-doc $(DOCSUBDIRS)

local-doc:
	@rm -f doxygen.log
	doxygen Doxyfile
	@cp *.png $(top_srcdir)/doxygen

$(DOCSUBDIRS): 
	$(MAKE) -C $(@:%-doc=%) $(AM_MAKEFLAGS) doc

.PHONY: local-doc $(DOCSUBDIRS)
