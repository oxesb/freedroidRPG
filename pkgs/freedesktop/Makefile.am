## Process this file with automake to produce Makefile.in

# For Freedesktop compliance

appstreamdir = $(datadir)/metainfo
appstream_in_files = org.freedroid.freedroidRPG.appdata.xml
dist_appstream_DATA = $(appstream_in_files)

desktopdir = $(datadir)/applications
desktop_in_files = org.freedroid.freedroidRPG.desktop
dist_desktop_DATA = $(desktop_in_files)

EXTRA_DIST = icons

install-data-local:
	@$(mkinstalldirs) $(DESTDIR)$(datadir)
	cd $(srcdir); \
	find icons -type d -exec echo $(DESTDIR)$(datadir)/{} \; | xargs $(mkinstalldirs) ; \
	find icons -type f -exec $(INSTALL_DATA) {} $(DESTDIR)$(datadir)/{} \;

uninstall-local:
	@find $(DESTDIR)$(datadir)/ -name freedroidrpg.png -exec rm -f {} \;
	@find $(DESTDIR)$(datadir)/ -name freedroidrpg.svg -exec rm -f {} \;
