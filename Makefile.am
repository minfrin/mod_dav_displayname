EXTRA_DIST = mod_dav_displayname.c mod_dav_displayname.spec

all-local:
	$(APXS) "-Wc,${CFLAGS}" -c -c $(DEF_LDLIBS) -Wc,"$(CFLAGS)" -Wc,"$(AM_CFLAGS)" -Wl,"$(LDFLAGS)" -Wl,"$(AM_LDFLAGS)" $(LIBS) @srcdir@/mod_dav_displayname.c

install-exec-local: 
	if test -z "$${LIBEXECDIR}"; then LIBEXECDIR=`$(APXS) -q LIBEXECDIR`; fi;\
	\
	mkdir -p $(DESTDIR)$${LIBEXECDIR}; \
	\
	$(APXS) "-Wc,${CFLAGS}" -S LIBEXECDIR=$(DESTDIR)$${LIBEXECDIR} -c -i -c $(DEF_LDLIBS) -Wc,"$(CFLAGS)" -Wc,"$(AM_CFLAGS)" -Wl,"$(LDFLAGS)" -Wl,"$(AM_LDFLAGS)" $(LIBS) @srcdir@/mod_dav_displayname.c

