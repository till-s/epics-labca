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

tar: TAG:=$(shell git describe)

tar: images documentation
	@if [ -z "$(TAG)" ] ; then \
		echo "Need a git tag (valid description) to create archive"; \
		exit 1; \
	else \
		$(RM) images/$(TAG).tar; \
		git archive --prefix=$(TAG)/ --format=tar -o images/$(TAG).tar $(TAG); \
		git submodule foreach 'git archive --prefix=$(TAG)/$$path/ --format=tar -o $$toplevel/images/tmp.tar $$sha1; tar Af $$toplevel/images/$(TAG).tar $$toplevel/images/tmp.tar; $(RM) $$toplevel/images/tmp.tar;'; \
		tar cf images/tmp.tar '--xform=s%^%$(TAG)/%' documentation; \
		tar Af images/$(TAG).tar images/tmp.tar; \
	fi

images:
	mkdir -p $@
