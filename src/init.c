/* 
 *
 *   Copyright (c) 2004-2010 Arthur Huillet
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
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
 * (Not all of the) Initialization routines for FreedroidRPG.
 */

#define _init_c 1

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "widgets/widgets.h"

#include "getopt.h"

#ifdef __WIN32__
// For _mkdir()
#include <direct.h>
// For SHGetSpecialFolderPath()
#ifndef _WIN32_IE
// From MSDN: The Microsoft Internet Explorer 4.0 Desktop Update must be installed for this function to be available.
#define _WIN32_IE 0x0400
#endif
#include <shlobj.h>
#endif

#ifdef ENABLE_NLS
#include <iconv.h>
#endif

void init_game_data(void);
void UpdateCountersForThisFrame();
void DoAllMovementAndAnimations(void);

struct dynarray difficulties;

int term_has_color_cap = FALSE;
int run_from_term = FALSE;

/**
 *
 *
 */
void clear_out_arrays_for_fresh_game(void)
{
	dynarray_free(&all_melee_shots);

	dynarray_free(&all_bullets);

	dynarray_free(&all_blasts);

	clear_active_spells();

	clear_enemies();
	clear_volatile_obstacles();
	ClearAutomapData();

	clear_npcs();

	init_tux();
}

/** 
 * This function displays a startup status bar that shows a certain
 * percentage of loading done.
 */
void next_startup_percentage(int done)
{
	static int startup_percent = 0;
	SDL_Rect Bar_Rect;

	/* Define STARTUP_PERCENTAGE_COMPUTE to get an estimate
	   of the real percentage associated to each step during the
	   game startup.
	   */
#ifdef STARTUP_PERCENTAGE_COMPUTE
	static long load_start_time;

	struct load_step {
		long elapsed;
		int percent;
	};

	static struct dynarray *step_times = NULL;
	if (!step_times)
		step_times = dynarray_alloc(10, sizeof(struct load_step));

	if (!startup_percent)
		load_start_time = SDL_GetTicks();
#endif

	startup_percent += done;
	if (startup_percent > 100)
		startup_percent = 100;

#ifdef STARTUP_PERCENTAGE_COMPUTE
	long elapsed = SDL_GetTicks() - load_start_time;

	printf("Step %d: %ld ms have elapsed (%d%%), now at %d%%\n", step_times->size, elapsed, done, startup_percent);

	struct load_step step = { elapsed, done };
	dynarray_add(step_times, &step, sizeof(struct load_step));

	if (startup_percent >= 100) {
		float ms_to_percent = 100.0 / elapsed;
		printf("Loading took %ld ms, 1 ms = %f%%\n", elapsed, ms_to_percent);

		int i;
		for (i = 1; i < step_times->size; i++) {
#define STEP(X) ((struct load_step *)step_times->arr)[X]
			int step_time = STEP(i).elapsed - STEP(i-1).elapsed;
			int step_percent = step_time * ms_to_percent;
			printf("Step %d: %d ms -> %d%%\n", i, step_time, step_percent);
		}
	}
#endif

	if (use_open_gl)
		blit_background("startup1.jpg");

	Bar_Rect.x = 160 * GameConfig.screen_width / 640;
	Bar_Rect.y = 288 * GameConfig.screen_height / 480;
	Bar_Rect.w = 3 * startup_percent * GameConfig.screen_width / 640;
	Bar_Rect.h = 30 * GameConfig.screen_height / 480;
	draw_rectangle(&Bar_Rect, 150, 200, 225, 255);

	Bar_Rect.x = (160 + 3 * startup_percent) * GameConfig.screen_width / 640;
	Bar_Rect.y = 288 * GameConfig.screen_height / 480;
	Bar_Rect.w = (300 - 3 * startup_percent) * GameConfig.screen_width / 640;
	Bar_Rect.h = 30 * GameConfig.screen_height / 480;
	draw_rectangle(&Bar_Rect, 0, 0, 0, 255);

	SDL_SetClipRect(Screen, NULL);

	set_current_font(Blue_Font);
	char percent[10];
	sprintf(percent, "%d%%", startup_percent);
	display_text(percent, UNIVERSAL_COORD_W(310) - 9, UNIVERSAL_COORD_H(301) - 7, NULL, 1.0);

	our_SDL_update_rect_wrapper(Screen, 0, 0, Screen->w, Screen->h);

}

/**
 * This function can be used to play a generic title file, containing 
 * 
 *  1. a background picture name
 *  2. a background music to play
 *  3. some text to display in a scrolling fashion
 *  4. a voice reading the text (scrolling is then synchronized to the duration of the voice sound)
 */
void play_title_file(int subdir_handle, char *filename)
{
	struct title_screen screen = { NULL };
	char fpath[PATH_MAX];

	while (SpacePressed() || MouseLeftPressed()) ;

	// Load the translated version of the text of the title's screen

	if (find_localized_file(fpath, subdir_handle, filename, PLEASE_INFORM)) {
		set_lua_ctor_upvalue(LUA_CONFIG, "title_screen", &screen);
		run_lua_file(LUA_CONFIG, fpath);

#ifdef ENABLE_NLS
		// The title_screen's text in the Lua file is UTF-8 encoded.
		// We need to convert it to the currently selected charset encoding.
		struct auto_string *tocode = alloc_autostr(64);
		autostr_printf(tocode, "%s//TRANSLIT", lang_get_encoding());
		iconv_t converter = iconv_open(tocode->value, "UTF-8");
		free_autostr(tocode);
		if (converter == (iconv_t)-1) {
			error_once_message(ONCE_PER_GAME, __FUNCTION__,
			                   "Error on iconv_open() (encoding: %s): %s",
			                   NO_REPORT, lang_get_encoding(), strerror(errno));
		} else {
#if __FreeBSD__
			// FreeBSD changed to its own iconv starting with 10-CURRENT
			// Future ref: check for OS version < 10-CURRENT would allow non-const
			const char *in_ptr = screen.text;
#else
			char *in_ptr = screen.text;
#endif	//__FreeBSD__
			size_t in_len = strlen(screen.text);
			// We currently only have 8 bits encodings, so we should not need
			// an output buffer larger than the input one.
			char *converted_text = MyMalloc(in_len+1);
			char *out_ptr = converted_text;
			size_t out_len = in_len;

			size_t nb = iconv(converter, &in_ptr, &in_len, &out_ptr, &out_len);

			// In case of error, use the un-converted text
			if (nb == (size_t)-1) {
				if (errno == EILSEQ || errno == EINVAL) {
					error_once_message(ONCE_PER_GAME, __FUNCTION__,
				                       "Error during Title text conversion (title: %s - encoding: %s): %s\n"
					                   "Invalid sequence:\n--->%.20s...<---",
									   PLEASE_INFORM, filename, lang_get_encoding(), strerror(errno), in_ptr);
				} else {
					error_once_message(ONCE_PER_GAME, __FUNCTION__,
				                       "Error during Title text conversion (title: %s - encoding: %s): %s",
									   NO_REPORT, filename, lang_get_encoding(), strerror(errno));
				}
				free(converted_text);
			} else {
				// Replace the title's text by the converted one
				free(screen.text);
				screen.text = converted_text;
			}
		}
		iconv_close(converter);
#endif

		// Remove trailing whitespaces and carriage returns from the text
		char *ptr = screen.text + strlen(screen.text) - 1;
		while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n')) *(ptr--) = '\0';

		// Get the full path of the voice file (possibly localized), if one is
		// defined
		if (screen.voice_acting) {
			if (find_localized_file(fpath, subdir_handle, screen.voice_acting, NO_REPORT)) {
				free(screen.voice_acting);
				screen.voice_acting = my_strdup(fpath);
			} else {
				free(screen.voice_acting);
				screen.voice_acting = NULL;
			}
		}

		// Create title_screen widget, set its content and display it
		SDL_SetClipRect(Screen, NULL);
		title_screen_create();
		title_screen_run(&screen);
		title_screen_free();

		free(screen.background);
		free(screen.song);
		free(screen.text);
	}
}

/**
 * Load the configuration of the fdrpg "engine", that is the game independent data
 */
static void load_fdrpg_config()
{
	char fpath[PATH_MAX];

	// Load the languages specs
	dynarray_free(&lang_specs);
	dynarray_free(&lang_codesets);
	if (find_file(fpath, BASE_DIR, "languages.lua", NULL, PLEASE_INFORM)) {
		run_lua_file(LUA_CONFIG, fpath);
	}
}

/**
 * This function loads all the constant variables of the game from
 * a data file, using mainly subroutines which do the main work.
 */
void init_game_data()
{
	char fpath[PATH_MAX];

	// Load difficulties.
	find_file(fpath, BASE_DIR, "difficulties.lua", NULL, PLEASE_INFORM | IS_FATAL);
	run_lua_file(LUA_CONFIG, fpath);

	// Load skills and programs (spells) information
	find_file(fpath, BASE_DIR, "skill_specs.lua", NULL, PLEASE_INFORM | IS_FATAL);
	run_lua_file(LUA_CONFIG, fpath);

	// Load the blast data (required for the bullets to load)
	find_file(fpath, BASE_DIR, "blast_specs.lua", NULL, PLEASE_INFORM | IS_FATAL);
	run_lua_file(LUA_CONFIG, fpath);

	// Load the bullet data (required for the item archetypes to load)
	//
	dynarray_free(&bullet_specs);
	find_file(fpath, BASE_DIR, "bullet_specs.lua", NULL, PLEASE_INFORM | IS_FATAL);
	run_lua_file(LUA_CONFIG, fpath);

	// Load Tux animation and rendering specifications.
	tux_rendering_load_specs("tuxrender_specs.lua");

	// Item archetypes must be loaded too
	find_file(fpath, BASE_DIR, "item_specs.lua", NULL, PLEASE_INFORM | IS_FATAL);
	run_lua_file(LUA_CONFIG, fpath);

	// Load add-on specifications.
	find_file(fpath, BASE_DIR, "addon_specs.lua", NULL, PLEASE_INFORM | IS_FATAL);
	run_lua_file(LUA_CONFIG, fpath);

	// Time to eat some droid archetypes...
	find_file(fpath, BASE_DIR, "droid_specs.lua", NULL, PLEASE_INFORM | IS_FATAL);
	run_lua_file(LUA_CONFIG, fpath);

	// Load obstacle specifications.
	dynarray_init(&obstacle_map, 512, sizeof(struct obstacle_spec));
	find_file(fpath, BASE_DIR, "obstacle_specs.lua", NULL, PLEASE_INFORM | IS_FATAL);
	run_lua_file(LUA_CONFIG, fpath);

	// Load floor tile specifications.
	find_file(fpath, BASE_DIR, "floor_tiles.lua", NULL, PLEASE_INFORM | IS_FATAL);
	run_lua_file(LUA_CONFIG, fpath);
	dirty_animated_floor_tile_list();

	next_startup_percentage(1);
}

char copyright[] = "\nFreedroidRPG comes with NO WARRANTY to the extent permitted by law.\n\
You may redistribute copies of FreedroidRPG\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the file named COPYING.\n";

char usage_string[] = ""
"Usage: freedroidRPG [-h | --help]\n"
"                    [-v | --version]\n"
"                    [-e | --editor]\n"
"                    [-s | --sound]        [-q | --nosound]\n"
"                    [-o | --open_gl]      [-n | --no_open_gl]\n"
"                    [-f | --fullscreen]   [-w | --window]\n"
"                    [-t | --system_lang]\n"
"                    [-l character-name | --load=character-name]\n"
"                    [-r Y | --resolution=Y]  Y = 99 lists hardcoded resolutions.\n"
"                                             Y may also be of the form 'WxH' e.g. '800x600'\n"
"                    [-d X | --debug=X]       X = 0-5; default 1\n"
"                    [-b Z | --benchmark=Z]   Z = text | dialog | loadship | loadgame | savegame | dynarray | mapgen | leveltest\n"
"\n"
"Please report bugs either by entering them into the bug tracker on our website at:\n\n"
"http://bugs.freedroid.org\n\n"
"or by sending an e-mail to:\n\n"
"freedroid-discussion AT lists.sourceforge.net\n\n"
"For more information and known issues please see README.\n"
"Thanks a lot in advance.\n"
"                          / The FreedroidRPG dev team.\n\n";

#define MAX_RESOLUTIONS 32
screen_resolution screen_resolutions[MAX_RESOLUTIONS];
screen_resolution hard_resolutions[] = {
	{640, 480, "640 x 480", TRUE},
	{800, 600, "800 x 600", TRUE},
	{1024, 768, "1024 x 768", TRUE},
	{1152, 864, "1152 x 864", TRUE},
	{1280, 960, "1280 x 960", TRUE},
	{1400, 1050, "1400 x 1050", TRUE},
	{1600, 1200, "1600 x 1200", TRUE}
};

/* -----------------------------------------------------------------
 *  parse command line arguments and set global switches 
 *  exit on error, so we don't need to return success status
 * -----------------------------------------------------------------*/
void parse_command_line(int argc, char *const argv[])
{
	static struct option long_options[] = {
		{"version",     0, 0, 'v'},
		{"help",        0, 0, 'h'},
		{"editor",      0, 0, 'e'},
		{"load",        1, 0, 'l'},
		{"open_gl",     0, 0, 'o'},
		{"no_open_gl",  0, 0, 'n'},
		{"nosound",     0, 0, 'q'},
		{"sound",       0, 0, 's'},
		{"debug",       1, 0, 'd'},
		{"window",      0, 0, 'w'},
		{"fullscreen",  0, 0, 'f'},
		{"resolution",  1, 0, 'r'},
		{"system_lang", 0, 0, 't'},
		{"benchmark",   1, 0, 'b'},
		{0, 0, 0, 0}
	};

	while (1) {
		int c = getopt_long(argc, argv, "vel:onqsb:h?d::r:wft", long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
			/* version statement -v or --version
			 * following gnu-coding standards for command line interfaces */
		case 'v':
			printf("\n%s %s  \n", PACKAGE, freedroid_version);
			printf("%s",copyright);
			exit(0);
			break;

		case 'h':
		case '?':
			printf("%s",usage_string);
			exit(0);
			break;

		case 'e':
			start_editor = TRUE;
			break;

		case 'l':
			if (!optarg) {
				fprintf(stderr, "Please provide saved game name.\n");
				exit(1);
			}

			load_saved = TRUE;
			saved_game_name = optarg;
			break;

		case 'o':
			use_open_gl = TRUE;
			break;

		case 'n':
			use_open_gl = FALSE;
			break;

		case 'q':
			sound_on = FALSE;
			break;

		case 's':
			sound_on = TRUE;
			break;

		case 'd':
			if (!optarg)
				debug_level = 1;
			else
				debug_level = atoi(optarg);
			break;

		case 'r':
			if (!optarg) {
				GameConfig.screen_width = DEFAULT_SCREEN_WIDTH;
				GameConfig.screen_height = DEFAULT_SCREEN_HEIGHT;
			} else {
				int nb_res = sizeof(hard_resolutions) / sizeof(hard_resolutions[0]);
				char *x = strtok(optarg, "x");
				char *y = strtok(NULL, "x");

				// User input a resolution
				if (y != NULL) {
					GameConfig.screen_width = atoi(x);
					GameConfig.screen_height = atoi(y);
				} else {

					int resolution_code = atoi(optarg);

					if (resolution_code >= 0 && resolution_code < nb_res) {
						GameConfig.screen_width = hard_resolutions[resolution_code].xres;
						GameConfig.screen_height = hard_resolutions[resolution_code].yres;
					} else {
						fprintf(stderr, "\nresolution code received: %d\n", resolution_code);
						char *txt = (char *)malloc(nb_res * 128 + 1);
						txt[0] = '\0';
						int i;
						for (i = 0; i < nb_res; i++) {
							char tmp[129];
							snprintf(tmp, 129, "\t\t%d = %s\n", i, hard_resolutions[i].comment);
							strncat(txt, tmp, 128);
						}
						error_message(__FUNCTION__, "  %s%s  %s", IS_FATAL,
								 "\tThe resolution identifier given is not a valid resolution code.\n"
								 "\tThese codes correspond to the following hardcoded resolutions available:\n",
								 txt,
								 "\tAdditional resolutions may be specified by the form 'WxH' e.g. '800x600'\n"
								 "\tThe in-game menu automatically detects fullscreen modes supported by your hardware.");
						free(txt);
					}
				}
			}
			// By default, after starting up, the current resolution should be
			// the resolution used at the next game startup too, so we preselect
			// that for now.  The user can still change that later inside the
			// game from within the options menu.
			//
			GameConfig.next_time_width_of_screen = GameConfig.screen_width;
			GameConfig.next_time_height_of_screen = GameConfig.screen_height;
			break;
		case 'b':
			if (!optarg) {
				fprintf(stderr, "Please specify what to benchmark.\n");
				exit(1);
			}

			do_benchmark = strdup(optarg);
			break;
		case 'f':
			GameConfig.fullscreen_on = TRUE;
			break;

		case 'w':
			GameConfig.fullscreen_on = FALSE;
			break;

		case 't':
			lang_set("", NULL);
			break;

		default:
			printf("\nOption %c not implemented yet! Ignored.", c);
			break;
		}		/* switch(c) */
	}			/* while(1) */
}

/* -----------------------------------------------------------------
 * This function initializes a completely new game within FreedroidRPG.
 * In contrast to InitFreedroid, this function should be called 
 * whenever or better before any new game is started.
 * -----------------------------------------------------------------*/
void PrepareStartOfNewCharacter(char *start_label)
{
	gps start_pos;

	Activate_Conservative_Frame_Computation();

	// We make sure we don't have garbage in our arrays from a 
	// previous game or failed load-game attempt...
	//
	clear_out_arrays_for_fresh_game();
	// We do the same as above for lua state
	reset_lua_state();

	GetEventTriggers("events.dat");

	if (!skip_initial_menus)
		play_title_file(MAP_TITLES_DIR, "StartOfGame.lua");

	init_npcs();
	init_factions();

	GetCrew("ReturnOfTux.droids");

	start_pos = get_map_label_center(start_label);
	
	reset_visible_levels();
	if (game_root_mode == ROOT_IS_LVLEDIT && level_exists(GameConfig.last_edited_level))
		teleport_to_level_center(GameConfig.last_edited_level);
	else
		Teleport(start_pos.z, start_pos.x, start_pos.y, FALSE, TRUE);
	clear_active_bullets();

	// At this point the position history can be initialized
	//
	InitInfluPositionHistory();

	// Now we read in the mission targets for this mission
	// Several different targets may be specified simultaneously
	//
	get_quest_list("quests.dat");

	switch_background_music(curShip.AllLevels[Me.pos.z]->Background_Song_Name);

	// Now we know that right after starting a new game, the Tux might have
	// to 'change clothes' i.e. a lot of tux images need to be updated which can
	// take a little time.  Therefore we print some message so the user will not
	// panic and push the reset button :)
	//
	our_SDL_flip_wrapper();

	widget_text_init(message_log, _("--- Message Log ---"));
	if (strcmp(start_label, "TutorialTuxStart") == 0)
		append_new_game_message(_("Starting tutorial."));
	else
		append_new_game_message(_("Starting new game."));
}

void prepare_level_editor()
{
	game_root_mode = ROOT_IS_LVLEDIT;
	game_status = INSIDE_LVLEDITOR;
	skip_initial_menus = 1;
	char fp[PATH_MAX];
	find_file(fp, MAP_DIR, "levels.dat", NULL, PLEASE_INFORM | IS_FATAL);
	LoadShip(fp, 0);
	PrepareStartOfNewCharacter("NewTuxStartGameSquare");
	skip_initial_menus = 0;
	free(Me.character_name);
	Me.character_name = strdup("MapEd");
}

/**
 * This function sets the GameConfig back to the default values, NOT THE
 * VALUES STORED IN THE USERS CONFIG FILE.  This function is useful if 
 * no config file if found or if the config file turns out to originate
 * from a different version of FreedroidRPG, which could be dangerous as
 * well.
 */
void ResetGameConfigToDefaultValues(void)
{
	// At first we set audio volume to maximum value.
	// This might be replaced later with values from a 
	// private user FreedroidRPG config file.  But for now
	// this code is good enough...
	//
	GameConfig.Current_BG_Music_Volume = 0.5;
	GameConfig.Current_Sound_FX_Volume = 0.5;
	GameConfig.Current_Sound_Output_Fmt = SOUND_OUTPUT_FMT_STEREO;
	GameConfig.current_gamma_correction = 1.00;
	GameConfig.WantedTextVisibleTime = 3;
	GameConfig.Draw_Framerate = TRUE;
	GameConfig.Draw_Position = TRUE;
	GameConfig.All_Texts_Switch = FALSE;
	GameConfig.enemy_energy_bars_visible = TRUE;
	GameConfig.framerate_limit = 40;
	GameConfig.skip_light_radius = FALSE;
	GameConfig.omit_obstacles_in_level_editor = FALSE;
	GameConfig.omit_map_labels_in_level_editor = TRUE;
	GameConfig.omit_enemies_in_level_editor = TRUE;
	GameConfig.zoom_is_on = FALSE;
	GameConfig.show_blood = TRUE;
	GameConfig.show_lvledit_tooltips = TRUE;
	GameConfig.show_grid = TRUE;
	GameConfig.show_wp_connections = FALSE;
	GameConfig.number_of_big_screen_messages = 4;
	GameConfig.delay_for_big_screen_messages = 3;
	GameConfig.enable_cheatkeys = FALSE;
	GameConfig.skip_shadow_blitting = FALSE;
	GameConfig.do_fadings = TRUE;
	GameConfig.xray_vision_for_tux = FALSE;
	GameConfig.cheat_running_stamina = FALSE;
	GameConfig.lazyload = 1;
	GameConfig.show_item_labels = 0;
	GameConfig.last_edited_level = -1;
	GameConfig.show_all_floor_layers = 1;
	GameConfig.screen_width = DEFAULT_SCREEN_WIDTH;
	GameConfig.screen_height = DEFAULT_SCREEN_HEIGHT;
	GameConfig.next_time_width_of_screen = GameConfig.screen_width;
	GameConfig.next_time_height_of_screen = GameConfig.screen_height;
	GameConfig.fullscreen_on = FALSE;
	GameConfig.difficulty_level = DIFFICULTY_NORMAL;
	if (GameConfig.locale) {
		free(GameConfig.locale);
	}
	GameConfig.locale = my_strdup("");
}

/**
 * Release all allocated memory stored in GameConfig
 */
void gameconfig_clean()
{
	if (GameConfig.locale) {
		free(GameConfig.locale);
	}
}

/** 
 * Set signal handlers for SIGSEGV and SIGFPE.
 */
static void set_signal_handlers(void)
{
#ifndef __WIN32__
#ifndef __APPLE_CC__

	struct sigaction action;

	// We set up the structure for the new signal handling
	// to give to the operating system
	//
	action.sa_handler = print_trace;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	sigaction(SIGSEGV, &action, NULL);
	sigaction(SIGFPE, &action, NULL);

#endif
#endif
}

/**
 * Enumerates resolutions supported by SDL
 */
static void detect_available_resolutions(void)
{
	SDL_Rect **modes;
	int size = 0;

	// Get available fullscreen/hardware modes (reported by SDL)
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
	if (modes == (SDL_Rect**) -1) {
		error_message(__FUNCTION__,
			"SDL reports all resolutions are supported in fullscreen mode.\n"
			"Please use -r WIDTHxHEIGHT to specify any one you like.\n"
			"Defaulting to a sane one for now", NO_REPORT);
	} else {
		// Add resolutions to the screen_resolutions array
		for (size = 0; size < MAX_RESOLUTIONS && modes[size]; ++size)
			screen_resolutions[size] = (screen_resolution) {modes[size]->w, modes[size]->h, "", TRUE};
	}

	if (size == 0) {
		screen_resolutions[0] =	(screen_resolution) {DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "", TRUE};
		screen_resolutions[1] = (screen_resolution) {-1, -1, "", FALSE};
		return;
	}

	// Sort
	int i, j;
	screen_resolution resolution_holder;

	for (i = 0; i < size; i++) {
		for (j = 0; j < size - 1; j++) {
			// Sort in descending order of xres, then yres
			if (screen_resolutions[j].xres < screen_resolutions[j + 1].xres ||
					(screen_resolutions[j].xres == screen_resolutions[j + 1].xres &&
					screen_resolutions[j].yres < screen_resolutions[j +1].yres)) {
				resolution_holder = screen_resolutions[j + 1];
				screen_resolutions[j + 1] = screen_resolutions[j];
				screen_resolutions[j] = resolution_holder;
			}
		}
	}

	// Add our terminator on the end, just in case
	screen_resolutions[size] = (screen_resolution) {-1, -1, "", FALSE};
}

void prepare_execution(int argc, char *argv[])
{
	// Note: If not run from term, we have no way to inform the user
	// at that point in case of error (the graphic subsystem is not
	// yet initialized).
	// So, the user will not catch the error output, and the game
	// will apparently abort silently...
	// TODO: Use SDL2 SDL_ShowSimpleMessageBox()

#if defined HAVE_UNISTD_H && defined HAVE_DIRNAME
	// Change working directory to the executable's directory,
	// all data dirs being searched relatively to that directory.
	if (chdir(dirname(argv[0])))
		fprintf(stderr, "Couldn't change working directory to %s.\n", dirname(argv[0]));
#endif

	// Get color capability of current output stream.
	// Real code to get such a capability has to use setupterm() and
	// tigetnum() from the ncurses lib.
	// In order to avoid a dependency to ncurses, we use here a simple trick.

	run_from_term = FALSE;
	term_has_color_cap = FALSE;
#ifndef __WIN32__
	run_from_term = isatty(STDOUT_FILENO);
	char *term = getenv("TERM");
	if (run_from_term && term && !strncmp(term, "xterm", 5))
		term_has_color_cap = TRUE;
#endif

	// Get the homedir, and define the directory where the config file and
	// the savegames will be stored

	data_dirs[CONFIG_DIR].path[0] = '\0';

#if __WIN32__

	// There is no real homedir on Windows.
	// So we use the user's "My Documents" as a home directory.
	char mydocuments_path[MAX_PATH];
	if (SHGetSpecialFolderPath(0, mydocuments_path, CSIDL_PERSONAL, FALSE)) {
		sprintf(data_dirs[CONFIG_DIR].path, "%s/FreedroidRPG", mydocuments_path);
	} else {
		sprintf(data_dirs[CONFIG_DIR].path, "./FreedroidRPG");
	}

#else

	char *our_homedir = NULL;

	if ((our_homedir = getenv("HOME")) == NULL) {
		our_homedir = ".";
	}
	sprintf(data_dirs[CONFIG_DIR].path, "%s/.freedroid_rpg", our_homedir);

#endif

	int check_dir = check_directory("", CONFIG_DIR, TRUE, SILENT);

	// If config dir is not writable, we have a problem !

	if (check_dir == 1) {
		fprintf(stderr, "config subdir %s is not writable.\n", data_dirs[CONFIG_DIR].path);
		exit(EXIT_FAILURE);
	}

	// If config dir does not exist, create it

	if (check_dir == 2) {
#		if __WIN32__
		int mkdir_result = _mkdir(data_dirs[CONFIG_DIR].path);
#		else
		int mkdir_result = mkdir(data_dirs[CONFIG_DIR].path, S_IREAD | S_IWRITE | S_IEXEC);
#		endif
		if (mkdir_result == -1) {
			fprintf(stderr, "Was not able to create %s to store the configuration and the savegames.\n", data_dirs[CONFIG_DIR].path);
			exit(EXIT_FAILURE);
		}
	}

	// If not run from a terminal, stdout and stderr are redirect to a text file
	// written in the config dir (fdrpg_out.txt).
	// Note: On Windows, in SDL_WinMain(), stdout and stderr are redirected to
	// files, before to call our main(). Those files are automatically created
	// in the directory of the current process. They will be removed when the process
	// ends, because they are empty.
	if (!run_from_term) {
		char filename[PATH_MAX];
		find_file(filename, CONFIG_DIR, "fdrpg_out.txt", NULL, NO_REPORT);
		if (!freopen(filename, "w", stdout)) {
			fprintf(stderr, "Was not able to redirect stdout to %s. Errno: %d", filename, errno);
			run_from_term = TRUE; // Pretend it, to avoid Terminate() to try to open fdrpg_out.txt
		} else {
			if (dup2(fileno(stdout), fileno(stderr)) == -1) {
				fprintf(stderr, "Was not able to redirect stderr to stdout. Errno: %d", errno);
				run_from_term = TRUE; // Pretend it, to avoid Terminate() to try to open fdrpg_out.txt
			} else {
				// stdout and stderr are redirected, print an
				// informative message
				fprintf(stderr, "Hello!  This window contains the DEBUG OUTPUT of FreedroidRPG.\n"
				                "\n"
				                "Normally you would not see this message or this window, but apparently\n"
				                "FreedroidRPG has terminated because of an error of some sort.\n"
				                "\n"
				                "You might wish to inspect the debug output below.  Maybe sending the\n"
				                "debug output (or at least the lower end of the debug output) to the\n"
				                "FreedroidRPG developers could help them to track down the problem.\n"
				                "\n"
				                "Well, it's no guarantee that we can solve any bug, but it's certainly\n"
				                "better than nothing.  Thanks anyway for your interest in FreedroidRPG.\n"
				                "\n\n"
				                "--start of real debug log--\n\n");
			}
		}
	}

	// We mention the version of FreedroidRPG, so that debug reports
	// are easier to assign to the different versions of the game.

	DebugPrintf(-4, "\nHello, this is FreedroidRPG, version %s.\n", freedroid_version);
}

/* -----------------------------------------------------------------
 * This function initializes the whole FreedroidRPG game.
 *
 * THIS MUST NOT BE CONFUSED WITH INITNEWGAME, WHICH
 * ONLY INITIALIZES A NEW MISSION FOR THE GAME.
 *
 * ----------------------------------------------------------------- */
void InitFreedroid(int argc, char **argv)
{
	set_signal_handlers();

	clear_out_arrays_for_fresh_game();

	timeout_from_item_drop = 0;

	Overall_Average = 0.041;
	SkipAFewFrames = 0;

	ResetGameConfigToDefaultValues();

	input_keyboard_init();
	init_lua();
	init_luaconfig();

	load_fdrpg_config();

	load_game_config();

	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		error_message(__FUNCTION__, "Couldn't initialize SDL: %s", PLEASE_INFORM | IS_FATAL, SDL_GetError());

	// So the video library could be initialized.  So it should also be
	// cleaned up and closed once we're done and quit FreedroidRPG.
	atexit(SDL_Quit);

	// Enable to compute the unicode value of a pressed key.
	// Needed to use the numkeys which are on top of the alphakeys on keyboards.
	SDL_EnableUNICODE(1);

	detect_available_resolutions();

	parse_command_line(argc, argv);

	LightRadiusInit();

	init_timer();

	init_video();

	// Adapt button positions for the current screen resolution. Note: At this
	// point the video mode was already initialized, therefore we know if OpenGL
	// is used or not and also which screen resolution is used.
	//
	adapt_button_positions_to_screen_resolution();

	Copy_Rect(Full_User_Rect, User_Rect);

	next_startup_percentage(0);

	init_keyboard_input_array();
	init_message_log();

	init_audio();

	init_game_data();

	/* 
	 * Initialize random-number generator in order to make 
	 * level-start etc really different at each program start
	 */
	srand(time(NULL));

	InitPictures(); //requires game data loaded in Init_Game_Data()

	next_startup_percentage(100);
	if (strstr(VERSION, "rc") && !do_benchmark) {
		blit_background("startup1.jpg");
		alert_window(_("You are playing a Release Candidate.\n"
		               "Strange bugs may still be present in the game.\n"
		               "Please report any issues you find to our bugtracker at http://bugs.freedroid.org/\n"
		               "Thank you for helping us test the game.\n\nGood luck!\n"));
	}
}
/**
 * This function displayes the last seconds of the game when the influencer
 * has actually been killed.  It generates some explosions and waits for
 * some seconds, where the user can reload his latest game, or after that
 * returns to finally quit the inner game loop and the program will 
 * (outside this function) ask for a completely new game or loading a different
 * saved game or quit as in the very beginning of the game.
 */
void ThouArtDefeated(void)
{
	int now;

	DebugPrintf(1, "\n%s(): Real function call confirmed.", __FUNCTION__);
	append_new_game_message(_("Game over.\n"));
	SetNewBigScreenMessage(_("Game over.\n"));
	GameConfig.Inventory_Visible = FALSE;
	GameConfig.CharacterScreen_Visible = FALSE;
	ThouArtDefeatedSound();
	start_tux_death_explosions();
	now = SDL_GetTicks();

	GameConfig.SkillScreen_Visible = FALSE;
	GameConfig.CharacterScreen_Visible = FALSE;
	GameConfig.Inventory_Visible = FALSE;

	// Now that the influencer is dead, all this precious items
	// spring off of him...
	//
	if (Me.weapon_item.type > 0) {
		DropItemAt(Me.weapon_item.type, Me.pos.z, Me.pos.x - 0.5, Me.pos.y - 0.5, 1);
	}
	if (Me.drive_item.type > 0) {
		DropItemAt(Me.drive_item.type, Me.pos.z, Me.pos.x + 0.5, Me.pos.y - 0.5, 1);
	}
	if (Me.shield_item.type > 0) {
		DropItemAt(Me.shield_item.type, Me.pos.z, Me.pos.x + 0.5, Me.pos.y + 0.5, 1);
	}
	if (Me.armour_item.type > 0) {
		DropItemAt(Me.armour_item.type, Me.pos.z, Me.pos.x - 0.5, Me.pos.y + 0.5, 1);
	}
	if (Me.special_item.type > 0) {
		DropItemAt(Me.special_item.type, Me.pos.z, Me.pos.x - 0.5, Me.pos.y, 1);
	}
	if (Me.Gold > 0) {
		DropItemAt(get_item_type_by_id("Valuable Circuits"), Me.pos.z, Me.pos.x, Me.pos.y, 1);
	}

	GameOver = TRUE;

	while ((SDL_GetTicks() - now < 1000 * WAIT_AFTER_KILLED)) {
		StartTakingTimeForFPSCalculation();

		AssembleCombatPicture(DO_SCREEN_UPDATE | SHOW_ITEMS);

		UpdateCountersForThisFrame();
		
		DoAllMovementAndAnimations();
		move_enemies();

		ComputeFPSForThisFrame();
	}
	input_handle();
	if (!skip_initial_menus && (game_root_mode == ROOT_IS_GAME))
		play_title_file(BASE_TITLES_DIR, "GameLost.lua");

	do_death_menu();

};				// void ThouArtDefeated(void)

/**
 * This function displays the last seconds of the game when the influencer
 * has actually won the game.  It anims the game while it's waits for
 * some seconds, after printed a winning message. After that, Credits.title
 * can be displayed, or the game reboots to main.
 */
void ThouHastWon(void)
{
	int now;

	DebugPrintf(1, "\n%s(): Real function call confirmed.", __FUNCTION__);
	append_new_game_message(_("Game won.\n"));
	SetNewBigScreenMessage(_("Game won.\n"));
	GameConfig.Inventory_Visible = FALSE;
	GameConfig.CharacterScreen_Visible = FALSE;
	now = SDL_GetTicks();

	GameOver = TRUE;

	while ((SDL_GetTicks() - now < 1000 * WAIT_AFTER_GAME_WON)) {
		StartTakingTimeForFPSCalculation();

		AssembleCombatPicture(DO_SCREEN_UPDATE | SHOW_ITEMS);

		UpdateCountersForThisFrame();
		
		DoAllMovementAndAnimations();
		move_enemies();

		ComputeFPSForThisFrame();
	}

	// Now it's time for the end game title file...
	//
	//PlayATitleFile("EndOfGame.title");
	if (!skip_initial_menus)
		play_title_file(BASE_TITLES_DIR, "Credits.lua");

	input_handle();

};				// void ThouHastWon(void)

void ThouHastDelay(float delay)
{
	int now;

	// Disable delay while performing benchmark.
	if (!skip_initial_menus) {
	    // Hide Screens
		GameConfig.Inventory_Visible = FALSE;
		GameConfig.CharacterScreen_Visible = FALSE;
		now = SDL_GetTicks();


		while ((SDL_GetTicks() - now < 1000 * delay)) { // Use a function variable...
			StartTakingTimeForFPSCalculation();

			AssembleCombatPicture(DO_SCREEN_UPDATE | SHOW_ITEMS);

			UpdateCountersForThisFrame();

			DoAllMovementAndAnimations();
			move_enemies();

			ComputeFPSForThisFrame();
		}
	}

	input_handle();

};				// void ThouHastDelay(float)


/**
 * This function displays the end-act title file.
 * Not only that, but this also changes the Act variables,
 * Keeping the act-related functions and such in-check.
 *
 * Please note that only Act 1/2 are supported (yet).
 * This function must be changed to accept a bigger deal of acts.
 */
void EndOfAct(void)
{
	// TODO: Here we should advance the lua and the C variable for act in 1.

	// Display that the act finished
	DebugPrintf(1, "\n%s(): Real function call confirmed.", __FUNCTION__);
	append_new_game_message(_("End of Act 1.\n"));
	SetNewBigScreenMessage(_("End of Act 1.\n"));

	// Make next act enemies a little stronger. It increments bot HP in 30% and EXP reward in 15%.
	// It also makes the bot react a little faster after being hit. It also affects friends, so
	// they don't be easier to kill than the hostile ones (keeping the balance)
	//
	// Although such way to increment difficulty has it's concerns, it's still doable
	// however should be changed later. It has shown to be useful when all you need is to
	// give the player a little more challenge and make new levelups a little more doable,
	// giving more experience, and as the player probably have a good weapon, increasing HP.
	int type;
	enemy *erot, *nerot;

	type=0;
	while (type <= Number_Of_Droid_Types) {
		Droidmap[type].maxenergy *= 1.3;
		// NOTE: Drop of decimal point is itendended. Don't try to "fix" this. (below)
		Droidmap[type].experience_reward *= 1.15;
		Droidmap[type].recover_time_after_getting_hit *= 0.95;

		type += 1;
	}

	// Now, we restore droid's HP. Otherwise, ugly HP bars would show up.
	BROWSE_ALIVE_BOTS_SAFE(erot, nerot) {
		erot->energy = Droidmap[erot->type].maxenergy;
	}


	// Update 123's and 139's sensors to see Tux invisible. Otherwise, they would be too weak.
	Droidmap[get_droid_type("123")].sensor_id=get_sensor_id_by_name("infrared");
	Droidmap[get_droid_type("139")].sensor_id=get_sensor_id_by_name("infrared");



	// Now it's time for the end act title file...
	if (!skip_initial_menus)
		play_title_file(MAP_TITLES_DIR, "EndOfAct1.lua");

	input_handle();

};				// void EndOfAct(void)


#undef _init_c
