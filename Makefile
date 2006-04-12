#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG
DIRS += configure
DIRS += ezca
DIRS += glue
DIRS += documentation
ifeq ($(MAKEFOR),MATLAB)
DIRS += matlab
endif
include $(TOP)/configure/RULES_TOP
