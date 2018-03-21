/*
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *   Copyright (c) 2004-2010 Arthur Huillet
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

/**
 * This file contains (all?) map-related functions, which also includes 
 * loading of decks and whole ships, starting the lifts and consoles if 
 * close to the paradroid, refreshes as well as determining the map brick 
 * that contains specified coordinates are done in this file.
 */

#define _map_c 1

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"

#include "lvledit/lvledit_actions.h"
#include "lvledit/lvledit_display.h"
#include "map.h"

#define 	TELEPORT_PAIR_STRING	"teleport pair:"

void GetThisLevelsDroids(char *section_pointer);

/*
 * Initialize a map tile with default values.
 */
void init_map_tile(struct map_tile* tile)
{
	int i;
	tile->floor_values[0] = ISO_FLOOR_SAND;
	for (i = 1; i < MAX_FLOOR_LAYERS; i++)
		tile->floor_values[i] = ISO_FLOOR_EMPTY;
	dynarray_init(&tile->glued_obstacles, 0, sizeof(int));
	tile->volatile_obstacles = (list_head_t*)MyMalloc(sizeof(list_head_t));
	INIT_LIST_HEAD(tile->volatile_obstacles);
	tile->timestamp = 0;
}

/**
 * This function will make all blood obstacles vanish, all dead bots come
 * back to life, and get all bots return to a wandering state.
 */
void respawn_level(int level_num)
{
	enemy *erot, *nerot;

	int wp_num = curShip.AllLevels[level_num]->waypoints.size;
	// wp_num can be zero, so we can not use a static array
	char *wp_used = NULL; // is a waypoint already used ?
	if (wp_num > 0) {
		wp_used = (char *)MyMalloc(wp_num * sizeof(char));
		memset(wp_used, 0, wp_num * sizeof(char));
	}

	// First we remove all the volatile obstacles...
	//
	remove_volatile_obstacles(level_num);

	// Now we can give new life to dead bots...
	//
	BROWSE_DEAD_BOTS_SAFE(erot, nerot) {
		if (erot->pos.z != level_num || Droidmap[erot->type].is_human || !erot->will_respawn)
			continue;
		/* Move the bot to the alive list */
		list_move(&(erot->global_list), &alive_bots_head);
		/* Reinsert it into the current level list */
		list_add(&(erot->level_list), &level_bots_head[level_num]);
	}

	// Finally, we reset the runtime attributes of the bots, place them
	// on a waypoint, and ask them to start wandering...
	//
	BROWSE_LEVEL_BOTS(erot, level_num) {

		// Unconditional reset of the 'transient state' attributes
		enemy_reset(erot);

		// Conditional reset of some 'global state' attributes
		if (erot->has_been_taken_over) {
			erot->faction = FACTION_BOTS;
			erot->has_been_taken_over = FALSE;
			erot->CompletelyFixed = FALSE;
			erot->follow_tux = FALSE;
		}
		
		erot->has_greeted_influencer = FALSE;

		if (wp_num > 0) {
			// Re-place the bots onto the waypoint system
			if (!erot->SpecialForce) {
				// Standard bots are randomly placed on one waypoint
				int wp = teleport_to_random_waypoint(erot, curShip.AllLevels[level_num], wp_used);
				wp_used[wp] = 1;
				erot->homewaypoint = erot->lastwaypoint;
				erot->combat_state = SELECT_NEW_WAYPOINT;
				erot->state_timeout = 0.0;
			} else {
				if (erot->homewaypoint == -1) {
					// If a special force droid has not yet been integrated onto
					// the waypoint system, place it near its current position.
					int wp = teleport_to_closest_waypoint(erot);
					wp_used[wp] = 1;
					erot->homewaypoint = erot->lastwaypoint;
					erot->combat_state = SELECT_NEW_WAYPOINT;
					erot->state_timeout = 0.0;
				} else {
					// Consider that the nextwaypoint of a special force droid
					// is occupied, so that a standard bot will not be placed here
					if (erot->nextwaypoint != -1)
						wp_used[erot->nextwaypoint] = 1;
				}
			}
		} else {
			error_message(__FUNCTION__, "There is no waypoint on level %d - unable to place random bots.",
					PLEASE_INFORM, level_num);
		}
	}

	free(wp_used);
}

/**
 * \brief Get the center coordinates of a given map label.
 * In case of fail, a fatal error is thrown.
 * \param map_label The name of map label to resolve.
 * \return The gps center of the map label.
 */
gps get_map_label_center(const char *label)
{
	struct map_label *m;
	gps position = {0., 0., -1};
	int i;

	for (i = 0; i < curShip.num_levels; i++) {
		if (!level_exists(i))
			continue;

		m = get_map_label(curShip.AllLevels[i], label);
		if (m) {
			position.x = m->pos.x + 0.5;
			position.y = m->pos.y + 0.5;
			position.z = i;
			return position;
		}
	}

	error_message(__FUNCTION__, "\
Resolving map label   \"%s\"   failed on the entire world!\n\
This is a severe error in the game data of FreedroidRPG.", PLEASE_INFORM, label);

	return position;
};

/**
 * Next we extract the level interface data from the human-readable data 
 * into the level struct, but WITHOUT destroying or damaging the 
 * human-readable data in the process!
 */
static void decode_interfaces(level *loadlevel, char *DataPointer)
{
	char *TempSectionPointer;
	char PreservedLetter;

	// We look for the beginning and end of the map statement section
	TempSectionPointer = LocateStringInData(DataPointer, MAP_BEGIN_STRING);

	// We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
	PreservedLetter = TempSectionPointer[0];
	TempSectionPointer[0] = 0;

	ReadValueFromString(DataPointer, "jump target north: ", "%d", &(loadlevel->jump_target_north), TempSectionPointer);
	ReadValueFromString(DataPointer, "jump target south: ", "%d", &(loadlevel->jump_target_south), TempSectionPointer);
	ReadValueFromString(DataPointer, "jump target east: ", "%d", &(loadlevel->jump_target_east), TempSectionPointer);
	ReadValueFromString(DataPointer, "jump target west: ", "%d", &(loadlevel->jump_target_west), TempSectionPointer);

	TempSectionPointer[0] = PreservedLetter;

}

static void decode_dimensions(level *loadlevel, char *DataPointer)
{

	int off = 0;

	/* Read levelnumber */
	char *fp = DataPointer;
	fp += strlen(LEVEL_HEADER_LEVELNUMBER);
	while (*(fp + off) != '\n')
		off++;
	fp[off] = 0;
	loadlevel->levelnum = atoi(fp);
	fp[off] = '\n';
	fp += off + 1;
	off = 0;

	/* Read xlen */
	fp += strlen("xlen of this level:");
	while (*(fp + off) != '\n')
		off++;
	fp[off] = 0;
	loadlevel->xlen = atoi(fp);
	fp[off] = '\n';
	fp += off + 1;
	off = 0;

	/* Read ylen */
	fp += strlen("ylen of this level:");
	while (*(fp + off) != '\n')
		off++;
	fp[off] = 0;
	loadlevel->ylen = atoi(fp);
	fp[off] = '\n';
	fp += off + 1;
	off = 0;

	/* Read floor_layers */
	fp += strlen("floor layers:");
	while (*(fp + off) != '\n')
		off++;
	fp[off] = 0;
	loadlevel->floor_layers = atoi(fp);
	fp[off] = '\n';
	fp += off + 1;
	off = 0;

	/* Read lrb */
	fp += strlen("light radius bonus of this level:");
	while (*(fp + off) != '\n')
		off++;
	fp[off] = 0;
	loadlevel->light_bonus = atoi(fp);
	fp[off] = '\n';
	fp += off + 1;
	off = 0;

	fp += strlen("minimal light on this level:");
	while (*(fp + off) != '\n')
		off++;
	fp[off] = 0;
	loadlevel->minimum_light_value = atoi(fp);
	fp[off] = '\n';
	fp += off + 1;
	off = 0;

	fp += strlen("infinite_running_on_this_level:");
	while (*(fp + off) != '\n')
		off++;
	fp[off] = 0;
	loadlevel->infinite_running_on_this_level = atoi(fp);
	fp[off] = '\n';
	fp += off + 1;
	off = 0;

	fp += strlen("random dungeon:");
	while (*(fp + off) != '\n')
		off++;
	fp[off] = 0;
	loadlevel->random_dungeon = atoi(fp);
	fp[off] = '\n';
	fp += off + 1;
	off = 0;

	if (!strncmp(fp, TELEPORT_PAIR_STRING, strlen(TELEPORT_PAIR_STRING))) {
		fp += strlen(TELEPORT_PAIR_STRING);
		while (*(fp + off) != '\n')
			off++;
		fp[off] = 0;
		loadlevel->teleport_pair = atoi(fp);
		fp[off] = '\n';
		fp += off + 1;
		off = 0;
	} else {
		loadlevel->teleport_pair = 0;
	}

	if (!strncmp(fp, "dungeon generated:", 18)) {
		fp += strlen("dungeon generated:");
		while (*(fp + off) != '\n')
			off++;
		fp[off] = 0;
		loadlevel->dungeon_generated = atoi(fp);
		fp[off] = '\n';
		fp += off + 1;
		off = 0;
	} else {
		loadlevel->dungeon_generated = 0;
	}

	if (!strncmp(fp, "environmental flags:", 20)) {
		fp += strlen("environmental flags:");
		while (*(fp + off) != '\n')
			off++;
		fp[off] = 0;
		loadlevel->flags = atoi(fp);
		fp[off] = '\n';
		fp += off + 1;
		off = 0;
	} else {
		loadlevel->flags = 0;
	}

	if (!strncmp(fp, "item drop class:", 16)) {
		fp += strlen("item drop class:");
		while (*(fp + off) != '\n')
			off++;
		fp[off] = 0;
		loadlevel->drop_class = atoi(fp);
		fp[off] = '\n';
		// Note: to be commented out if another decoding part is added after this one
		// fp += off + 1;
		// off = 0;
	} else {
		loadlevel->drop_class = -1;
	}

	// Note: if another decoding part is added, do not forget to comment out the 2 lines
	// above...

	if (loadlevel->ylen >= MAX_MAP_LINES) {
		error_message(__FUNCTION__, "\
A map/level in FreedroidRPG which was supposed to load has more map lines than allowed\n\
for a map/level as by the constant MAX_MAP_LINES in defs.h.\n\
Sorry, but unless this constant is raised, FreedroidRPG will refuse to load this map.", PLEASE_INFORM | IS_FATAL);
	}
}

static void decode_random_droids(level *loadlevel, char *data)
{
	char *search_ptr;
	char *end_ptr;

#define DROIDS_NUMBER_INDICATION_STRING "number of random droids: "
#define ALLOWED_TYPE_INDICATION_STRING "random droid types: "

	// Read the number of random droids for this level
	end_ptr = strstr(data, ALLOWED_TYPE_INDICATION_STRING);
	ReadValueFromString(data, DROIDS_NUMBER_INDICATION_STRING, "%d", &loadlevel->random_droids.nr, end_ptr);

	if (loadlevel->random_droids.nr <= 0)
		return;

	// Now we read in the type(s) of random droids for this level
	data = strstr(data, ALLOWED_TYPE_INDICATION_STRING);

	search_ptr = ReadAndMallocStringFromDataOptional(data, ALLOWED_TYPE_INDICATION_STRING, "\n");
	if (search_ptr) {
		char *droid_type_ptr = search_ptr;
		while (*droid_type_ptr) {
			while (*droid_type_ptr && isspace(*droid_type_ptr)) {
				droid_type_ptr++;
			}
			int droid_type_length = 0;
			char *ptr = droid_type_ptr;
			while (isalnum(*ptr)) {
				ptr++;
				droid_type_length++;
			}
			if (!droid_type_length)
				break;

			char type_indication_string[droid_type_length + 1];
			strncpy(type_indication_string, droid_type_ptr, droid_type_length);
			type_indication_string[droid_type_length] = 0;

			int droid_type = get_droid_type(type_indication_string);

			loadlevel->random_droids.types[loadlevel->random_droids.types_size++] = droid_type;

			droid_type_ptr += droid_type_length;
			if (*droid_type_ptr)
				droid_type_ptr++; //skip the comma
		}
		free(search_ptr);
	}
}

static int decode_header(level *loadlevel, char *data)
{
	data = strstr(data, LEVEL_HEADER_LEVELNUMBER);
	if (!data)
		return 1;

	decode_interfaces(loadlevel, data);
	decode_dimensions(loadlevel, data);
	decode_random_droids(loadlevel, data);

	// Read the levelname.
	loadlevel->Levelname = ReadAndMallocStringFromData(data, LEVEL_NAME_STRING, "\"");

	loadlevel->Background_Song_Name = ReadAndMallocStringFromData(data, BACKGROUND_SONG_NAME_STRING, "\n");

	return 0;
}

/**
 * Next we extract the human readable obstacle data into the level struct
 * WITHOUT destroying or damaging the human-readable data in the process!
 * This is an improved parser that is not quite readable but very performant.
 */
static char *decode_obstacles(level *load_level, char *data_pointer)
{
	// First we initialize the obstacles with 'empty' information

	int i;
	for (i = 0; i < MAX_OBSTACLES_ON_MAP; i++) {
		load_level->obstacle_list[i].type = -1;
		load_level->obstacle_list[i].pos.x = -1;
		load_level->obstacle_list[i].pos.y = -1;
		load_level->obstacle_list[i].pos.z = load_level->levelnum;
		load_level->obstacle_list[i].timestamp = 0;
		load_level->obstacle_list[i].frame_index = 0;
	}

	if (load_level->random_dungeon && !load_level->dungeon_generated)
		return data_pointer;

	// Now we look for the beginning and end of the obstacle section

	char *obstacle_SectionBegin = LocateStringInData(data_pointer, OBSTACLE_DATA_BEGIN_STRING) + strlen(OBSTACLE_DATA_BEGIN_STRING) + 1;

	// Now we decode all the obstacle information

	char *curfield = obstacle_SectionBegin;
	while (*curfield != '/') {
		//structure of obstacle entry is :      // t59 x2.50 y63.50 l-1 d-1 
		//we read the type
		curfield++;
		char *curfieldend = curfield;
		while ((*curfieldend) != ' ')
			curfieldend++;
		(*curfieldend) = 0;
		int type = atoi(curfield);
		(*curfieldend) = ' ';

		//we read the X position
		curfield = curfieldend + 2;
		curfieldend += 2;
		while ((*curfieldend) != ' ')
			curfieldend++;
		(*curfieldend) = 0;
		float x = atof(curfield);
		(*curfieldend) = ' ';

		//Y position
		curfield = curfieldend + 2;
		curfieldend += 2;
		while ((*curfieldend) != ' ')
			curfieldend++;
		(*curfieldend) = 0;
		float y = atof(curfield);
		(*curfieldend) = ' ';

		while ((*curfield) != '\n')
			curfield++;
		curfield++;

		// Even invalid obstacles are loaded. They can not be removed at this
		// point, or else obstacle extensions will point to the wrong obstacles.
		// decode_level(), our callee, will take care of them.
		add_obstacle_nocheck(load_level, x, y, type);
	}

	return curfield;
}

/**
 * Next we extract the map labels of this level WITHOUT destroying
 * or damaging the data in the process!
 */
static char *decode_map_labels(level *load_level, char *data)
{
	// Initialize map labels
	dynarray_init(&load_level->map_labels, 10, sizeof(struct map_label));

	if (load_level->random_dungeon && !load_level->dungeon_generated)
		return data;

	// Now we look for the beginning and end of the map labels section
	char *map_label_begin = LocateStringInData(data, MAP_LABEL_BEGIN_STRING) + strlen(MAP_LABEL_BEGIN_STRING) + 1;
	char *map_label_end = LocateStringInData(map_label_begin, MAP_LABEL_END_STRING);
	*map_label_end = '\0';

	// Get the number of map labels in this level
	int nb_map_labels_in_level = CountStringOccurences(map_label_begin, LABEL_ITSELF_ANNOUNCE_STRING);
	DebugPrintf(1, "\nNumber of map labels found in this level : %d.", nb_map_labels_in_level);

	// Now we decode all the map label information
	int i;
	for (i = 0; i < nb_map_labels_in_level ; i++) {
		if (i)
			map_label_begin = strstr(map_label_begin + 1, X_POSITION_OF_LABEL_STRING);

		// Get the position of the map label
		int x, y;
		ReadValueFromString(map_label_begin, X_POSITION_OF_LABEL_STRING, "%d", &x, map_label_end);
		ReadValueFromString(map_label_begin, Y_POSITION_OF_LABEL_STRING, "%d", &y, map_label_end);

		// Get the name of the map label
		char *label_name = ReadAndMallocStringFromData(map_label_begin, LABEL_ITSELF_ANNOUNCE_STRING, "\"");

		// Add the map label on the level
		add_map_label(load_level, x, y, label_name);

		DebugPrintf(1, "\npos.x=%d pos.y=%d label_name=\"%s\"", x, y, label_name);
	}

	*map_label_end = MAP_LABEL_END_STRING[0];
	return map_label_end;
}

static void ReadInOneItem(char *ItemPointer, char *ItemsSectionEnd, item *TargetItem)
{
	init_item(TargetItem);

	char *item_id = ReadAndMallocStringFromData(ItemPointer, ITEM_ID_STRING, "\"");
	TargetItem->type = get_item_type_by_id(item_id);
	free(item_id);

	ReadValueFromString(ItemPointer, ITEM_POS_X_STRING, "%f", &(TargetItem->pos.x), ItemsSectionEnd);
	ReadValueFromString(ItemPointer, ITEM_POS_Y_STRING, "%f", &(TargetItem->pos.y), ItemsSectionEnd);
	ReadValueFromStringWithDefault(ItemPointer, ITEM_ARMOR_CLASS_BASE_STRING, "%d", "0", &(TargetItem->armor_class), ItemsSectionEnd);
	ReadValueFromString(ItemPointer, ITEM_MAX_DURABILITY_STRING, "%d", &(TargetItem->max_durability), ItemsSectionEnd);
	ReadValueFromString(ItemPointer, ITEM_CUR_DURABILITY_STRING, "%f", &(TargetItem->current_durability), ItemsSectionEnd);
	ReadValueFromString(ItemPointer, ITEM_AMMO_CLIP_STRING, "%d", &(TargetItem->ammo_clip), ItemsSectionEnd);
	ReadValueFromString(ItemPointer, ITEM_MULTIPLICITY_STRING, "%d", &(TargetItem->multiplicity), ItemsSectionEnd);
	ReadValueFromStringWithDefault(ItemPointer, ITEM_QUALITY_STRING, "%d", "0", &(TargetItem->quality), ItemsSectionEnd);

	// Read the socket data of the item and calculate bonuses using it.
	int i;
	int socket_count;
	ReadValueFromStringWithDefault(ItemPointer, ITEM_SOCKETS_SIZE_STRING, "%d", "0", &socket_count, ItemsSectionEnd);
	for (i = 0; i < socket_count; i++) {
		char type_string[32];
		char addon_string[32];
		struct upgrade_socket socket;
		sprintf(type_string, "%s%d=", ITEM_SOCKET_TYPE_STRING, i);
		sprintf(addon_string, "%s%d=", ITEM_SOCKET_ADDON_STRING, i);
		ReadValueFromString(ItemPointer, type_string, "%d", &socket.type, ItemsSectionEnd);
		socket.addon = ReadAndMallocStringFromDataOptional(ItemPointer, addon_string, "\"");
		create_upgrade_socket(TargetItem, socket.type, socket.addon);
		free(socket.addon);
	}
	calculate_item_bonuses(TargetItem);

	DebugPrintf(1, "\nPosX=%f PosY=%f Item=%d", TargetItem->pos.x, TargetItem->pos.y, TargetItem->type);

}

static char *decode_extension_chest(char *ext, void **data)
{
	struct dynarray *chest = dynarray_alloc(1, sizeof(item));
	char *item_str, *item_end;
	
	item_str = ext;

	while (*item_str != '}') {
		// Find end of this item (beginning of next item)
		item_end = item_str;
		while (*item_end != '\n')
			item_end++;
		while (isspace(*item_end))
			item_end++;

		// Read the item on this line
		item new_item;
		ReadInOneItem(item_str, item_end, &new_item);

		// Add the item to the dynarray
		dynarray_add(chest, &new_item, sizeof(item));

		// Move to the next item definition
		item_str = item_end;
	}


	*data = chest;
	return item_str;
}

static char *decode_extension_label(char *ext, void **data)
{
	char *begin = NULL;
	char *end = NULL;

	if (*ext != '\"') {
		// For compatibility with old levels.dat and savegames, parse an
		// unquoted string.
		// TODO: to be removed in the future, when unquoted strings are no
		// more used by anybody...
		begin = ext;
		end = begin;
		while (*end != '\n')
			end++;
	} else {
		begin = ext + 1;
		end = begin;
		while (*end != '\"' && *end != '\n')
			end++;
	}

	*end = '\0';
	*data = strdup(begin);
	*end = '\n';

	while (*end != '}')
		end++;

	return end;
}

static char *decode_extension_dialog(char *ext, void **data)
{
	// Same format than label extension.
	return decode_extension_label(ext, data);
}

static char *decode_extension_signmessage(char *ext, void **data)
{
	char *begin = NULL;
	char *end = NULL;

	if (*ext != '_' && *ext != '\"') {
		// For compatibility with old levels.dat and savegames, parse an
		// unquoted string.
		// TODO: to be removed in the future, when unquoted strings are no
		// more used by anybody...
		begin = ext;
		end = begin;
		while (*end != '\n')
			end++;
	} else {
		if (*ext == '\"')
			begin = ext + 1;
		else
			begin = ext + 2;
		end = begin;
		while (*end != '\"' && *end != '\n')
			end++;
	}

	*end = '\0';
	*data = strdup(begin);
	*end = '\n';

	while (*end != '}')
		end++;

	return end;
}

static char *decode_obstacle_extensions(level *loadlevel, char *data)
{
	dynarray_init(&loadlevel->obstacle_extensions, 10, sizeof(struct obstacle_extension));

	if (loadlevel->random_dungeon && !loadlevel->dungeon_generated)
		return data;

	char *ext_begin = LocateStringInData(data, OBSTACLE_EXTENSIONS_BEGIN_STRING);
	char *ext_end = LocateStringInData(ext_begin, OBSTACLE_EXTENSIONS_END_STRING);
	*ext_end = '\0';

	while (1) {
		// Look for the next extension
		ext_begin = strstr(ext_begin, "idx=");
		if (!ext_begin)
			break;

		// Read extension information
		int index;
		int type;
		void *ext_data = NULL;
		sscanf(ext_begin, "idx=%d type=%d", &index, &type);

		// Move to the extension data definition
		ext_begin = strstr(ext_begin, "data={\n");
		while (*ext_begin != '\n')
			ext_begin++;
		while (isspace(*ext_begin))
			ext_begin++;

		// Read the extension data
		switch (type) {
			case OBSTACLE_EXTENSION_CHEST_ITEMS:
				ext_begin = decode_extension_chest(ext_begin, &ext_data);
				break;
			case OBSTACLE_EXTENSION_LABEL:
				ext_begin = decode_extension_label(ext_begin, &ext_data);
				break;
			case OBSTACLE_EXTENSION_DIALOGFILE:
			{
				// Old levels.dat and savegames use this type for
				// dialog extensions as well as for sign messages extensions.
				// New 'format' uses two different values.
				// For compatibility, at least during some time, we accept
				// to read sign message extension here, but we convert it to
				// its actual type.
				// The actual use of an obstacle extension, and thus its actual type,
				// is defined by the 'action' set to the obstacle (see action.c).
				// TODO: To be removed in the future
				obstacle *obs = &(loadlevel->obstacle_list[index]);
				obstacle_spec *spec = get_obstacle_spec(obs->type);

				if (spec->action && strcmp(spec->action, "sign")) {
					// This is really a dialog extension
					ext_begin = decode_extension_dialog(ext_begin, &ext_data);
					break;
				}
				// This is a sign message extension
				// We change the extension's type, and continue within the next
				// switch case
				type = OBSTACLE_EXTENSION_SIGNMESSAGE;
			}
			/* no break */
			case OBSTACLE_EXTENSION_SIGNMESSAGE:
				ext_begin = decode_extension_signmessage(ext_begin, &ext_data);
				break;
		}

		// Add the obstacle extension on the level
		add_obstacle_extension(loadlevel, &(loadlevel->obstacle_list[index]), type, ext_data);
	}

	*ext_end = OBSTACLE_EXTENSIONS_END_STRING[0];
	return ext_end;
}

static char *decode_item_section(level *loadlevel, char *data)
{
	int i;
	char Preserved_Letter;
	int NumberOfItemsInThisLevel;
	char *ItemPointer;
	char *ItemsSectionBegin;
	char *ItemsSectionEnd;

	// First we initialize the items arrays with 'empty' information
	//
	for (i = 0; i < MAX_ITEMS_PER_LEVEL; i++) {
		init_item(&loadlevel->ItemList[i]);
	}

	if (loadlevel->random_dungeon && !loadlevel->dungeon_generated)
		return data;

	// We look for the beginning and end of the items section
	ItemsSectionBegin = LocateStringInData(data, ITEMS_SECTION_BEGIN_STRING);
	ItemsSectionEnd = LocateStringInData(ItemsSectionBegin, ITEMS_SECTION_END_STRING);

	// We add a terminator at the end of the items section, but ONLY TEMPORARY.  
	// The damage will be restored later!
	Preserved_Letter = ItemsSectionEnd[0];
	ItemsSectionEnd[0] = 0;
	NumberOfItemsInThisLevel = CountStringOccurences(ItemsSectionBegin, ITEM_ID_STRING);
	DebugPrintf(1, "\nNumber of items found in this level : %d.", NumberOfItemsInThisLevel);

	// Now we decode all the item information
	ItemPointer = ItemsSectionBegin;
	char *NextItemPointer;
	for (i = 0; i < NumberOfItemsInThisLevel; i++) {
		if ((ItemPointer = strstr(ItemPointer + 1, ITEM_ID_STRING))) {
			NextItemPointer = strstr(ItemPointer + 1, ITEM_ID_STRING);
			if (NextItemPointer)
				NextItemPointer[0] = 0;
			ReadInOneItem(ItemPointer, ItemsSectionEnd, &(loadlevel->ItemList[i]));
			loadlevel->ItemList[i].pos.z = loadlevel->levelnum;
			if (NextItemPointer)
				NextItemPointer[0] = ITEM_ID_STRING[0];
		}
	}

	// Now we repair the damage done to the loaded level data
	ItemsSectionEnd[0] = Preserved_Letter;
	return ItemsSectionEnd;
}

static char *decode_map(level *loadlevel, char *data)
{
	char *map_begin, *map_end;
	char *this_line;
	int i;

	if ((map_begin = strstr(data, MAP_BEGIN_STRING)) == NULL)
		return NULL;
	map_begin += strlen(MAP_BEGIN_STRING) + 1;

	if ((map_end = strstr(data, MAP_END_STRING)) == NULL)
		return NULL;

	/* now scan the map */
	unsigned int curlinepos = 0;
	this_line = (char *)MyMalloc(4096);

	/* read MapData */
	for (i = 0; i < loadlevel->ylen; i++) {
		int col;
		int layer;
		map_tile *Buffer;
		int tmp;

		/* Select the next line */
		unsigned int nlpos = 0;
		memset(this_line, 0, 4096);
		while (map_begin[curlinepos + nlpos] != '\n')
			nlpos++;
		memcpy(this_line, map_begin + curlinepos, nlpos);
		this_line[nlpos] = '\0';
		nlpos++;

		/* Decode it */
		Buffer = MyMalloc((loadlevel->xlen + 10) * sizeof(map_tile));
		for (col = 0; col < loadlevel->xlen; col++) {
			// Make sure that all floor layers are always initialized properly.
			init_map_tile(&Buffer[col]);

			for (layer = 0; layer < loadlevel->floor_layers; layer++) {
				tmp = strtol(this_line + 4 * (loadlevel->floor_layers * col + layer), NULL, 10);
				if (tmp >= underlay_floor_tiles.size) {
					if (tmp < MAX_UNDERLAY_FLOOR_TILES || (tmp - MAX_UNDERLAY_FLOOR_TILES) >= overlay_floor_tiles.size) {
						error_message(__FUNCTION__, "Level %d at (%d, %d) in layer #%d uses an unknown floor tile: %d.", PLEASE_INFORM,
								loadlevel->levelnum, col, i, layer, tmp);
						tmp = ISO_FLOOR_EMPTY;
					}
				}

				Buffer[col].floor_values[layer] = (Uint16) tmp;
			}
		}

		// Now the old text pointer can be replaced with a pointer to the
		// correctly assembled struct...
		//
		loadlevel->map[i] = Buffer;

		curlinepos += nlpos;
	}

	free(this_line);
	return map_end;
}

static char *decode_waypoints(level *loadlevel, char *data)
{
	char *wp_begin, *wp_end;
	char *this_line;
	int nr, x, y, wp_rnd;

	// Initialize waypoints
	dynarray_init(&loadlevel->waypoints, 2, sizeof(struct waypoint));

	if (loadlevel->random_dungeon && !loadlevel->dungeon_generated)
		return data;

	// Find the beginning and end of the waypoint list
	if ((wp_begin = strstr(data, WP_BEGIN_STRING)) == NULL)
		return NULL;
	wp_begin += strlen(WP_BEGIN_STRING) + 1;

	if ((wp_end = strstr(data, WP_END_STRING)) == NULL)
		return NULL;

	int curlinepos = 0;
	this_line = (char *)MyMalloc(4096);
	
	while (1) {
		/* Select the next line */
		short int nlpos = 0;
		memset(this_line, 0, 4096);
		while (wp_begin[curlinepos + nlpos] != '\n')
			nlpos++;
		memcpy(this_line, wp_begin + curlinepos, nlpos);
		this_line[nlpos] = '\0';
		nlpos++;
		
		curlinepos += nlpos;

		if (!strncmp(this_line, wp_end, strlen(WP_END_STRING))) {
			break;
		}
		wp_rnd = 0;
		sscanf(this_line, "Nr.=%d \t x=%d \t y=%d   rnd=%d", &nr, &x, &y, &wp_rnd);

		// Create a new waypoint
		waypoint new_wp;
		new_wp.x = x;
		new_wp.y = y;
		new_wp.suppress_random_spawn = wp_rnd;

		// Initalize the connections of the new waypoint
		dynarray_init(&new_wp.connections, 2, sizeof(int));

		char *pos = strstr(this_line, CONNECTION_STRING);
		if (pos == NULL) {
			error_message(__FUNCTION__,
			              "Unable to find connection string, on line %s, level %i.\n"
			              "The data file seems to be corrupted.\n"
			              "We continue to load it, but you may encounter some errors while playing.",
						  PLEASE_INFORM, this_line, loadlevel->levelnum);
		} else {
			pos += strlen(CONNECTION_STRING);	// skip connection-string
			pos += strspn(pos, WHITE_SPACE);	// skip initial whitespace

			while (1) {
				if (*pos == '\0')
					break;
				int connection;
				int res = sscanf(pos, "%d", &connection);
				if ((connection == -1) || (res == 0) || (res == EOF))
					break;

				// Add the connection on this waypoint
				dynarray_add(&new_wp.connections, &connection, sizeof(int));

				pos += strcspn(pos, WHITE_SPACE);	// skip last token
				pos += strspn(pos, WHITE_SPACE);	// skip initial whitespace for next one
			}
		}

		// Add the waypoint on the level
		dynarray_add(&loadlevel->waypoints, &new_wp, sizeof(struct waypoint));
	}

	free(this_line);
	return wp_end;
}

/**
 * Blast a specific obstacle.
 *
 * First remove the obstacle from the map, and then start a blast.
 */
void blast_obstacle(level *lvl, obstacle *target_obstacle)
{
	struct pointf blast_start_pos = { .x = target_obstacle->pos.x, .y = target_obstacle->pos.y };

	struct obstacle_spec *spec = get_obstacle_spec(target_obstacle->type);
	int obstacle_drops_treasure = spec->flags & DROPS_RANDOM_TREASURE;

	// Let the automap know that we've updated things
	update_obstacle_automap(target_obstacle->pos.z, target_obstacle);

	// Now we really smash the obstacle, i.e. we can set it's type to
	// the debris that has been configured for this obstacle type.
	// If there is nothing configured (i.e. -1 set) then we'll just
	// delete the obstacle in question entirely. For this we got a
	// standard function to safely do it and not make some errors into
	// the glue structure or obstacles lists...
	if (spec->result_type_after_smashing_once == -1) {
		del_obstacle(target_obstacle);
	} else {
		target_obstacle->type = spec->result_type_after_smashing_once;
	}

	// Drop items after destroying the obstacle, in order to avoid collisions
	if (obstacle_drops_treasure)
		drop_random_item(target_obstacle->pos.z, target_obstacle->pos.x, target_obstacle->pos.y, lvl->drop_class, FALSE);

	// Now that the obstacle is removed AND ONLY NOW that the obstacle is
	// removed, we may start a blast at this position.  Otherwise we would
	// run into trouble, because the blast could in turn destroy this not yet
	// removed obstacle -> infinite loop danger here !!!
	start_blast(blast_start_pos.x, blast_start_pos.y, target_obstacle->pos.z,
		spec->blast_type, 0.0, FACTION_SELF, spec->smashed_sound);
}

/**
 * The smash_obstacle_near_pos function uses this function as a subfunction to
 * check for exploding obstacles glued to one specific map square.  Of
 * course also the player number (or -1 in case of no check/bullet hit)
 * must be supplied so as to be able to suppress hits through walls or
 * the like.
 */
static int smash_obstacles_only_on_tile(float x, float y, int lvl, int map_x, int map_y)
{
	struct level *box_level = curShip.AllLevels[lvl];
	int i;
	int target_idx;
	struct obstacle *target_obstacle;
	int smashed_something = FALSE;

	// First some security checks against touching the outsides of the map...

	if (!pos_inside_level(map_x, map_y, box_level))
		return (FALSE);

	// We check all the obstacles on this square if they are maybe destructible
	// and if they are, we destruct them, haha

	for (i = 0; i < box_level->map[map_y][map_x].glued_obstacles.size; i++) {
		// First we see if there is something glued to this map tile at all.

		target_idx = ((int *)(box_level->map[map_y][map_x].glued_obstacles.arr))[i];

		target_obstacle = &(box_level->obstacle_list[target_idx]);

		struct obstacle_spec *obstacle_spec = get_obstacle_spec(target_obstacle->type);
		if (!(obstacle_spec->flags & IS_SMASHABLE))
			continue;

		// Now we check if the item really was close enough to the strike target.
		// A range of 0.5 should do.

		if (fabsf(x - target_obstacle->pos.x) > 0.4)
			continue;
		if (fabsf(y - target_obstacle->pos.y) > 0.4)
			continue;

		struct colldet_filter filter = FlyableExceptIdPassFilter;
		filter.data = &target_idx;
		struct gps smash_pos = { x, y, lvl };
		struct gps vsmash_pos;
		update_virtual_position(&vsmash_pos, &smash_pos, Me.pos.z);
		if (vsmash_pos.x == -1)
			continue;
		if (!DirectLineColldet(vsmash_pos.x, vsmash_pos.y, Me.pos.x, Me.pos.y, Me.pos.z, &filter)) {
			continue;
		}

		smashed_something = TRUE;

		event_obstacle_action(target_obstacle);

		blast_obstacle(box_level, target_obstacle);
	}

	return smashed_something;
}



/**
 * When a destructible type of obstacle gets hit, e.g. by a blast 
 * exploding on the tile or a melee hit on the same floor tile, then some
 * of the obstacles around it (like barrels or crates) might explode, sometimes
 * leaving some treasure behind.
 *
 */
int smash_obstacles_near_pos(float x, float y, int lvl)
{
	int smashed_something = FALSE;
	int map_x = (int)rintf(x);
	int map_y = (int)rintf(y);

	// Try, on a 3x3 area around the obstacle, to smash things (if
	// they are near enough)
	for (int smash_y = map_y - 1; smash_y < map_y + 2; smash_y++) {
		for (int smash_x = map_x - 1; smash_x < map_x + 2; smash_x++) {
			if (smash_obstacles_only_on_tile(x, y, lvl, smash_x, smash_y))
				smashed_something = TRUE;
		}
	}

	return smashed_something;
}

/**
 * This function returns the map brick code of the tiles that occupies the
 * given position. The return value is an array for all floor layers.
 * Floor layers are indexed from 0 to lvl->floor_layers - 1. The lowest
 * floor layer is #0. Every map has at least one layer.
 * If there are no floor tiles, the function may return NULL, which is
 * equivalent to an array full of ISO_FLOOR_EMPTY.
 */
uint16_t *get_map_brick(level *lvl, float x, float y)
{
	gps vpos = { x, y, lvl->levelnum };
	gps rpos;
	if (!resolve_virtual_position(&rpos, &vpos)) {
		return NULL;
	}

	int roundX = (int)rintf(rpos.x);
	int roundY = (int)rintf(rpos.y);
	return curShip.AllLevels[rpos.z]->map[roundY][roundX].floor_values;
}

/**
 * This functions reads the specification for a level
 * taken from the ship file.
 *
 * @return pointer to the level
 * @param text buffer containing level description
 */
static level *decode_level(char **buffer)
{
	level *loadlevel;
	char *data = *buffer;

	loadlevel = (level *)MyMalloc(sizeof(level));
	
	if (decode_header(loadlevel, data)) {
		error_message(__FUNCTION__, "Unable to decode level header!", PLEASE_INFORM | IS_FATAL);
	}
	
	// The order of sections in the file has to match this.	
	data = decode_map(loadlevel, data);
	if (!data) {
		error_message(__FUNCTION__, "Unable to decode the map for level %d", PLEASE_INFORM | IS_FATAL, loadlevel->levelnum);
	}
	data = decode_obstacles(loadlevel, data);
	data = decode_map_labels(loadlevel, data);
	data = decode_item_section(loadlevel, data);
	data = decode_obstacle_extensions(loadlevel, data);
	data = decode_waypoints(loadlevel, data);

	// Due to currently unknown bugs, some obstacles can be located on invalid
	// positions. If we are starting the lvleditor, we will warn the user but
	// keep those obstacles to help investigating the bugs. Otherwise, those
	// obstacles and their associated extension are silently removed.
	int i;
	int need_defrag = FALSE;
	struct auto_string *error_msg = alloc_autostr(256);

	for (i = 0; i < MAX_OBSTACLES_ON_MAP; i++) {
		struct obstacle *obs = &loadlevel->obstacle_list[i];
		if (obs->type == -1)
			continue;
		if (!pos_inside_level(obs->pos.x, obs->pos.y, loadlevel)) {
			if (game_root_mode == ROOT_IS_LVLEDIT && game_status == INSIDE_LVLEDITOR) {
				autostr_append(error_msg, "Invalid obstacle (%s) position on level %d: t%d x%3.2f y%3.2f\n",
						((char **)get_obstacle_spec(obs->type)->filenames.arr)[0],
						loadlevel->levelnum, obs->type, obs->pos.x, obs->pos.y);
				alert_once_window(ONCE_PER_GAME,
						_("-- WARNING --\n"
						  "An obstacle with invalid coords was loaded onto a map.\n"
						  "We keep it to help finding the bug.\n"
						  "See the report in your terminal console."));
			} else if (game_root_mode != ROOT_IS_LVLEDIT) {
				unglue_obstacle(loadlevel, obs);
				obs->type = -1;
				del_obstacle_extensions(loadlevel, obs);
				need_defrag = TRUE;
			}
		}
	}
	if (error_msg->length != 0) {
		// Remove the last carriage return
		error_msg->value[error_msg->length - 1] = '\0';
		error_message(__FUNCTION__,	"%s", NO_REPORT, error_msg->value);
	}
	free_autostr(error_msg);
	if (need_defrag)
		defrag_obstacle_array(loadlevel);

	// Point the buffer to the end of this level, so the next level can be read
	*buffer = data;
	return loadlevel;
}


/** 
 * Call the random dungeon generator on this level  if this level is marked
 * as being randomly generated and if we are not in the "leveleditor" mode
 * in which case random dungeons must not be considered as generated (so that
 * they will be exported as being non-generated random levels).
 */
static void generate_dungeon_if_needed(level *l)
{
	if (!l->random_dungeon || l->dungeon_generated) {
		return;
	}

	// Generate random dungeon now
	set_dungeon_output(l);
	generate_dungeon(l->xlen, l->ylen, l->random_dungeon, l->teleport_pair);
	l->dungeon_generated = 1;
}

void free_ship_level(level *lvl)
{
	int row = 0;
	int col = 0;

	// Map tiles
	remove_volatile_obstacles(lvl->levelnum);
	for (row = 0; row < lvl->ylen; row++) {
		if (lvl->map[row]) {
			for (col = 0; col < lvl->xlen; col++) {
				dynarray_free(&lvl->map[row][col].glued_obstacles);
				free(lvl->map[row][col].volatile_obstacles);
			}

			free(lvl->map[row]);
			lvl->map[row] = NULL;
		}
	}

	// Level strings
	if (lvl->Levelname) {
		free(lvl->Levelname);
		lvl->Levelname = NULL;
	}

	if (lvl->Background_Song_Name) {
		free(lvl->Background_Song_Name);
		lvl->Background_Song_Name = NULL;
	}

	// Waypoints
	int w;
	for (w = 0; w < lvl->waypoints.size; w++) {
		struct waypoint *wpts = lvl->waypoints.arr;
		dynarray_free(&wpts[w].connections);
	}

	dynarray_free(&lvl->waypoints);

	// Obstacle extensions
	free_obstacle_extensions(lvl);

	// Map labels
	free_map_labels(lvl);

	// Random droids
	lvl->random_droids.types_size = 0;

	// Items
	for (w = 0; w < MAX_ITEMS_PER_LEVEL; w++) {
		if (lvl->ItemList[w].type != -1) {
			delete_upgrade_sockets(&(lvl->ItemList[w]));
		}
	}

	free(lvl);
}

void free_current_ship()
{
	struct level *lvl;
	BROWSE_LEVELS(lvl) {
		int lvlnum = lvl->levelnum;
		free_ship_level(lvl);
		curShip.AllLevels[lvlnum] = NULL;
	}
	curShip.num_levels = 0;
}

/**
 * This function loads the data for a whole ship
 * Possible return values are : OK and ERR
 */
int LoadShip(char *filename, int compressed)
{
	char *ShipData = NULL;
	FILE *ShipFile;

#define END_OF_SHIP_DATA_STRING "*** End of Ship Data ***"

	// Free existing level data
	free_current_ship();

	// Read the whole ship-data to memory 
	//
	ShipFile = fopen(filename, "rb");
	if (!ShipFile) {
		error_message(__FUNCTION__, "Unable to open ship file %s: %s.", PLEASE_INFORM | IS_FATAL, filename, strerror(errno));
	}

	if (compressed) {
		if (inflate_stream(ShipFile, (unsigned char **)&ShipData, NULL)) {
			error_message(__FUNCTION__, "Unable to decompress ship file %s.", PLEASE_INFORM | IS_FATAL, filename);
		}
	} else {
		int length = FS_filelength(ShipFile);
		ShipData = malloc(length + 1);
		if (!fread(ShipData, length, 1, ShipFile))
			error_message(__FUNCTION__, "Reading ship file %s failed with fread().", PLEASE_INFORM | IS_FATAL, filename);
		ShipData[length] = 0;
	}

	fclose(ShipFile);

	// Read each level
	int done = 0;
	char *pos = ShipData;
	while (!done) {
		level *this_level = decode_level(&pos);
		int this_levelnum = this_level->levelnum;

		if (this_levelnum >= MAX_LEVELS)
			error_message(__FUNCTION__, "One levelnumber in savegame (%d) is bigger than the maximum allowed (%d).",
					PLEASE_INFORM | IS_FATAL, this_levelnum, MAX_LEVELS - 1);
		if (level_exists(this_levelnum))
			error_message(__FUNCTION__, "Two levels with same levelnumber (%d) found in the savegame.",
					PLEASE_INFORM | IS_FATAL, this_levelnum);

		curShip.AllLevels[this_levelnum] = this_level;
		curShip.num_levels = this_levelnum + 1;
		
		generate_dungeon_if_needed(this_level);

		// Move to the level termination marker
		pos = strstr(pos, LEVEL_END_STRING);
		pos += strlen(LEVEL_END_STRING) + 1;

		// Check if there is another level
		if (!strstr(pos, LEVEL_HEADER_LEVELNUMBER)) {
			done = 1;
		} 
	}

	// Now that all the information has been copied, we can free the loaded data
	// again.
	//
	free(ShipData);

	// Compute the gps transform acceleration data
	gps_transform_map_dirty_flag = TRUE;
	gps_transform_map_init();

	return OK;

};				// int LoadShip ( ... ) 

/**
 * This should write the obstacle information in human-readable form into
 * a buffer.
 */
static void encode_obstacles_of_this_level(struct auto_string *shipstr, struct level *lvl)
{
	int i;
	struct auto_string *error_msg = alloc_autostr(256);

	defrag_obstacle_array(lvl);

	autostr_append(shipstr, "%s\n", OBSTACLE_DATA_BEGIN_STRING);

	for (i = 0; i < MAX_OBSTACLES_ON_MAP; i++) {
		if (lvl->obstacle_list[i].type == (-1))
			continue;

		// Invalid obstacles are saved, but with a warning message unless we
		// are playtesting, to enable further inspection of the bug.
		// Anyhow, invalid obstacles are not loaded.
		if (!pos_inside_level(lvl->obstacle_list[i].pos.x, lvl->obstacle_list[i].pos.y, lvl)) {
			if (game_root_mode != ROOT_IS_LVLEDIT) {
				error_once_message(ONCE_PER_GAME, __FUNCTION__,
					"Some obstacles with an invalid position were found"
					" while saving them. See the individual reports below.", PLEASE_INFORM);
			}
			if (game_root_mode != ROOT_IS_LVLEDIT || game_status == INSIDE_LVLEDITOR) {
				autostr_append(error_msg, "Invalid obstacle (%s) position on level %d: t%d x%3.2f y%3.2f\n",
						((char **)get_obstacle_spec(lvl->obstacle_list[i].type)->filenames.arr)[0],
						lvl->levelnum, lvl->obstacle_list[i].type, lvl->obstacle_list[i].pos.x, lvl->obstacle_list[i].pos.y);
			}
			// If we are inside the lvleditor, also display an alert
			if (game_root_mode == ROOT_IS_LVLEDIT && game_status == INSIDE_LVLEDITOR) {
				alert_once_window(ONCE_PER_GAME,
						_("-- WARNING --\n"
						  "An obstacle with invalid coords is saved to a map.\n"
						  "We accept to save it, for further inspection of the bug.\n"
						  "See the report in your terminal console."));
			}
		}
		autostr_append(shipstr, "%s%d %s%3.2f %s%3.2f\n", OBSTACLE_TYPE_STRING, lvl->obstacle_list[i].type,
				OBSTACLE_X_POSITION_STRING, lvl->obstacle_list[i].pos.x, OBSTACLE_Y_POSITION_STRING,
				lvl->obstacle_list[i].pos.y);
	}

	autostr_append(shipstr, "%s\n", OBSTACLE_DATA_END_STRING);

	if (error_msg->length != 0) {
		// Remove the last carriage return
		error_msg->value[error_msg->length - 1] = '\0';
		error_message(__FUNCTION__,	"%s", NO_REPORT, error_msg->value);
	}
	free_autostr(error_msg);
}

static void encode_map_labels(struct auto_string *shipstr, struct level *lvl)
{
	autostr_append(shipstr, "%s\n", MAP_LABEL_BEGIN_STRING);

	int i;
	for (i = 0; i < lvl->map_labels.size; i++) {
		// Get the map label
		struct map_label *map_label = &ACCESS_MAP_LABEL(lvl->map_labels, i);

		// Encode map label
		autostr_append(shipstr, "%s%d %s%d %s%s\"\n", X_POSITION_OF_LABEL_STRING, map_label->pos.x, Y_POSITION_OF_LABEL_STRING,
				            map_label->pos.y, LABEL_ITSELF_ANNOUNCE_STRING, map_label->label_name);
	}

	autostr_append(shipstr, "%s\n", MAP_LABEL_END_STRING);
}

/**
 *
 * 
 */
static void WriteOutOneItem(struct auto_string *shipstr, item *ItemToWriteOut)
{

	autostr_append(shipstr, "%s%s\" %s%f %s%f ", ITEM_ID_STRING, ItemMap[ItemToWriteOut->type].id,
			ITEM_POS_X_STRING, ItemToWriteOut->pos.x, ITEM_POS_Y_STRING, ItemToWriteOut->pos.y);

	if (ItemToWriteOut->armor_class) {
		autostr_append(shipstr, "%s%d ", ITEM_ARMOR_CLASS_BASE_STRING, ItemToWriteOut->armor_class);
	}

	autostr_append(shipstr, "%s%d %s%f %s%d %s%d %s%d",
			ITEM_MAX_DURABILITY_STRING, ItemToWriteOut->max_durability,
			ITEM_CUR_DURABILITY_STRING, ItemToWriteOut->current_durability,
			ITEM_AMMO_CLIP_STRING, ItemToWriteOut->ammo_clip,
			ITEM_MULTIPLICITY_STRING, ItemToWriteOut->multiplicity,
			ITEM_QUALITY_STRING, ItemToWriteOut->quality);

	// Write the sockets of the item. The bonuses can be reconstructed from
	// these easily so we don't need to write them at all.
	if (ItemToWriteOut->upgrade_sockets.size) {
		int i;
		autostr_append(shipstr, "%s%d ", ITEM_SOCKETS_SIZE_STRING, ItemToWriteOut->upgrade_sockets.size);
		for (i = 0; i < ItemToWriteOut->upgrade_sockets.size; i++) {
			struct upgrade_socket *socket = (struct upgrade_socket *)dynarray_member(&ItemToWriteOut->upgrade_sockets, i, sizeof(struct upgrade_socket));

			autostr_append(shipstr, "%s%d=%d ", ITEM_SOCKET_TYPE_STRING, i, socket->type);
			if (socket->addon) {
				autostr_append(shipstr, "%s%d=%s\" ", ITEM_SOCKET_ADDON_STRING, i, socket->addon);
			}
		}
	}

	autostr_append(shipstr, "\n");
}

static void EncodeItemSectionOfThisLevel(struct auto_string *shipstr, level *Lev)
{
	int i;

	autostr_append(shipstr, "%s\n", ITEMS_SECTION_BEGIN_STRING);

	// Now we write out the bulk of items infos
	//
	for (i = 0; i < MAX_ITEMS_PER_LEVEL; i++) {
		if (Lev->ItemList[i].type == (-1))
			continue;

		WriteOutOneItem(shipstr, &(Lev->ItemList[i]));

	}

	autostr_append(shipstr, "%s\n", ITEMS_SECTION_END_STRING);
}

static void encode_extension_chest(struct auto_string *shipstr, struct obstacle_extension *ext)
{
	int i;
	struct dynarray *da = ext->data;

	for (i = 0; i < da->size; i++) {
		item *it = &((item *)da->arr)[i];
		if (it->type == -1)
			continue;

		autostr_append(shipstr, "\t");
		WriteOutOneItem(shipstr, it);
	}
}

static void encode_extension_label(struct auto_string *shipstr, struct obstacle_extension *ext)
{
	const char *label = ext->data;

	autostr_append(shipstr, "\t\"%s\"\n", label);
}

static void encode_extension_dialog(struct auto_string *shipstr, struct obstacle_extension *ext)
{
	const char *label = ext->data;

	autostr_append(shipstr, "\t\"%s\"\n", label);
}

static void encode_extension_signmessage(struct auto_string *shipstr, struct obstacle_extension *ext)
{
	const char *label = ext->data;

	// Sign messages can be localized. Use gettext marker.
	autostr_append(shipstr, "\t_\"%s\"\n", label);
}

static void encode_obstacle_extensions(struct auto_string *shipstr, level *l)
{
	int i;
	autostr_append(shipstr, "%s\n", OBSTACLE_EXTENSIONS_BEGIN_STRING);
	for (i = 0; i < l->obstacle_extensions.size; i++) {
		struct obstacle_extension *ext = &ACCESS_OBSTACLE_EXTENSION(l->obstacle_extensions, i);

		if (ext->type == 0)
			continue;

		autostr_append(shipstr, "idx=%d type=%d data={\n", get_obstacle_index(l, ext->obs), ext->type);

		switch ((enum obstacle_extension_type)(ext->type)) {
			case OBSTACLE_EXTENSION_CHEST_ITEMS:
				encode_extension_chest(shipstr, ext);
				break;
			case OBSTACLE_EXTENSION_LABEL:
				encode_extension_label(shipstr, ext);
				break;
			case OBSTACLE_EXTENSION_DIALOGFILE:
				encode_extension_dialog(shipstr, ext);
				break;
			case OBSTACLE_EXTENSION_SIGNMESSAGE:
				encode_extension_signmessage(shipstr, ext);
				break;
		}

		autostr_append(shipstr, "}\n");
	}
	autostr_append(shipstr, "%s\n", OBSTACLE_EXTENSIONS_END_STRING);
}

static void encode_waypoints(struct auto_string *shipstr, struct level *lvl)
{
	waypoint *wpts = lvl->waypoints.arr;

	autostr_append(shipstr, "%s\n", WP_BEGIN_STRING);

	int i;
	for (i = 0; i < lvl->waypoints.size; i++) {
		// Encode the waypoint
		autostr_append(shipstr, "Nr.=%3d x=%4d y=%4d rnd=%1d\t %s", i, wpts[i].x, wpts[i].y, wpts[i].suppress_random_spawn, CONNECTION_STRING);

		// Get the connections of the waypoint
		int *connections = wpts[i].connections.arr;

		int j;
		for (j = 0; j < wpts[i].connections.size; j++) {
			// Check connected waypoint validity
			int connected_waypoint = connections[j];
			if (connected_waypoint < 0 || connected_waypoint >= lvl->waypoints.size) {
				error_message(__FUNCTION__, "A connection to an invalid waypoint (#%d) was found while encoding level #%d\n."
				              "We discard it.", PLEASE_INFORM, connected_waypoint, lvl->levelnum);
				continue;
			}
			// Encode the connection of the waypoint
			autostr_append(shipstr, " %3d", connected_waypoint);
		}

		autostr_append(shipstr, "\n");
	}
}

/**
 * This function translates map data into human readable map code, that
 * can later be written to the map file on disk.
 */
static void TranslateToHumanReadable(struct auto_string *str, map_tile * MapInfo, int LineLength, int layers)
{
#ifdef __clang_analyzer__
	// Avoid Clang Static Analyser to report a possible OOB access
	// on MapInfo[col].floor_values
	if (layers >= MAX_FLOOR_LAYERS) return;
#endif

	int col;
	int layer;

	for (col = 0; col < LineLength; col++) {
		for (layer = 0; layer < layers; layer++)
			autostr_append(str, "%3d ", MapInfo[col].floor_values[layer]);
	}

	autostr_append(str, "\n");
}

/**
 * This function generates savable text out of the current level data
 *
 * If reset_random_levels is TRUE, then the random levels are saved
 * "un-generated" (typical usage: levels.dat).
 *
 */
static void encode_level_for_saving(struct auto_string *shipstr, level *lvl, int reset_random_levels)
{
	int i;
	int xlen = lvl->xlen, ylen = lvl->ylen;

	// Write level header	
	autostr_append(shipstr, "%s %d\n\
xlen of this level: %d\n\
ylen of this level: %d\n\
floor layers: %d\n\
light radius bonus of this level: %d\n\
minimal light on this level: %d\n\
infinite_running_on_this_level: %d\n\
random dungeon: %d\n\
teleport pair: %d\n\
dungeon generated: %d\n\
environmental flags: %d\n\
item drop class: %d\n\
jump target north: %d\n\
jump target south: %d\n\
jump target east: %d\n\
jump target west: %d\n", LEVEL_HEADER_LEVELNUMBER, lvl->levelnum,
		lvl->xlen, lvl->ylen, lvl->floor_layers,
		lvl->light_bonus, lvl->minimum_light_value,
		lvl->infinite_running_on_this_level,
		lvl->random_dungeon,
		lvl->teleport_pair,
		(reset_random_levels && lvl->random_dungeon) ? 0 : lvl->dungeon_generated,
		lvl->flags,
		lvl->drop_class,
		lvl->jump_target_north,
		lvl->jump_target_south,
		lvl->jump_target_east,
		lvl->jump_target_west);

	autostr_append(shipstr, "number of random droids: %d\n", lvl->random_droids.nr);
	autostr_append(shipstr, "random droid types: ");

	for (i = 0; i < lvl->random_droids.types_size; i++) {
		if (i)
			autostr_append(shipstr, ", ");
		autostr_append(shipstr, "%s", Droidmap[lvl->random_droids.types[i]].droidname);
	}

	autostr_append(shipstr, "\n%s%s\"\n%s%s\n", LEVEL_NAME_STRING, lvl->Levelname,
			BACKGROUND_SONG_NAME_STRING, lvl->Background_Song_Name);

	autostr_append(shipstr, "%s\n", MAP_BEGIN_STRING);

	// Now in the loop each line of map data should be saved as a whole
	for (i = 0; i < ylen; i++) {
		if (!(reset_random_levels && lvl->random_dungeon)) {
			TranslateToHumanReadable(shipstr, lvl->map[i], xlen, lvl->floor_layers);
		} else {
			int j = xlen;
			while (j--) {
				autostr_append(shipstr, "  0 ");
			}
			autostr_append(shipstr, "\n");
		}
	}

	autostr_append(shipstr, "%s\n", MAP_END_STRING);

	if (!(reset_random_levels && lvl->random_dungeon)) {
		encode_obstacles_of_this_level(shipstr, lvl);

		encode_map_labels(shipstr, lvl);

		EncodeItemSectionOfThisLevel(shipstr, lvl);

		encode_obstacle_extensions(shipstr, lvl);

		encode_waypoints(shipstr, lvl);
	}

	autostr_append(shipstr, "%s\n----------------------------------------------------------------------\n", 
			LEVEL_END_STRING);
}

/**
 * This function should save a whole ship to disk to the given filename.
 * It is not only used by the level editor, but also by the function that
 * saves whole games.
 *
 * If reset_random_levels is TRUE, then the random levels are saved
 * "un-generated" (typical usage: levels.dat).
 * @return 0 if OK, 1 on error
 */
int SaveShip(const char *filename, int reset_random_levels, int compress)
{
	int i;
	FILE *ShipFile = NULL;
	struct auto_string *shipstr;

	// Open the ship file 
	if ((ShipFile = fopen(filename, "wb")) == NULL) {
		error_message(__FUNCTION__, "Error opening ship file %s for writing.", NO_REPORT, filename);
		return ERR;
	}

	shipstr	= alloc_autostr(1048576);
	autostr_printf(shipstr, "\n");
	
	// Save all the levels
	for (i = 0; i < curShip.num_levels; i++) {
		if (level_exists(i)) {
			encode_level_for_saving(shipstr, curShip.AllLevels[i], reset_random_levels);
		}
	}

	autostr_append(shipstr, "%s\n\n", END_OF_SHIP_DATA_STRING);

	if (compress) { 
		deflate_to_stream((unsigned char *)shipstr->value, shipstr->length, ShipFile);
	}	else {
		if (fwrite((unsigned char *)shipstr->value, shipstr->length, 1, ShipFile) != 1) {
			error_message(__FUNCTION__, "Error writing ship file %s.", PLEASE_INFORM, filename);
			fclose(ShipFile);
			free_autostr(shipstr);
			return ERR;
		}
	}

	if (fclose(ShipFile) == EOF) {
		error_message(__FUNCTION__, "Closing of ship file failed!", PLEASE_INFORM);
		free_autostr(shipstr);
		return ERR;
	}

	free_autostr(shipstr);
	return OK;
}

int save_special_forces(const char *filename)
{
	FILE *s_forces_file = NULL;
	struct auto_string *s_forces_str;
	level *lvl;
	int i;

	if ((s_forces_file = fopen(filename, "wb")) == NULL) {
		error_message(__FUNCTION__, "Error opening Special Forces file %s for writing.", NO_REPORT, filename);
		return ERR;
	}

	s_forces_str = alloc_autostr(64);

	for (i = 0; i < curShip.num_levels; i++) {
		if (!level_exists(i))
			continue;

		lvl = curShip.AllLevels[i];
		autostr_append(s_forces_str, "** Beginning of new Level **\n");
		autostr_append(s_forces_str, "Level=%d\n\n",  lvl->levelnum);

		enemy *en;

		list_for_each_entry_reverse(en, &level_bots_head[lvl->levelnum], level_list) {
			if (!en->SpecialForce)
				continue;

			autostr_append(s_forces_str, "T=%s: ", Droidmap[en->type].droidname);
			autostr_append(s_forces_str, "PosX=%d PosY=%d ", (int)en->pos.x, (int)en->pos.y);
			autostr_append(s_forces_str, "Faction=\"%s\" ", get_faction_from_id(en->faction));
			autostr_append(s_forces_str, "UseDialog=\"%s\" ", en->dialog_section_name);

			autostr_append(s_forces_str, "ShortLabel=_\"%s\" ", en->short_description_text);
			autostr_append(s_forces_str, "Marker=%d ", en->marker);
			autostr_append(s_forces_str, "RushTux=%d ", en->will_rush_tux);

			autostr_append(s_forces_str, "Fixed=%hi ", en->CompletelyFixed);
			autostr_append(s_forces_str, "DropItemId=\"%s\" ",
						(en->on_death_drop_item_code == -1) ? "none" : ItemMap[en->on_death_drop_item_code].id);
			autostr_append(s_forces_str, "MaxDistanceToHome=%hd", en->max_distance_to_home);
			if (en->sensor_id != Droidmap[en->type].sensor_id) // Only save sensor if it's not the default.
				autostr_append(s_forces_str, " UseSensor=\"%s\"", get_sensor_name_by_id(en->sensor_id));
			autostr_append(s_forces_str, "\n");
		}

		autostr_append(s_forces_str, "** End of this levels Special Forces data **\n");
		autostr_append(s_forces_str, "---------------------------------------------------------\n");
	}

	autostr_append(s_forces_str, "*** End of Droid Data ***\n");

	if (fwrite((unsigned char *)s_forces_str->value, s_forces_str->length, 1, s_forces_file) != 1) {
		error_message(__FUNCTION__, "Error writing SpecialForces file %s.", PLEASE_INFORM, filename);
		fclose(s_forces_file);
		goto out;
	}

	if (fclose(s_forces_file) == EOF) {
		error_message(__FUNCTION__, "Closing of Special Forces file failed!", PLEASE_INFORM);
		goto out;
	}

out:	free_autostr(s_forces_str);
	return OK;
}

/**
 * This function is used to calculate the number of the droids on the 
 * ship, which is a global variable.
 */
void CountNumberOfDroidsOnShip(void)
{
	Number_Of_Droids_On_Ship = 0;

	enemy *erot;
	BROWSE_ALIVE_BOTS(erot) {
		Number_Of_Droids_On_Ship++;
	}

	BROWSE_DEAD_BOTS(erot) {
		Number_Of_Droids_On_Ship++;
	}

};				// void CountNumberOfDroidsOnShip ( void )

/* -----------------------------------------------------------------
 * This function initializes all enemies, which means that enemies are
 * filled in into the enemy list according to the enemies types that 
 * are to be found on each deck.
 * ----------------------------------------------------------------- */
int GetCrew(char *filename)
{
	char fpath[PATH_MAX];
	char *MainDroidsFilePointer;
	char *DroidSectionPointer;
	char *EndOfThisDroidSectionPointer;

#define START_OF_DROID_DATA_STRING "*** Beginning of Droid Data ***"
#define END_OF_DROID_DATA_STRING "*** End of Droid Data ***"
#define DROIDS_LEVEL_DESCRIPTION_START_STRING "** Beginning of new Level **"
#define DROIDS_LEVEL_DESCRIPTION_END_STRING "** End of this levels Special Forces data **"

	//Now its time to start decoding the droids file.
	//For that, we must get it into memory first.
	//The procedure is the same as with LoadShip
	//
	find_file(fpath, MAP_DIR, filename, NULL, PLEASE_INFORM | IS_FATAL);

	MainDroidsFilePointer = read_and_malloc_and_terminate_file(fpath, END_OF_DROID_DATA_STRING);

	// The Droid crew file for this map is now completely read into memory
	// It's now time to decode the file and to fill the array of enemys with
	// new droids of the given types.
	//
	enemy_reset_fabric();

	DroidSectionPointer = MainDroidsFilePointer;
	while ((DroidSectionPointer = strstr(DroidSectionPointer, DROIDS_LEVEL_DESCRIPTION_START_STRING)) != NULL) {
		DroidSectionPointer += strlen(DROIDS_LEVEL_DESCRIPTION_START_STRING);
		DebugPrintf(1, "\nFound another levels droids description starting point entry!");
		EndOfThisDroidSectionPointer = strstr(DroidSectionPointer, DROIDS_LEVEL_DESCRIPTION_END_STRING);
		if (EndOfThisDroidSectionPointer == NULL) {
			error_message(__FUNCTION__, "Unterminated droid section encountered!", PLEASE_INFORM | IS_FATAL);
		}
		// EndOfThisDroidSectionPointer[0]=0;
		GetThisLevelsDroids(DroidSectionPointer);
		DroidSectionPointer = EndOfThisDroidSectionPointer + 2;	// Move past the inserted String terminator
	}

	free(MainDroidsFilePointer);
	return (OK);

};				// int GetCrew ( ... ) 

/**
 *
 *
 */
static void get_this_levels_special_forces(char *search_pointer, int our_level_number, char *lvl_end_location)
{
#define SPECIAL_FORCE_INDICATION_STRING "T="

	while ((search_pointer = strstr(search_pointer, SPECIAL_FORCE_INDICATION_STRING)) != NULL) {
		char *special_droid = ReadAndMallocStringFromData(search_pointer, SPECIAL_FORCE_INDICATION_STRING, "\n");
		char *special_droid_end = special_droid + strlen(special_droid);
		search_pointer += strlen(SPECIAL_FORCE_INDICATION_STRING);
		//identify what model of droid to display:
		char *ptr = special_droid;
		int droid_type_length = 0;
		while (isalnum(*ptr)) {
			ptr++;
			droid_type_length++;
		}
		char type_indication_string[droid_type_length + 1];
		strncpy(type_indication_string, special_droid, droid_type_length);
		type_indication_string[droid_type_length] = 0;

		int droid_type = get_droid_type(type_indication_string);

		// Create a new enemy, and initialize its 'identity' and 'global state'
		// (the enemy will be fully initialized by respawn_level())


		enemy *newen = enemy_new(droid_type);


		newen->SpecialForce = TRUE;


		char *tmp_sensor_ID = ReadAndMallocStringFromDataOptional(special_droid, "UseSensor=\"","\"");
		if (!tmp_sensor_ID)
			newen->sensor_id=Droidmap[newen->type].sensor_id;	//Not declared? In this case, use default.
		else
			newen->sensor_id=get_sensor_id_by_name(tmp_sensor_ID);	// Otherwise, use the specified sensor!
		free(tmp_sensor_ID);

		ReadValueFromStringWithDefault(special_droid, "Fixed=", "%hd", "0", &(newen->CompletelyFixed), special_droid_end);
		ReadValueFromStringWithDefault(special_droid, "Marker=", "%d", "0000", &(newen->marker), special_droid_end);
		ReadValueFromStringWithDefault(special_droid, "MaxDistanceToHome=", "%hd", "0", &(newen->max_distance_to_home),
					       special_droid_end);

		char *faction = ReadAndMallocStringFromData(special_droid, "Faction=\"", "\"");
		newen->faction = get_faction_id(faction);
		free(faction);

		char *x, *y;
		x = ReadAndMallocStringFromData(special_droid, "PosX=", " ");
		y = ReadAndMallocStringFromData(special_droid, "PosY=", " ");

		newen->pos.x = strtof(x, NULL);
		newen->pos.y = strtof(y, NULL);
		newen->pos.z = our_level_number;
		free(x);
		free(y);

		ReadValueFromStringWithDefault(special_droid, "RushTux=", "%hu", "0", &(newen->will_rush_tux), special_droid_end);

		newen->dialog_section_name = ReadAndMallocStringFromData(special_droid, "UseDialog=\"", "\"");
		npc_get(newen->dialog_section_name); // Check that we have a valid dialog.

		if (newen->short_description_text)
			free(newen->short_description_text);

		newen->short_description_text = ReadAndMallocStringFromDataOptional(special_droid, "ShortLabel=_\"", "\"");
		if (!newen->short_description_text) {
			newen->short_description_text = ReadAndMallocStringFromData(special_droid, "ShortLabel=\"", "\"");
		}

		char *death_drop;
		death_drop = ReadAndMallocStringFromData(special_droid, "DropItemId=\"", "\"");
		if (strcmp(death_drop, "none")) {
			newen->on_death_drop_item_code = get_item_type_by_id(death_drop);
		} else {
			newen->on_death_drop_item_code = -1;
		}
		free(death_drop);
		free(special_droid);
		enemy_insert_into_lists(newen, TRUE);
	}

};

/**
 * This function receives a pointer to the already read in crew section
 * in a already read in droids file and decodes all the contents of that
 * droid section to fill the AllEnemys array with droid types according
 * to the specifications made in the file.
 */
void GetThisLevelsDroids(char *section_pointer)
{
	int our_level_number;
	char *search_ptr;
	char *lvl_end_location;
	int random_droids;
	int *allowed_type_list;
	level *lvl;

#define DROIDS_LEVEL_INDICATION_STRING "Level="
#define DROIDS_LEVEL_END_INDICATION_STRING "** End of this levels Special Forces data **"

	lvl_end_location = LocateStringInData(section_pointer, DROIDS_LEVEL_END_INDICATION_STRING);
	lvl_end_location[0] = 0;

	// Now we read in the level number for this level
	ReadValueFromString(section_pointer, DROIDS_LEVEL_INDICATION_STRING, "%d", &our_level_number, lvl_end_location);

	lvl = curShip.AllLevels[our_level_number];

	// At this point, the List "allowed_type_list" has been filled with the NUMBERS of
	// the allowed types.  The number of different allowed types found is also available.
	// That means that now we can add the appropriate droid types into the list of existing
	// droids in that mission.

	random_droids = lvl->random_droids.nr;
	allowed_type_list = lvl->random_droids.types;

	while (random_droids--) {
		// Create a new enemy, and initialize its 'identity' and 'global state'
		// (the enemy will be fully initialized by respawn_level())
		enemy *newen = enemy_new(allowed_type_list[MyRandom(lvl->random_droids.types_size - 1)]);
		newen->pos.x = newen->pos.y = -1;
		newen->pos.z = our_level_number;
		newen->on_death_drop_item_code = -1;
		newen->dialog_section_name = strdup("AfterTakeover");
		newen->faction = FACTION_BOTS;

		enemy_insert_into_lists(newen, TRUE);
	}			// while (enemy-limit of this level not reached) 

	search_ptr = section_pointer;
	get_this_levels_special_forces(search_ptr, our_level_number, lvl_end_location);

	// End bot's initialization, and put them onto a waypoint.
	respawn_level(our_level_number);
};

/**
 * This function determines whether a given object on x/y is visible to
 * the 001 or not (due to some walls or something in between
 * 
 * Return values are TRUE or FALSE accordingly
 *
 */
int IsVisible(gps *objpos)
{

	// For the purpose of visibility checking, we might as well exclude objects
	// that are too far away to ever be visible and thereby save some checks of
	// longer lines on the map, that wouldn't be necessary or helpful anyway.
	//
	if ((fabsf(Me.pos.x - objpos->x) > FLOOR_TILES_VISIBLE_AROUND_TUX) ||
	    (fabsf(Me.pos.y - objpos->y) > FLOOR_TILES_VISIBLE_AROUND_TUX))
		return (FALSE);

	// So if the object in question is close enough to be visible, we'll do the
	// actual check and see if the line of sight is free or blocked, a rather
	// time-consuming and often re-iterated process.  (Maybe some do-it-every-
	// -10th-frame-only code could be added here later... and in the meantime
	// old values could be used from a stored flag?!
	//
	return (DirectLineColldet(objpos->x, objpos->y, Me.pos.x, Me.pos.y, objpos->z, &VisiblePassFilter));

};				// int IsVisible( Point objpos )

/**
 *
 *
 */
inline float translate_pixel_to_map_location(float axis_x, float axis_y, int give_x)
{

	// NOTE:  This function does not expect absolute screen coordinates but rather coordinates relative
	// to the center of the screen.
	//
	// That's also why it's 'axis' rather than 'pos' or 'point'.
	//
	// That is because mouse clicks can best be analyzed this way.
	//

	if (give_x) {
		return (Me.pos.x + (axis_x / ((float)FLOOR_TILE_WIDTH)) + (axis_y / ((float)FLOOR_TILE_HEIGHT)));
	} else {
		return (Me.pos.y - (axis_x / ((float)FLOOR_TILE_WIDTH)) + (axis_y / ((float)FLOOR_TILE_HEIGHT)));
	}

};				// int translate_pixel_to_map_location ( int axis_x , int axis_y , int give_x ) 

/**
 *
 *
 */
float translate_pixel_to_zoomed_map_location(float axis_x, float axis_y, int give_x)
{
	float zf = lvledit_zoomfact();
	if (give_x) {
		return (Me.pos.x + (zf * axis_x / ((float)FLOOR_TILE_WIDTH)) + (zf * axis_y / ((float)FLOOR_TILE_HEIGHT)));
		// return ( ( axis_x / ISO_WIDTH ) + ( axis_y / ISO_HEIGHT ) ) ;
	} else {
		return (Me.pos.y - (zf * axis_x / ((float)FLOOR_TILE_WIDTH)) + (zf * axis_y / ((float)FLOOR_TILE_HEIGHT)));
		// return ( - ( axis_x / ISO_WIDTH ) + ( axis_y / ISO_HEIGHT ) ) ;
	}

};				// int translate_pixel_to_zoomed_map_location ( int axis_x , int axis_y , int give_x ) 

/**
 *
 *
 */
pointf translate_point_to_map_location(float axis_x, float axis_y, int zoom_is_on)
{
	pointf position;
	if (zoom_is_on) {
		position.x = translate_pixel_to_zoomed_map_location(axis_x, axis_y, TRUE);
		position.y = translate_pixel_to_zoomed_map_location(axis_x, axis_y, FALSE);
	} else {
		position.x = translate_pixel_to_map_location(axis_x, axis_y, TRUE);
		position.y = translate_pixel_to_map_location(axis_x, axis_y, FALSE);
	}
	return position;
}

int map_is_zoomed_out(void)
{
	/* XXX should not check for leveleditor here! */
	return game_status == INSIDE_LVLEDITOR && GameConfig.zoom_is_on;
}

/**
 * This function translates a given map point to screen coordinates.
 *
 * @param x_map_pos X position on map
 * @param y_map_pos Y position on map
 * @param x_res	pointer to the int that will hold the x position on screen
 * @param y_res pointer to the y position on screen
 * @param zoom_factor zoom factor in use
 * 
 */
void translate_map_point_to_screen_pixel_func(float X, float Y, int *x_res, int *y_res)
{
	float zoom_factor = 1.0;

#define R (int)
#define factX FLOOR_TILE_WIDTH*0.5*zoom_factor
#define factY FLOOR_TILE_HEIGHT*0.5*zoom_factor
	if (map_is_zoomed_out()) {
		zoom_factor = lvledit_zoomfact_inv();
	}

	/* Translating from the map coordinate system into the screen coordinate
	   system is not easy to do correctly, because the screen coordinate system
	   is discrete (integer coordinates).  The result is that careless
	   translation can lead to elements that are fixed on the map (such as
	   obstacles) to "jitter" when the player is moving, especially when movement
	   happens along the screen axes.  This is why "rounding" is needed at an
	   intermediary step.

	   Translating a vector on the map to a vector on screen is as follows: x =
	   (X - Y) * FLOOR_TILE_WIDTH y = (X + Y) * FLOOR_TILE_HEIGHT

	   To translate a point, one has to take into account the "base"
	   coordinate: the screen's origin is centered on the player (Me.pos) and depends
	   on the interface screens currently opened (UserCenter).  The player coordinate
	   is rounded at an intermediary step to avoid jitter when the player moves.
	  */
	*x_res = UserCenter_x + factX * (X - Y) - R(factX * (Me.pos.x - Me.pos.y));
	*y_res = UserCenter_y + factY * (X + Y) - R(factY * (Me.pos.x + Me.pos.y));
#undef R
#undef factX
#undef factY
}

#undef _map_c
