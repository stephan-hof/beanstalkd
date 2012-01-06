default:
	@echo "Explicit target required"

distclean:
	-(cd jemalloc && [ -f Makefile ] && $(MAKE) distclean) > /dev/null || true

jemalloc:
	cd jemalloc && ./configure --with-jemalloc-prefix=je_ --enable-cc-silence && $(MAKE) lib/libjemalloc.a

.PHONY: default distclean jemalloc
