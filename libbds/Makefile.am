header-links: remove-links
	if test -n "$(BDS_INCLUDE_PREFIX)"; then \
		$(MKDIR_P) "../$(BDS_INCLUDE_PREFIX)"; \
		$(LN_S) $(PWD) "../$(BDS_INCLUDE_PREFIX)/libbds"; \
	fi
	HEADERLIST="$(top_srcdir)/bds*.h $(top_srcdir)/*/bds*.h"; \
	for h in $$HEADERLIST; do \
	  BASENAME=`basename $$h`; \
	  test -r $$BASENAME || $(LN_S) $$h $$BASENAME; \
	done

remove-links:
	if test -n "$(BDS_INCLUDE_PREFIX)"; then \
		rm -rf "../$(BDS_INCLUDE_PREFIX)"; \
	fi
	rm -f bds*.h


all: all-am header-links

clean-local: remove-links
distclean-local: remove-links
