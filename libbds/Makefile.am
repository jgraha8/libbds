header-links: remove-links
	test -n $(BDS_INCLUDE_PREFIX) && $(MKDIR_P) ../$(BDS_INCLUDE_PREFIX)
	test -n $(BDS_INCLUDE_PREFIX) && $(LN_S) $(PWD) ../$(BDS_INCLUDE_PREFIX)/libbds
	HEADERLIST="$(top_srcdir)/bds*.h $(top_srcdir)/*/bds*.h"; \
	for h in $$HEADERLIST; do \
	  BASENAME=`basename $$h`; \
	  test -r $$BASENAME || $(LN_S) $$h $$BASENAME; \
	done

remove-links:
	test -n $(BDS_INCLUDE_PREFIX) && rm -rf ../$(BDS_INCLUDE_PREFIX)
	rm -f bds*.h


all: all-am header-links

clean-local: remove-links
distclean-local: remove-links
