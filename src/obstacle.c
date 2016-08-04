/**
 *
 *  Copyright (c) 2011 Samuel Pitoiset
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

#define _obstacle_c 1

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"

/**
 * This files contains obstacles related functions.
 */

static void obstacle_boundaries(obstacle *o, int *x_min, int *x_max, int *y_min, int *y_max)
{
	obstacle_spec *spec = get_obstacle_spec(o->type);

	*x_min = floor(o->pos.x + spec->left_border);
	*x_max = floor(o->pos.x + spec->right_border);
	*y_min = floor(o->pos.y + spec->upper_border);
	*y_max = floor(o->pos.y + spec->lower_border);
}

void glue_obstacle(level *lvl, obstacle *o)
{
	int x_min, x_max, x;
	int y_min, y_max, y;
	int idx;

	if (o->type == -1)
		return;

	obstacle_boundaries(o, &x_min, &x_max, &y_min, &y_max);

	idx = get_obstacle_index(lvl, o);

	for (x = x_min; x <= x_max; x++) {
		for (y = y_min; y <= y_max; y++) {
			if (x < 0 || y < 0 || x >= lvl->xlen || y >= lvl->ylen)
				continue;

			dynarray_add(&lvl->map[y][x].glued_obstacles, &idx, sizeof(idx));
		}
	}
}

void unglue_obstacle(level *lvl, obstacle *o)
{
	int x_min, x_max, x;
	int y_min, y_max, y;
	int idx, i;

	if (o->type == -1)
		return;

	obstacle_boundaries(o, &x_min, &x_max, &y_min, &y_max);

	idx = get_obstacle_index(lvl, o);

	for (x = x_min; x <= x_max; x++) {
		for (y = y_min; y <= y_max; y++) {
			if (x < 0 || y < 0 || x >= lvl->xlen || y >= lvl->ylen)
				continue;

			int *glued_obstacles = lvl->map[y][x].glued_obstacles.arr;

			for (i = 0; i < lvl->map[y][x].glued_obstacles.size; i++) {
				if (glued_obstacles[i] == idx) {
					dynarray_del(&lvl->map[y][x].glued_obstacles, i, sizeof(int));
					break;
				}
			}
		}
	}
}

/**
 * \brief Add a new obstacle and glue it on a map, even on invalid position
 *
 * \details Used when loading a map, this function adds the obstacle even if
 * the position is invalid. The loading function is responsible for checking
 * the validity of the obstacle.
 *
 * \param lvl    Pointer to the level where to load the obstacle
 * \param x      X position
 * \param y      Y position
 * \param type   Obstacle's type
 *
 * \return Pointer to the added obstacle or NULL on error
 */
struct obstacle *add_obstacle_nocheck(struct level *lvl, float x, float y, int type)
{
	int i;

	for (i = 0; i < MAX_OBSTACLES_ON_MAP; i++) {
		if (lvl->obstacle_list[i].type != -1)
			continue;

		lvl->obstacle_list[i].pos.x = x;
		lvl->obstacle_list[i].pos.y = y;
		lvl->obstacle_list[i].pos.z = lvl->levelnum;
		lvl->obstacle_list[i].type = type;
		lvl->obstacle_list[i].timestamp = 0;

		glue_obstacle(lvl, &lvl->obstacle_list[i]);

		return &lvl->obstacle_list[i];
	}

	error_message(__FUNCTION__,
			"Ran out of obstacle positions (%d) in level %d !",
			PLEASE_INFORM | IS_FATAL, MAX_OBSTACLES_ON_MAP, lvl->levelnum);

	return NULL;
}

/**
 * \brief Add a new obstacle and glue it on a map.
 *
 * \details This function checks the position of the obstacle to be added.
 * If the position is invalid, a warning message is displayed, and the obstacle
 * is not added.
 *
 * \param lvl    Pointer to the level where to add the obstacle
 * \param x      X position
 * \param y      Y position
 * \param type   Obstacle's type
 *
 * \return Pointer to the added obstacle or NULL on invalid position or error
 */
struct obstacle *add_obstacle(struct level *lvl, float x, float y, int type)
{
	// Pre-condition - check if the obstacle's position is valid
	if (!pos_inside_level(x, y, lvl)) {
		error_message(__FUNCTION__,
			"Invalid obstacle (%s) position on level %d: t%d x%3.2f y%3.2f",
			PLEASE_INFORM, ((char **)get_obstacle_spec(type)->filenames.arr)[0], lvl->levelnum, type, x, y);
		if (game_root_mode == ROOT_IS_LVLEDIT && game_status == INSIDE_LVLEDITOR) {
			alert_once_window(ONCE_PER_GAME,
				_("-- WARNING --\n"
				  "An obstacle with invalid coords was tried to be added to a map.\n"
				  "See the report in your terminal console."));
		}
		return NULL;
	}

	return add_obstacle_nocheck(lvl, x, y, type);
}

/**
 * \brief Remove an obstacle with its extensions and unglue it from the map.
 */
void del_obstacle(obstacle *o)
{
	level *lvl = curShip.AllLevels[o->pos.z];

	unglue_obstacle(lvl, o);

	o->type = -1;

	del_obstacle_extensions(lvl, o);
}

/**
 * \brief Move an obstacle at the given position on its level.
 */
void move_obstacle(obstacle *o, float newx, float newy)
{
	level *lvl = curShip.AllLevels[o->pos.z];

	// Do not move an obstacle to an invalid position
	if (!pos_inside_level(newx, newy, lvl)) {
		error_message(__FUNCTION__,
				"Invalid obstacle new position (%f, %f) on level %d - Old position: (%f, %f)",
				NO_REPORT, newx, newy, o->pos.z, o->pos.x, o->pos.y);
		if (game_root_mode == ROOT_IS_LVLEDIT && game_status == INSIDE_LVLEDITOR) {
			alert_once_window(ONCE_PER_GAME,
					_("An obstacle was tried to be moved to an invalid position.\n"
					  "Not moving it.\n"
					  "See the report in your terminal console."));
		}
		return;
	}

	unglue_obstacle(lvl, o);

	o->pos.x = newx;
	o->pos.y = newy;

	glue_obstacle(lvl, o);
}

obstacle_spec *get_obstacle_spec(int index)
{
	return dynarray_member(&obstacle_map, index, sizeof(obstacle_spec));
}

struct obstacle_group *get_obstacle_group_by_name(const char *group_name)
{
	int i;
	struct obstacle_group *group = obstacle_groups.arr;

	for (i = 0; i < obstacle_groups.size; i++) {
		if (!strcmp(group[i].name, group_name))
			return &group[i];
	}

	return NULL;
}

struct obstacle_group *find_obstacle_group(int type)
{
	int i, j;
	struct obstacle_group *group = obstacle_groups.arr;

	for (i = 0; i < obstacle_groups.size;i ++) {
		for (j = 0; j < group[i].members.size; j++) {
			if (((int *)group[i].members.arr)[j] == type)
				return &group[i];
		}
	}

	return NULL;
}

void add_obstacle_to_group(const char *group_name, int type)
{
	struct obstacle_group new_group;
	struct obstacle_group *group = get_obstacle_group_by_name(group_name);

	// Create group if it doesn't exist
	if (!group) {
		new_group.name = strdup(group_name);
		dynarray_init(&new_group.members, 8, sizeof(int));
		dynarray_add(&obstacle_groups, &new_group, sizeof(struct obstacle_group));

		group = dynarray_member(&obstacle_groups, obstacle_groups.size - 1, sizeof(struct obstacle_group));
	}

	dynarray_add(&group->members, &type, sizeof(int));
}

int change_obstacle_type(const char *obslabel, int type)
{
	int obstacle_level_num;
	obstacle *our_obstacle = give_pointer_to_obstacle_with_label(obslabel, &obstacle_level_num);
	level *obstacle_level = curShip.AllLevels[obstacle_level_num];

	// tell the automap to update
	update_obstacle_automap(obstacle_level->levelnum, our_obstacle);
	
	if (type != -1) {
		// Unglue and reglue the obstacle, to take into account any collrect change
		unglue_obstacle(obstacle_level, our_obstacle);
		our_obstacle->type = type;
		our_obstacle->frame_index = 0;
		glue_obstacle(obstacle_level, our_obstacle);
	} else {
		del_obstacle(our_obstacle);
	}

	// Now we make sure the door lists and that are all updated...
	dirty_animated_obstacle_list(obstacle_level->levelnum);

	return 0;
}

/**
 * \brief Get the type of the obstacle spec with the specified name.
 * \param The name of the obstacle spec to get.
 * \return The obstacle type or -1 if the obstacle spec don't exist.
 */
int get_obstacle_type_by_name(char *name)
{
	int id = 0;

	for (id = 0; id < obstacle_map.size; id++) {
		char *obstacle_spec_name = get_obstacle_spec(id)->name;
		
		if (obstacle_spec_name == NULL)
			continue;
		
		if (!strcmp(obstacle_spec_name, name))
			return id;
	}

	error_message(__FUNCTION__, "Unable to find the obstacle specs with name \"%s\"", PLEASE_INFORM, name);
	return -1;
}

//===============================================================================================

struct volatile_obstacle *add_volatile_obstacle(struct level *lvl, float x, float y, int type, float timeout)
{
	struct obstacle_spec *obs_spec = get_obstacle_spec(type);

	if (!(obs_spec->flags & IS_VOLATILE)) {
		error_message(__FUNCTION__, "Can not add the obstacle: it is not marked as IS_VOLATILE (obs type: %d).",
				PLEASE_INFORM, type);
		return NULL;
	}

	if (!pos_inside_level(x, y, lvl)) {
		error_message(__FUNCTION__, "Can not add the obstacle: it is outside the level boundary (type: %d - gps: %f/%f/%d).",
				PLEASE_INFORM, type, x, y, lvl->levelnum);
		return NULL;
	}

	struct volatile_obstacle *volatile_obs = (struct volatile_obstacle *)MyMalloc(sizeof(struct volatile_obstacle));

	volatile_obs->obstacle.pos.x = x;
	volatile_obs->obstacle.pos.y = y;
	volatile_obs->obstacle.pos.z = lvl->levelnum;
	volatile_obs->obstacle.type = type;
	volatile_obs->vanish_timeout = timeout;

	int cell_x = (int)floorf(x);
	int cell_y = (int)floorf(y);

	list_add(&(volatile_obs->volatile_list), &(lvl->map[cell_y][cell_x].volatile_obstacles));

	return volatile_obs;
}

/**
 * This function removes all volatile obstacles from a given level.
 * An example of a volatile obstacle is the blood.
 * If the blood doesn't vanish, then there will be more and more blood,
 * especially after the bots on the level have respawned a few times.
 * Therefore we need this function, which will remove all traces of blood
 * from a given level.
 */
void remove_volatile_obstacles(int lvl_num)
{
	struct level *lvl = curShip.AllLevels[lvl_num];
	int x, y;
	for (y = 0; y < lvl->ylen; y++) {
		for (x = 0; x < lvl->xlen; x++) {
			struct volatile_obstacle *volatile_obs, *next;
			list_for_each_entry_safe(volatile_obs, next, &lvl->map[y][x].volatile_obstacles, volatile_list) {
				free(volatile_obs);
			}
			INIT_LIST_HEAD(&lvl->map[y][x].volatile_obstacles);
		}
	}
}

void clear_volatile_obstacles(void)
{
	int i;
	for (i = 0; i < MAX_LEVELS; i++) {
		if (level_exists(i)) {
			remove_volatile_obstacles(i);
		}
	}
}


#undef _obstacle_c
