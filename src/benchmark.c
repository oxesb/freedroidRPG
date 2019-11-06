/* 
 *
 *   Copyright (c) 2009-2010 Arthur Huillet 
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

#define _benchmark_c 1

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "lvledit/lvledit_validator.h"
#include "lvledit/lvledit_display.h"

static int start_stamp;
static int stop_stamp;

static void timer_start()
{
	start_stamp = SDL_GetTicks();
}

static void timer_stop()
{
	stop_stamp = SDL_GetTicks();
}

/* Text rendering performance measurement */
static int text_bench()
{
    char *str = "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz";

	// Make sure all glyphs are loaded
	put_string(get_current_font(), 0, 0, str);

	// Display the string many times	
	int nb = 10000;

	timer_start();
	while (nb--) {
		put_string(get_current_font(), 0, 0, str);
	}
		
	our_SDL_flip_wrapper();
	timer_stop();

	return 0;
}

/* Dialog validator (not an actual benchmark) */
static int dialog_test()
{
	int failed;

	timer_start();
	failed = validate_dialogs();
	timer_stop();

	return failed;
}

/* Event's lua code validator (not an actual benchmark) */
static int event_test()
{
	int failed;

	timer_start();
	failed = validate_events();
	timer_stop();

	return failed;
}

/* LoadShip (level loading) performance test */
static int loadship_bench()
{
	int failed = TRUE;

	// Find a ship file to load
	char fp[PATH_MAX];
	if (find_file(fp, MAP_DIR, "levels.dat", NULL, NO_REPORT)) {
		// Load it many times
		timer_start();
		int loop = 10;
		while (loop--) {
			LoadShip(fp, 0);
		}
		timer_stop();
		failed = FALSE;
	}

	return failed;
}

/* LoadGame (savegame loading) performance test
 *
 * To measure game loading only, loaded data are not cleared between
 * 2 calls. So expect some memory leaks !
 */
static int loadgame_bench()
{
	int loop = 3;

	// Use MapEd.savegame
	free(Me.character_name);
	Me.character_name = strdup("MapEd");

	// Load it many times
	timer_start();
	while (loop--) {
		if (load_game() == ERR) {
			timer_stop();
			return ERR;
		}
	}
	timer_stop();

	return 0;
}

/* SaveGame (savegame writing) performance test */
static int savegame_bench()
{
	int loop = 10;

	// Use MapEd.savegame
	free(Me.character_name);
	Me.character_name = strdup("MapEd");

	// Load it
	if (load_game() == ERR) {
		error_message(__FUNCTION__, "Whoops, that failed. Maybe you have to save a game under the name \"MapEd\" to make this work?",
				NO_REPORT);
		return -1;
	}
	
	// Write it many times.
	timer_start();
	while (loop--) {
		save_game();
	}
	timer_stop();

	return 0;
}

/* Test of dynamic arrays */
static int dynarray_test()
{
	int loop = 5;
	item dummy;
	memset(&dummy, 0, sizeof(item));
	dummy.pos.x = 1.0;
	dummy.type = 52;

	timer_start();
	while (loop--) {
		int i;
		struct dynarray *dyn = dynarray_alloc(100, sizeof(item));
		for (i = 0; i < 1000000; i++) {
			dynarray_add(dyn, &dummy, sizeof(item));
		}
		for (i = 0; i < 1000000; i++) {
			item *it = dyn->arr;
			if (memcmp(&dummy, it, sizeof(item))) {
				fprintf(stderr, "Error reading out item %d\n", i);
			}
		}
		dynarray_free(dyn);
		free(dyn);
		dyn = NULL;
	}

	timer_stop();

	return 0;
}

static int mapgen_bench()
{
	int loop = 100;
	extern void CreateNewMapLevel(int);
	extern void delete_map_level(int);
	timer_start();
	while (loop--) {
		CreateNewMapLevel(0);
		curShip.num_levels = 1;
		level *l = curShip.AllLevels[0];
		l->xlen = 90;
		l->ylen = 90;
		l->random_dungeon = 2;
		l->teleport_pair = 0;
		set_dungeon_output(l);
		generate_dungeon(l->xlen, l->ylen, l->random_dungeon, l->teleport_pair);
		delete_map_level(0);
	}
	timer_stop();

	return 0;
}

/* Levels validator (not an actual benchmark) */
static int level_test()
{
	int failed = FALSE;
	timer_start();

	for (int i = 0; i < game_acts.size; i++) {
		struct game_act *act = (struct game_act *)dynarray_member(&game_acts, i, sizeof(struct game_act));
		game_act_set_current(act);

		// Load default ship
		char fp[PATH_MAX];
		if (find_file(fp, MAP_DIR, "levels.dat", NULL, NO_REPORT)) {
			LoadShip(fp, 0);
			failed |= level_validation_on_console_only(act->name);
		} else {
			failed = TRUE;
		}

		// Prepare the next round
		free_game_data();
	}

	timer_stop();
	return failed;
}

/**
 * Create a map of the levels
 */
#include "lvledit/lvledit_display.h"
static int map_graph()
{
	timer_start();

	// Open output file
	struct auto_string *str = alloc_autostr(128);
	FILE *out = fopen("/tmp/freedroid_level_graph.graph", "w");
	if (!out)
		return 1;
	
	// Load ship
	char fp[2048];
	find_file(fp, MAP_DIR, "levels.dat", NULL, NO_REPORT);
	LoadShip(fp, 0);

	// For each level, dump its connections
	autostr_printf(str, "strict graph level_map {\nlayout=neato\nstart=random\n");
	int i;
	for (i = 0; i <= curShip.num_levels; i++) {
		level *l = curShip.AllLevels[i];
		if (!l) {
			continue;
		}

		// Save level connections
		autostr_append(str, "%d [href=\"freedroid_level_%02d.png\"];\n%d  -- {", i, i, i);

		int links[4] = { l->jump_target_north, l->jump_target_south, l->jump_target_east, l->jump_target_west };
		int j;
		for (j = 0; j < 4; j++) {
			if (links[j] != -1) {
				autostr_append(str, "%d ", links[j]);
			}
		}

	   	autostr_append(str, " }\n");
		
		// Take level screenshot
		teleport_to_level_center(i);
		lvledit_set_zoomfact(9.0);
		GameConfig.zoom_is_on = TRUE;
		game_status = INSIDE_LVLEDITOR;
		AssembleCombatPicture(ONLY_SHOW_MAP_AND_TEXT | SHOW_ITEMS | OMIT_TUX | OMIT_ENEMIES | ZOOM_OUT | OMIT_BLASTS | SKIP_LIGHT_RADIUS | NO_CURSOR | OMIT_ITEMS_LABEL);

		char fname[50];
		sprintf(fname, "/tmp/freedroid_level_%02d.png", i);
		save_screenshot(fname, 0);
	}

	autostr_append(str, "}\n");

	// Write output file	
	fprintf(out, "%s", str->value);
	free_autostr(str);
	fclose(out);

	timer_stop();
	return 0;
}

static int graphics_bench()
{
	// Load ship
	char fp[PATH_MAX];
	find_file(fp, MAP_DIR, "levels.dat", NULL, NO_REPORT);
	LoadShip(fp, 0);

	// Prepare leveleditor zoomed-out benchmark
	teleport_to_level_center(0);
	lvledit_set_zoomfact(9.0);
	GameConfig.zoom_is_on = TRUE;
	game_status = INSIDE_LVLEDITOR;

	AssembleCombatPicture(ONLY_SHOW_MAP_AND_TEXT | SHOW_ITEMS | OMIT_TUX | OMIT_ENEMIES | ZOOM_OUT | OMIT_BLASTS | SKIP_LIGHT_RADIUS | NO_CURSOR | OMIT_ITEMS_LABEL);
	our_SDL_flip_wrapper();
	timer_start();
	for (int i = 0; i < 1000; i++) {
		AssembleCombatPicture(ONLY_SHOW_MAP_AND_TEXT | SHOW_ITEMS | OMIT_TUX | OMIT_ENEMIES | ZOOM_OUT | OMIT_BLASTS | SKIP_LIGHT_RADIUS | NO_CURSOR | OMIT_ITEMS_LABEL);
		our_SDL_flip_wrapper();
	}
	timer_stop();

	return 0;
}

static int graphicsloading_bench()
{
	int iter = 5;

	timer_start();
	while (iter--) {
		reload_graphics();
	}

	our_SDL_flip_wrapper();
	timer_stop();

	return 0;
}

int benchmark()
{
	struct {
		char *name;
		int (*func)();
	} benchs[] = {
			{ "text",            text_bench },
			{ "dialog",          dialog_test },
			{ "event",           event_test },
			{ "loadship",        loadship_bench },
			{ "loadgame",        loadgame_bench },
			{ "savegame",        savegame_bench },
			{ "dynarray",        dynarray_test },
			{ "mapgen",          mapgen_bench },
			{ "leveltest",       level_test },
			{ "graphics",        graphics_bench },
			{ "graphicsloading", graphicsloading_bench },
			{ "mapgraph",        map_graph },
	};

	int i;
	char str[1024];

	clear_screen();
	sprintf(str, "Testing \"%s\"...", do_benchmark);
	put_string(get_current_font(), 10, 100, str);
	our_SDL_flip_wrapper();

	for (i = 0; i < sizeof(benchs)/sizeof(benchs[0]); i++) {
		if (!strcmp(do_benchmark, benchs[i].name)) {
			if (benchs[i].func() == 0) {
				if (benchs[i].func == graphics_bench) {
					printf("Running test %s took %d milliseconds, that is, %f FPS on average.\n", do_benchmark, stop_stamp - start_stamp, 1000 * 1000.0 / (stop_stamp - start_stamp));
					printf("Make sure that vsync was disabled, to get meaningful results\n(vblank_mode=0 on Mesa, __GL_SYNC_TO_VBLANK=0 on NVIDIA, for instance).\n");
				} else {
					printf("Running test %s took %d milliseconds.\n", do_benchmark, stop_stamp - start_stamp);
				}
				return OK;
			} else {
				if (term_has_color_cap)
					printf("\033[31mTest failed!\033[0m\n");
				else
					printf("Test failed!\n");
				return ERR;
			}
		}
	}
	
	fprintf(stderr, "Unrecognized test %s. Existing tests are:\n", do_benchmark);
	for (i = 0; i < sizeof(benchs)/sizeof(benchs[0]); i++) {
		fprintf(stderr, "\t%s\n", benchs[i].name);
	}
	exit(EXIT_FAILURE);
}


#undef _benchmark_c
