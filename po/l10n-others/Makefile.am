# The localized files handled here are not based on .po/.mo getext files.
# We generate one whole file per language, in a <lang> subdir.
# However, this Makefile implements the standard gettext rules, to be
# compatible with the main po/ Makefile. 

.NOTPARALLEL:

update-pot:
tx-push:

EXTRA_DIST = LINGUAS Makevars

.PHONY: update-pot update-po update-gmo
.PHONY: l10n-stats
.PHONY: tx-update tx-pull tx-push

#== Output statistics

l10n-stats:
	@echo "    Not yet implemented."

#== Clean obsolete strings

l10n-clean:
	@echo "    Not yet implemented."

#== Transifex synchronization - Shared functions definition

CURL     = curl
TX       = tx
TXORG    = artic-games
TXPROJ   = freedroid-rpg
TXPREFIX = other
TXPROPS  = '{"category":["other"]}'
TXCRED   = $(shell $(top_srcdir)/po/tx-credentials -t)
TXHOST   = $(shell $(top_srcdir)/po/tx-credentials -r)

LANGS = $(shell tail -n +2 $(srcdir)/LINGUAS)
comma:= ,
empty:=
space:= $(empty) $(empty)
TXLANGS = $(subst $(space),$(comma),$(LANGS))


#-- Update tx rsrc
# $(1) = rsrc name
# $(2) = rsrc file

define rsrc_update =
	if ! $(TX) status $(TXPROJ).$(1) >/dev/null 2>/dev/null; then \
	  echo "Creating $(1)"; \
	  $(TX) add --file-filter="tx-$(TXPREFIX)/$(1)-<lang>.txt" --type=TXT --organization=$(TXORG) --resource=$(1) $(2); \
	  if $(TX) push -s $(TXPROJ).$(1) >/dev/null 2>/dev/null; then \
	    curl_output=`$(CURL) --request PATCH \
	                        -s -o /dev/null -w "%{http_code}" \
	                        --url $(TXHOST)/resources/o%3A$(TXORG)%3Ap%3A$(TXPROJ)%3Ar%3A$(1) \
	                        --header 'Accept: application/vnd.api+json' \
	                        --header 'Authorization: Bearer $(TXCRED)' \
	                        --header 'Content-Type: application/vnd.api+json' \
	                        --data '{ "data": { "attributes": $(TXPROPS), \
	                                            "id": "o:$(TXORG):p:$(TXPREFIX):r:$(1)", \
	                                            "type": "resources" } \
	                                }'`; \
	    rtn=$$?; \
	    if test $$rtn -ne 0 -o " $$curl_output" != " 200"; then echo "Warning. Failed to set properties: error code $$curl_output"; fi; \
	  fi; \
	else \
	  echo "Updating $(1)"; \
	  $(TX) push -s $(TXPROJ).$(1); \
	fi
endef

#-- Pull a translated rsrc
#-- If the translated rsrc does not contain any translated string, we remove it
#-- Note: TX does not correctly handle last blank lines and the last newline.
#-- So we add the possibly missing newline with a 'sed' script and do not
#-- take blank lines into account to compare files.
# $(1) = rsrc name
# $(2) = lang

define rsrc_pull =
	echo "Pulling $(1)-<lang> from transifex"; \
	$(TX) pull -l $(TXLANGS) $(TXPROJ).$(1); \
	for lang in $(LANGS); do \
	  sed -i -e '$$a\' tx-$(TXPREFIX)/$(1)-$$lang.txt; \
	  if diff -q --ignore-blank-lines tx-$(TXPREFIX)/$(1).txt tx-$(TXPREFIX)/$(1)-$$lang.txt >/dev/null; then \
	    echo "    no translated strings: removing"; \
	    rm tx-$(TXPREFIX)/$(1)-$$lang.txt; \
	  fi; \
	done
endef

#-- Copy src file to dest file if src and dest are different
#-- (this avoid to change dest file timestamp if src and dest are identicals) 
# $(1) = src file
# $(2) = dest file

define copy_on_diff =
	if ! cmp -s $(1) $(2) ; then \
	  rm -f $(2) && mv $(1) $(2); \
	  echo "$(2) updated"; \
	else \
	  rm $(1); \
	fi
endef

#== l10n of fdrpg title files

TITLES_DIR   = $(top_srcdir)/data/base/titles $(top_srcdir)/data/storyline/act1/titles $(top_srcdir)/data/storyline/act2/titles

# Extract "<act_name>-" from $(1), given $(1) contains 'storyline/<act_name>'
act_name = $(shell dirname $(1) | grep -o -E "storyline/.*" | sed -E "s,storyline/([^/]*).*,\1-,")
# Extract the file basename, without extension, from $(1)
src_basename = $(basename $(notdir $(1)))
# Generate 'title-<act_name>-<file_basename>' from $(1), without extension
rsrc_full_name = title-$(call act_name,$(1))$(call src_basename,$(1))
# Insert $(2) before the filename in $(1) [dir/subdir/name.ext -> dir/subdir/$(2)/name.ext]
l10n_name = $(dir $(1))$(2)/$(notdir $(1))

TITLES_POTSRC = $(wildcard $(addsuffix /*.lua, $(TITLES_DIR)))
TITLES_RSRC = $(foreach potsrc,$(TITLES_POTSRC),$(call rsrc_full_name,$(potsrc)))
TITLES_TX = $(TITLES_RSRC:%=tx-$(TXPREFIX)/%.txt)
TITLES_L10N = $(foreach lang,$(LANGS),$(foreach potsrc,$(TITLES_POTSRC),$(call l10n_name,$(potsrc),$(lang))))

#-- Create a TX rsrc by extracting the text part of a title
# $(1) = source file name

define update-po-title =
 tx-$(TXPREFIX)/$(call rsrc_full_name,$(1)).txt: $(1)
	@$(mkdir_p) tx-$(TXPREFIX); \
	 $(AWK) "BEGIN { isintext=0; } /^text[[:space:]]*=[[:space:]]*[\[]+/ { isintext=1; next; } /^[\]]+/ { exit; } isintext == 1 { print; }" $$< > $$@
endef

$(foreach potsrc,$(TITLES_POTSRC),$(eval $(call update-po-title,$(potsrc))))

#-- Merge a l10n version of a title's text with the title's header,
#-- and copy the result to data/storyline/act1/titles
#-- Note: $(wildcard) if used so that an empty prerequisite list is generated if
#-- the translation file does not exist
# $(1) = pot name
# $(2) = lang

define update-gmo-title =
 $(call l10n_name,$(1),$(2)): $(wildcard tx-$(TXPREFIX)/$(call rsrc_full_name,$(1))-$(2).txt)
	@if test -r tx-$(TXPREFIX)/$(call rsrc_full_name,$(1))-$(2).txt ; then \
	   $(mkdir_p) $(dir $(1))$(2); \
	   $(AWK) "/^text[[:space:]]*=[[:space:]]*[\[]+/ { print; nextfile; } { print; } END { print \"]]\"; print \"}\"; }" \
	        $(1) $$< > $$<.tmp; \
	   $(call copy_on_diff,$$<.tmp,$$@); \
	 fi
endef

$(foreach lang,$(LANGS),\
    $(foreach potsrc,$(TITLES_POTSRC),\
        $(eval $(call update-gmo-title,$(potsrc),$(lang)))))

#-- update-po creates the new tx/title-<title name>.txt files

update-po:: $(TITLES_TX)

#-- Push all title rsrcs to TX

tx-update:: $(TITLES_TX)
	@if [ " $(TXCRED)" = " " ] ; then \
		echo "You do not have a Transifex account"; \
		exit 1; \
	 fi
	@for rsrc in $(TITLES_RSRC); do \
		$(call rsrc_update,$$rsrc,tx-$(TXPREFIX)/$$rsrc.txt); \
	 done

#-- Pull all l10n versions of the titles' text from TX

tx-pull:: $(TITLES_TX)
	@if [ " $(TXCRED)" = " " ] ; then \
		echo "You do not have a Transifex account"; \
		exit 1; \
	 fi
	@for rsrc in $(TITLES_RSRC); do \
	   $(call rsrc_pull,$$rsrc); \
	 done

#-- update-gmo creates the new l10n version of the titles

update-gmo:: $(TITLES_L10N)

#== l10n of the main README

README_DIR  = $(top_srcdir)
README_RSRC = README
README_TX   = tx-$(TXPREFIX)/README.txt
README_L10N = $(foreach lang,$(LANGS),$(README_RSRC:%=$(README_DIR)/README-$(lang)))

#-- Copy the README into the tx/ subdir

tx-$(TXPREFIX)/README.txt: $(README_DIR)/README
	@$(mkdir_p) tx-$(TXPREFIX); \
	 cp $< $@


#-- Put localized versions in place
# $(1) = lang

define update-gmo-readme =
 $(README_DIR)/README-$(1): $(wildcard tx-$(TXPREFIX)/README-$(1).txt)
	@if test -r tx-$(TXPREFIX)/README-$(1).txt ; then \
	   cp $$< $$<.tmp; \
	   $(call copy_on_diff,$$<.tmp,$$@); \
	 fi
endef

$(foreach lang,$(LANGS),\
    $(eval $(call update-gmo-readme,$(lang))))

#-- update-po creates the new tx/README.txt files

update-po:: $(README_TX)

#-- Push the README rsrc to TX

tx-update:: $(README_TX)
	@if [ " $(TXCRED)" = " " ] ; then \
		echo "You do not have a Transifex account"; \
		exit 1; \
	 fi
	@$(call rsrc_update,README,tx-$(TXPREFIX)/README.txt)

#-- Pull all l10n versions of the README from TX

tx-pull:: $(README_TX)
	@if [ " $(TXCRED)" = " " ] ; then \
		echo "You do not have a Transifex account"; \
		exit 1; \
	 fi
	@$(call rsrc_pull,README)

#-- update-gmo creates the new l10n version of the README

update-gmo:: $(README_L10N)

all: update-gmo
