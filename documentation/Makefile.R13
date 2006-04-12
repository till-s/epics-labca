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

HELPS+=help/lcaDebugOn.html
HELPS+=help/lcaGetControlLimits.html
HELPS+=help/lcaGetGraphicLimits.html
HELPS+=help/lcaGet.html
HELPS+=help/lcaGetNelem.html
HELPS+=help/lcaGetPrecision.html
HELPS+=help/lcaGetRetryCount.html
HELPS+=help/lcaGetStatus.html
HELPS+=help/lcaGetTimeout.html
HELPS+=help/lcaGetUnits.html
HELPS+=help/lcaPut.html
HELPS+=help/lcaPutNoWait.html
HELPS+=help/lcaSetSeverityWarnLevel.html
HELPS+=help/lcaClear.html
HELPS+=help/lcaSetMonitor.html
HELPS+=help/lcaNewMonitorValue.html
HELPS+=help/lcaDelay.html

MHELPS=$(HELPS:%.html=%.m)

DOCS+=manual.pdf
MANHTMLS=$(wildcard manual/*)
HTMLS+=$(HELPS) help/whatis.htm $(MANHTMLS)

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
	latex2html -no_footnode -split 5 -link 1 manual

$(HELPS): manual.aux
	if [ ! -x help ] ; then mkdir help; fi
	latex2html -no_footnode -split 5 -link 0 -no_navigation -no_antialias_text -long_titles 1 -dir help manual

help/whatis.htm: whatis.htm
	if [ ! -x help ] ; then mkdir help; fi
	$(RM) $@
	cp $< $@

help/Contents.m: Contents.m
	if [ ! -x help ] ; then mkdir help; fi
	$(RM) $@
	cp $< $@


# html->text, add leading '%' and remove long URLs, just leave filename
%.m:%.html
	$(RM) $@
	lynx -dump $^ | sed -e 's/^/%/g'  -e 's/\([0-9]*[.][ ]*\)\(file:\([^/]*[/]\)*\)/\1/g'  > $@

build:: manual/index.html $(MHELPS) help/Contents.m

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
