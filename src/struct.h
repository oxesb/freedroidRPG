/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *   Copyright (c) 2004-2007 Arthur Huillet 
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

#ifndef _struct_h
#define _struct_h

#include "system.h"
#include "defs.h"

#ifndef _BYTE
typedef unsigned char byte;
#define _BYTE
#endif
#ifndef _WORD
typedef unsigned word;
#define _WORD
#endif
#ifndef _BOOL
typedef char bool;
#define _BOOL
#endif

typedef struct tColorRGBA {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} tColorRGBA, myColor;

typedef struct tColorY {
    Uint8 y;
} tColorY;
/* ----------------------------------------------------------------------
 * Here comes the struct for an iso image.  It also contains some 
 * placeholder for a possible 'zoomed-out' version of the iso image and
 * also some placeholder for a possible OpenGL texture as well.
 *
 * In order to have a convenient means to initialize variables of this
 * type in several places, even in case this struct changes shape in the
 * future, the 'UNLOADED_ISO_IMAGE' definition below is made.  This
 * definition should be used always when initializing a variable of this
 * so that later changes to the struct can be made with minimal effort 
 * and mistakes.
 * ---------------------------------------------------------------------- */
typedef struct
{
    SDL_Surface* surface;
    int offset_x;
    int offset_y;
    SDL_Surface* zoomed_out_surface;
    int texture_width;
    int texture_height;
    int original_image_width;
    int original_image_height;
    int force_color_key;
    int texture_has_been_created;
    void* attached_pixel_data;
#ifdef HAVE_LIBGL
    GLuint *texture;  // this is to store an open_gl texture...
#else
    int *placeholder_for_texture_value;  // this is to store an open_gl texture...
#endif
}
iso_image, *Iso_image;
#define UNLOADED_ISO_IMAGE { NULL , 0 , 0 , NULL , 0 , 0 , 0 }


typedef struct
{
    // SDL_Surface *button_surface;
    iso_image button_image;
    char *button_image_file_name;
    SDL_Rect button_rect;
    int scale_this_button;
    int use_true_alpha_blending;
}
mouse_press_button, *Mouse_press_button;

typedef struct
{
    float WantedTextVisibleTime;
    int Draw_Framerate;
    int Draw_Energy;
    int Draw_Position;
    int Influencer_Refresh_Text;
    int Influencer_Blast_Text;
    int Enemy_Hit_Text;
    int Enemy_Bump_Text;
    int Enemy_Aim_Text;
    int All_Texts_Switch;
    float Current_BG_Music_Volume;
    float Current_Sound_FX_Volume;
    float current_gamma_correction;
    int StandardEnemyMessages_On_Off;
    int StandardInfluencerMessages_On_Off;
    int Mouse_Input_Permitted;

    //--------------------
    // Now we add all the variables for the current screen/hud
    // configuration:  Is inventory screen or character screen
    // or something like that currently visible or not.
    //
    int Mission_Log_Visible;
    float Mission_Log_Visible_Time;
    float Mission_Log_Visible_Max_Time;
    int Inventory_Visible;
    float dummy1 ;
    float dummy2 ;
    int CharacterScreen_Visible;
    float dummy3 ;
    float dummy4 ;
    int SkillScreen_Visible;
    float dummy5 ;
    float dummy6 ;
    int Automap_Visible;
    int spell_level_visible;

    int terminate_on_missing_speech_sample;
    int show_subtitles_in_dialogs;
    char freedroid_version_string[500];
    int skip_light_radius;
    int skill_explanation_screen_visible;
    int enemy_energy_bars_visible;
    int hog_CPU;
    int highlighting_mode_full;
    int omit_tux_in_level_editor;
    int omit_obstacles_in_level_editor;
    int omit_enemies_in_level_editor;
    int level_editor_edit_mode;
    int zoom_is_on;
    int show_quick_inventory;
    int show_blood; // this can be used to make the game more friendly for children...
    int show_tooltips;
    int tux_image_update_policy ;
    int number_of_big_screen_messages ;
    float delay_for_big_screen_messages ;
    int enable_cheatkeys;
    int transparency ;
    int automap_manual_shift_x ;
    int automap_manual_shift_y ;
    int screen_width;
    int screen_height;
    int next_time_width_of_screen;
    int next_time_height_of_screen;
    float automap_display_scale ;
    int skip_shadow_blitting ;
    int language ; // index of language, see in defs.h
    int do_fadings; // do the screen fadings
    int auto_display_to_help; // display the takeover help
    int fullscreen_on;
    int talk_to_bots_after_takeover;
}
configuration_for_freedroid , *Configuration_for_freedroid;

typedef struct
{
    int x;
    int y;
}
point, *Point;

typedef struct
{
    float x;
    float y;
}
moderately_finepoint, *Moderately_finepoint;

typedef struct
{
    double x;
    double y;
}
finepoint, *Finepoint;

typedef struct
{
    double x;
    double y;
    int z;
} gps, *GPS;

typedef struct
{
    byte level;			
    int x;			
    int y;
}
location, *Location;

typedef struct
{
    char* label_name;
    point pos; // how many blocks does this big map insert cover?
}
map_label, *Map_Label;

typedef struct
{
    char MissionName[500];  // this should be the name of the mission, currently uninitialized
    int MissionWasAssigned; // has be influencer been assigned to this mission? , currently uninitialized
    int MissionIsComplete; // has the mission been completed?
    int MissionWasFailed; // has the mission been failed?
    int MissionExistsAtAll; // is this mission entry used at all, or is it just unused memory?
    int AutomaticallyAssignThisMissionAtGameStart; // well...
    
    int fetch_item;
    int KillAll;
    int KillClass;
    int KillOne;
    int must_clear_first_level;
    int must_clear_second_level;
    int   MustReachLevel;
    point MustReachPoint;
    double MustLiveTime;
    int MustBeClass;
    int MustBeType;
    int MustBeOne;
    
    int ListOfActionsToBeTriggeredAtAssignment[ MAX_MISSION_TRIGGERED_ACTIONS ];
    int ListOfActionsToBeTriggeredAtCompletition[ MAX_MISSION_TRIGGERED_ACTIONS ];

    int mission_description_visible [ MAX_MISSION_DESCRIPTION_TEXTS ] ;    
    float mission_description_time [ MAX_MISSION_DESCRIPTION_TEXTS ] ;
    int expanded_display_for_this_mission;
}
mission, *Mission;

//--------------------
// This structure can contain things, that might be triggered by a special
// condition, that can be specified in the mission file as well.
//
typedef struct
{
    char* ActionLabel;  // this is a better reference than a number
    
    // Maybe the triggered action will change some obstacle on some level...
    char* modify_obstacle_with_label;
    int modify_obstacle_to_type;

    char * modify_event_trigger_with_action_label;
    int modify_event_trigger_value;
    
    // Maybe the triggered event teleports the influencer somewhere
    point TeleportTarget;
    int TeleportTargetLevel;
    
    char * also_execute_action_label; //execute another action (linked action)
    
}
triggered_action , *Triggered_action;

//--------------------
// This structure can contain conditions that must be fulfilled, so that a special
// event is triggered.  Such conditions may be specified in the mission file as well
//
typedef struct
{
    // Maybe the event is triggerd by the influencer stepping somewhere
    int Influ_Must_Be_At_Level;
    point Influ_Must_Be_At_Point;
    
    // Maybe the event is triggered by time
    float Mission_Time_Must_Have_Passed;
    float Mission_Time_Must_Not_Have_Passed;
    
    int DeleteTriggerAfterExecution;
    // And now of course which event to trigger!!!!
    // Thats propably the most important information at all!!!
    // int EventNumber;
    char* TargetActionLabel;

    int enabled; //is the trigger enabled?
}
event_trigger , *Event_trigger;

typedef struct
{
    int x;
    int y;
    char* Statement_Text;
} map_statement , *Map_statement;

typedef struct
{
    iso_image image;
    iso_image shadow_image;
    SDL_Surface* automap_version;
    //--------------------
    // Some obstacles will block the Tux from walking through them.
    // Currently only rectangles are supported block areas.  The width
    // (i.e. east-west=parm1) and height (i.e. north-south=parm2) of
    // the blocking rectangle can ge specified below.
    //
    int block_area_type;
    int block_vision_too;
    float block_area_parm_1;
    float block_area_parm_2;
    float upper_border ;
    float lower_border ;
    float left_border ;
    float right_border ;
    
    //--------------------
    // Some obstacles will explode when hit by a weapon swing or bullet
    // and maybe they will even occasionally drop some treasure too...
    //
    int is_smashable;
    int result_type_after_smashing_once;
    int drop_random_treasure;
    
    //--------------------
    // Some obstacles will emitt light.  Specify light strength here.
    // A value of 0 light will be sufficient in most cases...
    //
    int emitted_light_strength;
    
    int transparent;
    //--------------------
    // This is a special property for obstacles, that can be 
    // stepped on, like a rug or floor plate, for proper visibility...
    //
    int needs_pre_put; 
    char* filename;
    char* obstacle_short_name;
    char* obstacle_long_description;
}
obstacle_spec, *Obstacle_spec;

typedef struct
{
    point inv_size;
    SDL_Surface* Surface;
    iso_image ingame_iso_image;
    SDL_Surface* scaled_surface_for_shop;
} item_image_spec , *Item_image_spec;

typedef struct
{
    char* bonus_name;
    int base_bonus_to_dex;
    int modifier_to_bonus_to_dex;
    int base_bonus_to_str;
    int modifier_to_bonus_to_str;
    int base_bonus_to_vit;
    int modifier_to_bonus_to_vit;
    int base_bonus_to_mag;
    int modifier_to_bonus_to_mag;
    int base_bonus_to_all_attributes;
    int modifier_to_bonus_to_all_attributes;
    
    int base_bonus_to_life;
    int modifier_to_bonus_to_life;
    int base_bonus_to_force;
    int modifier_to_bonus_to_force;

    float base_bonus_to_health_recovery;
    float base_bonus_to_cooling_rate;
    


    int base_bonus_to_tohit;
    int modifier_to_bonus_to_tohit;
    int base_bonus_to_ac_or_damage; // this is a percentage
    int modifier_to_bonus_to_ac_or_damage; // this is a percentage

    int base_bonus_to_resist_fire;  // this is a percentage
    int modifier_to_bonus_to_resist_fire;  // this is a percentage
    int base_bonus_to_resist_electricity; // this is a percentage
    int modifier_to_bonus_to_resist_electricity; // this is a percentage
    int base_bonus_to_resist_disruptor; // this is a percentage
    int modifier_to_bonus_to_resist_disruptor; // this is a percentage
    int light_bonus_value;
    float price_factor;

    int level; //"level" of the prefix (is it good or not)
} item_bonus , *Item_bonus;

typedef struct
{
    char* item_name;
    char* item_rotation_series_prefix;
    char* item_description;
    char* item_drop_sound_file_name;
    char* item_inv_file_name;
    
    char item_can_be_applied_in_combat;
    char item_can_be_installed_in_weapon_slot;
    char item_can_be_installed_in_drive_slot;
    char item_can_be_installed_in_armour_slot;
    char item_can_be_installed_in_shield_slot;
    char item_can_be_installed_in_special_slot;
    
    char item_group_together_in_inventory;
    
    // How good is the item as weapon???
    double item_gun_recharging_time;       // time until the next shot can be made, measures in seconds
    double item_gun_reloading_time;       // time needed to put a new charger
    short    item_gun_bullet_image_type;       // which type of image to use for displaying this bullet
    double item_gun_speed; // how fast should a bullet move straightforward?
    short    base_item_gun_damage; //	damage done by this bullettype 
    short    item_gun_damage_modifier; // modifier to the damage done by this bullettype 
    double item_gun_bullet_lifetime;      // how long does a 'bullet' from this gun type live?
    char    item_gun_bullet_reflect_other_bullets; // can this 'bullet' reflect other bullets
    char    item_gun_bullet_pass_through_explosions; // can this 'bullet' reflect other bullets
    char    item_gun_bullet_pass_through_hit_bodies; // does this bullet go through hit bodies (e.g. like a laser sword)
    char    item_gun_bullet_ignore_wall_collisions; // can this bullet pass through walls and map barriers?
    short    item_gun_ammo_clip_size; //max. number of bullets in the charger
    
    // the following values have only relevance in case of a melee weapon
    double item_gun_angle_change;	// how fast to do a melee weapon swing
    double item_gun_start_angle_modifier;	// where to start with a melee weapon swing
    double item_gun_fixed_offset;         // how far away from the swinger should a melee weapon swing occur?
    short    item_gun_use_ammunition; // which ammunition does this gun use? - 1 laser 2 plasma 3 exterminator
    char    item_gun_requires_both_hands; // is this a (strictly) 2-handed weapon?
    
    // how good is the item as armour or shield or other protection???
    short base_ac_bonus;
    short ac_bonus_modifier;
    
    // which requirement for strength, dexterity and magic (force) does the item have?
    short int item_require_strength;
    short int item_require_dexterity;
    short int item_require_magic;
    
    // what duration does the item have?
    short int base_item_duration;
    short int item_duration_modifier;
    
    // Which picture to use for this item, when it's lying on the floor?
    // int picture_number;
    item_image_spec inv_image ;
    short int base_list_price;         // the base price of this item at the shop

    short int min_drop_class;
    short int max_drop_class;
    
} itemspec , *Itemspec;

typedef struct
{
    // Here are the rather short-lived properties of the item
    finepoint pos;
    SDL_Rect text_slot_rectangle;
    int type;
    int currently_held_in_hand;      // is the item currently held 'in hand' with the mouse cursor?
    int is_identified;               // is the item identified already?
    int max_duration;                // the maximum item durability reachable for this item
    float current_duration;          // the currently remaining durability for this item
    float throw_time;                // has this item just jumped out from a chest maybe or is it jumping right now?
    
    // Here are the rather long-lived properties of the item
    int prefix_code;
    int suffix_code;
    
    int bonus_to_dex;
    int bonus_to_str;
    int bonus_to_vit;
    int bonus_to_mag;
    int bonus_to_life;
    int bonus_to_force;
    float bonus_to_health_recovery;
    float bonus_to_cooling_rate;
    int bonus_to_tohit;
    int bonus_to_all_attributes;
    int bonus_to_ac_or_damage;       // this is a percentage
    int bonus_to_resist_fire;        // this is a percentage
    int bonus_to_resist_electricity; // this is a percentage
    int bonus_to_resist_disruptor;       // this is a percentage
    
    int ac_bonus;                    // how much is ac increased by this item worn
    int damage;                      // how much damage does this item
    int damage_modifier;             // how much additional damage can add to the base damage
    int multiplicity;
    int ammo_clip; 		// how much bullets in the clip, IN CASE OF WEAPON
    point inventory_position;
} item, *Item;

typedef struct
{
    char *druidname;
    char *droid_portrait_rotation_series_prefix;
    char *droid_death_sound_file_name;
    char *droid_attack_animation_sound_file_name;
    int class;
    double maxenergy;		// the maximum energy the batteries can carry 
    double max_temperature;		// the maximum force this droids mind can carry 
    double lose_health;		// the energy/time the duid loses under influence-control 
    
    double maxspeed;
    double accel;
    
    float physical_damage;        // the amount of physical (base) damage an attack of this bot will do
    
    int aggression;		// The aggressiveness of this druidtype 
    int flashimmune;		// is the droid immune to FLASH-bullets 
    int experience_reward;			// experience_reward for the elimination of one droid of this type 
    unsigned char brain;
    unsigned char sensor1;
    unsigned char sensor2;
    unsigned char sensor3;
    
    float range_of_vision;
    float time_spent_eyeing_tux;
    float minimal_range_hostile_bots_are_ignored;
    
    short int monster_level;
    short int forced_magic_items;
    
    item weapon_item;
    
    int amount_of_plasma_transistors;
    int amount_of_superconductors;
    int amount_of_antimatter_converters;
    int amount_of_entropy_inverters;
    int amount_of_tachyon_condensators;
    
    int greeting_sound_type;              // which sample to play in order to 'greet' the influencer?
    int got_hit_sound_type;               // which sample to play in order to 'greet' the influencer?
    int to_hit;                           // chance that this droid hits an unarmoured target
    int getting_hit_modifier;             // modifier for this droid to receive a hit from the player
    float recover_time_after_getting_hit;
    char *notes;			        // notes on the druid of this type 
    int is_human;
    int individual_shape_nr;
    int suppress_bullet_generation_when_attacking; // some bots have weapon and attack built into the animation images...
}
druidspec, *Druidspec;

typedef struct
{
    Sint8 type;			  // ... currently unused ...
    Sint8 status;		  // ... only little meaning any more ...
    
    float current_game_date;      // seconds since game start, will be printed as a different 'date'
                                  // inside the game, like 14:23 is afternoon
    int current_power_bonus;
    float power_bonus_end_date;
    int current_dexterity_bonus;
    float dexterity_bonus_end_date;
    
    finepoint speed;		  // the current speed of the druid 
    gps pos;		          // current position in the whole ship 
    gps teleport_anchor;            // where from have you last teleported home
    gps mouse_move_target;          // where the tux is going automatically by virtue of mouse move 
    int current_enemy_target; // which enemy has been targeted (for a melee shot) (DIFFERS FROM ABOVE !)
    int mouse_move_target_combo_action_type; // what extra action has to be done upon arrival?
    int mouse_move_target_combo_action_parameter; // extra data to use for the combo action
    
    int light_bonus_from_tux ;
    int map_maker_is_present ;
    
    double health;		  // the max. possible energy in the moment 
    float maxenergy; // current top limit for the influencers energy
    double energy;		  // current energy level 
    float max_temperature;   // current top limit for temperature (highest is better)
    float temperature;                    // current temperature
    float old_temperature;                    // current temperature
    float max_running_power;
    float running_power;
    int running_must_rest;
    int running_power_bonus;

    float health_recovery_rate; //points of health recovered each second
    float cooling_rate; //temperature points recovered each second
    
    Sint16 LastMouse_X;             // mostly for other players:  Where was the last mouseclick...
    Sint16 LastMouse_Y;             // mostly for other players:  Where was the last mouseclick...
    
    double busy_time;		// time remaining, until the weapon is ready to fire again...
    int busy_type; 		// reason why tux is busy (enum)
    double phase;			// the current phase of animation 
    float angle ;
    float walk_cycle_phase;       // 
    float weapon_swing_time;	// How long is the current weapon swing in progress (in seconds of course) 
    float MissionTimeElapsed;
    float got_hit_time;           // how long stunned now since the last time tux got hit 
    
    char freedroid_version_string[1000]; // a string to identify games from older freedroid versions
    
    int Strength;  // character Strength value = 'power supply max. capacity'
    int Magic;     // character Magic value = 
    int Dexterity; // character Dexterity value = 'power redistribution speed'
    int base_vitality;  // character Vitality value = 'cloaking field maximum strength'
    int base_strength;  // character Strength value = 'power supply max. capacity'
    int base_magic;     // character Magic value = 
    int base_dexterity; // character Dexterity value = 'power redistribution speed'
    int Vitality;  // character Vitality value = 'cloaking field maximum strength'
    int points_to_distribute; // these are the points that are available to distribute upon the character stats
    float base_damage; // the current damage the influencer does
    float damage_modifier; // the modifier to the damage the influencer currently does
    float AC; // the current Armour Class of the influencer
    float to_hit;            // percentage chance, that Tux will hit a random lv 1 bot
    int lv_1_bot_will_hit_percentage; // percentage chance that a random lv 1 bot will hit
    int resist_disruptor;        // percentage to reduce from discruptor aka. "flash" damage
    int resist_fire;         // percentage to reduce from fire damage
    int resist_electricity;  // percentage to reduce from electricity damage
    
    int freezing_melee_targets; // does this Tux freeze melee targets upon hit?
    int double_ranged_damage;   // does this Tux do double ranged weapon damage?
    
    unsigned long Experience; // character Experience = 'spare droid elements found'
    int exp_level;       // which 'experience level' is the influencer currenly at?
    unsigned long ExpRequired;    // how much experience required for the next level?
    unsigned long ExpRequired_previously;    // how was required for the previous level?
    
    long Gold;
    char character_name[ MAX_CHARACTER_NAME_LENGTH ];
    mission AllMissions[ MAX_MISSIONS_IN_GAME ];         // What must be done to fullfill this mission?
    int marker;                   // In case you've taken over a marked droid, this will contain the marker
    float LastCrysoundTime;
    float LastTransferSoundTime;
    float TextVisibleTime;
    char* TextToBeDisplayed;
    float Current_Victim_Resistance_Factor;
    int FramesOnThisLevel;        // how many frames has the influ spent on this level already?
    
    //--------------------
    // Here we note all the 'skill levels' of the Tux and also which skill is
    // currently readied and that...
    //
    int readied_skill; 
    int SkillLevel[MAX_NUMBER_OF_PROGRAMS];
    int base_skill_level[MAX_NUMBER_OF_PROGRAMS];
    int melee_weapon_skill;
    int ranged_weapon_skill;
    int spellcasting_skill;
    int hacking_skill;

    //--------------------
    // The inventory slots.  Some items are residing in general inventory,
    // other items might be equiped in some of the corresponding slots of
    // the inventory screen.
    //
    item Inventory[ MAX_ITEMS_IN_INVENTORY ];
    item weapon_item;
    item drive_item;
    item armour_item;
    item shield_item;
    item special_item;
    
    //--------------------
    // A record of when and if the tux has been on some maps...
    //
    unsigned char HaveBeenToLevel [ MAX_LEVELS ]; // record of the levels the player has visited yet.
    float time_since_last_visit_or_respawn [ MAX_LEVELS ]; // record of the levels the player has visited yet.
    
    //--------------------
    // Some story-based variables:  which persons has the Tux talked to and
    // what are the dialog options currently open, which 'cookies' have been
    // set by the dialogs for coordination among each other, and also status
    // of the Tux like town guard member or not and the like...
    //
    unsigned char Chat_Flags[ MAX_PERSONS ][ MAX_ANSWERS_PER_PERSON ];
    char cookie_list[ MAX_COOKIES ] [ MAX_COOKIE_LENGTH ] ;
    int is_town_guard_member;
    char chat_character_initialized [ MAX_PERSONS ]; 
    
    //--------------------
    // THE FOLLOWING ARE INFORMATION, THAT ARE HUGE AND THAT ALSO DO NOT NEED
    // TO BE COMMUNICATED FROM THE CLIENT TO THE SERVER OR VICE VERSA
    //
    moderately_finepoint next_intermediate_point [ MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX ] ;  // waypoints for the tux, when target not directly reachable
    Uint16 KillRecord[ 200 ];      // how many ( of the first 1000 monster types) have been killed yet?
    Uint8 Automap [ MAX_LEVELS ] [ 100 ][ 100 ]; // this is the data for the automatic map
    int current_zero_ring_index;
    gps Position_History_Ring_Buffer[ MAX_INFLU_POSITION_HISTORY ];
    
    int BigScreenMessageIndex;
    char BigScreenMessage [ MAX_BIG_SCREEN_MESSAGES ] [ 5000 ];
    float BigScreenMessageDuration [ MAX_BIG_SCREEN_MESSAGES ];

    float slowdown_duration;
    float paralyze_duration;
}
tux_t, *Tux_t;

typedef struct
{
    int type;			// the number of the droid specifications in Druidmap 
    gps pos;		        // coordinates of the current position in the level
    gps virt_pos;		// the virtual position with respect to remote levels
    finepoint speed;		// current speed  
    double energy;		// current energy of this droid
    
    double phase;	                // current phase of rotation of this droid
    float animation_phase;        // the current animation frame for this enemy (starting at 0 of course...)
    int animation_type;           // walk-animation, attack-animation, gethit animation, death animation
    
    int nextwaypoint;		// the next waypoint target
    int lastwaypoint;		// the waypoint from whence this robot just came
    int homewaypoint;		// the waypoint this robot started at
    int max_distance_to_home;	// how far this robot will go before returning to it's home waypoint
    int Status;			// current status like OUT=TERMINATED or not OUT
    
    int combat_state;             // MOVE_ALONG_RANDOM_WAYPOINTS, BACK_OFF_AFTER_GETTING_HIT, MAKE_ATTACK_RUN, SEEK_BETTER_POSITION...
    float state_timeout;          // when will this state automatically time out...
    
    float frozen;                 // is this droid currently frozen and for how long will it stay this way?
    float poison_duration_left;   // is this droid currently poisoned and for how long will it stay this way?
    float poison_damage_per_sec;  // is this droid currently poisoned and how much poison is at work?
    float paralysation_duration_left;  // is this droid currently paralyzed and for how long will it stay this way?
    double pure_wait;		// time till the droid will start to move again
    double firewait;		// time this robot still takes until it's gun/weapon will be fully reloaded
    int ammo_left; 		  // ammunition left in the charger
    
    int CompletelyFixed;          // set this flat to make the robot entirely immobile
    int follow_tux;               // does this robot try to follow tux via it's random movements?
    int FollowingInflusTail;      // does this robot follow influs tail? (trott behind him? )
    int SpecialForce;             // This flag will exclude the droid from initial shuffling of droids
    int on_death_drop_item_code;  // drop a pre-determined item when dying?
    
    int marker;                   // This provides a marker for special mission targets
    
    int is_friendly;              // is this a friendly droid or is it a MS controlled one?
    int has_been_taken_over;      // has the Tux made this a friendly bot via takeover subgame?
    int attack_target_type ;      // attack NOTHING, PLAYER, or BOT
    int attack_target_index ;     // index of bot or (-1) if there is no target
    int attack_run_only_when_direct_line; // require direct line to target before switching into attach run mode
    char dialog_section_name[ MAX_LENGTH_FOR_DIALOG_SECTION_NAME ]; // This should indicate one of the many sections of the Freedroid.dialogues file
    char short_description_text[ MAX_LENGTH_OF_SHORT_DESCRIPTION_STRING ]; // This should indicate one of the many sections of the Freedroid.dialogues file
    int will_rush_tux;            // will this robot approach the Tux on sight and open communication?
    int persuing_given_course;    // is this robot persuing a given course via PersueGivenCourse( EnemyNum )?
    int StayHowManyFramesBehind;  // how many frames shall this droid trott behind the influ when follwing his tail?
    int StayHowManySecondsBehind;  // how many seconds shall this droid trott behind the influ when follwing his tail?
    int has_greeted_influencer;   // has this robot issued his first-time-see-the-Tux message?
    float previous_angle;         // which angle has this robot been facing the frame before?
    float current_angle;          // which angle will the robot be facing now?
    float last_phase_change;      // when did the robot last change his (8-way-)direction of facing
    float previous_phase;         // which (8-way) direction did the robot face before?
    float last_combat_step;       // when did this robot last make a step to move in closer or farther away from Tux in combat?
    
    //--------------------
    // FROM HERE ON, THERE IS ONLY INFORMATION, THAT DOES NOT NEED TO BE
    // COMMUNICATED BETWEEN THE CLIENT AND THE SERVER
    //
    float TextVisibleTime;
    char* TextToBeDisplayed;
    moderately_finepoint PrivatePathway[ MAX_STEPS_IN_GIVEN_COURSE ];
    int stick_to_waypoint_system_by_default;

    int bot_stuck_in_wall_at_previous_check;
    float time_since_previous_stuck_in_wall_check;
}
enemy, *Enemy;

typedef struct
{
    int phases;			// how many phases in motion to show 
    double phase_changes_per_second; // how many different phases to display every second
    iso_image image [ BULLET_DIRECTIONS ] [ MAX_PHASES_IN_A_BULLET ] ;
} 
bulletspec, *Bulletspec;

typedef struct
{
    int type;
    gps pos;
    moderately_finepoint speed;
    byte phase;
    int damage; // damage done by this particular bullet 
    int time_in_frames;    // how long does the bullet exist, measured in number of frames
    double time_in_seconds; // how long does the bullet exist in seconds
    double bullet_lifetime; // how long can this bullet exist at most
    float time_to_hide_still; // countdown to when the bullet will actually appear
    signed char mine;
    int owner;
    double angle;
    byte total_miss_hit [ MAX_ENEMYS_ON_SHIP ] ;
    byte miss_hit_influencer;
    int to_hit;
    
    // collision behaviour of bullets
    int pass_through_hit_bodies; // does this bullet go through hit bodies (e.g. like a laser sword stike)
    int pass_through_explosions; // does this bullet go through explosions (e.g. laser sword stike though dead droid)
    int ignore_wall_collisions; // does this bullet go through walls (e.g. a laser sword strike)
    int was_reflected; // has this bullet been reflected once in it's lifetime?
    int reflect_other_bullets; // does this bullet reflect other bullets in case of a collision?
    int freezing_level;        // does this bullet freeze the target?
    float poison_duration;
    float poison_damage_per_sec;
    float paralysation_duration;
    
    // these are values only of relevance in case of a melee weapon
    double angle_change_rate;
    float fixed_offset;
    gps* owner_pos;
    
    // these are technical parameters, not for the game behaviour
    SDL_Surface *SurfacePointer[ MAX_PHASES_IN_A_BULLET ];
    int Surfaces_were_generated; // 
}
bullet, *Bullet;

typedef struct
{
    int phases;
    double total_animation_time;
    iso_image image [ PHASES_OF_EACH_BLAST ] ;
}
blastspec, *Blastspec;

typedef struct
{
    gps pos;
    int type;
    double phase;
    int MessageWasDone;
    float damage_per_second;
}
blast, *Blast;

typedef struct
{
    int img_type; // what type of spell is active?
    int damage;
    int poison_duration;
    int poison_dmg;
    int freeze_duration;
    int paralyze_duration;
    moderately_finepoint spell_center;
    float spell_radius;
    float spell_age;
    int active_directions [ RADIAL_SPELL_DIRECTIONS ] ;
    int mine;
}
spell_active, *Spell_Active;

typedef struct
{
    char* name;
    char* icon_name;
    iso_image icon_surface;
    int heat_cost;
    int heat_cost_per_level;
    int damage_base;
    int damage_mod;
    int damage_per_level;
    int hurt_bots;
    int hurt_humans;
    char* description;
    char* effect;
    int form;
    int present_at_startup;
    int graphics_code;
}
spell_skill_spec, *Spell_Skill_Spec;

typedef struct
{
    int x;			
    int y;
    int num_connections;
    int suppress_random_spawn;
    int connections [ MAX_WP_CONNECTIONS ] ;
}
waypoint, *Waypoint;

typedef struct
{
    int type;
    moderately_finepoint pos;
    int name_index;
    int description_index;
}
obstacle, *Obstacle;

typedef struct
{
    Uint16 floor_value;
    int obstacles_glued_to_here [ MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ];
}
map_tile, *Map_tile;

typedef struct
{
    int levelnum;
    int xlen;
    int ylen;
    int light_radius_bonus;
    int minimum_light_value;
    int infinite_running_on_this_level;
    char *Levelname;
    char *Background_Song_Name;
    char *Level_Enter_Comment;
    map_statement StatementList [ MAX_STATEMENTS_PER_LEVEL ];
    char *obstacle_name_list [ MAX_OBSTACLE_NAMES_PER_LEVEL ];
    char *obstacle_description_list [ MAX_OBSTACLE_DESCRIPTIONS_PER_LEVEL ];
    map_tile *map [ MAX_MAP_LINES ];	// this is a vector of pointers
    int jump_threshold_north;
    int jump_threshold_south;
    int jump_threshold_east;
    int jump_threshold_west;
    int jump_target_north;
    int jump_target_south;
    int jump_target_east;
    int jump_target_west;
    int use_underground_lighting;
    
    obstacle obstacle_list[ MAX_OBSTACLES_ON_MAP ];
    
    //--------------------
    // Now the list of indices that need to be known every
    // frame...
    //
    int refresh_obstacle_indices [ MAX_REFRESHES_ON_LEVEL ] ;
    int teleporter_obstacle_indices [ MAX_TELEPORTERS_ON_LEVEL ] ;
    int door_obstacle_indices [ MAX_DOORS_ON_LEVEL ];
    int autogun_obstacle_indices [ MAX_AUTOGUNS_ON_LEVEL ] ;
    
    map_label labels [ MAX_MAP_LABELS_PER_LEVEL ];
    int num_waypoints;
    waypoint AllWaypoints[MAXWAYPOINTS];
    item    ItemList [ MAX_ITEMS_PER_LEVEL ] ;
    item OldItemList [ MAX_ITEMS_PER_LEVEL ] ;
    item ChestItemList [ MAX_ITEMS_PER_LEVEL ] ;
}
level, *Level;

typedef struct
{
    int num_levels;
    char* AreaName;
    Level AllLevels[MAX_LEVELS];
}
ship, *Ship;

typedef struct
{
    int position_x;
    int position_y;
    char* option_text;
    char* option_sample_file_name;
    
    char* reply_sample_list[ MAX_REPLIES_PER_OPTION ] ;
    char* reply_subtitle_list[ MAX_REPLIES_PER_OPTION ];
    
    char* extra_list[ MAX_EXTRAS_PER_OPTION ];
    
    char* on_goto_condition;
    int on_goto_first_target;
    int on_goto_second_target;
    int link_target;
    int always_execute_this_option_prior_to_dialog_start;
    
    int change_option_nr [ MAX_DIALOGUE_OPTIONS_IN_ROSTER ];
    int change_option_to_value [ MAX_DIALOGUE_OPTIONS_IN_ROSTER ];
}
dialogue_option, *Dialogue_option;

#endif
