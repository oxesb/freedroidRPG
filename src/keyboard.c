/*
 * Copyright 2006, 2007, 2008 Edgar Simo Serra
 * Copyright 2008 Arthur Huillet
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file keyboard.c
 *
 * @brief Handles all the keybindings and input.
 */

#define _keyboard_c 1

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#include "lvledit/lvledit.h"
#include "lvledit/lvledit_actions.h"
#include "lvledit/lvledit_beautify_actions.h"
#include "lvledit/lvledit_tools.h"
#include "lvledit/lvledit_widgets.h"
#include "lvledit/lvledit_validator.h"

#define KEY_PRESS    ( 1.) /**< Key is pressed. */
#define KEY_RELEASE  (-1.) /**< Key is released. */
#define ISKEYPAD(k) ((k) >= SDLK_KP0 && (k) <= SDLK_KP_EQUALS)

static int input_keyboard_locked; // Block the execution of the keybindings.

/* Default keybindings
 *
 * If changing defaults, please make sure to update relevant parts in
 * README
 * map/titles/StartOfGame.title
 * map/titles/level_editor_help.title
 */

const keybind_t default_keybinds[] = {
	{ N_("keychart"), SDLK_F1, KMOD_NONE },
	{ N_("fullscreen"), SDLK_F2, KMOD_NONE },
	{ N_("grab_input"), SDLK_g, KMOD_LCTRL },
	{ N_("quit"), SDLK_q, KMOD_LCTRL },
	{ N_("wall_transparency"), SDLK_t, KMOD_NONE },
	{ N_("take_screenshot"), SDLK_PRINT, KMOD_NONE },

	/* Game */
	{ N_("inventory"), SDLK_i, KMOD_NONE },
	{ N_("skill"), SDLK_s, KMOD_NONE },
	{ N_("character"), SDLK_c, KMOD_NONE },
	{ N_("quests"), SDLK_q, KMOD_NONE },
	{ N_("reload"), SDLK_r, KMOD_NONE },
	{ N_("autorun"), SDLK_u, KMOD_NONE },

	{ N_("move_north"), SDLK_UP, KMOD_NONE },
	{ N_("move_south"), SDLK_DOWN, KMOD_NONE },
	{ N_("move_east"), SDLK_RIGHT, KMOD_NONE },
	{ N_("move_west"), SDLK_LEFT, KMOD_NONE },

	{ N_("quicksave"), SDLK_F3, KMOD_NONE },
	{ N_("quickload"), SDLK_F4, KMOD_NONE },
	{ N_("pause"), SDLK_p, KMOD_NONE },
	{ N_("show_item_labels"), SDLK_z, KMOD_NONE },
	{ N_("automap"), SDLK_TAB, KMOD_NONE },

	{ N_("activate_program0"), SDLK_F5, KMOD_NONE },
	{ N_("activate_program1"), SDLK_F6, KMOD_NONE },
	{ N_("activate_program2"), SDLK_F7, KMOD_NONE },
	{ N_("activate_program3"), SDLK_F8, KMOD_NONE },
	{ N_("activate_program4"), SDLK_F9, KMOD_NONE },
	{ N_("activate_program5"), SDLK_F10, KMOD_NONE },
	{ N_("activate_program6"), SDLK_F11, KMOD_NONE },
	{ N_("activate_program7"), SDLK_F12, KMOD_NONE },

	{ N_("quick_inventory0"), SDLK_0, KMOD_NONE },
	{ N_("quick_inventory1"), SDLK_1, KMOD_NONE },
	{ N_("quick_inventory2"), SDLK_2, KMOD_NONE },
	{ N_("quick_inventory3"), SDLK_3, KMOD_NONE },
	{ N_("quick_inventory4"), SDLK_4, KMOD_NONE },
	{ N_("quick_inventory5"), SDLK_5, KMOD_NONE },
	{ N_("quick_inventory6"), SDLK_6, KMOD_NONE },
	{ N_("quick_inventory7"), SDLK_7, KMOD_NONE },
	{ N_("quick_inventory8"), SDLK_8, KMOD_NONE },
	{ N_("quick_inventory9"), SDLK_9, KMOD_NONE },

	{ N_("close_windows"), SDLK_SPACE, KMOD_NONE },
	{ N_("game_escape_menu"), SDLK_ESCAPE, KMOD_NONE },

	/* Level editor */
	{ N_("drop_item"), SDLK_g, KMOD_NONE },
	{ N_("place_obstacle_kp0"), SDLK_KP0, KMOD_NONE },
	{ N_("place_obstacle_kp1"), SDLK_KP1, KMOD_NONE },
	{ N_("place_obstacle_kp2"), SDLK_KP2, KMOD_NONE },
	{ N_("place_obstacle_kp3"), SDLK_KP3, KMOD_NONE },
	{ N_("place_obstacle_kp4"), SDLK_KP4, KMOD_NONE },
	{ N_("place_obstacle_kp5"), SDLK_KP5, KMOD_NONE },
	{ N_("place_obstacle_kp6"), SDLK_KP6, KMOD_NONE },
	{ N_("place_obstacle_kp7"), SDLK_KP7, KMOD_NONE },
	{ N_("place_obstacle_kp8"), SDLK_KP8, KMOD_NONE },
	{ N_("place_obstacle_kp9"), SDLK_KP9, KMOD_NONE },
	{ N_("change_obstacle_label"), SDLK_h, KMOD_NONE },
	{ N_("change_map_label"), SDLK_m, KMOD_NONE },
	{ N_("zoom_out"), SDLK_o, KMOD_NONE },
	{ N_("cycle_marked_object"), SDLK_n, KMOD_NONE },
	{ N_("next_tab"), SDLK_f, KMOD_NONE },
	{ N_("previous_tab"), SDLK_f, KMOD_LSHIFT },
	{ N_("undo"), SDLK_z, KMOD_NONE },
	{ N_("redo"), SDLK_y, KMOD_NONE },
	{ N_("beautify_grass"), SDLK_b, KMOD_LCTRL },
	{ N_("beautify_water"), SDLK_w, KMOD_LCTRL },
	{ N_("toggle_waypoint"), SDLK_w, KMOD_NONE },
	{ N_("toggle_waypoint_randomspawn"), SDLK_w, KMOD_LSHIFT },
	{ N_("connect_waypoint"), SDLK_c, KMOD_NONE },
	{ N_("toolbar_scroll_left"), SDLK_PAGEUP, KMOD_NONE },
	{ N_("toolbar_scroll_right"), SDLK_PAGEDOWN, KMOD_NONE },
	{ N_("toolbar_step_left"), SDLK_PAGEUP, KMOD_LCTRL },
	{ N_("toolbar_step_right"), SDLK_PAGEDOWN, KMOD_LCTRL },
	{ N_("cut"), SDLK_x, KMOD_LCTRL },
	{ N_("copy"), SDLK_c, KMOD_LCTRL },
	{ N_("paste"), SDLK_v, KMOD_LCTRL },
	{ N_("delete"), SDLK_BACKSPACE, KMOD_NONE },
	{ N_("next_selection_type"), SDLK_TAB, KMOD_NONE },
	{ N_("previous_selection_type"), SDLK_TAB, KMOD_LSHIFT },
	{ N_("run_map_validator"), SDLK_e, KMOD_LCTRL },

	/* Cheat */
	{ N_("cheat_xp+_1k"), SDLK_KP1, KMOD_NONE },
	{ N_("cheat_xp*_2"), SDLK_KP2, KMOD_NONE },
	{ N_("cheat_melee"), SDLK_KP7, KMOD_NONE },
	{ N_("cheat_range"), SDLK_KP8, KMOD_NONE },
	{ N_("cheat_programing"), SDLK_KP9, KMOD_NONE },
	{ N_("cheat_melee_down"), SDLK_KP4, KMOD_NONE },
	{ N_("cheat_range_down"), SDLK_KP5, KMOD_NONE },
	{ N_("cheat_programing_down"), SDLK_KP6, KMOD_NONE },
	{ N_("cheat_drop_random_item"), SDLK_r, KMOD_LCTRL },
	{ N_("cheat_drop_random_magical_item"), SDLK_r, KMOD_LCTRL | KMOD_LSHIFT },
	{ N_("cheat_respawn_level"), SDLK_r, KMOD_LCTRL | KMOD_LALT | KMOD_LSHIFT },
	{ N_("cheat_level_editor"), SDLK_e, KMOD_LCTRL | KMOD_LALT },
	{ N_("cheat_menu"), SDLK_c, KMOD_LCTRL | KMOD_LALT | KMOD_LSHIFT },
	{ N_("cheat_reload_graphics"), SDLK_g, KMOD_LCTRL | KMOD_LALT | KMOD_LSHIFT },
	{ N_("cheat_inc_repair"), SDLK_r, KMOD_RCTRL  },
	{ N_("cheat_dec_repair"), SDLK_r, KMOD_RCTRL | KMOD_RSHIFT  },

#ifdef WITH_RTPROF
	/* RT Profiler */
	{ N_("rtprof_switch_activation"), SDLK_p, KMOD_LALT },
	{ N_("rtprof_clear_probes"), SDLK_o, KMOD_LALT },
#endif

	{ NULL }
};

/**
 * @fn void input_keyboard_init (void)
 *
 * @brief Initializes the input subsystem (using default keybindings).
 */
void input_keyboard_init(void)
{
	int i;

	GameConfig.input_keybinds[0].name = NULL;

	if (sizeof(default_keybinds) / sizeof(default_keybinds[0]) > sizeof(GameConfig.input_keybinds) / sizeof(GameConfig.input_keybinds[0]))
		error_message(__FUNCTION__,
			     "There are " SIZE_T_F " keyboard commands defined in keyboard.c, but GameConfig structure only supports " SIZE_T_F ".",
			     PLEASE_INFORM | IS_FATAL,
			     sizeof(default_keybinds) / sizeof(default_keybinds[0]),
			     sizeof(GameConfig.input_keybinds) / sizeof(GameConfig.input_keybinds[0]));

	/* creates a null keybinding for each */
	for (i = 0; default_keybinds[i].name != NULL; i++) {
		GameConfig.input_keybinds[i].name = (char *)default_keybinds[i].name;
		GameConfig.input_keybinds[i].key = default_keybinds[i].key;
		GameConfig.input_keybinds[i].mod = default_keybinds[i].mod;
	}

	GameConfig.input_keybinds[i].name = NULL;

	input_keyboard_locked = 0;
}

/**
 *
 * @brief Binds key to action keybind.
 *
 *    @param keybind The name of the keybind defined above.
 *    @param key The key to bind to.
 *    @param mod the modifier associated to the key
 */
void input_set_keybind(char *keybind, SDLKey key, SDLMod mod)
{
	int i;
	for (i = 0; GameConfig.input_keybinds[i].name != NULL; i++)
		if (!strcmp(keybind, GameConfig.input_keybinds[i].name)) {
			GameConfig.input_keybinds[i].key = key;
			GameConfig.input_keybinds[i].mod = mod;
			return;
		}
	error_message(__FUNCTION__,
	             "Unable to set keybinding '%s', that command doesn't exist.",
	             NO_REPORT,
	             keybind);
}

void input_get_keybind(const char *cmdname, SDLKey * key, SDLMod * mod)
{
	int i;
	for (i = 0; GameConfig.input_keybinds[i].name != NULL; i++) {
		if (!strcmp(cmdname, GameConfig.input_keybinds[i].name)) {
			if (key)
				*key = GameConfig.input_keybinds[i].key;
			if (mod)
				*mod = GameConfig.input_keybinds[i].mod;
			return;
		}
	}
	error_message(__FUNCTION__,
	             "Unable to get keybinding for command '%s', that command does not exist.",
	             PLEASE_INFORM | IS_FATAL,
	             cmdname);

	/* This dead code writes "key" and "mod" to shut up GCC */
	*key = GameConfig.input_keybinds[0].key;
	*mod = GameConfig.input_keybinds[0].mod;
}

/**
 * Print the key mapping of a given command into a string.
 * @param cmd name of the command
 * @param out pointer to a sufficiently large buffer to hold the key string
 */
void input_get_keybind_string(const keybind_t *kb, char *out)
{
	const char *ctrl_modstr = "";
	const char *alt_modstr = "";
	const char *shift_modstr = "";

	if ((kb->mod & KMOD_LCTRL) || (kb->mod & KMOD_RCTRL))
		ctrl_modstr = "Ctrl+";
	if ((kb->mod & KMOD_LALT)  || (kb->mod & KMOD_RALT))
		alt_modstr = "Alt+";
	if ((kb->mod & KMOD_LSHIFT) || (kb->mod & KMOD_RSHIFT))
		shift_modstr = "Shift+";

	sprintf(out, "%s%s%s%s", ctrl_modstr, alt_modstr, shift_modstr, SDL_GetKeyName(kb->key));
}

#define KEYCHART_RECT_X (GameConfig.screen_width / 100)
#define KEYCHART_RECT_Y (GameConfig.screen_height / 10)
#define KEYCHART_RECT_W (GameConfig.screen_width - 2*KEYCHART_RECT_X)
#define KEYCHART_RECT_H (GameConfig.screen_height - 2*KEYCHART_RECT_Y)

/**
 * Display the keychart
 *
 * @return The index of the last command displayed on screen.
 */
static int display_keychart(unsigned int startidx, unsigned int cursor, int highlight)
{
	char txt[1024];
	int i;
	SDL_Rect keychart_rect = { KEYCHART_RECT_X, KEYCHART_RECT_Y, KEYCHART_RECT_W, KEYCHART_RECT_H };
	int ypos = keychart_rect.y;
	int xpos = keychart_rect.x;

	blit_background("shoppe.jpg");

	draw_shadowing_rectangle(keychart_rect);

	put_string_centered(Para_Font, get_font_height(Para_Font), _("Key chart"));

	if (startidx >= sizeof(default_keybinds) / sizeof(default_keybinds[0]))
		return -1;

	for (i = startidx; GameConfig.input_keybinds[i].name != NULL; i++) {
		char keystr[100] = "";
		const char *font_str = font_switchto_neon;
		
		if (i == cursor && highlight) {
			font_str = font_switchto_red;
		}

		input_get_keybind_string(&(GameConfig.input_keybinds[i]), &keystr[0]);

		sprintf(txt, "%s%s%s: %s", font_str, (i == cursor) ? "> " : "   ",
			_(GameConfig.input_keybinds[i].name), keystr);
		put_string(get_current_font(), xpos, ypos, txt);

		ypos += get_font_height(get_current_font());

		if (ypos > keychart_rect.y + keychart_rect.h - get_font_height(get_current_font())) {
			if (xpos > keychart_rect.x)
				break;
			else {
				xpos += keychart_rect.w / 2;
				ypos = keychart_rect.y;
			}
		}
	}

	put_string(FPS_Display_Font, 100, GameConfig.screen_height - get_font_height(FPS_Display_Font),
		      _("ARROWS to select, ENTER to remap, ESCAPE to exit"));
	our_SDL_flip_wrapper();

	return i;
}

/**
 * This function simulates displaying the keychart in order to know how many keys 
 * appear per page
 */
static int get_nb_commands_per_page(void)
{
	int ypos = KEYCHART_RECT_Y;
	struct font *our_font = FPS_Display_Font;
	int i = 0;

	while (1) {
		ypos += get_font_height(our_font);
		i++;
		if (ypos > KEYCHART_RECT_Y + KEYCHART_RECT_H - get_font_height(our_font)) {
			break;
		}
	}

	return i;
}

/**
 * Display a keychart and allow modification of keys
 */
void keychart(void)
{
	int done = 0;
	int startpos = 0;
	int endpos;
	int cursor = 0;
	SDL_Event event;
	int per_page = get_nb_commands_per_page();
	// The actual number of keybinds is the length of the default_keybinds array
	const int maxcmds = sizeof(default_keybinds) / sizeof(default_keybinds[0]) - 2;

	Activate_Conservative_Frame_Computation();

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	while (!done) {
		if (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				Terminate(EXIT_SUCCESS);
			}

			endpos = display_keychart(startpos, cursor, FALSE);

			if (event.type == SDL_KEYDOWN) {
				fix_keysym(&(event.key.keysym));

				if (event.key.keysym.sym == SDLK_ESCAPE)
					done = 1;
				if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_PAGEDOWN) {
					cursor += per_page;
					if (cursor >= maxcmds)
						cursor = maxcmds;

					if (cursor > endpos) {
						startpos += per_page;
					}
				}
				if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_PAGEUP) {
					cursor -= per_page;

					if (cursor < 0) {
						cursor = 0;
					}

					if (cursor < startpos) {
						startpos -= per_page;
					}

					if (startpos < 0) {
						startpos = 0;
					}
				}
				if (event.key.keysym.sym == SDLK_UP) {
					cursor--;
					if (cursor < 0)
						cursor = 0;
					if (cursor < startpos)
						startpos--;
				}
				if (event.key.keysym.sym == SDLK_DOWN) {
					if (cursor < maxcmds)
						cursor++;
					if (cursor > endpos)
						startpos++;
				}
				if (event.key.keysym.sym == SDLK_RETURN) {
					SDLKey newkey;
					SDLMod newmod;
					int oldkey, oldmod;
					int i;
					display_keychart(startpos, cursor, TRUE);
					oldkey = GameConfig.input_keybinds[cursor].key;
					oldmod = GameConfig.input_keybinds[cursor].mod;

					newkey = getchar_raw(&newmod);
					newmod &= ~(KMOD_CAPS | KMOD_NUM | KMOD_MODE);	/* We want to ignore "global" modifiers. */
					GameConfig.input_keybinds[cursor].key = (int)newkey;
					GameConfig.input_keybinds[cursor].mod = (int)newmod;

					for (i = 0; GameConfig.input_keybinds[i].name != NULL; i++) {
						if (i == cursor)
							continue;

						if (GameConfig.input_keybinds[i].key == (int)newkey &&
						    GameConfig.input_keybinds[i].mod == (int)newmod) {
							/* If the key we have just assigned was already assigned to another command... */
							GameConfig.input_keybinds[i].key = oldkey;
							GameConfig.input_keybinds[i].mod = oldmod;
							/* ... swap the keys (assign the old key to the conflicting command) */
						}
					}

				}
			}
		} else {
			SDL_Delay(1);
		}
	}
	SDL_EnableKeyRepeat(0, SDL_DEFAULT_REPEAT_INTERVAL);
}

/**
 * Hold the keyboard input handler.
 *
 * Some UI panels have their own keyboard input handler, or in some cases
 * we do not want the main keyboard input handler to react to key events.
 * Some keybindings could however be 'un-interruptibles'.
 * The opposite function is input_release_keyboard().
 */
void input_hold_keyboard(void)
{
	// Several UI elements may want to hold the keyboard handler, and
	// thus call this function and its release companion.
	// We thus need to count how many times this function is called, and
	// how many times the release function is called.
	input_keyboard_locked++;
}

/**
 * Release the keyboard input handler.
 *
 * See input_hold_keyboard().
 */
void input_release_keyboard(void)
{
	if (input_keyboard_locked > 0)
		input_keyboard_locked--;
}

/**
 * @fn static void input_key( int keynum, int value)
 *
 * @brief Runs the input command.
 *
 *    @param keynum The index of the  keybind.
 *    @param value The value of the keypress (defined above).
 */
#define KEYPRESS(s)    (!strcmp(GameConfig.input_keybinds[keynum].name,(s)) && value==KEY_PRESS)
#define INGAME()	(game_status == INSIDE_GAME)
#define INMENU()	(game_status == INSIDE_MENU)
#define INLVLEDIT()	(game_status == INSIDE_LVLEDITOR)

static int input_key(int keynum, int value)
{
	/* Non lockable keybindings */
 	if (KEYPRESS("quit")) {
		Terminate(EXIT_SUCCESS);
		return 0;
	}

	/* Do not execute the other key actions, if the the keyboard input is 'held' */
	if (input_keyboard_locked)
		return -1;

	/* Cheat keys - those all fall through to normal game keys! */
	if (GameConfig.enable_cheatkeys && INGAME()) {
		if (KEYPRESS("cheat_xp+_1k")) {
			Me.Experience += 1000;
		} else if (KEYPRESS("cheat_xp*_2")) {
			Me.Experience *= 2;
		} else if (KEYPRESS("cheat_melee")) {
			if (Me.melee_weapon_skill < 9)
				Me.melee_weapon_skill += 1;
		} else if (KEYPRESS("cheat_range")) {
			if (Me.ranged_weapon_skill < 9)
				Me.ranged_weapon_skill += 1;
		} else if (KEYPRESS("cheat_programing")) {
			if (Me.spellcasting_skill < 9)
				Me.spellcasting_skill += 1;
		} else if (KEYPRESS("cheat_melee_down")) {
			if (Me.melee_weapon_skill > 0)
				Me.melee_weapon_skill -= 1;
		} else if (KEYPRESS("cheat_range_down")) {
			if (Me.ranged_weapon_skill > 0)
				Me.ranged_weapon_skill -= 1;
		} else if (KEYPRESS("cheat_programing_down")) {
			if (Me.spellcasting_skill > 0)
				Me.spellcasting_skill -= 1;
		} else if (KEYPRESS("cheat_drop_random_item")) {
			drop_random_item(Me.pos.z, Me.pos.x, Me.pos.y, 3, FALSE);
		} else if (KEYPRESS("cheat_drop_random_magical_item")) {
			drop_random_item(Me.pos.z, Me.pos.x, Me.pos.y, 3, TRUE);
		} else if (KEYPRESS("cheat_respawn_level")) {
			respawn_level(Me.pos.z);
		} else if (KEYPRESS("cheat_reload_graphics")) {
			reload_graphics();
		} else if (KEYPRESS("cheat_inc_repair")) {
			if(Me.skill_level[get_program_index_with_name("Repair equipment")] < NUMBER_OF_SKILL_LEVELS-1) {
				Me.skill_level[get_program_index_with_name("Repair equipment")] += 1;
			}
		} else if (KEYPRESS("cheat_dec_repair")) {
			if(Me.skill_level[get_program_index_with_name("Repair equipment")] > 1) {
				Me.skill_level[get_program_index_with_name("Repair equipment")] -= 1;
			}
		}
	}

	/* Ingame input functions */
	if (INGAME()) {
		if (KEYPRESS("inventory")) {
			toggle_game_config_screen_visibility(GAME_CONFIG_SCREEN_VISIBLE_INVENTORY);
			return 0;
		} else if (KEYPRESS("skill")) {
			toggle_game_config_screen_visibility(GAME_CONFIG_SCREEN_VISIBLE_SKILLS);
			return 0;
		} else if (KEYPRESS("character")) {
			toggle_game_config_screen_visibility(GAME_CONFIG_SCREEN_VISIBLE_CHARACTER);
			return 0;
		} else if (KEYPRESS("quests")) {
			toggle_quest_browser();
			return 0;
		} else if (KEYPRESS("autorun")) {
			GameConfig.autorun_activated = !GameConfig.autorun_activated;
			return 0;
		} else if (KEYPRESS("move_north")) {
			set_movement_with_keys(0, -1);
			return 0;
		} else if (KEYPRESS("move_south")) {
			set_movement_with_keys(0, 1);
			return 0;
		} else if (KEYPRESS("move_east")) {
			set_movement_with_keys(1, 0);
			return 0;
		} else if (KEYPRESS("move_west")) {
			set_movement_with_keys(-1, 0);
			return 0;
		} else if (KEYPRESS("reload")) {
			TuxReloadWeapon();
			return 0;
		} else if (KEYPRESS("quicksave")) {
			save_game();
			return 0;
		} else if (KEYPRESS("quickload")) {
			load_game();
			return 0;
		} else if (KEYPRESS("pause")) {
			Pause();
			return 0;
		} else if (KEYPRESS("show_item_labels")) {
			GameConfig.show_item_labels = !GameConfig.show_item_labels;
			return 0;
		} else if (!strncmp(GameConfig.input_keybinds[keynum].name, "activate_program", strlen("activate_program"))
			   && value == KEY_PRESS) {
			int number = atoi(GameConfig.input_keybinds[keynum].name + strlen("activate_program"));
			if (number >= 10 || number < 0) {
				error_message(__FUNCTION__,
					     "Tried to activate skill number %d - only shortcuts from F1 to F9 are supported",
					     PLEASE_INFORM, number);
				return 0;
			}

			int assign_shortcut = GameConfig.SkillScreen_Visible
			    && CursorIsOnWhichSkillButton(GetMousePos_x(), GetMousePos_y()) != -1;

			if (!assign_shortcut) {
				if (Me.program_shortcuts[number] != -1)
					activate_nth_skill(Me.program_shortcuts[number]);
			} else {
				set_nth_quick_skill(number);

			}
		} else if (!strncmp(GameConfig.input_keybinds[keynum].name, "quick_inventory", strlen("quick_inventory"))
			   && value == KEY_PRESS) {
			int number = atoi(GameConfig.input_keybinds[keynum].name + strlen("quick_inventory"));
			if (number >= 10 || number < 0) {
				error_message(__FUNCTION__, "Tried to use quick inventory item %d - only 1-9 + 0 are supported.",
					     PLEASE_INFORM, number);
				return 0;
			}

			Quick_ApplyItem(number);
			return 0;
		} else if (KEYPRESS("automap")) {
			toggle_automap();
			return 0;
		} else if (KEYPRESS("cheat_level_editor")) {
			LevelEditor();
			return 0;
		} else if (KEYPRESS("cheat_menu")) {
			Cheatmenu();
			return 0;
		} else if (KEYPRESS("close_windows")) {
			if (GameConfig.Inventory_Visible ||
			    GameConfig.CharacterScreen_Visible ||
			    GameConfig.SkillScreen_Visible || GameConfig.skill_explanation_screen_visible) {
				GameConfig.Inventory_Visible = FALSE;
				GameConfig.CharacterScreen_Visible = FALSE;
				GameConfig.SkillScreen_Visible = FALSE;
				GameConfig.skill_explanation_screen_visible = FALSE;
			}
		} else if (KEYPRESS("game_escape_menu")) {
			if (GameConfig.Inventory_Visible ||
			    GameConfig.CharacterScreen_Visible ||
			    GameConfig.SkillScreen_Visible || GameConfig.skill_explanation_screen_visible) {
				GameConfig.Inventory_Visible = FALSE;
				GameConfig.CharacterScreen_Visible = FALSE;
				GameConfig.SkillScreen_Visible = FALSE;
				GameConfig.skill_explanation_screen_visible = FALSE;
			} else {
				EscapeMenu();
			}
			return 0;
		}
#ifdef WITH_RTPROF
		else if (KEYPRESS("rtprof_switch_activation")) {
			rtprof_switch_activation();
			return 0;
		} else if (KEYPRESS("rtprof_clear_probes")) {
			rtprof_clear_probes();
			return 0;
		}
#endif
	} else if (INLVLEDIT()) {
		if (KEYPRESS("drop_item")) {
			ItemDropFromLevelEditor();
			return 0;
		} else if (KEYPRESS("change_obstacle_label")) {
			if (single_tile_selection(OBJECT_OBSTACLE)) {
				action_change_obstacle_label_user(EditLevel(), single_tile_selection(OBJECT_OBSTACLE));
			}
		} else if (KEYPRESS("change_map_label")) {
			level_editor_action_change_map_label_user(CURLEVEL(), Me.pos.x, Me.pos.y);
			return 0;
		} else if (KEYPRESS("zoom_out")) {
			GameConfig.zoom_is_on = !GameConfig.zoom_is_on;
			return 0;
		} else if (KEYPRESS("cycle_marked_object")) {
			level_editor_cycle_marked_object();
			return 0;
		} else if (KEYPRESS("next_tab")) {
			lvledit_categoryselect_switch(1);
			return 0;
		} else if (KEYPRESS("previous_tab")) {
			lvledit_categoryselect_switch(-1);
			return 0;
		} else if (KEYPRESS("next_selection_type")) {
			level_editor_switch_selection_type(1);
			return 0;
		} else if (KEYPRESS("previous_selection_type")) {
			level_editor_switch_selection_type(-1);
			return 0;
		} else if (KEYPRESS("undo")) {
			level_editor_action_undo();
			return 0;
		} else if (KEYPRESS("redo")) {
			level_editor_action_redo();
			return 0;
		} else if (KEYPRESS("beautify_grass")) {
			level_editor_beautify_grass_tiles(EditLevel());
			return 0;
		} else if (KEYPRESS("beautify_water")) {
			level_editor_beautify_water_tiles(EditLevel());
			return 0;
		} else if (KEYPRESS("toolbar_scroll_left")) {
			widget_lvledit_toolbar_scroll_left();
			return 0;
		} else if (KEYPRESS("toolbar_scroll_right")) {
			widget_lvledit_toolbar_scroll_right();
			return 0;
		} else if (KEYPRESS("toolbar_step_left")) {
			widget_lvledit_toolbar_left();
			return 0;
		} else if (KEYPRESS("toolbar_step_right")) {
			widget_lvledit_toolbar_right();
			return 0;
		} else if (KEYPRESS("run_map_validator")) {
			level_validation();	
			return 0;
		} else if (!strncmp(GameConfig.input_keybinds[keynum].name, "place_obstacle_kp", strlen("place_obstacle_kp"))
			   && value == KEY_PRESS) {
			int number = atoi(GameConfig.input_keybinds[keynum].name + strlen("place_obstacle_kp"));
			if (number >= 10 || number < 0) {
				error_message(__FUNCTION__, "Tried to place obstacle using kp%d - only kp1 to kp9 are supported",
					     PLEASE_INFORM, number);
				return 0;
			}
			level_editor_place_aligned_object(number);
			return 0;
		} else if (KEYPRESS("toggle_waypoint")) {
			lvledit_action_toggle_waypoint(FALSE);
			return 0;
		} else if (KEYPRESS("toggle_waypoint_randomspawn")) {
			lvledit_action_toggle_waypoint(TRUE);
			return 0;
		} else if (KEYPRESS("connect_waypoint")) {
			level_editor_action_toggle_waypoint_connection_user(CURLEVEL(), EditX(), EditY());
			return 0;
		} else if (KEYPRESS("cut")) {
			level_editor_cut_selection();
			return 0;
		} else if (KEYPRESS("copy")) {
			level_editor_copy_selection();
			return 0;
		} else if (KEYPRESS("paste")) {
			level_editor_paste_selection();
			return 0;
		} else if (KEYPRESS("delete")) {
			level_editor_delete_selection();
			return 0;
		}
	}

	/* Global keybindings */
	if (KEYPRESS("fullscreen")) {
		/* This doesn't seem to work so well on Windows */
#ifndef __WIN32__
		SDL_WM_ToggleFullScreen(Screen);
		GameConfig.fullscreen_on = !GameConfig.fullscreen_on;
#endif
		return 0;
	} else if (KEYPRESS("grab_input")) {
		SDL_GrabMode mode = SDL_WM_GrabInput(SDL_GRAB_QUERY);
		if (mode == SDL_GRAB_OFF)
			mode = SDL_GRAB_ON;
		else
			mode = SDL_GRAB_OFF;

		SDL_WM_GrabInput(mode);
		return 0;
	} else if (KEYPRESS("take_screenshot")) {
		play_sound("effects/CameraTakesPicture.ogg");
		char filename[PATH_MAX];
		char relative_filename[PATH_MAX];
		sprintf(relative_filename, "%s.screenshot-%d.png", Me.character_name, SDL_GetTicks()/1000);
		find_file(filename, CONFIG_DIR, relative_filename, NULL, SILENT);
		save_screenshot(filename, FALSE);
		alert_window(_("Screenshot saved to \"%s\" in your .freedroid_rpg/ directory."), relative_filename);
		return 0;
	} else if (KEYPRESS("wall_transparency")) {
		GameConfig.transparency = !GameConfig.transparency;
		return 0;
	} else if (KEYPRESS("keychart")) {
		keychart();
		return 0;
	}

	return -1;
}

static int input_key_event(SDLKey key, SDLMod mod, int value)
{
	int i;
	int noteaten = -1;
	mod &= ~(KMOD_CAPS | KMOD_NUM | KMOD_MODE);	/* We want to ignore "global" modifiers. */

	for (i = 0; GameConfig.input_keybinds[i].name != NULL; i++)
		if ((GameConfig.input_keybinds[i].key == key) && (GameConfig.input_keybinds[i].mod == mod)) {
			if (!(noteaten = input_key(i, value)))
				break;
		}
	return noteaten;
}

/*
 * Fix some raw keysym values, such as 'shifted numerical key' or non
 * printable keys from the numpad.
 */
void fix_keysym(SDL_keysym *keysym)
{
	// First case: on some keyboards (such as AZERTY), the Shift modifier has
	// to be pressed to input a numeral from the 'main' keyboard.
	// We want to return the actual numeric value instead of 'shift+<some_key>'.
	// If the user uses the numpad to enter a numeral, we however keep its keysym
	// (to be able to make a difference between the two pads).
	//
	// Note that the transformation does not fully work when Ctrl is pressed.
	// For instance, on an AZERTY keyboard, 'shift+(' is correctly transformed
	// into '5', but 'ctrl+shift+(' is not transformed (we would like to have
	// 'ctrl+5'). This is due to the special behavior of the Ctrl key, which is
	// not only a modifier but also modifies to value of some keys (for instance,
	// ctrl+c == 03 == ETX).
	//
	// Remark: we use an 'optimized' test to minimize its cost

	if ( (keysym->mod & KMOD_SHIFT) && (keysym->sym < SDLK_KP0) &&
	     ('0' <= keysym->unicode) && (keysym->unicode <= '9')  ) {
		keysym->sym = (SDLKey)keysym->unicode;
		keysym->mod = keysym->mod & ~KMOD_SHIFT;

		return;
	}

	// Second case: transform the non numerals of the numpad into their
	// printable equivalents.

	if (ISKEYPAD(keysym->sym)) {
		switch (keysym->sym) {
			case SDLK_KP_PERIOD:   keysym->sym = SDLK_PERIOD; break;
			case SDLK_KP_DIVIDE:   keysym->sym = SDLK_SLASH; break;
			case SDLK_KP_MULTIPLY: keysym->sym = SDLK_ASTERISK; break;
			case SDLK_KP_MINUS:    keysym->sym = SDLK_MINUS; break;
			case SDLK_KP_PLUS:     keysym->sym = SDLK_PLUS; break;
			case SDLK_KP_ENTER:    keysym->sym = SDLK_RETURN; break;
			case SDLK_KP_EQUALS:   keysym->sym = SDLK_EQUALS; break;
			default: break;
		}
	}
}

int input_key_press(SDL_Event *event)
{
	fix_keysym(&(event->key.keysym));
	SDLKey sym = event->key.keysym.sym;
	SDLMod mod = event->key.keysym.mod;

	input_key_event(sym, mod, KEY_PRESS);

	return 0;
}

/**
 * Read a character from the keyboard and return its ASCII code.
 *
 * This function is to be used only to fill an editable string widget.
 * It only returns if the value of the pressed key is in the ASCII map, or
 * if the key can be used to 'interact' with the widget (arrows, home/end...)
 *
 * @return ASCII code of the pressed key, or SDL virtual keysym for 'interaction' keys
 */

int getchar_ascii(void)
{
	SDL_Event event;
	int return_key = 0;

	while (1) {
		SDL_WaitEvent(&event);	/* wait for next event */

		if (event.type == SDL_QUIT) {
			Terminate(EXIT_SUCCESS);
		}

		if (event.type == SDL_KEYDOWN) {
			fix_keysym(&(event.key.keysym));
			// First check for 'arrow' keys
			if (SDLK_UP <= event.key.keysym.sym && event.key.keysym.sym <= SDLK_PAGEDOWN) {
				return_key = (int)event.key.keysym.sym;
			} else if (event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_DELETE) {
				// On MacOSX, the conversion of thse keys to unicode is buggy
				// so we directly return the keysym value
				return_key = (int)event.key.keysym.sym;
			} else if ((event.key.keysym.unicode & 0xFF00) != 0) {
				// This is a non ASCII character, we do not handle it
				continue;
			} else {
				return_key = (int)(event.key.keysym.unicode & 0x00FF);
			}

			// A key was pressed, stop the loop
			break;
		}
	}

	return return_key;
}

/**
 * Read a character from the keyboard and return its SDL virtual keysym
 * as well as the pressed modifier keys.
 *
 * The function returns when an 'actual' key is pressed (that is, not a modifier key)
 *
 * @param mod Pointer to an integer that will contain the state of the modifier keys
 *
 * @return SDL virtual keysym of the pressed key
 */

SDLKey getchar_raw(SDLMod *mod)
{
	SDL_Event event;
	int return_key = 0;

	while (1) {
		SDL_WaitEvent(&event);  /* wait for next event */

		if (event.type == SDL_QUIT) {
			Terminate(EXIT_SUCCESS);
		}

		if (event.type == SDL_KEYDOWN) {

			// If a modifier key was just pressed, wait for the next keypress
			if (SDLK_NUMLOCK <= event.key.keysym.sym && event.key.keysym.sym <= SDLK_COMPOSE) {
				continue;
			}

			fix_keysym(&event.key.keysym);
			return_key = event.key.keysym.sym;
			*mod = event.key.keysym.mod;

			break;
		}
	}

	return return_key;
}
