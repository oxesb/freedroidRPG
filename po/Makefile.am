SUBDIRS = po-src po-dialogs po-data l10n-others
EXTRA_DIST = xgettext-wrapper

# disables -j , hack to fix some race condition
.NOTPARALLEL:

# Special constructs to enable recursive call of
# update-po and update-gmo :
#   1/ A prefix is added to each dir in SUBDIRS (see L10NSTATSDIRS, for
#      instance)
#   2/ This new list is used as a target, to call a rule in loop on each
#      prefixed dir
#   3/ In this rule, the prefix is removed to extract the subdir name
#      (see the rule of the $(L10NSTATSDIRS) target)
#
# Should be replaced by AM_EXTRA_RECURSIVE_TARGETS in
# top configure.ac, but this needs automake 1.13
# Inspired by http://lackof.org/taggart/hacking/make-example/

#
# Output the stats for each language
#
L10NSTATSDIRS = $(SUBDIRS:%=l10nstats-%)

l10n-stats: l10n-stats-@USE_NLS@
l10n-stats-no:
	@echo "NLS is disabled. Nothing to do."
l10n-stats-yes: $(L10NSTATSDIRS)

$(L10NSTATSDIRS):
	echo "$(@:l10nstats-%=%):"
	$(MAKE) -C $(@:l10nstats-%=%) $(AM_MAKEFLAGS) l10n-stats

.PHONY: l10n-stats-no l10n-stats-yes $(L10NSTATSDIRS)

#
# Output the translators list for each language
#
L10NTRSLDIRS = $(SUBDIRS:%=l10ntrsl-%)

l10n-translators: l10n-translators-@USE_NLS@
l10n-translators-no:
	@echo "NLS is disabled. Nothing to do."
l10n-translators-yes: $(L10NTRSLDIRS)

$(L10NTRSLDIRS):
	echo "$(@:l10ntrsl-%=%):"
	$(MAKE) -C $(@:l10ntrsl-%=%) $(AM_MAKEFLAGS) l10n-translators

.PHONY: l10n-translators-no l10n-translators-yes $(L10NTRSLDIRS)

#
# Clean PO files (remove obsolete strings and clear fuzzy flags)
#
L10NCLEANDIRS = $(SUBDIRS:%=l10nclean-%)

l10n-clean: l10n-clean-@USE_NLS@
l10n-clean-no:
	@echo "NLS is disabled. Nothing to do."
l10n-clean-yes: $(L10NCLEANDIRS)

$(L10NCLEANDIRS):
	echo "$(@:l10nclean-%=%):"
	$(MAKE) -C $(@:l10nclean-%=%) $(AM_MAKEFLAGS) l10n-clean

.PHONY: l10n-clean-no l10n-clean-yes $(L10NCLEANDIRS)

#
# Update the .pot files and their translations (.po files)
# to include the lastest changes made to the sources.
#
UPDATEPODIRS = $(SUBDIRS:%=updatepo-%)

update-po: update-po-@USE_NLS@
update-po-no:
	@echo "NLS is disabled. Nothing to do."
update-po-yes: $(UPDATEPODIRS)

$(UPDATEPODIRS):
	$(MAKE) -C $(@:updatepo-%=%) $(AM_MAKEFLAGS) update-po

.PHONY: update-po-no update-po-yes $(UPDATEPODIRS)

#
# Compile the translations (.po files) into catalogs (.gmo files).
# The update-gmo target in the subdirs also copy the translations
# locally (in $top_srcdir/locale) to be used by the game without
# installing it (see Rules-others in subdirs).
#
UPDATEGMODIRS = $(SUBDIRS:%=updategmo-%)

update-gmo: update-gmo-@USE_NLS@
update-gmo-no:
	@echo "NLS is disabled. Nothing to do."
update-gmo-yes: $(UPDATEGMODIRS)

$(UPDATEGMODIRS):
	$(MAKE) -C $(@:updategmo-%=%) $(AM_MAKEFLAGS) update-gmo

.PHONY: update-gmo-no update-gmo-yes $(UPDATEGMODIRS)

# TX synchronization rules
# The same rule is used for 3 targets (TXTARGETS):
# - tx-update: it updates the transifex individual resources, with the
#   latest changes of the sources (using the current .pot files)
#   Note: update-po should be called before tx-update
# - tx-pull: gets the translated individual resources, and combine them
#   into .po files
#   Note: tx-update should be called before tx-pull
# - tx-pull: overload transifex translations with the content of our
#   translations
#   Note: TO BE USED WITH EXTREME CARE. The translations not already pulled
#         from transifex will be lost ! Given the risk of this command,
#         it has to be granted by the presence of a 'tx-push-granted' file in
#         the subdir.
#
# Note that we call subdirs in sequence, to avoid to flood the Transifex server
# too much

TXTARGETS = tx-update tx-pull tx-push

TX        = tx
TXORG     = artic-games
TXPROJ    = freedroid-rpg
TXPROPS   = '{"category":["autoTM"]}'
TXCRED    = $(shell $(top_srcdir)/po/tx-credentials -t)
TXHOST    = $(shell $(top_srcdir)/po/tx-credentials -r)

# Create a list of potfiles:
#  foreach dir in $SUBDIRS - $(foreach ...)
#    - extract from Makevars the value of the DOMAIN parameter - $(shell ...)
#    - strip leading/trailing whitespaces - $(strip ...)
#    - add the dirname as a prefix - $(addprefix ...)
#    - add a .pot suffix - $(addsuffix ...)
TXPOTS  = $(foreach dir, $(SUBDIRS), $(addsuffix .pot, $(addprefix $(dir)/, $(strip $(shell $(AWK) -F'=' -e '/^DOMAIN/{ print $$2; }' $(dir)/Makevars)))))

$(TXTARGETS):
	@test " $(TXCRED)" = " " \
	 && { \
	      echo "You do not have a Transifex account"; \
	      exit 1; \
	    }; \
	 for dir in $(SUBDIRS) ; do \
	     $(MAKE) -C $$dir $@; \
	 done; \
	 case $@ in \
	   tx-update) \
	     $(mkdir_p) tx; \
	     msgcat --width=80 --no-wrap --sort-by-file --use-first -o tx/all_in_one.pot $(TXPOTS); \
	     if test -r tx/all_in_one.pot; then \
	       if ! $(TX) status $(TXPROJ).all_in_one >/dev/null 2>/dev/null; then \
	         echo "Creating all_in_one"; \
	         $(TX) add --file-filter="tx/all_in_one-<lang>.po" --type=PO --organization=$(TXORG) --project=$(TXPROJ) --resource="all_in_one" tx/all_in_one.pot; \
	         if $(TX) push -s $(TXPROJ).all_in_one; then \
	           curl_output=`curl --request PATCH \
	                        -s -o /dev/null -w "%{http_code}" \
	                        --url $(TXHOST)/resources/o%3A$(TXORG)%3Ap%3A$(TXPROJ)%3Ar%3Aall_in_one \
	                        --header 'Accept: application/vnd.api+json' \
	                        --header 'Authorization: Bearer $(TXCRED)' \
	                        --header 'Content-Type: application/vnd.api+json' \
	                        --data '{ "data": { "attributes": $(TXPROPS), \
	                                            "id": "o:$(TXORG):p:$(TXPREFIX):r:all_in_one", \
	                                            "type": "resources" } \
	                                }'`; \	           
	           rtn=$$?; \
	           if test $$rtn -ne 0 -o " $$curl_output" != " 200"; then echo "Warning. Failed to set properties: error code $$culr_output"; fi; \
	         fi; \
	       else \
	         echo "Updating all_in_one"; \
	         $(TX) push -s $(TXPROJ).all_in_one; \
	       fi; \
	     fi; \
	     rm -rf tx; \
	     ;; \
	 esac;

.PHONY: $(TXTARGETS)

clean-local:
	-rm -f ./po-data/*.gmo
	-rm -f ./po-dialogs/*.gmo
	-rm -f ./po-src/*.gmo

distclean-local:
	-rm -f ./*/stamp-li
	-rm -f ./*/Makevars
