/*
 *
 *   Copyright (c) 2016 Samuel Degrande
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

#include "savegame.h"

// In 0.16.1, melee shots, bullets, blasts and spells were stored in statically
// allocated arrays, and a specific object's type (INFOUT) was used to define
// empty slots in that array.
// Now objects are stored in a sparse dynarray, and the INFOUT object's type
// is no more used.
// This filter removes 'INFOUT objects' from the savegame.

static int convert_to_sparse_array(struct savegame_data *savegame, struct auto_string *report, const char* section, const char* type, const char* infout, int nb_lines)
{
	char *array_start = strstr(savegame->sav_buffer, section);
	if (!array_start) {
		// An empty list in that savegame ? Strange, but let's be silent
		// about that...
		return FILTER_NOT_APPLIED;
	}

	// Loop over the MAXOBJECTS (=100) object definitions to find INFOUT's ones

	char *object_start = array_start + strlen(section) + 1;

	int object_index;
	for (object_index = 0; object_index < 100; object_index++) {

		// Find the end of the object's definition by passing over
		// nb_lines lines in the savegame stream

		char *object_end = object_start;

		int i;
		for (i = 0; i < nb_lines; i++) {
			while (*object_end != '\n' && *object_end != '\0') object_end++;
			if (*object_end == '\0') {
				// Reach the end of savegame before to find the end of the
				// object definition ?
				autostr_append(report,
				               _("Error during savegame filtering (%s:%s): End of savegame reached while "
				                 "reading a object's definition..\n"
				                 "The savegame seems to be corrupted."),
				               savegame->running_converter->id, __FUNCTION__);
				return FILTER_ABORT;
			}
			object_end++;
		}

		// "},\n" expected on the nth line

		if (strncmp(object_end, "},\n", 3)) {
			autostr_append(report,
			               _("Error during savegame filtering (%s:%s): End marker of a object's definition "
			                 "not found where it was expected..\n"
			                 "The savegame seems to be corrupted."),
			               savegame->running_converter->id, __FUNCTION__);
			return FILTER_ABORT;
		}

		// Now check the object's type

		char *type_start = strstr(array_start, type);
		if (type_start) {
			type_start += strlen(type);
			if (!strncmp(type_start, infout, strlen(infout))) {
				// INFOUT object - Remove it by replacing its definition by a series of spaces
				memset(object_start, ' ', (object_end + 2) - object_start + 1);
			}
		}

		// Points to the beginning of the next object

		object_start = object_end + 3;
	}

	return FILTER_APPLIED;
}

static int rename_array(struct savegame_data *savegame, struct auto_string *report, const char* old, const char* new)
{
	char *array_start = strstr(savegame->sav_buffer, old);
	if (!array_start) {
		// the array wasn't found
		return FILTER_ABORT;
	}

	int length_old = strlen(old);
	int length_new = strlen(new);

	if (length_new <= length_old) {
		// Avoid a huge mem copy, by filling with blanks
		int i;
		strncpy(array_start, new, length_new+1);
		for (i = length_new; i < length_old; ++i) {
			array_start[i] = ' ';
		}
	} else {
		// Some chars have to be inserted, so we need to create a new buffer
		char *new_savegame = (char*)MyMalloc(sizeof(char)*(savegame->sav_buffer_size + (length_new - length_old) + 1));

		// Copy the beginning of the savegame buffer, add the new name, and add the rest of the savegame
		ptrdiff_t offset = array_start - savegame->sav_buffer;

		memcpy(new_savegame, savegame->sav_buffer, offset);
		memcpy(&new_savegame[offset], new, strlen(new));
		memcpy(&new_savegame[offset+strlen(new)], &savegame->sav_buffer[offset+strlen(old)], savegame->sav_buffer_size - (offset+strlen(old)));
		new_savegame[savegame->sav_buffer_size + (length_new - length_old)] = '\0';

		// Replace the old buffer by the new one
		free(savegame->sav_buffer);
		savegame->sav_buffer = new_savegame;
		savegame->sav_buffer_size = savegame->sav_buffer_size + (length_new - length_old);
	}

	return FILTER_APPLIED;
}

int filter_0_16_1_convert_bullets_array(struct savegame_data *savegame, struct auto_string *report)
{
	return convert_to_sparse_array(savegame, report, "bullet_array{", "type = ", "-30,", 26);
}

int filter_0_16_1_convert_melee_shots_array(struct savegame_data *savegame, struct auto_string *report)
{
	return convert_to_sparse_array(savegame, report, "melee_shot_array{", "attack_target_type = ", "103,", 8);
}

int filter_0_16_1_convert_blasts_array(struct savegame_data *savegame, struct auto_string *report)
{
	return convert_to_sparse_array(savegame, report, "blast_array{", "type = ", "-30,", 10);
}

int filter_0_16_1_convert_spellactives_array(struct savegame_data *savegame, struct auto_string *report)
{
	return convert_to_sparse_array(savegame, report, "spell_active_array{", "img_type = ", "-1,", 34);
}

int filter_0_16_1_rename_spellactives_array(struct savegame_data *savegame, struct auto_string *report)
{
	return rename_array(savegame, report, "spell_active_array{", "spell_array{");
}

// Since 1601:5, a 'game_config' Lua table is stored at the beginning of the savegame.
// It contains the currently played game act (game acts were not available before).
// This filter adds the missing table, using the starting game act.

int filter_0_16_1_add_game_config(struct savegame_data *savegame, struct auto_string *report)
{
	char *ptr = strstr(savegame->sav_buffer, "--]]\n\n"); // skip the header
	ptr += strlen("--]]\n\n");

	if (!strncmp(ptr, "game_config{", 12)) {
		// The table is already there. Nothing to do.
		return FILTER_NOT_NEEDED;
	}

	// Prepare the text to be added
	struct auto_string *game_config_string = alloc_autostr(256);
	autostr_printf(game_config_string, "game_config{\nplayed_game_act = [=[%s]=],\n}\n", game_act_get_starting()->name);

	// Allocate a new savegame buffer
	char *new_savegame = (char*)MyMalloc(sizeof(char)*(savegame->sav_buffer_size + game_config_string->length + 1));

	// Copy the header + the new data + the rest of the savegame
	size_t header_len = ptr - savegame->sav_buffer;
	memcpy(new_savegame, savegame->sav_buffer, header_len);
	memcpy(new_savegame+header_len, game_config_string->value, game_config_string->length);
	memcpy(new_savegame+header_len+game_config_string->length, savegame->sav_buffer+header_len, savegame->sav_buffer_size-header_len);

	// Replace the old buffer by the new one
	free(savegame->sav_buffer);
	savegame->sav_buffer = new_savegame;
	savegame->sav_buffer_size = savegame->sav_buffer_size + game_config_string->length;
	free_autostr(game_config_string);

	return FILTER_APPLIED;
}

// In 1601:5, the name of the played game act is stored in the savegame.
// But a name can possibly be changed. So an identifier was introduced, for
// future proofing.
// This filter replaces the game act's name by its identifier.

int filter_0_16_1_use_game_act_id(struct savegame_data *savegame, struct auto_string *report)
{
	char *ptr = strstr(savegame->sav_buffer, "--]]\n\n"); // skip the header
	ptr += strlen("--]]\n\n");

	// Only 'Act 1' is known for 1601:5 savegames

	const char *searched_config = "game_config{\nplayed_game_act = [=[Act 1]=],";
	const char *new_config      = "game_config{\nplayed_game_act = [=[act1]=], ";

	if (strncmp(ptr, searched_config, strlen(searched_config))) {
		autostr_append(report,
		               _("Error during savegame filtering (%s:%s): game_config "
		                 "table was not found where or as it was expected.\n"
		                 "The savegame seems to be corrupted."),
		               savegame->running_converter->id, __FUNCTION__);
		return FILTER_ABORT;
	}

	// Replace the old config by the new one

	memcpy(ptr, new_config, strlen(new_config));

	return FILTER_APPLIED;
}

// The 'AllMissions' attribute of Tux as been changed to 'missions'

int filter_0_16_1_change_allmissions(struct savegame_data *savegame, struct auto_string *report)
{
	char *ptr = strstr(savegame->sav_buffer, "AllMissions = {");
	if (!ptr) {
		// the attribute wasn't found
		autostr_append(report,
		               _("Error during savegame filtering (%s:%s): Tux "
		                 "'AllMissions' attribute was not found.\n"
		                 "The savegame seems to be corrupted."),
		               savegame->running_converter->id, __FUNCTION__);
		return FILTER_ABORT;
	}

	memcpy(ptr, "missions    = {", strlen("AllMissions"));

	return FILTER_APPLIED;
}

// Convert item's type from index in ItemMap to itemspec's id

int filter_0_16_1_convert_item_type(struct savegame_data *savegame, struct auto_string *report)
{
	// The item type to change are in the section starting with 'Inventory = {'
	// and ending with 'HaveBeenToLevel = {'

	char *start_section = strstr(savegame->sav_buffer, "Inventory = {");
	if (!start_section) {
		// the attribute wasn't found
		autostr_append(report,
		               _("Error during savegame filtering (%s:%s): Tux "
		                 "'Inventory' attribute was not found.\n"
		                 "The savegame seems to be corrupted."),
		               savegame->running_converter->id, __FUNCTION__);
		return FILTER_ABORT;
	}

	char *end_section = strstr(savegame->sav_buffer, "HaveBeenToLevel = {");
	if (!end_section) {
		// the attribute wasn't found
		autostr_append(report,
		               _("Error during savegame filtering (%s:%s): Tux "
		                 "'HaveBeenToLevel' attribute was not found.\n"
		                 "The savegame seems to be corrupted."),
		               savegame->running_converter->id, __FUNCTION__);
		return FILTER_ABORT;
	}

	// Allocate a new savegame buffer.
	// 'MAX_ITEMS_IN_INVENTORY + 5' item types are to be rewritten.
	// We suppose a max string id of 256 chars (far enough for the items existing
	// at this point of git history)

	char *new_savegame = (char*)MyMalloc(sizeof(char)*(savegame->sav_buffer_size + (MAX_ITEMS_IN_INVENTORY + 5)*256));
	char *copy_from = savegame->sav_buffer;
	char *copy_to = new_savegame;

	// Loop on each ',\ntype ='

	char *ptr = strstr(start_section, ",\ntype =");

	while (ptr && (ptr < end_section)) {
		// Copy up to the current ptr
		ptrdiff_t copy_len = ptr - copy_from;
		memcpy(copy_to, copy_from, copy_len);
		copy_to += copy_len;

		// Extract the item type index
		ptr += strlen(",\ntype = ");
		char *endptr;
		int item_type = (int)strtol(ptr, &endptr, 0);

		if (endptr == ptr) {
			autostr_append(report,
			               _("Error during savegame filtering (%s:%s): Malformed "
			                 "item type string in Tux struct.\n"
			                 "The savegame seems to be corrupted."),
			               savegame->running_converter->id, __FUNCTION__);
			free(new_savegame);
			return FILTER_ABORT;
		}

		// Convert to an itemspec id and write it
		struct auto_string *item_type_string = alloc_autostr(256);
		if (item_type == -1) {
			autostr_printf(item_type_string, ",\ntype = \"none\"");
		} else {
			autostr_printf(item_type_string, ",\ntype = [=[%s]=]", ItemMap[item_type].id);
		}
		memcpy(copy_to, item_type_string->value, item_type_string->length);
		copy_to += item_type_string->length;
		free_autostr(item_type_string);

		// Prepare for next round
		copy_from = endptr;
		ptr = strstr(copy_from, ",\ntype =");
	}

	// Copy remaining part of the input buffer
	ptrdiff_t copy_len = (savegame->sav_buffer + savegame->sav_buffer_size) - copy_from;
	memcpy(copy_to, copy_from, copy_len);

	// Replace the old buffer by the new one
	free(savegame->sav_buffer);
	savegame->sav_buffer = new_savegame;
	savegame->sav_buffer_size = strlen(savegame->sav_buffer);

	return FILTER_APPLIED;
}
