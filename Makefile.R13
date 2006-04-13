#Makefile at top of application tree
TOP = .
include $(TOP)/config/CONFIG_APP
DIRS += config
DIRS += ezca
DIRS += glue
DIRS += documentation
ifeq ($(MAKEFOR),MATLAB)
DIRS += matlab
endif
include $(TOP)/config/RULES_TOP
