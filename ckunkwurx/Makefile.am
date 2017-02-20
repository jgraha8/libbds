header-links: remove-links
	HEADERLIST="$(top_srcdir)/cku*.h $(top_srcdir)/*/cku*.h"; \
	for h in $$HEADERLIST; do \
	  BASENAME=`basename $$h`; \
	  test -r $$BASENAME || $(LN_S) $$h $$BASENAME; \
	done

remove-links: 
	rm -f cku*.h


all: all-am header-links

clean-local: remove-links
distclean-local: remove-links
