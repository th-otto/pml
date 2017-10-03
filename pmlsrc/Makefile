srcdir = .
top_srcdir = ..
subdir = pmlsrc

include $(top_srcdir)/CONFIGVARS
include $(top_srcdir)/RULES

ALL_LIBS = normal
ifeq ($(WITH_SHORT_LIBS),yes)
ALL_LIBS += mshort
endif

ifeq ($(WITH_020_LIB),yes)
ALL_LIBS += m68020
ifeq ($(WITH_SHORT_LIBS),yes)
ALL_LIBS += m68020mshort
endif
endif

ifeq ($(WITH_V4E_LIB),yes)
ALL_LIBS += m5475
ifeq ($(WITH_SHORT_LIBS),yes)
ALL_LIBS += m5475mshort
endif
endif

all : $(ALL_LIBS)

normal:
	$(MAKE) libpml.a TARGET=""

mshort:
	$(MAKE) libpml16.a TARGET="16"

m68020:
	$(MAKE) libpml020.a TARGET="020"

m68020mshort:
	$(MAKE) libpml02016.a TARGET="02016"

m5475:
	$(MAKE) libpmlv4e.a TARGET="v4e"

m5475mshort:
	$(MAKE) libpmlv4e16.a TARGET="v4e16"

clean::
	rm -rf .lib* objs *.a

distclean:: clean

libpml$(TARGET).a: objs
	$(AM_V_at)$(RM) $@
	$(AM_V_AR)$(AR) rc $@ \
		$(shell for file in `cat .lib$(TARGET)/objs` ; \
			do echo .lib$(TARGET)/$$file ; done)
	$(AM_V_RANLIB)$(RANLIB) $@

objs: .lib$(TARGET)
	cd .lib$(TARGET); $(MAKE)
.PHONY: objs

.lib$(TARGET): Makefile.objs
	$(AM_V_at)$(MKDIR) -p $@
	$(AM_V_at)$(CP) Makefile.objs $@/Makefile

install:
	install -m 755 -d $(PREFIX)/lib
	install -m 644 libpml.a $(PREFIX)/lib/libm.a
ifeq ($(WITH_SHORT_LIBS),yes)
	install -m 755 -d $(PREFIX)/lib/mshort
	install -m 644 libpml16.a $(PREFIX)/lib/mshort/libm.a
endif
ifeq ($(WITH_020_LIB),yes)
	install -m 755 -d $(PREFIX)/lib/m68020-60
	install -m 644 libpml020.a $(PREFIX)/lib/m68020-60/libm.a
ifeq ($(WITH_SHORT_LIBS),yes)
	install -m 755 -d $(PREFIX)/lib/m68020-60/mshort
	install -m 644 libpml02016.a $(PREFIX)/lib/m68020-60/mshort/libm.a
endif
endif
ifeq ($(WITH_V4E_LIB),yes)
	install -m 755 -d $(PREFIX)/lib/m5475
	install -m 644 libpmlv4e.a $(PREFIX)/lib/m5475/libm.a
ifeq ($(WITH_SHORT_LIBS),yes)
	install -m 755 -d $(PREFIX)/lib/m5475/mshort
	install -m 644 libpmlv4e16.a $(PREFIX)/lib/m5475/mshort/libm.a
endif
endif
	install -m 755 -d $(PREFIX)/include
	install -m 644 math.h $(PREFIX)/include

uinstall:
	for file in libm.a mshort/libm.a m68020-60/libm.a m68020-60/mshort/libm.a m5475/libm.a m5475/mshort/libm.a; do \
		$(RM) $(PREFIX)/lib/$$file; \
	done
	for file in math.h; do \
		$(RM) $(PREFIX)/include/$$file; \
	done
