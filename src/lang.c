/*
 *
 *   Copyright (c) 2014 Samuel Degrande
 *
 *
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 */

#define _lang_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/**
 * Set the locale used by the game for text translation.
 *
 * On success, the locale is set into the game config.
 * Note: GameConfig.locale is set to "" to denote the use of the system
 * default locale.
 *
 * \param locale Pointer to the locale name, or "" to use system default.
 */

void lang_set(const char *locale)
{
#ifdef ENABLE_NLS
	// When called with an actual locale name, setlocale() checks if the
	// locale is configured on the system.
	// Unfortunately, some distros such as Debian and its children, or Arch,
	// only have a few locales pre-configured.
	//
	// So in order to be able to use all our translations, we rather rely
	// on the LANGUAGE envvar and ask gettext to use the system default
	// locale. (gettext() gives precedence to LANGUAGE over any other envvar,
	// see GNU gettext manual)

	if (!locale || strlen(locale) == 0) {
		if (fd_unsetenv("LANGUAGE") != 0)
			return;
	} else {
		if (fd_setenv("LANGUAGE", locale, TRUE) != 0)
			return;
	}

	// Use LANGUAGE or system default locale
	if (!setlocale(LC_MESSAGES, "")) {
		error_message(__FUNCTION__, "Error when calling setlocale() to set %s locale", PLEASE_INFORM, (locale) ? locale : "NULL");
		return;
	}

	// 'locale' and GameConfig.locale can possibly be the same pointer.
	// So we make a copy of 'locale' to avoid any issue.
	char *ll = (locale) ? strdup(locale) : NULL;
	if (GameConfig.locale)
		free(GameConfig.locale);
	if (ll)
		GameConfig.locale = strdup(ll);
	else
		GameConfig.locale = strdup("");
	if (ll)
		free(ll);
#endif
}

/**
 * Return the locale used by the game for text translation.
 *
 * \return Pointer to the locale name, or NULL if localization is disabled.
 */
char *lang_get()
{
#ifdef ENABLE_NLS
	// If GameConfig.locale is not set, then return the system default locale

	if (GameConfig.locale && strlen(GameConfig.locale) != 0) {
		return GameConfig.locale;
	} else {
		return setlocale(LC_MESSAGES, "");
	}
#else
	return NULL;
#endif
}

/**
 * Initialize localization.
 *
 * The game is started in the system default locale.
 */

void lang_init()
{
#ifdef ENABLE_NLS
	// Only use localization on messages, to avoid issues when
	// reading/writing numerical data.

	setlocale(LC_ALL, "C");
	setlocale(LC_MESSAGES, "");

	const char *localedir = data_dirs[LOCALE_DIR].path;
	if (strlen(localedir) == 0) {
		error_message(__FUNCTION__, "Locale dir not found. Disabling localization.",
		             NO_REPORT);
		setlocale(LC_MESSAGES, "C");
	} else {
		// i18n text domain declarations.
		// Note: our bitmap fonts are not utf-8 compliant, but contain part of the
		// latin-1 alphabet. So we enforce a conversion to iso-8859-1.

		bindtextdomain("freedroidrpg", localedir);
		bind_textdomain_codeset("freedroidrpg", "ISO-8859-1");
		bindtextdomain("freedroidrpg-dialogs", localedir);
		bind_textdomain_codeset("freedroidrpg-dialogs", "ISO-8859-1");
		bindtextdomain("freedroidrpg-data", localedir);
		bind_textdomain_codeset("freedroidrpg-data", "ISO-8859-1");

		// Default domain to use, if none is specified
		textdomain("freedroidrpg");
	}
#endif
}

#undef _lang_c