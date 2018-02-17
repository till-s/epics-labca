#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG
DIRS += configure
DIRS += ezca
DIRS += glue
ifndef NO_DOCS
DIRS += documentation
endif
ifeq ($(MAKEFOR),MATLAB)
DIRS += matlab
endif
DIRS += testing
include $(TOP)/configure/RULES_TOP

UNINSTALL_DIRS+=jar

tarclean: UNINSTALL_DIRS:=$(filter-out %jar %html %doc,$(UNINSTALL_DIRS))

tarclean: uninstall

tar: images
	@if [ -z "$(TAG)" ] || ! (git tag -l | grep -q "$(TAG)") ; then \
		echo "Need a git tag to create archive (define TAG= on cmdline)"; \
		exit 1; \
	else \
		git archive --prefix=$(TAG)/ --format=tgz -o images/$(TAG).tgz $(TAG); \
	fi

images:
	mkdir -p $@
