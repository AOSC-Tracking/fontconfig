# -*- encoding: utf-8 -*-
#
# fontconfig/doc/Makefile.am
#
# Copyright © 2003 Keith Packard
#
# Permission to use, copy, modify, distribute, and sell this software and its
# documentation for any purpose is hereby granted without fee, provided that
# the above copyright notice appear in all copies and that both that
# copyright notice and this permission notice appear in supporting
# documentation, and that the name of the author(s) not be used in
# advertising or publicity pertaining to distribution of the software without
# specific, written prior permission.  The authors make no
# representations about the suitability of this software for any purpose.  It
# is provided "as is" without express or implied warranty.
#
# THE AUTHOR(S) DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
# EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
# DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
# TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

NULL =
EXTRA_DIST =			\
	$(BUILT_DOCS)		\
	$(DOC_FUNCS_FNCS)	\
	$(HTML_DIR)/*		\
	$(XML_FILES)		\
	$(check_SCRIPTS)	\
	confdir.xml.in		\
	func.xml		\
	$(NULL)
BUILT_SOURCES =			\
	$(DOC_FUNCS_XML)	\
	$(NULL)

if USEDOCBOOK
maintainerdoccleanfiles =	\
	$(NULL)
cleandocfiles =			\
	$(BUILT_DOCS)		\
	$(NULL)
else
maintainerdoccleanfiles =	\
	$(BUILT_DOCS)		\
	$(NULL)
cleandocfiles =			\
	$(NULL)
endif
MAINTAINERCLEANFILES =			\
	$(DOC_FUNCS_XML)		\
	$(maintainerdoccleanfiles)	\
	$(NULL)
CLEANFILES =			\
	$(cleandocfiles)	\
	$(LOCAL_XML_FILES)	\
	confdir.xml		\
	func.refs		\
	$(NULL)
SUFFIXES =	\
	.fncs	\
	.xml	\
	.txt	\
	.html	\
	$(NULL)
TESTS =				\
	check-missing-doc	\
	$(NULL)
TESTS_ENVIRONMENT = \
	top_srcdir=${top_srcdir}; export top_srcdir; \
	$(NULL)
LOG_COMPILER = sh
#
DOC2HTML = docbook2html
DOC2TXT  = docbook2txt
DOC2MAN  = docbook2man
DOC2PDF  = docbook2pdf

DOC_FUNCS_FNCS =		\
	fcatomic.fncs		\
	fcblanks.fncs		\
	fccache.fncs		\
	fccharset.fncs		\
	fcconfig.fncs		\
	fcconstant.fncs		\
	fcdircache.fncs		\
	fcfile.fncs		\
	fcfontset.fncs		\
	fcformat.fncs		\
	fcfreetype.fncs		\
	fcinit.fncs		\
	fclangset.fncs		\
	fcmatrix.fncs		\
	fcobjectset.fncs	\
	fcobjecttype.fncs	\
	fcpattern.fncs		\
	fcrange.fncs		\
	fcstring.fncs		\
	fcstrset.fncs		\
	fcvalue.fncs		\
	fcweight.fncs		\
	$(NULL)
XML_FILES =			\
	fontconfig-user.xml	\
	fontconfig-devel.xml	\
	$(NULL)
LOCAL_XML_FILES =			\
	local-fontconfig-user.xml	\
	local-fontconfig-devel.xml	\
	$(NULL)

DOC_FUNCS_XML = $(DOC_FUNCS_FNCS:.fncs=.xml)
BUILT_DOCS =		\
	$(HTML_FILES)	\
	$(PDF_FILES)	\
	$(TXT_FILES)	\
	$(man3_MANS)	\
	$(man5_MANS)	\
	$(NULL)
DOCS_DEPS =			\
	$(DOC_FUNCS_XML)	\
	confdir.xml		\
	version.xml		\
	$(NULL)

TXT_FILES = $(XML_FILES:.xml=.txt)
PDF_FILES = $(XML_FILES:.xml=.pdf)
HTML_FILES =			\
	fontconfig-user.html	\
	$(NULL)
HTML_DIR = fontconfig-devel
#
noinst_PROGRAMS =	\
	$(NULL)
noinst_SCRIPTS =		\
	edit-xml.py		\
	$(NULL)
##
check_SCRIPTS =			\
	check-missing-doc	\
	$(NULL)
#
man3_MANS =		\
	$(DOCMAN3)	\
	$(NULL)
man5_MANS =		\
	fonts-conf.5	\
	$(NULL)
#
doc_DATA =		\
	$(TXT_FILES)	\
	$(PDF_FILES)	\
	$(HTML_FILES)	\
	$(NULL)
#
htmldocdir = $(docdir)/$(HTML_DIR)
htmldoc_DATA =		\
	$(NULL)

if USEDOCBOOK
BUILT_SOURCES +=		\
	$(LOCAL_XML_FILES)	\
	$(NULL)
htmldoc_DATA += $(HTML_DIR)/*

##
.fncs.xml:
	$(AM_V_GEN) $(RM) $@; \
	$(PYTHON) $(srcdir)/edit-xml.py $(srcdir)/func.xml '$(srcdir)/$*.fncs' $*.xml
.xml.txt:
	$(AM_V_GEN) $(RM) $@; \
	$(DOC2TXT) $*.xml
.xml.pdf:
	$(AM_V_GEN) $(RM) $@; \
	$(DOC2PDF) $*.xml
.xml.html:
	$(AM_V_GEN) $(RM) $@; \
	$(DOC2HTML) -u $*.xml > $@
##
fonts-conf.5: local-fontconfig-user.xml version.xml confdir.xml
	$(AM_V_GEN) $(RM) $@; \
	$(DOC2MAN) local-fontconfig-user.xml && \
	$(RM) manpage.*
##
$(man3_MANS): func.refs
func.refs: local-fontconfig-devel.xml $(DOCS_DEPS)
	$(AM_V_GEN) $(RM) $@; \
	$(DOC2MAN) -o devel-man local-fontconfig-devel.xml && \
	mv devel-man/manpage.refs func.refs &&	\
	mv devel-man/*.3 . &&			\
	$(RM) devel-man/manpage.* &&		\
	rmdir devel-man || rm $@ || :
confdir.xml: $(srcdir)/confdir.xml.in
	$(AM_V_GEN) sed -e 's,@BASECONFIGDIR\@,${BASECONFIGDIR},' $(srcdir)/$@.in | awk '{if (NR > 1) printf("\n"); printf("%s", $$0);}' > $@
##
$(DOC_FUNCS_XML): $(DOC_FUNCS_FNCS) $(srcdir)/edit-xml.py $(srcdir)/func.xml
$(TXT_FILES): $(DOCS_DEPS)
$(PDF_FILES): $(DOCS_DEPS)
$(HTML_FILES): $(DOCS_DEPS)
$(HTML_DIR)/*: $(HTML_DIR)
$(HTML_DIR): local-fontconfig-devel.xml $(DOCS_DEPS)
	$(AM_V_GEN) $(RM) -r $@; \
	$(DOC2HTML) -V '%use-id-as-filename%' -o $@ local-fontconfig-devel.xml
local-fontconfig-user.xml: $(srcdir)/fontconfig-user.xml
	$(AM_V_GEN) $(LN_S) $(srcdir)/fontconfig-user.xml $@;	\
	[ ! -f $(builddir)/fontconfig-user.xml ] && cp -a $(srcdir)/fontconfig-user.xml $(builddir)/fontconfig-user.xml || :
local-fontconfig-devel.xml: $(srcdir)/fontconfig-devel.xml
	$(AM_V_GEN) $(LN_S) $(srcdir)/fontconfig-devel.xml $@;	\
	[ ! -f $(builddir)/fontconfig-devel.xml ] && cp -a $(srcdir)/fontconfig-devel.xml $(builddir)/fontconfig-devel.xml || :
#
all-local: $(BUILT_DOCS) $(HTML_DIR)/*
clean-local:
	$(RM) -r $(HTML_DIR) devel-man
	[ "x$(builddir)" != "x$(srcdir)" ] && $(RM) $(builddir)/*.xml || :
dist-local-check-docs-enabled:
	@true
else
htmldoc_DATA += $(srcdir)/$(HTML_DIR)/*
.fncs.xml:
	$(AM_V_GEN) $(RM) $@; \
	touch -r $< $@
all-local:
clean-local:
dist-local-check-docs-enabled:
	@echo "*** --enable-man must be used in order to make dist"
	@false
endif

# force doc rebuild after configure
dist-hook-local: dist-local-check-docs-enabled

-include $(top_srcdir)/git.mk
