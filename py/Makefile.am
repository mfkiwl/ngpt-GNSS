EXTRA_DIST = antex_view/__init__.py \
		antex_view/antenna.py \
		antex_view/parse_antenna_stream.py \
		MANIFEST.in \
		setup.py

all-local:
	(cd $(srcdir); $(PYTHON) setup.py build \
	--build-base $(shell readlink -f $(builddir))/build \
	--verbose)

install-exec-local:
	$(PYTHON) $(srcdir)/setup.py install \
	--prefix $(DESTDIR)$(prefix) \
	--single-version-externally-managed \
	--record $(DESTDIR)$(pkgpythondir)/install_files.txt \
	--verbose

uninstall-local:
	cat $(DESTDIR)$(pkgpythondir)/install_files.txt | xargs rm -rf
	rm -rf $(DESTDIR)$(pkgpythondir)
