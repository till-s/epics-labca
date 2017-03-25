#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#	Makefile.Host

TOP = ..
include $(TOP)/configure/CONFIG

PROD_HOST += ezcaVarArrayTest

ezcaVarArrayTest_SRCS	+=	ezcaVarArrayTest.c
ezcaVarArrayTest_LIBS	+=	ezcamt
ezcaVarArrayTest_LIBS	+=	$(EPICS_BASE_IOC_LIBS)

install: buildInstall

buildInstall: build

ifneq '$(T_A)' ''
build: ../lcaTest.m

#convert scilab test script to matlab
../lcaTest.m:	../lcaTest.sce
	@if ! sed -e 's$$//$$%$$' -e 's/\<sleep(1000[*]/pause(/g' -e's/mtlb_//g' -e"s/%nan/nan('double')/g" -e 's/%[ \t]*MATLABWARN/disp/' -e 's/\([ \t]then\)\([ \t]\|$$\)/\2/g' $< > $@ ;  then \
		echo "*** WARNING: Unable to create test script for MATLAB" ;  \
		echo "%*** WARNING: Unable to create test script for MATLAB" > $@ ;  \
	fi

clean: test_clean

realclean: test_clean
endif

test_clean:
	$(RM) ../lcaTest.m

#test:	$(TOP)/bin/$(T_A)/ezcaVarArrayTest
#	@ $*

.PHONY:	test_clean test

include $(TOP)/configure/RULES
