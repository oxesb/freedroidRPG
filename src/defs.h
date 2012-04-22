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
/* ----------------------------------------------------------------------
 * This file contains all the define-constants and macros
 * ---------------------------------------------------------------------- */
#ifndef _defs_h
#define _defs_h

//--------------------
// Some technical definitions...
//
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define ERR 	-1
#define OK		0

#ifndef max
#define max(x,y) ((x) < (y) ? (y) : (x) )
#endif
#ifndef min
#define min(x,y) ((x) > (y) ? (y) : (x) )
#endif

//--------------------
// Here you can control the debug level for the various modules, or at least
// that's the way it is planned to be.  Turn on and off debugging as you like.
//
#define SAVE_LOAD_GAME_DEBUG 1
enum {
	NO_NEED_TO_INFORM = 0,
	PLEASE_INFORM = 1
};
enum {
	IS_WARNING_ONLY = 0,
	IS_FATAL = 1
};
enum {
	MENU_MODE_FAST = 2,
	MENU_MODE_DEFAULT,
	MENU_MODE_DOUBLE,
};

enum {
	COLLISION_TYPE_NONE = 1,
	COLLISION_TYPE_RECTANGLE = 2,
	COLLISION_TYPE_CIRCLE = 3
};

enum {
	PART_GROUP_HEAD = 0,
	PART_GROUP_SHIELD = 1,
	PART_GROUP_TORSO = 2,
	PART_GROUP_FEET = 3,
	PART_GROUP_WEAPON = 4,
	PART_GROUP_WEAPONARM = 5,
	ALL_PART_GROUPS = 6
};

#define ENEMY_ROTATION_MODELS_AVAILABLE 42
#define ROTATION_ANGLES_PER_ROTATION_MODEL 8

#define MAX_ENEMY_MOVEMENT_PHASES 70
#define WALK_ANIMATION 113
#define ATTACK_ANIMATION 114
#define GETHIT_ANIMATION 115
#define DEATH_ANIMATION 116
#define DEAD_ANIMATION 118
#define STAND_ANIMATION 117

#define NUMBER_OF_SHADOW_IMAGES 20

#define MAX_OBSTACLES_ON_MAP 4000
#define MAX_OBSTACLE_NAMES_PER_LEVEL 100
#define MAX_OBSTACLE_DESCRIPTIONS_PER_LEVEL 100

#define FLOOR_TILES_VISIBLE_AROUND_TUX ((GameConfig . screen_width >= 1024 ? 13 : GameConfig . screen_width >= 800 ? 9 : 7))
#define MAX_ITEMS_PER_LEVEL 300
#define MAX_ITEMS_IN_INVENTORY 100
#define MAX_ITEMS_IN_NPC_SHOPLIST 50
#define INVENTORY_GRID_WIDTH 10
#define INVENTORY_GRID_HEIGHT 6
#define INV_SUBSQUARE_WIDTH (31)
#define INV_SUBSQUARE_HEIGHT 32
#define NUMBER_OF_MOUSE_CURSOR_PICTURES 2

// Balancing of loot and items.
#define MONEY_PER_BOT_CLASS 12
#define ITEM_DROP_PERCENTAGE 10
#define GOLD_DROP_PERCENTAGE 40
#define SOCKET_DROP_PERCENTAGE 20
#define ELECTRIC_SOCKET_COST 100
#define MECHANICAL_SOCKET_COST 100
#define UNIVERSAL_SOCKET_COST 400
#define ARMOUR_DURABILITYLOSS_PERCENTAGE_WHEN_HIT 30
#define WEAPON_DURABILITYLOSS_PERCENTAGE_WHEN_USED 10

#define CHAT_DEBUG_LEVEL 1

#define MAX_BIG_SCREEN_MESSAGES 10

#define NUMBER_OF_SKILL_LEVELS 10
#define NUMBER_OF_SKILLS_PER_SKILL_PAGE 5
#define MAX_ACTIVE_SPELLS 100
#define MAX_NUMBER_OF_PROGRAMS 50

enum upgrade_socket_types {
	UPGRADE_SOCKET_TYPE_MECHANICAL,
	UPGRADE_SOCKET_TYPE_ELECTRIC,
	UPGRADE_SOCKET_TYPE_UNIVERSAL
};

enum skill_forms {
	PROGRAM_FORM_INSTANT = 10,	//instant hit whatever behind cursor
	PROGRAM_FORM_RADIAL,	//nova
	PROGRAM_FORM_BULLET,	//fire a bullet
	PROGRAM_FORM_SELF,	//instant hit player
};

enum _game_mouse_cursors {
	MOUSE_CURSOR_NORMAL = 7124,
	MOUSE_CURSOR_SCROLL_UP = 7127,
	MOUSE_CURSOR_SCROLL_DOWN = 7128,
	MOUSE_CURSOR_REPAIR = 7129,
	MOUSE_CURSOR_SELECT_TOOL = 7130,
	MOUSE_CURSOR_DRAGDROP_TOOL = 7131,
};

enum _game_config_screen_visible_parameters {
	GAME_CONFIG_SCREEN_VISIBLE_INVENTORY = 8001,
	GAME_CONFIG_SCREEN_VISIBLE_SKILLS = 8002,
	GAME_CONFIG_SCREEN_VISIBLE_CHARACTER = 8003,
	GAME_CONFIG_SCREEN_VISIBLE_SKILL_EXPLANATION = 8004,
};

#define END_OF_OFFSET_FILE_STRING "** End of iso_image offset file **"
#define OFFSET_FILE_OFFSETX_STRING "OffsetX="
#define OFFSET_FILE_OFFSETY_STRING "OffsetY="

#define Set_Rect(rect, xx, yy, ww, hh) {\
(rect).x = (xx); (rect).y = (yy); (rect).w = (ww); (rect).h = (hh); }

#define Copy_Rect(src, dst) {\
(dst).x = (src).x; (dst).y = (src).y; (dst).w = (src).w; (dst).h = (src).h; }

#define UserCenter_x (User_Rect.x+User_Rect.w/2)
#define UserCenter_y (User_Rect.y+User_Rect.h/2)

#define MAX_CHARACTER_NAME_LENGTH 13

// The flags for AssembleCombatWindow are:
enum { ONLY_SHOW_MAP = 1, DO_SCREEN_UPDATE = 2, ONLY_SHOW_MAP_AND_TEXT = 4,
		SHOW_ITEMS = 8, OMIT_OBSTACLES = 16, OMIT_TUX = 32, OMIT_ENEMIES = 64, ZOOM_OUT = 128, OMIT_BLASTS = 256,
		SKIP_LIGHT_RADIUS = 512, NO_CURSOR = 1024, OMIT_ITEMS_LABEL = 2048 };

// Flags for obstacles
enum {
	/* Obstacle groups */
	IS_WALL = 1,

	/* Walls */
	IS_HORIZONTAL = 2,
	IS_VERTICAL = 4,
	CORNER_NE = 2048,
	CORNER_NW = 4096,
	CORNER_SE = 8192,
	CORNER_SW = 16384,

	/* Obstacle specs */
	BLOCKS_VISION_TOO = 8,	/* Light will not pass through this obstacle */
	IS_SMASHABLE = 16,
	DROPS_RANDOM_TREASURE = 32,
	NEEDS_PRE_PUT = 64,
	GROUND_LEVEL = 128,	/* This obstacle has a small height */
	IS_WALKABLE = 256,	/* A bot can traverse this obstacle */
	IS_CLICKABLE = 512,	/* Player can click on this obstacle */
	IS_VOLATILE = 1024	/* This obstacle vanishes when level is respawned. */
};

#define AUTOMAP_ZOOM_OUT_FACT 8.0

//--------------------
// Constants for Paths and names of Data-files
// the root "FD_DATADIR" should be defined in the Makefile as $(pkgdatadir)
// if not, we set it here:
//
#ifndef FD_DATADIR
#define FD_DATADIR ".."		// our local fallback
#endif

#define GRAPHICS_DIR		"graphics/"
#define SOUND_DIR		"sound/"
#define MAP_DIR			"map/"
#define TITLES_DIR		"map/titles/"
#define DIALOG_DIR		"dialogs/"

#define ICON_FILE		"paraicon.bmp"

//--------------------
#define SKILL_LEVEL_BUTTON_HEIGHT 32
#define SKILL_LEVEL_BUTTON_WIDTH 30

#define NUMBER_OF_ITEMS_ON_ONE_SCREEN 4
#define ITEM_MENU_DISTANCE 80
#define ITEM_FIRST_POS_Y 130

#define INITIAL_BLOCK_WIDTH	64
#define INITIAL_BLOCK_HEIGHT	64

#define TABCHAR 'W'
#define TABWIDTH 8
//--------------------
// Startpos + dimensions of Banner-Texts 
//
#define LEFT_INFO_X 	(13*2)
#define LEFT_INFO_Y	(10*2)
#define RIGHT_INFO_X	(242*2)
#define RIGHT_INFO_Y	(8*2)
#define LEFT_TEXT_LEN 20
#define RIGHT_TEXT_LEN 6
#define LINE_HEIGHT_FACTOR (6.0/5.0)

#define BACKGROUND_TEXT_RECT_ALPHA 110
#define TEXT_BANNER_HORIZONTAL_MARGIN 4

//--------------------
// dimensions of the droids as required for functions NotPassable() and CheckEnemyCollsion() )
//
#define DROIDRADIUSXY	        (7*2/64.0)
#define BULLET_BULLET_COLLISION_DIST (10/64.0)

#define TAKEOVER_BACKGROUND_MUSIC_SOUND "Bleostrada.ogg"
#define MENU_BACKGROUND_MUSIC_SOUND "menu.ogg"
#define BIGFIGHT_BACKGROUND_MUSIC_SOUND "hellforce.ogg"
#define SILENCE (NULL)
enum _sounds {
	ERRORSOUND = 0,
};

//--------------------
// The sounds when the influencers energy is low or when he is in transfer mode
// occur periodically.  These constants specify which intervals are to be used
// for these periodic happenings...
#define CRY_SOUND_INTERVAL 2

#define MAX_DIALOGUE_OPTIONS_IN_ROSTER 100
#define MAX_ANSWERS_PER_PERSON MAX_DIALOGUE_OPTIONS_IN_ROSTER

enum {
	UP_BUTTON,
	DOWN_BUTTON,
	ITEM_BROWSER_LEFT_BUTTON,
	ITEM_BROWSER_RIGHT_BUTTON,
	ITEM_BROWSER_EXIT_BUTTON,

	LEFT_SHOP_BUTTON,
	RIGHT_SHOP_BUTTON,
	LEFT_TUX_SHOP_BUTTON,
	RIGHT_TUX_SHOP_BUTTON,
	LEFT_LEVEL_EDITOR_BUTTON,
	LEFT_LEVEL_EDITOR_BUTTON_PUSHED,
	RIGHT_LEVEL_EDITOR_BUTTON,
	RIGHT_LEVEL_EDITOR_BUTTON_PUSHED,

	NUMBER_SELECTOR_OK_BUTTON,
	NUMBER_SELECTOR_LEFT_BUTTON,
	NUMBER_SELECTOR_RIGHT_BUTTON,

	BUY_BUTTON,
	SELL_BUTTON,
	REPAIR_BUTTON,

	OPEN_CLOSE_SKILL_EXPLANATION_BUTTON,

	EXPORT_THIS_LEVEL_BUTTON,
	EXPORT_THIS_LEVEL_BUTTON_PUSHED,
	LEVEL_EDITOR_SAVE_SHIP_BUTTON,
	LEVEL_EDITOR_SAVE_SHIP_BUTTON_PUSHED,
	LEVEL_EDITOR_SAVE_SHIP_BUTTON_OFF,
	LEVEL_EDITOR_SAVE_SHIP_BUTTON_OFF_PUSHED,

	LEVEL_EDITOR_DELETE_OBSTACLE_BUTTON,
	LEVEL_EDITOR_DELETE_OBSTACLE_BUTTON_PUSHED,
	LEVEL_EDITOR_NEXT_OBJECT_BUTTON,
	LEVEL_EDITOR_NEXT_OBJECT_BUTTON_PUSHED,

	LEVEL_EDITOR_TYPESELECT_OBSTACLE_BUTTON,
	LEVEL_EDITOR_TYPESELECT_OBSTACLE_BUTTON_PUSHED,
	LEVEL_EDITOR_TYPESELECT_OBSTACLE_BUTTON_OFF,
	LEVEL_EDITOR_TYPESELECT_OBSTACLE_BUTTON_OFF_PUSHED,
	LEVEL_EDITOR_TYPESELECT_ENEMY_BUTTON,
	LEVEL_EDITOR_TYPESELECT_ENEMY_BUTTON_PUSHED,
	LEVEL_EDITOR_TYPESELECT_ENEMY_BUTTON_OFF,
	LEVEL_EDITOR_TYPESELECT_ENEMY_BUTTON_OFF_PUSHED,
	LEVEL_EDITOR_TYPESELECT_FLOOR_BUTTON,
	LEVEL_EDITOR_TYPESELECT_FLOOR_BUTTON_PUSHED,
	LEVEL_EDITOR_TYPESELECT_FLOOR_BUTTON_OFF,
	LEVEL_EDITOR_TYPESELECT_FLOOR_BUTTON_OFF_PUSHED,
	LEVEL_EDITOR_TYPESELECT_ITEM_BUTTON,
	LEVEL_EDITOR_TYPESELECT_ITEM_BUTTON_PUSHED,
	LEVEL_EDITOR_TYPESELECT_ITEM_BUTTON_OFF,
	LEVEL_EDITOR_TYPESELECT_ITEM_BUTTON_OFF_PUSHED,
	LEVEL_EDITOR_TYPESELECT_WAYPOINT_BUTTON,
	LEVEL_EDITOR_TYPESELECT_WAYPOINT_BUTTON_PUSHED,
	LEVEL_EDITOR_TYPESELECT_WAYPOINT_BUTTON_OFF,
	LEVEL_EDITOR_TYPESELECT_WAYPOINT_BUTTON_OFF_PUSHED,

	LEVEL_EDITOR_TYPESELECT_MAP_LABEL_BUTTON,
	LEVEL_EDITOR_TYPESELECT_MAP_LABEL_BUTTON_PUSHED,
	LEVEL_EDITOR_TYPESELECT_MAP_LABEL_BUTTON_OFF,
	LEVEL_EDITOR_TYPESELECT_MAP_LABEL_BUTTON_OFF_PUSHED,

	LEVEL_EDITOR_BEAUTIFY_GRASS_BUTTON,
	LEVEL_EDITOR_BEAUTIFY_GRASS_BUTTON_PUSHED,
	LEVEL_EDITOR_ZOOM_IN_BUTTON,
	LEVEL_EDITOR_ZOOM_IN_BUTTON_PUSHED,
	LEVEL_EDITOR_ZOOM_OUT_BUTTON,
	LEVEL_EDITOR_ZOOM_OUT_BUTTON_PUSHED,
	LEVEL_EDITOR_NEW_OBSTACLE_LABEL_BUTTON,
	LEVEL_EDITOR_NEW_OBSTACLE_LABEL_BUTTON_PUSHED,
	LEVEL_EDITOR_EDIT_CHEST_BUTTON,
	LEVEL_EDITOR_EDIT_CHEST_BUTTON_PUSHED,
	LEVEL_EDITOR_QUIT_BUTTON,
	LEVEL_EDITOR_QUIT_BUTTON_PUSHED,
	LEVEL_EDITOR_TOGGLE_ENEMIES_BUTTON,
	LEVEL_EDITOR_TOGGLE_ENEMIES_BUTTON_PUSHED,
	LEVEL_EDITOR_TOGGLE_ENEMIES_BUTTON_OFF,
	LEVEL_EDITOR_TOGGLE_ENEMIES_BUTTON_OFF_PUSHED,
	LEVEL_EDITOR_TOGGLE_OBSTACLES_BUTTON,
	LEVEL_EDITOR_TOGGLE_OBSTACLES_BUTTON_PUSHED,
	LEVEL_EDITOR_TOGGLE_OBSTACLES_BUTTON_OFF,
	LEVEL_EDITOR_TOGGLE_OBSTACLES_BUTTON_OFF_PUSHED,
	LEVEL_EDITOR_TOGGLE_TOOLTIPS_BUTTON,
	LEVEL_EDITOR_TOGGLE_TOOLTIPS_BUTTON_PUSHED,
	LEVEL_EDITOR_TOGGLE_TOOLTIPS_BUTTON_OFF,
	LEVEL_EDITOR_TOGGLE_TOOLTIPS_BUTTON_OFF_PUSHED,
	LEVEL_EDITOR_TOGGLE_COLLISION_RECTS_BUTTON,
	LEVEL_EDITOR_TOGGLE_COLLISION_RECTS_BUTTON_PUSHED,
	LEVEL_EDITOR_TOGGLE_COLLISION_RECTS_BUTTON_OFF,
	LEVEL_EDITOR_TOGGLE_COLLISION_RECTS_BUTTON_OFF_PUSHED,
	LEVEL_EDITOR_TOGGLE_GRID_BUTTON_OFF,
	LEVEL_EDITOR_TOGGLE_GRID_BUTTON_OFF_PUSHED,
	LEVEL_EDITOR_TOGGLE_GRID_BUTTON,
	LEVEL_EDITOR_TOGGLE_GRID_BUTTON_PUSHED,
	LEVEL_EDITOR_TOGGLE_GRID_BUTTON_FULL,
	LEVEL_EDITOR_TOGGLE_GRID_BUTTON_FULL_PUSHED,
	LEVEL_EDITOR_TOGGLE_WAYPOINT_CONNECTIONS_BUTTON,
	LEVEL_EDITOR_TOGGLE_WAYPOINT_CONNECTIONS_BUTTON_PUSHED,
	LEVEL_EDITOR_TOGGLE_WAYPOINT_CONNECTIONS_BUTTON_OFF,
	LEVEL_EDITOR_TOGGLE_WAYPOINT_CONNECTIONS_BUTTON_OFF_PUSHED,
	LEVEL_EDITOR_ALL_FLOOR_LAYERS_BUTTON,
	LEVEL_EDITOR_ALL_FLOOR_LAYERS_BUTTON_PUSHED,
	LEVEL_EDITOR_SINGLE_FLOOR_LAYER_BUTTON,
	LEVEL_EDITOR_SINGLE_FLOOR_LAYER_BUTTON_PUSHED,

	LEVEL_EDITOR_NEXT_ITEM_GROUP_BUTTON,
	LEVEL_EDITOR_PREV_ITEM_GROUP_BUTTON,
	LEVEL_EDITOR_CANCEL_ITEM_DROP_BUTTON,
	LEVEL_EDITOR_UNDO_BUTTON,
	LEVEL_EDITOR_UNDO_BUTTON_PUSHED,
	LEVEL_EDITOR_REDO_BUTTON,
	LEVEL_EDITOR_REDO_BUTTON_PUSHED,

	WEAPON_RECT_BUTTON,
	DRIVE_RECT_BUTTON,
	SHIELD_RECT_BUTTON,
	HELMET_RECT_BUTTON,
	ARMOUR_RECT_BUTTON,

	SCROLL_DIALOG_MENU_UP_BUTTON,
	SCROLL_DIALOG_MENU_DOWN_BUTTON,

	MORE_STR_BUTTON,
	MORE_MAG_BUTTON,
	MORE_DEX_BUTTON,
	MORE_VIT_BUTTON,

	SCROLL_TEXT_UP_BUTTON,
	SCROLL_TEXT_DOWN_BUTTON,

	DESCRIPTION_WINDOW_UP_BUTTON,
	DESCRIPTION_WINDOW_DOWN_BUTTON,

	DROID_SHOW_EXIT_BUTTON,

	CHAT_LOG_SCROLL_UP_BUTTON,
	CHAT_LOG_SCROLL_DOWN_BUTTON,
	CHAT_LOG_SCROLL_OFF_BUTTON,
	CHAT_LOG_SCROLL_OFF2_BUTTON,

	QUEST_BROWSER_ITEM_SHORT_BUTTON,
	QUEST_BROWSER_ITEM_LONG_BUTTON,

	TAKEOVER_HELP_BUTTON,

	WEAPON_MODE_BUTTON,
	SKI_ICON_BUTTON,

	ITEM_UPGRADE_APPLY_BUTTON,
	ITEM_UPGRADE_APPLY_BUTTON_DISABLED,
	ITEM_UPGRADE_CLOSE_BUTTON,

	ADDON_CRAFTING_APPLY_BUTTON,
	ADDON_CRAFTING_APPLY_BUTTON_DISABLED,
	ADDON_CRAFTING_CLOSE_BUTTON,
	ADDON_CRAFTING_SCROLL_UP_BUTTON,
	ADDON_CRAFTING_SCROLL_DOWN_BUTTON,
	ADDON_CRAFTING_SCROLL_DESC_UP_BUTTON,
	ADDON_CRAFTING_SCROLL_DESC_DOWN_BUTTON,

	//--------------------
	// Please leave this here as the last entry, since it conveniently and
	// automatically counts the number of buttons defined.  Other buttons
	// can be inserted above.
	//
	MAX_MOUSE_PRESS_BUTTONS
};

#define CHAT_SUBDIALOG_WINDOW_X ((260)*GameConfig . screen_width/640.0)
#define CHAT_SUBDIALOG_WINDOW_Y ((35)*GameConfig . screen_height/480.0)
#define CHAT_SUBDIALOG_WINDOW_W ((337)*GameConfig . screen_width/640.0)
#define CHAT_SUBDIALOG_WINDOW_H ((225)*GameConfig . screen_height/480.0)

#define STR_X 90
#define STR_Y 140
#define DEX_Y 165
#define VIT_Y 190
#define MAG_Y 215

#define BUTTON_WIDTH 35
#define BUTTON_HEIGHT 19

#define DEFAULT_SCREEN_WIDTH  (800)
#define DEFAULT_SCREEN_HEIGHT (600)
#define CHARACTERRECT_X (GameConfig.screen_width - 320)
#define CHARACTERRECT_W (320)
#define CHARACTERRECT_H (480)

#define ITEM_UPGRADE_RECT_X 320
#define ITEM_UPGRADE_RECT_Y 0
#define ITEM_UPGRADE_RECT_W 320
#define ITEM_UPGRADE_RECT_H 480

#define ADDON_CRAFTING_RECT_X 0
#define ADDON_CRAFTING_RECT_Y 0
#define ADDON_CRAFTING_RECT_W 320
#define ADDON_CRAFTING_RECT_H 480

// Number of keyframes in Tux image_archive files
#define TUX_TOTAL_PHASES 35

// The speed is measured in tiles per second.
#define TUX_RUNNING_SPEED 5.5
#define TUX_WALKING_SPEED 2.5

#define MAX_TUX_DIRECTIONS      16
#define RADIAL_SPELL_DIRECTIONS 16
#define BULLET_DIRECTIONS       16

#define WAIT_AFTER_KILLED	3.0	// time to wait and still display pictures after the destruction of
				     // the players droid.  This is now measured in seconds and can be a float
#define WAIT_AFTER_GAME_WON	5.0	// how long to show tux after the game is won
#define WAIT_COLLISION		1	// after a little collision with Tux or another enemy, hold position for a while
				    // this variable describes the amount of time in SECONDS 
#define WAIT_BEFORE_ROTATE 0.7 // Wait a bit to avoid jittering between two rotation angles

#define ALLBLASTTYPES		3	/* number of different explosions */

#define MAX_MELEE_SHOTS		100
#define MAXBULLETS		100	/* maximum possible Bullets in the air */
#define MAXBLASTS		100	/* max. possible Blasts visible */
#define ATTACK_MOVE_RATE 0.2	// Minimum number of seconds between to displacement computation
			     // during attack

#define DEFAULT_BULLET_LIFETIME 5.0	// prevent bullets to infinitely move (this is merely a protection against bugs)

//--------------------
// Map- and levelrelated defines
// WARNING leave them here, they are required in struct.h
//

#define MIN_MAP_LINES		30
#define MAX_MAP_LINES 		100
#define MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX 100

#define EW_WALL_BIT 1
#define NS_WALL_BIT 2
#define VISIBLE_EVENT_BIT 4
#define SQUARE_SEEN_AT_ALL_BIT 8
#define UPDATE_SQUARE_BIT 16

#define MAX_SAVED_CHARACTERS_ON_DISK 7	// Not actually amount of chars, but amount "per page"

#define MAX_INFLU_POSITION_HISTORY 500
#define MAX_MISSIONS_IN_GAME 50	// how many missions can there be at most in FreedroidRPG
#define MAX_MISSION_DESCRIPTION_TEXTS 25

#define MAX_LEVELS		100	// how many map levels are allowed in one ship

#define MAX_PHASES_IN_A_BULLET 12

#define UNIVERSAL_COORD_W(W) (int)((float)(W) * ((float)(GameConfig . screen_width) / 640.0))
#define UNIVERSAL_COORD_H(H) (int)((float)(H) * ((float)(GameConfig . screen_height) / 480.0))

//--------------------
// Rectangle definitions

// the inventory rect is the rectangle inside the inventory screen,
// where all the items "in the backpack" including the quick inventory
// are displayed.  it's relative to the top left corner, as always.
//

#define INVENTORY_RECT_Y 260
#define INVENTORY_RECT_X 8

// 388 is the maximum y size to fit items up to 6 inventory squares high
// 398 is the maximum y size to fit items up to 5 inventory squares high

//--------------------
// The weapon rectangle is of course for the weapon the Tux is
// using.  When using a 2-handed weapon, the 'shield' rectangle
// will ALSO be used to show this weapon, since the shield-hand
// is also needed to wield such a thing.
//
// 31 is the minimum x size to fit items up to 4 inventory squares wide
#define WEAPON_RECT_WIDTH 67
#define WEAPON_RECT_HEIGHT 112
#define WEAPON_RECT_X 31
#define WEAPON_RECT_Y 117

//--------------------
// The armor rectangle is where you can place some armor
// for the body of the Tux.
//
#define ARMOUR_RECT_WIDTH 66
#define ARMOUR_RECT_HEIGHT 92
#define ARMOUR_RECT_X 130
#define ARMOUR_RECT_Y 70

//--------------------
// The shield rectangle is obviously for the shields, but not only
// that:  when using a 2-handed weapon, its also good for the weapon
// again, to indicate, that no shield can be used in conjunction with
// such a 2-handed weapon.
//
#define SHIELD_RECT_WIDTH 70
#define SHIELD_RECT_HEIGHT 112
#define SHIELD_RECT_X 230
#define SHIELD_RECT_Y 116

//--------------------
// The 'drive' rectangle is actually for the footwear.
//
#define DRIVE_RECT_WIDTH 64
#define DRIVE_RECT_HEIGHT 64
#define DRIVE_RECT_X 130
#define DRIVE_RECT_Y 175

//--------------------
// Now this is the 'helmet' rectangle.
//
#define HELMET_RECT_WIDTH 64
#define HELMET_RECT_HEIGHT 64
#define HELMET_RECT_X 25
#define HELMET_RECT_Y 22

// Flags for thrown items. see PutItem function in view.c
#define PUT_ONLY_THROWN_ITEMS 3
#define PUT_NO_THROWN_ITEMS 4

//--------------------
// item qualities
enum item_quality {
	NORMAL_QUALITY = 0,
	BAD_QUALITY = 1,
	GOOD_QUALITY = 2
};

#define MAX_BULLET_DEFS 16

//--------------------
// explosion types
//
enum _explosions {
	BULLETBLAST = 0,
	DROIDBLAST,
	EXTERMINATORBLAST,
};

enum _status {
	INFOUT = -30,
};

enum _busytype {
	NONE = 0,
	DRINKING_POTION = 1,
	WEAPON_FIREWAIT,
	WEAPON_RELOAD,
	THROWING_GRENADE,
	RUNNING_PROGRAM,
};

enum _attackhit {
	ATTACK_HIT_BOTS = 1,
	ATTACK_HIT_HUMANS = 2,
	ATTACK_HIT_ALL = 3,
};

// possible directions for a line of walls
// We use 1 et -1, because north is the opposite of south.
enum _level_editor_directions {
	UNDEFINED = 0,
	NORTH = 1,
	EAST = 2,
	SOUTH = -1,
	WEST = -2
};

enum _enemy_combat_state_machine_states {
	MOVE_ALONG_RANDOM_WAYPOINTS = 0,
	STOP_AND_EYE_TARGET = 1,
	ATTACK = 2,
	PARALYZED = 3,
	COMPLETELY_FIXED = 4,
	FOLLOW_TUX = 5,
	RETURNING_HOME = 6,
	SELECT_NEW_WAYPOINT = 7,
	TURN_TOWARDS_NEXT_WAYPOINT = 8,
	RUSH_TUX_AND_OPEN_TALK = 9,
	WAYPOINTLESS_WANDERING = 10,
	UNDEFINED_STATE = 100
};
enum _enemy_combat_target_codes {
	ATTACK_TARGET_IS_ENEMY = 101,
	ATTACK_TARGET_IS_PLAYER = 102,
	ATTACK_TARGET_IS_NOTHING = 103
};
enum {
	MOUSE_CURSOR_ARROW_SHAPE = -402
};
enum {
	TRANSPARENCY_NONE = 0,
	TRANSPARENCY_FOR_WALLS = 10
};

//--------------------
// It is possible, that the Tux must fist move somewhere AND
// then open a chest or move somewhere AND then pick something up
// or the like.  These are called 'combo_actions' and some definition
// has to be made about the type of current combo_action.
//
enum _combo_action_types {
	NO_COMBO_ACTION_SET = -10,
	COMBO_ACTION_OBSTACLE = 15,
	COMBO_ACTION_PICK_UP_ITEM = 16,
};

//--------------------
// For shop interfaces and chest interfaces we need some constants to
// transmit the intended shop/chest action.
//
enum {
	DO_NOTHING = -1,
	BUY_1_ITEM = 1,
	SELL_1_ITEM = 4,
	REPAIR_ITEM = 12,
};

enum obstacle_types {

	ISO_TREE_4 = 0,

	ISO_V_WALL = 1,
	ISO_H_WALL = 2,
	ISO_V_WALL_WITH_DOT = 3,
	ISO_H_WALL_WITH_DOT = 4,

	ISO_TREE_5 = 5,

	ISO_H_DOOR_000_OPEN = 6,
	ISO_H_DOOR_025_OPEN = 7,
	ISO_H_DOOR_050_OPEN = 8,
	ISO_H_DOOR_075_OPEN = 9,
	ISO_H_DOOR_100_OPEN = 10,
	ISO_V_DOOR_000_OPEN = 11,
	ISO_V_DOOR_025_OPEN = 12,
	ISO_V_DOOR_050_OPEN = 13,
	ISO_V_DOOR_075_OPEN = 14,
	ISO_V_DOOR_100_OPEN = 15,

	ISO_TELEPORTER_1 = 16,

	ISO_REFRESH_1 = 21,

	ISO_H_DOOR_LOCKED = 26,
	ISO_V_DOOR_LOCKED = 27,

	ISO_H_CHEST_CLOSED = 28,
	ISO_V_CHEST_CLOSED = 29,
	ISO_H_CHEST_OPEN = 30,
	ISO_V_CHEST_OPEN = 31,

	ISO_AUTOGUN_W = 32,
	ISO_AUTOGUN_N = 33,
	ISO_AUTOGUN_E = 34,
	ISO_AUTOGUN_S = 35,
	//see at the end for disabled autoguns

	ISO_CAVE_WALL_H = 36,
	ISO_CAVE_WALL_V = 37,
	ISO_CAVE_CORNER_NE = 38,
	ISO_CAVE_CORNER_SE = 39,
	ISO_CAVE_CORNER_NW = 40,
	ISO_CAVE_CORNER_SW = 41,

	ISO_COOKING_POT = 42,

	ISO_CONSOLE_S = 43,
	ISO_CONSOLE_E = 44,
	ISO_CONSOLE_N = 45,
	ISO_CONSOLE_W = 46,

	ISO_PILLAR_TALL = 47,
	ISO_PILLAR_SHORT = 48,
	ISO_TV_PILLAR_W = 49,

	ISO_BARREL_1 = 50,
	ISO_BARREL_2 = 51,
	ISO_BARREL_3 = 52,
	ISO_BARREL_4 = 53,

	ISO_LAMP_S,

	ISO_ENHANCER_RU = 57,
	ISO_ENHANCER_LU = 56,
	ISO_ENHANCER_RD = 58,
	ISO_ENHANCER_LD = 55,

	ISO_V_WOOD_FENCE = 59,
	ISO_H_WOOD_FENCE = 60,

	ISO_V_MESH_FENCE = 63,
	ISO_H_MESH_FENCE = 64,
	ISO_V_WIRE_FENCE = 65,
	ISO_H_WIRE_FENCE = 66,

	//--------------------
	// These obstacles are generated from 'commercial_models_for_furniture.blend' file
	// which is currently not in the cvs rep. for licensing issues, but I'll send a
	// version to any FreedroidRPG co-worker involved with the graphics
	//
	ISO_N_TOILET_SMALL = 67,
	ISO_E_TOILET_SMALL,
	ISO_S_TOILET_WHITE_SMALL,
	ISO_W_TOILET_WHITE_SMALL,
	ISO_N_TOILET_BIG,
	ISO_E_TOILET_BIG,
	ISO_S_TOILET_BIG,
	ISO_W_TOILET_BIG,
	ISO_N_CHAIR,
	ISO_E_CHAIR,
	ISO_S_CHAIR,
	ISO_W_CHAIR,
	ISO_N_DESK,
	ISO_E_DESK,
	ISO_S_DESK,
	ISO_W_DESK,
	ISO_N_SCHOOL_CHAIR,
	ISO_E_SCHOOL_CHAIR,
	ISO_S_SCHOOL_CHAIR,
	ISO_W_SCHOOL_CHAIR,

	//--------------------
	// These obstacles are generated from 'commercial_models_for_furniture2.blend' file
	// which is currently not in the cvs rep. for licensing issues, but I'll send a
	// version to any FreedroidRPG co-worker involved with the graphics
	//
	ISO_N_BED = 87,
	ISO_E_BED,
	ISO_S_BED,
	ISO_W_BED,

	// Except these ones which were done by Basse :)
	ISO_SHELF_FULL_V,
	ISO_SHELF_FULL_H,
	ISO_SHELF_EMPTY_V,
	ISO_SHELF_EMPTY_H,
	ISO_SHELF_SMALL_FULL_V,
	ISO_SHELF_SMALL_FULL_H,
	ISO_SHELF_SMALL_EMPTY_V,
	ISO_SHELF_SMALL_EMPTY_H,

	ISO_N_FULL_PARK_BENCH,
	ISO_E_FULL_PARK_BENCH,
	ISO_S_FULL_PARK_BENCH,
	ISO_W_FULL_PARK_BENCH,

	//--------------------
	// These obstacles are generated from 'commercial_models_for_furniture3.blend' file
	// which is currently not in the cvs rep. for licensing issues, but I'll send a
	// version to any FreedroidRPG co-worker involved with the graphics
	//
	ISO_H_BATHTUB = 103,
	ISO_V_BATHTUB,
	ISO_H_WASHTUB,
	ISO_V_WASHTUB,
	ISO_V_CURTAIN,
	ISO_H_CURTAIN,
	ISO_E_SOFA,
	ISO_S_SOFA,
	ISO_W_SOFA,
	ISO_N_SOFA,

	//--------------------
	// Here we insert a few trees generated with the LSystem python script.  wow!
	//
	ISO_TREE_1,		// = 113  ,
	ISO_TREE_2,
	ISO_TREE_3,

	ISO_THICK_WALL_H,	// = 116 ,
	ISO_THICK_WALL_V,
	ISO_THICK_WALL_CORNER_NE,
	ISO_THICK_WALL_CORNER_SE,
	ISO_THICK_WALL_CORNER_NW,
	ISO_THICK_WALL_CORNER_SW,
	ISO_THICK_WALL_T_N,
	ISO_THICK_WALL_T_E,
	ISO_THICK_WALL_T_S,
	ISO_THICK_WALL_T_W,

	ISO_CAVE_WALL_END_W,	// = 126 ,
	ISO_CAVE_WALL_END_N,
	ISO_CAVE_WALL_END_E,
	ISO_CAVE_WALL_END_S,

	ISO_GREY_WALL_END_W,	// = 130 ,
	ISO_GREY_WALL_END_N,
	ISO_GREY_WALL_END_E,
	ISO_GREY_WALL_END_S,

	ISO_BRICK_WALL_H,	// = 134 ,
	ISO_BRICK_WALL_V,
	ISO_BRICK_WALL_END,

	ISO_BRICK_WALL_CORNER_NE,	// = 137 , 
	ISO_BRICK_WALL_CORNER_SW,
	ISO_BRICK_WALL_CORNER_NW,
	ISO_BRICK_WALL_CORNER_SE,

	ISO_BLOOD_1,		// = 141 ,
	ISO_BLOOD_2,
	ISO_BLOOD_3,
	ISO_BLOOD_4,
	ISO_BLOOD_5,
	ISO_BLOOD_6,
	ISO_BLOOD_7,
	ISO_BLOOD_8,

	ISO_EXIT_1,		// = 149 ,
	ISO_EXIT_2,

	ISO_ROCKS_N_PLANTS_1,	// = 151 ,
	ISO_ROCKS_N_PLANTS_2,
	ISO_ROCKS_N_PLANTS_3,
	ISO_ROCKS_N_PLANTS_4,

	ISO_ROOM_WALL_V_RED,	// = 155 ,
	ISO_ROOM_WALL_H_RED,
	ISO_ROOM_WALL_V_GREEN,
	ISO_ROOM_WALL_H_GREEN,

	ISO_SHOP_FURNITURE_1,	// = 159 ,
	ISO_SHOP_FURNITURE_2,
	ISO_SHOP_FURNITURE_3,
	ISO_SHOP_FURNITURE_4,
	ISO_SHOP_FURNITURE_5,
	ISO_SHOP_FURNITURE_6,

	ISO_OUTER_WALL_N1,	// = 165 ,
	ISO_OUTER_WALL_N2,
	ISO_OUTER_WALL_N3,
	ISO_OUTER_WALL_S1,
	ISO_OUTER_WALL_S2,
	ISO_OUTER_WALL_S3,
	ISO_OUTER_WALL_E1,
	ISO_OUTER_WALL_E2,
	ISO_OUTER_WALL_E3,
	ISO_OUTER_WALL_W1,
	ISO_OUTER_WALL_W2,
	ISO_OUTER_WALL_W3,

	ISO_OUTER_WALL_CORNER_NW,	// = 177 ,
	ISO_OUTER_WALL_CORNER_SW,
	ISO_OUTER_WALL_CORNER_SE,
	ISO_OUTER_WALL_CORNER_NE,

	ISO_OUTER_DOOR_V_00,	// = 181 ,
	ISO_OUTER_DOOR_V_25,
	ISO_OUTER_DOOR_V_50,
	ISO_OUTER_DOOR_V_75,
	ISO_OUTER_DOOR_V_100,

	ISO_OUTER_DOOR_H_00,
	ISO_OUTER_DOOR_H_25,
	ISO_OUTER_DOOR_H_50,
	ISO_OUTER_DOOR_H_75,
	ISO_OUTER_DOOR_H_100,

	ISO_OUTER_DOOR_V_LOCKED,	// =191
	ISO_OUTER_DOOR_H_LOCKED,

	ISO_TV_PILLAR_N,	// =193,
	ISO_TV_PILLAR_E,
	ISO_TV_PILLAR_S,

	ISO_YELLOW_CHAIR_N,	// = 196
	ISO_YELLOW_CHAIR_E,
	ISO_YELLOW_CHAIR_S,
	ISO_YELLOW_CHAIR_W,
	ISO_RED_CHAIR_N,	// = 200
	ISO_RED_CHAIR_E,
	ISO_RED_CHAIR_S,
	ISO_RED_CHAIR_W,

	ISO_BODY_RED_GUARD_N,	// 204
	ISO_BODY_RED_GUARD_E,
	ISO_BODY_RED_GUARD_S,
	ISO_BODY_RED_GUARD_W,

	ISO_CONFERENCE_TABLE_N,	//208
	ISO_CONFERENCE_TABLE_E,
	ISO_CONFERENCE_TABLE_S,
	ISO_CONFERENCE_TABLE_W,

	ISO_RED_FENCE_V,	//212
	ISO_RED_FENCE_H,
	ISO_BED_1,
	ISO_BED_2,
	ISO_BED_3,
	ISO_BED_4,
	ISO_BED_5,
	ISO_BED_6,
	ISO_BED_7,
	ISO_BED_8,
	ISO_PROJECTOR_E,	// 222
	ISO_PROJECTOR_W,

	ISO_LAMP_E,
	ISO_LAMP_N,
	ISO_LAMP_W,

	ISO_ROCKS_N_PLANTS_5,	//227
	ISO_ROCKS_N_PLANTS_6,
	ISO_ROCKS_N_PLANTS_7,
	ISO_ROCKS_N_PLANTS_8,

	ISO_BRICK_WALL_JUNCTION_1,	//231
	ISO_BRICK_WALL_JUNCTION_2,
	ISO_BRICK_WALL_JUNCTION_3,
	ISO_BRICK_WALL_JUNCTION_4,
	ISO_BRICK_WALL_CRACKED_1,
	ISO_BRICK_WALL_CRACKED_2,
	ISO_BRICK_WALL_RUBBLE_1,
	ISO_BRICK_WALL_RUBBLE_2,

	ISO_PROJECTOR_SCREEN_N,	// 239
	ISO_PROJECTOR_SCREEN_E,
	ISO_PROJECTOR_SCREEN_S,
	ISO_PROJECTOR_SCREEN_W,

	ISO_PROJECTOR_N,	// 243
	ISO_PROJECTOR_S,

	ISO_SIGN_1,		//245
	ISO_SIGN_2,
	ISO_SIGN_3,

	ISO_LIGHT_GREEN_WALL_1,	// 248
	ISO_LIGHT_GREEN_WALL_2,
	ISO_FUNKY_WALL_1,
	ISO_FUNKY_WALL_2,
	ISO_FUNKY_WALL_3,
	ISO_FUNKY_WALL_4,

	ISO_COUNTER_MIDDLE_1,
	ISO_COUNTER_MIDDLE_2,
	ISO_COUNTER_MIDDLE_3,
	ISO_COUNTER_MIDDLE_4,

	ISO_COUNTER_CORNER_ROUND_1,
	ISO_COUNTER_CORNER_ROUND_2,
	ISO_COUNTER_CORNER_ROUND_3,
	ISO_COUNTER_CORNER_ROUND_4,

	ISO_COUNTER_CORNER_SHARP_1,
	ISO_COUNTER_CORNER_SHARP_2,
	ISO_COUNTER_CORNER_SHARP_3,
	ISO_COUNTER_CORNER_SHARP_4,

	ISO_LIBRARY_FURNITURE_1,
	ISO_LIBRARY_FURNITURE_2,

	ISO_3_BATHTUB,
	ISO_4_BATHTUB,
	ISO_BAR_TABLE,

	ISO_EXIT_3,
	ISO_EXIT_4,

	ISO_OUTER_WALL_SMALL_CORNER_1,
	ISO_OUTER_WALL_SMALL_CORNER_2,
	ISO_OUTER_WALL_SMALL_CORNER_3,
	ISO_OUTER_WALL_SMALL_CORNER_4,

	ISO_TABLE_OVAL_1,
	ISO_TABLE_OVAL_2,
	ISO_TABLE_GLASS_1,
	ISO_TABLE_GLASS_2,

	ISO_GLASS_WALL_1,
	ISO_GLASS_WALL_2,

	ISO_CYAN_WALL_WINDOW_1,
	ISO_CYAN_WALL_WINDOW_2,
	ISO_RED_WALL_WINDOW_1,
	ISO_RED_WALL_WINDOW_2,
	ISO_FLOWER_WALL_WINDOW_1,
	ISO_FLOWER_WALL_WINDOW_2,
	ISO_FUNKY_WALL_WINDOW_1,
	ISO_FUNKY_WALL_WINDOW_2,

	ISO_RESTAURANT_SHELVES_1,
	ISO_RESTAURANT_SHELVES_2,
	ISO_RESTAURANT_SHELVES_3,
	ISO_RESTAURANT_SHELVES_4,
	ISO_RESTAURANT_SHELVES_5,
	ISO_RESTAURANT_SHELVES_6,
	ISO_RESTAURANT_SHELVES_7,
	ISO_RESTAURANT_SHELVES_8,
	ISO_RESTAURANT_SHELVES_9,
	ISO_RESTAURANT_SHELVES_10,

	ISO_SOFFA_1,
	ISO_SOFFA_2,
	ISO_SOFFA_3,
	ISO_SOFFA_4,
	ISO_SOFFA_CORNER_1,
	ISO_SOFFA_CORNER_2,
	ISO_SOFFA_CORNER_3,
	ISO_SOFFA_CORNER_4,
	ISO_SOFFA_CORNER_PLANT_1,
	ISO_SOFFA_CORNER_PLANT_2,
	ISO_SOFFA_CORNER_PLANT_3,
	ISO_SOFFA_CORNER_PLANT_4,

	ISO_OIL_STAINS_1,
	ISO_OIL_STAINS_2,
	ISO_OIL_STAINS_3,
	ISO_OIL_STAINS_4,
	ISO_OIL_STAINS_5,
	ISO_OIL_STAINS_6,
	ISO_OIL_STAINS_7,
	ISO_OIL_STAINS_8,

	ISO_TRANSP_FOR_WATER,

	ISO_BRICK_WALL_EH,	// = 322 ,
	ISO_BRICK_WALL_EV,

	ISO_DIS_AUTOGUN_W,
	ISO_DIS_AUTOGUN_N,
	ISO_DIS_AUTOGUN_E,
	ISO_DIS_AUTOGUN_S,

	ISO_BRICK_WALL_CABLES_H,
	ISO_BRICK_WALL_CABLES_V,

	ISO_BRICK_WALL_CABLES_CORNER_NE,
	ISO_BRICK_WALL_CABLES_CORNER_SW,
	ISO_BRICK_WALL_CABLES_CORNER_NW,
	ISO_BRICK_WALL_CABLES_CORNER_SE,

	ISO_RESTAURANT_DESK_1,
	ISO_RESTAURANT_DESK_2,
	ISO_RESTAURANT_BIGSHELF_1,
	ISO_RESTAURANT_BIGSHELF_2,

	ISO_CRYSTALS_1,
	ISO_CRYSTALS_2,
	ISO_CRYSTALS_3,
	ISO_CRYSTALS_4,
	ISO_CRYSTALS_5,
	ISO_CRYSTALS_6,

	ISO_GREY_WALL_CORNER_1,
	ISO_GREY_WALL_CORNER_2,
	ISO_GREY_WALL_CORNER_3,
	ISO_GREY_WALL_CORNER_4,	// = 347

	ISO_BROKEN_GLASS_WALL_1,
	ISO_OUTER_DOOR_V_OFFLINE,
	ISO_OUTER_DOOR_H_OFFLINE,
	ISO_DH_DOOR_LOCKED,
	ISO_DV_DOOR_LOCKED,

	ISO_DH_DOOR_000_OPEN,
	ISO_DH_DOOR_025_OPEN,
	ISO_DH_DOOR_050_OPEN,
	ISO_DH_DOOR_075_OPEN,
	ISO_DH_DOOR_100_OPEN,

	ISO_DV_DOOR_000_OPEN,
	ISO_DV_DOOR_025_OPEN,
	ISO_DV_DOOR_050_OPEN,
	ISO_DV_DOOR_075_OPEN,
	ISO_DV_DOOR_100_OPEN,	// = 362

	ISO_BASIN_1,
	ISO_BASIN_2,
	ISO_BASIN_3,
	ISO_BASIN_4,

	ISO_DESKCHAIR_1,
	ISO_DESKCHAIR_2,
	ISO_DESKCHAIR_3,

	ISO_EXIT_5,
	ISO_EXIT_6,
	
	ISO_E_CHEST2_CLOSED,  // = 372
	ISO_S_CHEST2_CLOSED,
	ISO_E_CHEST2_OPEN,
	ISO_S_CHEST2_OPEN,
	ISO_N_CHEST2_CLOSED, // = 376
	ISO_W_CHEST2_CLOSED,
	ISO_N_CHEST2_OPEN,
	ISO_W_CHEST2_OPEN, // = 379

	ISO_SECURITY_GATE_GREEN_E,
	ISO_SECURITY_GATE_GREEN_S,
	ISO_SECURITY_GATE_RED_E,
	ISO_SECURITY_GATE_RED_S,
	ISO_SECURITY_GATE_OPEN_E,
	ISO_SECURITY_GATE_OPEN_S,
	ISO_SECURITY_GATE_CLOSED_E,
	ISO_SECURITY_GATE_CLOSED_S,
	ISO_SOLAR_PANEL_E,
	ISO_BOTLINE_01_N,
	ISO_BOTLINE_01_E,
	ISO_BOTLINE_02_E,
	ISO_BOTLINE_02_N,
	ISO_BOTLINE_02_W,
	ISO_BOTLINE_02_S,
	ISO_BOTLINE_06_N,
	ISO_BOTLINE_06_W,
	ISO_BOTLINE_06_S,
	ISO_BOTLINE_06_E,
	ISO_BOTLINE_07_N,
	ISO_BOTLINE_07_W,
	ISO_FREIGHTER_RAILWAY_01_N,
	ISO_FREIGHTER_RAILWAY_01_E,
	ISO_FREIGHTER_RAILWAY_02_S,
	ISO_FREIGHTER_RAILWAY_02_E,
	ISO_FREIGHTER_RAILWAY_02_N,
	ISO_FREIGHTER_RAILWAY_02_W,
	ISO_SOLAR_PANEL_BROKEN,
	ISO_BARREL_5,
	ISO_REACTOR_S,
	ISO_REACTOR_E,
	ISO_REACTOR_N,
	ISO_REACTOR_W,
	ISO_WALL_TERMINAL_S,
	ISO_WALL_TERMINAL_E,
	ISO_WALL_TERMINAL_N,
	ISO_WALL_TERMINAL_W,
	ISO_TURBINES_SMALL_W,
	ISO_TURBINES_SMALL_N,
	ISO_TURBINES_SMALL_E,
	ISO_TURBINES_SMALL_S,
	ISO_WEAPON_CRATE,
	ISO_DEFAULT_DEAD_BODY_0_1,
	ISO_DEFAULT_DEAD_BODY_1_1,
	ISO_DEFAULT_DEAD_BODY_2_1,
	ISO_DEFAULT_DEAD_BODY_3_1,
	ISO_DEFAULT_DEAD_BODY_4_1,
	ISO_DEFAULT_DEAD_BODY_5_1,
	ISO_DEFAULT_DEAD_BODY_6_1,
	ISO_DEFAULT_DEAD_BODY_7_1,
	ISO_DEFAULT_DEAD_BODY_0_2,
	ISO_LADDER_1,
	ISO_LADDER_2,
	ISO_WRECKED_CAR_1,
	ISO_WRECKED_CAR_2,
	ISO_WRECKED_CAR_3,
	ISO_WRECKED_CAR_4,
	ISO_N_TOILET_WHITE_SMALL,
	ISO_E_TOILET_WHITE_SMALL,
	ISO_BOTLINE_07_S,
	ISO_BOTLINE_07_E,
	ISO_BOTLINE_08_N,
	ISO_BOTLINE_08_W,
	ISO_BOTLINE_08_S,
	ISO_BOTLINE_08_E,
	ISO_CRUSHED_476,
	ISO_BROKEN_GLASS_WALL_2,
	ISO_SIGN_1_FLASH,
	ISO_SIGN_2_FLASH,
	ISO_SIGN_3_FLASH,
	ISO_BARREL_RADIOACTIVE,
	ISO_VENDING_MACHINE_1_E,
	ISO_VENDING_MACHINE_1_N,
	ISO_VENDING_MACHINE_1_W,
	ISO_VENDING_MACHINE_1_S,
	ISO_VENDING_MACHINE_2_E,
	ISO_VENDING_MACHINE_2_N,
	ISO_VENDING_MACHINE_2_W,
	ISO_VENDING_MACHINE_2_S,
	ISO_VENDING_MACHINE_3_E,
	ISO_VENDING_MACHINE_3_N,
	ISO_VENDING_MACHINE_3_W,
	ISO_VENDING_MACHINE_3_S,
	ISO_VENDING_MACHINE_4_E,
	ISO_VENDING_MACHINE_4_N,
	ISO_VENDING_MACHINE_4_W,
	ISO_VENDING_MACHINE_4_S
};

enum {
	ISO_FLOOR_EMPTY,
	ISO_FLOOR_ERROR_TILE,
	ISO_FLOOR_STONE_FLOOR,
	ISO_FLOOR_STONE_FLOOR_WITH_DOT,
	ISO_FLOOR_STONE_FLOOR_WITH_GRATE,
	ISO_FLOOR_HOUSE_FLOOR,

	ISO_RED_WAREHOUSE_FLOOR,

	ISO_MISCELLANEOUS_FLOOR_9,
	ISO_MISCELLANEOUS_FLOOR_10,
	ISO_MISCELLANEOUS_FLOOR_11,
	ISO_MISCELLANEOUS_FLOOR_12,
	ISO_MISCELLANEOUS_FLOOR_13,
	ISO_MISCELLANEOUS_FLOOR_14,
	ISO_MISCELLANEOUS_FLOOR_15,
	ISO_MISCELLANEOUS_FLOOR_16,
	ISO_MISCELLANEOUS_FLOOR_17,
	ISO_MISCELLANEOUS_FLOOR_18,
	ISO_MISCELLANEOUS_FLOOR_19,
	ISO_MISCELLANEOUS_FLOOR_20,
	ISO_MISCELLANEOUS_FLOOR_21,
	ISO_MISCELLANEOUS_FLOOR_22,
	ISO_MISCELLANEOUS_FLOOR_23,

	ISO_SIDEWALK_0,
	ISO_SIDEWALK_1,
	ISO_SIDEWALK_2,
	ISO_SIDEWALK_3,
	ISO_SIDEWALK_4,
	ISO_SIDEWALK_5,
	ISO_SIDEWALK_6,
	ISO_SIDEWALK_7,
	ISO_SIDEWALK_8,
	ISO_SIDEWALK_9,
	ISO_SIDEWALK_10,
	ISO_SIDEWALK_11,
	ISO_SIDEWALK_12,
	ISO_SIDEWALK_13,
	ISO_SIDEWALK_14,
	ISO_SIDEWALK_15,
	ISO_SIDEWALK_16,
	ISO_SIDEWALK_17,
	ISO_SIDEWALK_18,
	ISO_SIDEWALK_19,
	ISO_SIDEWALK_20,
	ISO_SIDEWALK_21,
	ISO_SIDEWALK_22,
	ISO_SIDEWALK_23,
	ISO_SIDEWALK_24,

	ISO_FLOOR_SAND,
	ISO_SAND_FLOOR_1,
	ISO_SAND_FLOOR_2,
	ISO_SAND_FLOOR_3,
	ISO_SAND_FLOOR_4,
	ISO_SAND_FLOOR_5,
	ISO_SAND_FLOOR_6,

	ISO_WATER,
	ISO_WATER_EDGE_1,
	ISO_WATER_EDGE_2,
	ISO_WATER_EDGE_3,
	ISO_WATER_EDGE_4,
	ISO_WATER_EDGE_5,
	ISO_WATER_EDGE_6,
	ISO_WATER_EDGE_7,
	ISO_WATER_EDGE_8,
	ISO_WATER_EDGE_9,
	ISO_WATER_EDGE_10,
	ISO_WATER_EDGE_11,
	ISO_WATER_EDGE_12,
	ISO_WATER_EDGE_13,
	ISO_WATER_EDGE_14,
	ISO_WATER_SIDEWALK_01,
	ISO_WATER_SIDEWALK_02,
	ISO_WATER_SIDEWALK_03,
	ISO_WATER_SIDEWALK_04,

	ISO_CARPET_TILE_0001,
	ISO_CARPET_TILE_0002,
	ISO_CARPET_TILE_0003,
	ISO_CARPET_TILE_0004,
	ISO_LARGE_SQUARE_BBB,
	ISO_LARGE_SQUARE_BRB,
	ISO_LARGE_SQUARE_BRR,
	ISO_LARGE_SQUARE_GBB,
	ISO_LARGE_SQUARE_GRB,
	ISO_LARGE_SQUARE_GRR,
	ISO_LARGE_SQUARE_RBB,
	ISO_LARGE_SQUARE_RRB,
	ISO_LARGE_SQUARE_RRR,
	ISO_MINI_SQUARE_0001,
	ISO_MINI_SQUARE_0002,
	ISO_MINI_SQUARE_0003,
	ISO_MINI_SQUARE_0004,
	ISO_MINI_SQUARE_0005,
	ISO_MINI_SQUARE_0006,
	ISO_MINI_SQUARE_0007,
	ISO_MINI_SQUARE_0008,
	ISO_SQUARE_TILE_AAB,
	ISO_SQUARE_TILE_ACB2,
	ISO_SQUARE_TILE_ACB,
	ISO_SQUARE_TILE_ADB2,
	ISO_SQUARE_TILE_ADB,
	ISO_SQUARE_TILE_CAB2,
	ISO_SQUARE_TILE_CAB,
	ISO_SQUARE_TILE_CCB,
	ISO_SQUARE_TILE_DAB2,
	ISO_SQUARE_TILE_DAB,
	ISO_SQUARE_TILE_DDB,

	ISO_COMPLICATED_CMM,
	ISO_COMPLICATED_CMM2,
	ISO_COMPLICATED_P4,
	ISO_COMPLICATED_PMG,
	ISO_COMPLICATED_PMG2,
	ISO_COMPLICATED_PMM,

	ISO_TWOSQUARE_0001,
	ISO_TWOSQUARE_0002,
	ISO_TWOSQUARE_0003
};

#define MAX_UNDERLAY_FLOOR_TILES 500
#define MAX_OVERLAY_FLOOR_TILES  500
#define MAX_FLOOR_TILES (MAX_UNDERLAY_FLOOR_TILES + MAX_OVERLAY_FLOOR_TILES)

/*
 * Underlay and overlay floor tiles share the same namespace.
 * Underlay floor tiles are numbered from 0 to MAX_UNDERLAY_FLOOR_TILES - 1.
 * Overlay floor tiles are numbered from MAX_UNDERLAY_FLOOR_TILES
 * to MAX_FLOOR_TILES - 1.
 */
enum {
	ISO_OVERLAY_GRASS_01 = MAX_UNDERLAY_FLOOR_TILES,
	ISO_OVERLAY_GRASS_02,
	ISO_OVERLAY_GRASS_03,
	ISO_OVERLAY_GRASS_04,
	ISO_OVERLAY_GRASS_05,
	ISO_OVERLAY_GRASS_06,
	ISO_OVERLAY_GRASS_07,
	ISO_OVERLAY_GRASS_08,
	ISO_OVERLAY_GRASS_09,
	ISO_OVERLAY_GRASS_10,
	ISO_OVERLAY_GRASS_11,
	ISO_OVERLAY_GRASS_12,
	ISO_OVERLAY_GRASS_13,
	ISO_OVERLAY_GRASS_14,
	ISO_OVERLAY_GRASS_15,
	ISO_OVERLAY_GRASS_16,
	ISO_OVERLAY_GRASS_17,
	ISO_OVERLAY_GRASS_18,
	ISO_OVERLAY_GRASS_19,
	ISO_OVERLAY_GRASS_20,
	ISO_OVERLAY_GRASS_21,

	ISO_OVERLAY_FLOOR_SAND_WITH_GRASS_1,
	ISO_OVERLAY_FLOOR_SAND_WITH_GRASS_2,
	ISO_OVERLAY_FLOOR_SAND_WITH_GRASS_3,
	ISO_OVERLAY_FLOOR_SAND_WITH_GRASS_4,
	ISO_OVERLAY_FLOOR_SAND_WITH_GRASS_5,

	ISO_OVERLAY_FLOOR_SAND_WITH_GRASS_25,
	ISO_OVERLAY_FLOOR_SAND_WITH_GRASS_26,
	ISO_OVERLAY_FLOOR_SAND_WITH_GRASS_27,

	ISO_GROUNDMARKER_BLUE_1,
	ISO_GROUNDMARKER_BLUE_2,
	ISO_GROUNDMARKER_BLUE_3,
	ISO_GROUNDMARKER_BLUE_4,
	ISO_GROUNDMARKER_BLUE_5,
	ISO_GROUNDMARKER_BLUE_6,
	ISO_GROUNDMARKER_BLUE_7,
	ISO_GROUNDMARKER_BLUE_8,
	ISO_GROUNDMARKER_RED_1,
	ISO_GROUNDMARKER_RED_2,
	ISO_GROUNDMARKER_RED_3,
	ISO_GROUNDMARKER_RED_4,
	ISO_GROUNDMARKER_RED_5,
	ISO_GROUNDMARKER_RED_6,
	ISO_GROUNDMARKER_RED_7,
	ISO_GROUNDMARKER_RED_8,
	ISO_GROUNDMARKER_BLUE_Y_1,
	ISO_GROUNDMARKER_BLUE_Y_2,
	ISO_GROUNDMARKER_BLUE_Y_3,
	ISO_GROUNDMARKER_BLUE_Y_4,
	ISO_GROUNDMARKER_BLUE_X_1,
	ISO_GROUNDMARKER_RED_Y_1,
	ISO_GROUNDMARKER_RED_Y_2,
	ISO_GROUNDMARKER_RED_Y_3,
	ISO_GROUNDMARKER_RED_Y_4,
	ISO_GROUNDMARKER_RED_X_1
};

#define MAX_FLOOR_LAYERS 2

// Maximum distance for both taking and dropping items
#define ITEM_TAKE_DIST (0.7)
// Maximum and minimum distance for a melee fight
#define SQUARED_MELEE_MAX_DIST (2.25)
#define MELEE_MIN_DIST (1.0)
// Distance at which a bot with a melee weapon will start to find 
// a free place around its target 
#define SQUARED_MELEE_APPROACH_DIST (16.0)
// Minimum distance to shoot with a range weapon
#define SQUARED_RANGE_SHOOT_MIN_DIST (7.0)

// Margins added to obstacles size when an item is being dropped.
#define COLLDET_DROP_ITEM_MARGIN (0.2)
// Margin's size added to obstacles size when colldet is called by the pathfinder.
// Also used to validate the walkability of a path between 2 waypoints.
#define COLLDET_WALKABLE_MARGIN (0.04)
// Distance to next intermediate point before to follow the next subpath.
// Also used as the distance to next waypoint before to choose a new one.
// It has to be smaller than COLLDET_WALKABLE_MARGIN. Near half of
// COLLDET_WALKABLE_MARGIN is a good candidate
#define DIST_TO_INTERM_POINT (0.02)

//-----
// Pathfinder constants
#define DEBUG_TUX_PATHFINDING 1	// debug level for tux pathfinding...

enum {
	DIFFICULTY_EASY,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD
};

enum faction_id {
	FACTION_SELF = 0,
	FACTION_BOTS,
	FACTION_REDGUARD,
	FACTION_RESISTANCE,
	FACTION_CIVILIAN,
	FACTION_CRAZY,
	FACTION_SINGULARITY,
	FACTION_NEUTRAL,

	FACTION_NUMBER_OF_FACTIONS, //keep this last
};

enum faction_state {
	HOSTILE = 0,
	FRIENDLY,
};

#define ACCESS_OBSTACLE_EXTENSION(X,Y) ((struct obstacle_extension *)(X.arr))[Y]
#define ACCESS_MAP_LABEL(X,Y) ((struct map_label *)(X.arr))[Y]

enum obstacle_extension_type {
	OBSTACLE_EXTENSION_LABEL = 30, // This extension indicates an obstacle label. The associated data is a const char * containing the label.
	OBSTACLE_EXTENSION_CHEST_ITEMS, // This extension indicates an item list. The associated data is a struct dynarray containing struct item.
	OBSTACLE_EXTENSION_DIALOGFILE, // This extension indicates the dialog filename associated to an interactive obstacle. Contains the dialog basename.
	OBSTACLE_EXTENSION_SIGNMESSAGE = OBSTACLE_EXTENSION_DIALOGFILE, // This extension contains the message written on a signpost. It is exclusive with a dialogfile specification
};

enum lua_target {
	LUA_CONFIG,
	LUA_DIALOG
};

enum slot_type {
	NO_SLOT = 0,
	WEAPON_SLOT = 1,
	SHIELD_SLOT = 2,
	HELM_SLOT = 4,
	ARMOR_SLOT = 8,
	BOOT_SLOT = 16,
};

// These are flags to change the level's environment
enum level_flags {
	TELEPORT_BLOCKED = 1
};

#endif
