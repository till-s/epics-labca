#Makefile at top of application tree
TOP = .
include $(TOP)/config/CONFIG_APP
DIRS += config
DIRS += ezca
DIRS += glue
include $(TOP)/config/RULES_TOP
