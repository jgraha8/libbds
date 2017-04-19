header-links: remove-links
	HEADERLIST="$(top_srcdir)/bds*.h $(top_srcdir)/*/bds*.h"; \
	for h in $$HEADERLIST; do \
	  BASENAME=`basename $$h`; \
	  test -r $$BASENAME || $(LN_S) $$h $$BASENAME; \
	done

remove-links: 
	rm -f bds*.h


all: all-am header-links

clean-local: remove-links
distclean-local: remove-links
