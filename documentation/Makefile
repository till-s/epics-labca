HELPS+=manual/ezcaDebugOn.html
HELPS+=manual/ezcaGetControlLimits.html
HELPS+=manual/ezcaGetGraphicLimits.html
HELPS+=manual/ezcaGet.html
HELPS+=manual/ezcaGetNelem.html
HELPS+=manual/ezcaGetPrecision.html
HELPS+=manual/ezcaGetRetryCount.html
HELPS+=manual/ezcaGetStatus.html
HELPS+=manual/ezcaGetTimeout.html
HELPS+=manual/ezcaGetUnits.html
HELPS+=manual/ezcaPut.html
HELPS+=manual/ezcaSetSeverityWarnLevel.html

MHELPS=$(HELPS:%.html=%.m)

all: manual.pdf htmdoc help

htmdoc: html html/index.html manual.aux

%.dvi %.aux: %.tex
	latex $^
	latex $^

%.pdf: %.dvi
	dvipdf $^ $@

html:
	mkdir $@

$(sort html/index.html $(wildcard html/*.html)): manual.tex
	latex2html -no_footnode -dir html manual

help: $(HELPS) $(MHELPS)

$(HELPS):%.html: manual.tex
	latex2html -no_navigation -no_footnode -no_antialias_text -long_titles 1 manual

%.m:%.html
	$(RM) $@
	lynx -dump $^ | sed -e 's/^/%/g' > $@


purge:
	$(RM) -r html
	$(RM) -r manual 
	$(RM) *.aux *.dvi *.log *.pdf
	$(RM) *~
