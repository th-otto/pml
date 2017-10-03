SUBDIRS = pmlsrc

#disabled for now because of missing complex functions
#SUBDIRS += pmltests

all clean distclean install uninstall:
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || exit 1; done
