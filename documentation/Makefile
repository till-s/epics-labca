#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# Makefile,v 1.3 2002/08/02 15:38:41 jba Exp
#
TOP = ..
include $(TOP)/config/CONFIG_APP

HELPS+=help/ezcaDebugOn.html
HELPS+=help/ezcaGetControlLimits.html
HELPS+=help/ezcaGetGraphicLimits.html
HELPS+=help/ezcaGet.html
HELPS+=help/ezcaGetNelem.html
HELPS+=help/ezcaGetPrecision.html
HELPS+=help/ezcaGetRetryCount.html
HELPS+=help/ezcaGetStatus.html
HELPS+=help/ezcaGetTimeout.html
HELPS+=help/ezcaGetUnits.html
HELPS+=help/ezcaPut.html
HELPS+=help/ezcaSetSeverityWarnLevel.html
HELPS+=help/whatis.htm

MHELPS=$(HELPS:%.html=%.m)

DOCS+=manual.pdf
MANHTMLS=$(wildcard manual/*)
HTMLS+=$(HELPS) $(MANHTMLS)

$(INSTALL_HTML)/$(HTMLS_DIR)/%: manual/%
	@echo "Installing Html Documentation $@"
	@$(INSTALL) -d -m 644 $< $(@D)

$(INSTALL_HTML)/$(HTMLS_DIR)/%: help/%
	@echo "Installing Scilab Help $@"
	@$(INSTALL) -d -m 644 $< $(@D)

include $(TOP)/config/RULES.Host


%.pdf %.aux: %.tex
	pdflatex $^
	pdflatex $^

$(sort manual/index.html $(wildcard manual/*.html)): manual.aux
	latex2html -no_footnode -split 5 manual

$(HELPS): manual.aux
	if [ ! -x help ] ; then mkdir help; fi
	latex2html -no_footnode -split 5 -link 0 -no_navigation -no_antialias_text -long_titles 1 -dir help manual
	cp whatis.htm help
	cp Contents.m help

%.m:%.html
	$(RM) $@
	lynx -dump $^ | sed -e 's/^/%/g' > $@

build:: manual/index.html $(MHELPS)

# make again for the HTMLS wildcard expansion to find the newly created files
ifeq ($(MANHTMLS)xx,xx)
buildInstall::
	$(MAKE)
endif

.SECONDARY:

purge::
	$(RM) -r help
	$(RM) -r manual 
	$(RM) *.aux *.dvi *.log *.pdf *.out *.ps
	$(RM) *~
