/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *   Copyright (c) 2004-2007 Arthur Huillet 
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
#ifndef _proto_h
#define _proto_h

#include "struct.h"

// main.c 
#undef EXTERN
#ifdef _main_c
#define EXTERN
#else
#define EXTERN extern
#endif

// automap.c
#undef EXTERN
#ifdef _automap_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void show_automap_data_sdl ( void );
EXTERN void toggle_automap();

// init.c
#undef EXTERN
#ifdef _init_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ResetGameConfigToDefaultValues ( void );
EXTERN void ShowStartupPercentage ( int Percentage ) ;
EXTERN void ParseCommandLine ( int argc , char *const argv[] );
EXTERN void ClearAutomapData( void );
EXTERN void InitFreedroid ( int, char ** );
EXTERN void PrepareStartOfNewCharacter ( void ) ;
EXTERN void ThouArtDefeated ( void );
EXTERN void ThouHastWon ( void );
EXTERN void GetEventsAndEventTriggers ( const char* EventsAndEventTriggersFilename );
EXTERN void PlayATitleFile ( char* Filename );
EXTERN void Get_Item_Data ( char* DataPointer );

// influ.c 
#undef EXTERN
#ifdef _influ_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN float calc_euklid_distance ( float pos1_x , float pos1_y , float pos2_x , float pos2_y ); 
EXTERN float vect_len ( moderately_finepoint our_vector );
EXTERN enemy * GetLivingDroidBelowMouseCursor ( void );
EXTERN int GetObstacleBelowMouseCursor ( void );
EXTERN int find_free_floor_items_index ( int levelnum ) ;
EXTERN int closed_chest_below_mouse_cursor ( void ) ;
EXTERN int smashable_barrel_below_mouse_cursor ( void ) ;
EXTERN void tux_wants_to_attack_now ( int use_mouse_cursor_for_targeting ) ;
EXTERN int PerformTuxAttackRaw ( int use_mouse_cursor_for_targeting ) ;
EXTERN void TuxReloadWeapon ( void ) ;
EXTERN void correct_tux_position_according_to_jump_thresholds ( void );
EXTERN int isignf (float);
EXTERN void InitInfluPositionHistory( void );
EXTERN float GetInfluPositionHistoryX( int Index );
EXTERN float GetInfluPositionHistoryY( int Index );
EXTERN float GetInfluPositionHistoryZ( int Index );
EXTERN void FillInDefaultBulletStruct ( bullet * CurBullet, int bullet_image_type, short int weapon_item_type );
EXTERN void FireTuxRangedWeaponRaw ( short int weapon_item_type , int bullet_image_type , bullet *, moderately_finepoint target_location ) ;
EXTERN void move_tux ( void ) ;
EXTERN void animate_tux ( void ) ;
EXTERN void check_tux_enemy_collision (void);
EXTERN void start_tux_death_explosions (void);
EXTERN void skew_and_blit_rect( float x1, float y1, float x2, float y2, Uint32 color);
EXTERN moderately_finepoint translate_point_to_map_location ( float axis_x , float axis_y , int zoom_is_on );
EXTERN void blit_zoomed_iso_image_to_map_position ( iso_image* our_iso_image , float pos_x , float pos_y );
EXTERN int tux_can_walk_this_line ( float x1, float y1 , float x2 , float y2 );
EXTERN void clear_out_intermediate_points ( gps *, moderately_finepoint *, int);
EXTERN int set_up_intermediate_course_between_positions ( enemy * droid, gps * curpos, moderately_finepoint * move_target, moderately_finepoint * waypoints, int maxwp );
EXTERN void adapt_position_for_jump_thresholds ( gps* old_position, gps* new_position );

// bullet.c 
#undef EXTERN
#ifdef _bullet_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void RotateVectorByAngle ( moderately_finepoint* vector , float rot_angle );
EXTERN void AnalyzePlayersMouseClick ( void ) ;
EXTERN void MoveBullets (void);
EXTERN void DoMeleeDamage(void);
EXTERN void DeleteBullet (int num , int StartBlast );
EXTERN void StartBlast ( float x , float y , int level , int type, int dmg );
EXTERN void animate_blasts (void);
EXTERN void DeleteBlast (int num);
EXTERN void MoveActiveSpells (void);
EXTERN void DeleteSpell (int num);
EXTERN void clear_active_spells ( void );
EXTERN void CheckBulletCollisions (int num);
EXTERN void CheckBlastCollisions (int num);
EXTERN int find_free_bullet_index ( void ) ;
EXTERN int find_free_melee_shot_index ( void ) ;
EXTERN void delete_melee_shot ( melee_shot * );

// view.c 
#undef EXTERN
#ifdef _view_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void update_virtual_position ( gps* target_pos , gps* source_pos , int level_num );
EXTERN void FdFillRect (SDL_Rect rect, SDL_Color color);
EXTERN void ShowPosition (void);
EXTERN void DisplayItemImageAtMouseCursor( int ItemImageCode );
EXTERN void ShowCombatScreenTexts ( int mask );
EXTERN void isometric_show_floor_around_tux_without_doublebuffering (int mask);
EXTERN void set_up_ordered_blitting_list ( int mask );
EXTERN void blit_preput_objects_according_to_blitting_list ( int mask );
EXTERN void blit_nonpreput_objects_according_to_blitting_list ( int mask );
EXTERN void show_obstacle_labels ( int mask );
EXTERN void draw_grid_on_the_floor (int mask);
EXTERN void blit_leveleditor_point ( int x, int y );
EXTERN void update_item_text_slot_positions ( void );
EXTERN void blit_all_item_slots ( void );
EXTERN void AssembleCombatPicture (int );
EXTERN void blit_tux ( int x , int y );
EXTERN void PutBullet ( int Bullet_number , int mask );
EXTERN void PutItem ( int ItemNumber , int mask , int put_thrown_items_flag , int highlight_item );
EXTERN void PutBlast (int);
EXTERN void PutEnemy (enemy * e, int x , int y , int mask , int highlight );
EXTERN void PutMouseMoveCursor ( void ) ;
EXTERN void ShowInventoryScreen ( void );
EXTERN void clear_all_loaded_tux_images ( int with_free );
EXTERN int set_rotation_index_for_this_robot ( enemy* ThisRobot );
EXTERN int set_rotation_model_for_this_robot ( enemy* ThisRobot );
EXTERN void grab_enemy_images_from_archive ( int enemy_model_nr );
EXTERN int level_is_visible ( int level_num );

// light.c 
#undef EXTERN
#ifdef _light_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int get_light_strength_screen ( int x, int y );
EXTERN int get_light_strength ( moderately_finepoint target_pos );
EXTERN void update_light_list ( void );
EXTERN void blit_light_radius ( void );

// open_gl.c 
#undef EXTERN
#ifdef _open_gl_c
#define EXTERN
#else
#define EXTERN extern
#endif

enum {
		Z_DIR,
		X_DIR,
		Y_DIR
};

EXTERN int our_SDL_flip_wrapper ( ) ;
EXTERN int our_SDL_blit_surface_wrapper(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
EXTERN void our_SDL_update_rect_wrapper ( SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h ) ;
EXTERN int our_SDL_fill_rect_wrapper (SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);
EXTERN int blit_quad ( int x1 , int y1 , int x2, int y2, int x3, int y3, int x4 , int y4 , Uint32 color );
EXTERN void drawIsoEnergyBar(int dir, int x, int y, int z, int w, int d, int length, float fill, myColor *c1, myColor *c2  ) ;

EXTERN SDL_Surface* our_SDL_display_format_wrapperAlpha ( SDL_Surface *surface );
EXTERN SDL_Surface* our_IMG_load_wrapper( const char *file );
EXTERN void flip_image_vertically ( SDL_Surface* tmp1 ) ;
EXTERN void make_texture_out_of_surface ( iso_image* our_image ) ;
EXTERN void make_texture_out_of_prepadded_image ( iso_image* our_image ) ;
EXTERN SDL_Surface* pad_image_for_texture ( SDL_Surface* our_surface ) ;
EXTERN void clear_screen (void) ;
EXTERN void blit_open_gl_stretched_texture_light_radius ( void ) ;
EXTERN void PutPixel_open_gl ( int x, int y, Uint32 pixel);
EXTERN void GL_HighlightRectangle ( SDL_Surface* Surface , SDL_Rect * Area , unsigned char r , unsigned char g , unsigned char b , unsigned char alpha );
EXTERN void ShowInventoryScreenBackground ( void );
EXTERN void show_character_screen_background ( void );
EXTERN int safely_initialize_our_default_open_gl_parameters ( void ) ;
EXTERN void draw_gl_textured_quad_at_map_position ( iso_image * our_floor_iso_image , float our_col , float our_line , float r , float g , float b , int highlight_texture, int blend, float zoom_factor ) ;
EXTERN void draw_gl_textured_quad_at_screen_position ( iso_image * our_floor_iso_image , int x , int y ) ;
EXTERN void draw_gl_scaled_textured_quad_at_screen_position ( iso_image * our_floor_iso_image , int x , int y , float scale_factor ) ;
EXTERN void blit_semitransparent_open_gl_texture_to_screen_position ( iso_image * our_floor_iso_image , int x , int y , float scale_factor ) ;
EXTERN void blit_special_background ( int background_code );
EXTERN void flush_background_image_cache ( void );
EXTERN void open_gl_check_error_status ( const char* name_of_calling_function );
EXTERN void draw_gl_bg_textured_quad_at_screen_position ( iso_image * our_floor_iso_image , int x , int y ) ;

// blocks.c 
#undef EXTERN
#ifdef _blocks_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int wall_orientation(int wall);
EXTERN void try_to_load_ingame_item_surface ( int item_type );
EXTERN void iso_load_bullet_surfaces ( void );
EXTERN void get_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image , int use_offset_file ) ;
EXTERN void make_sure_zoomed_surface_is_there ( iso_image* our_iso_image );
EXTERN void make_sure_automap_surface_is_there ( obstacle_spec* our_obstacle_spec );
EXTERN void load_item_surfaces_for_item_type ( int item_type );
EXTERN void Load_Mouse_Move_Cursor_Surfaces(void);
EXTERN void Load_Skill_Level_Button_Surfaces( void );
EXTERN void LoadOneSkillSurfaceIfNotYetLoaded ( int SkillSpellNr );
EXTERN void LoadAndPrepareEnemyRotationModelNr ( int RotationModel );
EXTERN void LoadAndPrepareRedEnemyRotationModelNr ( int RotationModel );
EXTERN void LoadAndPrepareGreenEnemyRotationModelNr ( int RotationModel );
EXTERN void LoadAndPrepareBlueEnemyRotationModelNr ( int RotationModel );
EXTERN void Load_Enemy_Surfaces (void);
EXTERN void Load_Tux_Surfaces( void );
EXTERN void LoadOneMapInsertSurfaceIfNotYetLoaded ( int i );
EXTERN void Load_Bullet_Surfaces (void);
EXTERN void Load_Blast_Surfaces (void);
EXTERN void load_floor_tiles ( void );
EXTERN void load_all_obstacles ( void );
EXTERN void blit_iso_image_to_map_position ( iso_image * our_iso_image , float pos_x , float pos_y );
EXTERN void blit_iso_image_to_map_position_in_buffer ( SDL_Surface *current_buffer , 
						       iso_image * our_iso_image , float pos_x , float pos_y );
EXTERN void blit_iso_image_to_screen_position ( iso_image * our_iso_image , float pos_x , float pos_y );
EXTERN void blit_outline_of_iso_image_to_map_position ( iso_image * our_iso_image , float pos_x , float pos_y );

// graphics.c 
#undef EXTERN
#ifdef _graphics_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void set_mouse_cursor_to_shape ( int given_shape );
EXTERN void make_sure_system_mouse_cursor_is_turned_off ( void );
EXTERN void make_sure_system_mouse_cursor_is_turned_on ( void );
EXTERN void blit_our_own_mouse_cursor ( void );
EXTERN void blit_mouse_cursor_corona ( void );
EXTERN SDL_Surface* rip_rectangle_from_alpha_image ( SDL_Surface* our_surface , SDL_Rect our_rect ) ;
EXTERN SDL_Surface* CreateAlphaCombinedSurface ( SDL_Surface* FirstSurface , SDL_Surface* SecondSurface );
EXTERN SDL_Surface* CreateColorFilteredSurface ( SDL_Surface* FirstSurface , int FilterType );
EXTERN void fade_out_using_gamma_ramp ( void );
EXTERN void fade_in_using_gamma_ramp ( void );
EXTERN void DisplayImage(char *file_name);
EXTERN void MakeGridOnScreen( SDL_Rect* Grid_Rectangle );
EXTERN void SetCombatScaleTo(float);
EXTERN void InitPictures (void);
EXTERN void InitTimer (void);
EXTERN void InitVideo (void);
EXTERN void InitOurBFonts (void);
EXTERN void FreeOurBFonts (void);
EXTERN void ClearGraphMem ( void );
EXTERN void SDL_HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area );
EXTERN void HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area );
EXTERN void ShadowingRectangle ( SDL_Surface* Surface , SDL_Rect Area );
EXTERN int do_graphical_number_selection_in_range ( int lower_range , int upper_range, int default_value );
EXTERN Uint8 GetAlphaComponent ( SDL_Surface* surface , int x , int y );

// saveloadgame.c 
#undef EXTERN
#ifdef _saveloadgame_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ShowSaveLoadGameProgressMeter( int Percentage , int IsSavegame ) ;
EXTERN void LoadAndShowThumbnail ( char* CoreFilename );
EXTERN int SaveGame( void );
EXTERN int LoadGame( void );
EXTERN int DeleteGame( void );
EXTERN void LoadAndShowStats ( char* CoreFilename );

/* Primitive types */

/* Saving is done via macros */
#define save_pritype(Z,X,Y) fprintf(SaveGameFile, Z, X, *(Y))
#define save_char(X,Y) save_pritype("%s: %hhd\n", X, Y)
#define save_uchar(X,Y) save_pritype("%s: %hhu\n", X, Y)
#define save_uint16_t(X,Y) save_pritype("%s: %hu\n", X, Y)
#define save_int16_t(X,Y) save_pritype("%s: %hd\n", X, Y)
#define save_int32_t(X,Y) save_pritype("%s: %d\n", X, Y)
#define save_uint32_t(X,Y) save_pritype("%s: %u\n", X, Y)
#define save_float(X,Y) save_pritype("%s: %f\n", X, Y)
#define save_double(X,Y) save_pritype("%s: %lf\n", X, Y)
#define save_string(X,Y) save_pritype("%s: %s\n", X, Y)

/* Reading is slightly more difficult so we do it with functions */
EXTERN void read_int32_t(const char *, const char *, int32_t *);
EXTERN void read_int16_t(const char *, const char *, int16_t *);
EXTERN void read_char(const char *, const char *, char *);
EXTERN void read_uint32_t(const char *, const char *, uint32_t *);
EXTERN void read_uint16_t(const char *, const char *, uint16_t *);
EXTERN void read_uchar(const char *, const char *, unsigned char *);
EXTERN void read_double(const char *, const char *, double *);
EXTERN void read_float(const char *, const char *, float *);
EXTERN void read_string(const char *, const char *, char *);
EXTERN void read_enemy_ptr(const char *, const char *, enemy **);

/* Array writing/reading */
EXTERN void save_moderately_finepoint_array(const char *, moderately_finepoint *, int);
EXTERN void read_moderately_finepoint_array(const char *, const char *, moderately_finepoint *, int);
EXTERN void save_mission_array(const char *, mission *, int);
EXTERN void read_mission_array(const char *, const char *, mission *, int);
EXTERN void save_int32_t_array(const char *, int *, int);
EXTERN void read_int32_t_array(const char *, const char *, int *, int);
EXTERN void save_item_array(const char *, item *, int);
EXTERN void read_item_array(const char *, const char *, item *, int);
EXTERN void save_uchar_array(const char *, unsigned char *, int);
EXTERN void read_uchar_array(const char *, const char *, unsigned char *, int);
EXTERN void save_uint16_t_array(const char *, uint16_t *, int);
EXTERN void read_uint16_t_array(const char *, const char *, uint16_t *, int);
EXTERN void save_gps_array(const char *, gps *, int);
EXTERN void read_gps_array(const char *, const char *, gps *, int);
EXTERN void save_float_array(const char *, float *, int);
EXTERN void read_float_array(const char *, const char *, float *, int);

/* Hacks */
EXTERN void save_chatflags_t_array(const char *, chatflags_t * , int);
EXTERN void read_chatflags_t_array(const char *, const char *, chatflags_t *, int);
EXTERN void save_cookielist_t_array(const char *, cookielist_t *, int);
EXTERN void read_cookielist_t_array(const char *, const char *, cookielist_t *, int);
#define save_automap_data_t_array save_automap_data
EXTERN void save_automap_data(const char*, automap_data_t *, int);
EXTERN void read_automap_data_t_array(char *, char *, automap_data_t *, int);
EXTERN void save_bigscrmsg_t_array(const char *, bigscrmsg_t *, int);
EXTERN void read_bigscrmsg_t_array(const char *, const char *, bigscrmsg_t *, int);
EXTERN void save_sdl_rect(const char *, SDL_Rect *);
EXTERN int read_sdl_rect(const char *, const char *, SDL_Rect *);
#define save_list_head_t(X,Y) 
#define read_list_head_t(X,Y,Z)

// mission.c 
#undef EXTERN
#ifdef _mission_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void quest_browser_interface ( void );
EXTERN void AssignMission( int MissNum );
EXTERN void GetQuestList ( char* QuestListFilename ) ;
EXTERN void clear_tux_mission_info ( void ) ;
EXTERN void CheckIfMissionIsComplete ( void );
EXTERN void quest_browser_enable_new_diary_entry ( int mis_num , int mis_diary_entry_num );

// map.c 
#undef EXTERN
#ifdef _map_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void respawn_level ( int level_num );
EXTERN void glue_obstacles_to_floor_tiles_for_level ( int level_num );
EXTERN void ResolveMapLabelOnShip ( char* MapLabel , location* PositionPointer );
EXTERN void CollectAutomapData ( void ) ;
EXTERN int smash_obstacle ( float x , float y );
EXTERN void AnimateTeleports (void);
EXTERN Uint16 GetMapBrick (Level deck, float x, float y);

EXTERN void CountNumberOfDroidsOnShip ( void );
EXTERN int LoadShip (char *filename);
EXTERN int SaveShip(const char *filename);
EXTERN void GetAllAnimatedMapTiles (Level Lev);
EXTERN int GetCrew (char *shipname);

EXTERN void AnimateCyclingMapTiles (void);
EXTERN void MoveLevelDoors ( void ) ;
EXTERN void WorkLevelGuns ( void ) ;
EXTERN int IsVisible ( GPS objpos ) ;
EXTERN void DeleteWaypoint (level *Lev, int num);
EXTERN void CreateWaypoint (level *Lev, int x, int y);
#define translate_map_point_to_screen_pixel translate_map_point_to_screen_pixel_func
#define translate_map_point_to_screen_pixel_x(X,Y)  ( UserCenter_x + rintf(( (X) + Me . pos . y - Me . pos . x - (Y) ) * (iso_floor_tile_width_over_two) ) )
#define translate_map_point_to_screen_pixel_y(X,Y)  ( UserCenter_y + rintf(( (X) + (Y) - Me . pos . x - Me . pos . y ) * (iso_floor_tile_height_over_two))) 
EXTERN void translate_map_point_to_screen_pixel_func( float x_map_pos, float y_map_pos, int * x_res, int * y_res, float zoom_factor);
EXTERN float translate_pixel_to_map_location ( float axis_x , float axis_y , int give_x ) ;

//colldet.c
#undef EXTERN
#ifdef _map_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int FilterWalkableCallback(colldet_filter* this, obstacle* obs, int obs_idx);
EXTERN int FilterFlyableCallback(colldet_filter* this, obstacle* obs, int obs_idx);
EXTERN int FilterVisibleCallback(colldet_filter* this, obstacle* obs, int obs_idx);
EXTERN int FilterObstacleByIdCallback(colldet_filter* this, obstacle* obs, int obs_idx);
EXTERN colldet_filter FilterWalkable;
EXTERN colldet_filter FilterFlyable;
EXTERN colldet_filter FilterVisible;
EXTERN colldet_filter FilterObstacleById;
EXTERN int CheckIfWayIsFreeOfDroids (char test_tux, float x1 , float y1 , float x2 , float y2 , int OurLevel , enemy * ExceptedRobot ) ;
EXTERN int EscapeFromObstacle( float* posX, float* posY, int posZ, colldet_filter* filter );
EXTERN int SinglePointColldet ( float x , float y , int z, colldet_filter* filter ) ;
EXTERN int DirectLineColldet( float x1 , float y1 , float x2 , float y2 , int z, colldet_filter* filter );
EXTERN int normalize_vect ( float, float, float *, float *);

// sound.c  OR nosound.c 
#undef EXTERN
#ifdef _sound_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void LoadAllStaticModFiles( void );
EXTERN void PlayOnceNeededSoundSample( const char* SoundSampleFileName , const int With_Waiting , const int no_double_catching );
EXTERN void InitAudio(void);
EXTERN void SetBGMusicVolume(float);
EXTERN void SetSoundFXVolume(float);
EXTERN void SwitchBackgroundMusicTo ( char * filename_raw );
EXTERN void GotHitSound (void);
EXTERN void tux_scream_sound (void);
EXTERN void No_Ammo_Sound ( void );
EXTERN void Not_Enough_Power_Sound( void );
EXTERN void Not_Enough_Dist_Sound( void );
EXTERN void Not_Enough_Mana_Sound( void );
EXTERN void CrySound (void);
EXTERN void CantCarrySound (void);
EXTERN void TransferSound (void);
EXTERN void MenuItemSelectedSound (void);
EXTERN void MenuItemDeselectedSound (void);
EXTERN void MoveMenuPositionSound (void);
EXTERN void teleport_arrival_sound (void);
EXTERN void healing_spell_sound ( void );
EXTERN void application_requirements_not_met_sound ( void );
EXTERN void Fire_Bullet_Sound (int);
EXTERN void Mission_Status_Change_Sound (void);
EXTERN void BounceSound (void);
EXTERN void DruidBlastSound (void);
EXTERN void ExterminatorBlastSound (void);
EXTERN void ThouArtDefeatedSound (void);
EXTERN void Takeover_Set_Capsule_Sound (void);
EXTERN void Takeover_Game_Won_Sound (void);
EXTERN void Takeover_Game_Deadlock_Sound (void);
EXTERN void Takeover_Game_Lost_Sound (void);
EXTERN void PlayGreetingSound ( int SoundCode );
EXTERN void play_enter_attack_run_state_sound ( int SoundCode );
EXTERN void play_death_sound_for_bot ( enemy* ThisRobot );
EXTERN void play_item_sound ( int item_type ) ;
EXTERN void PlayLevelCommentSound ( int levelnum );
EXTERN void PlayEnemyGotHitSound ( int enemytype );
EXTERN void BulletReflectedSound (void);
EXTERN void Play_Spell_ForceToEnergy_Sound( void );
EXTERN void Play_Spell_DetectItems_Sound( void );
EXTERN void play_melee_weapon_hit_something_sound ( void );
EXTERN void play_melee_weapon_missed_sound ( void );
EXTERN void play_open_chest_sound ( void );
EXTERN void play_sample_using_WAV_cache( char* SoundSampleFileName , int With_Waiting , int no_double_catching ) ;
EXTERN void play_sample_using_WAV_cache_v( char* SoundSampleFileName , int With_Waiting , int no_double_catching ,double volume) ;
// items.c
#undef EXTERN
#ifdef _items_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void handle_player_identification_command ( void );
EXTERN void MoveItem( item* SourceItem , item* DestItem );
EXTERN void silently_unhold_all_items ( void );
EXTERN int required_magic_stat_for_next_level_and_item ( int item_type );
EXTERN int CountItemtypeInInventory( int Itemtype );
EXTERN void DeleteInventoryItemsOfType( int Itemtype , int amount );
EXTERN void DeleteOneInventoryItemsOfType( int Itemtype );
EXTERN void DamageItem( item* CurItem );
EXTERN void DamageWeapon( item* CurItem );
EXTERN int GetFreeInventoryIndex( void );
EXTERN int ItemCanBeDroppedInInv ( int ItemType , int InvPos_x , int InvPos_y );
EXTERN long calculate_item_buy_price ( item* BuyItem );
EXTERN long calculate_item_repair_price ( item* repair_item );
EXTERN long calculate_item_sell_price ( item* BuyItem );
EXTERN void FillInItemProperties( item* ThisItem , int FullDuration , int multiplicity);
EXTERN void DamageProtectiveEquipment( void ) ;
EXTERN void DropItemAt( int ItemType , int level_num , float x , float y , int prefix , int suffix , int multiplicity );
EXTERN void Quick_ApplyItem( int ItemKey );
EXTERN int MatchItemWithName ( int type, const char * name );
EXTERN int GetItemIndexByName ( const char * name );
EXTERN void ApplyItem( item* CurItem );
EXTERN int Inv_Pos_Is_Free( int x , int y );
EXTERN int GetInventoryItemAt ( int x , int y );
EXTERN item* GetHeldItemPointer( void );
EXTERN Item FindPointerToPositionCode ( int PositionCode ) ;
EXTERN int ItemUsageRequirementsMet( item* UseItem , int MakeSound );
EXTERN int MouseCursorIsInInventoryGrid( int x , int y );
EXTERN int MouseCursorIsInUserRect( int x , int y );
EXTERN int MouseCursorIsInInvRect( int x , int y );
EXTERN int MouseCursorIsInChaRect( int x , int y );
EXTERN int MouseCursorIsInSkiRect( int x , int y );
EXTERN int GetHeldItemCode ( void );
EXTERN int GetInventorySquare_x( int x );
EXTERN int GetInventorySquare_y( int x );
EXTERN void DropHeldItemToInventory( void );
EXTERN int DropHeldItemToTheFloor ( void );
EXTERN void DropItemToTheFloor ( Item DropItemPointer , float x , float y , int levelnum ) ;
EXTERN void ShowQuickInventory ( void );
EXTERN void HandleInventoryScreen ( void );
EXTERN int AddFloorItemDirectlyToInventory( item* ItemPointer );
EXTERN void CopyItem( item* SourceItem , item* DestItem , int MakeSound );
EXTERN void DeleteItem( item* Item );
EXTERN void DropRandomItem( int level_num , float x , float y , int class , int ForceMagical );
EXTERN int get_floor_item_index_under_mouse_cursor ( void );
EXTERN int item_is_currently_equipped( item* Item );
EXTERN int Get_Prefixes_Data ( char * DataPointer );

// character.c
#undef EXTERN
#ifdef _character_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void DisplayButtons( void );
EXTERN void UpdateAllCharacterStats ( void );
EXTERN void ShowCharacterScreen ( void );
EXTERN void HandleCharacterScreen ( void );

#undef EXTERN
#ifdef _leveleditor_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void create_new_obstacle_on_level ( Level EditLevel , int our_obstacle_type , float pos_x , float pos_y );
EXTERN void show_button_tooltip ( char* tooltip_text );
EXTERN void delete_obstacle ( level* EditLevel , obstacle* our_obstacle );
EXTERN void ExportLevelInterface ( int level_num ) ;
EXTERN void give_new_name_to_obstacle ( Level EditLevel , obstacle* our_obstacle , char* predefined_name );
EXTERN void action_remove_obstacle ( Level EditLevel, obstacle *our_obstacle);
EXTERN void ItemDropFromLevelEditor ();
EXTERN void level_editor_cycle_marked_obstacle();
EXTERN void level_editor_place_aligned_obstacle();
EXTERN void level_editor_next_tab();
EXTERN void level_editor_action_change_map_label_user();
EXTERN void level_editor_action_undo();
EXTERN void level_editor_action_redo();
EXTERN void level_editor_action_toggle_waypoint_connection_user (void);
EXTERN void level_editor_beautify_grass_tiles();

// skills.c
#undef EXTERN
#ifdef _skills_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void RadialVMXWave ( gps ExpCenter , int SpellCostsMana );
EXTERN void RadialEMPWave ( gps ExpCenter , int SpellCostsMana );
EXTERN void RadialFireWave ( gps ExpCenter , int SpellCostsMana );
EXTERN void ShowSkillsScreen ( void );
EXTERN void HandleCurrentlyActivatedSkill( void );
EXTERN int DoSkill( int skill_index, int SpellCost);
EXTERN void activate_nth_aquired_skill ( int skill_num );
EXTERN void ImproveSkill(int *skill);
EXTERN int get_program_index_with_name(const char *);

// input.c 
#undef EXTERN
#ifdef _input_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN Uint8 * key_state_array;
EXTERN Uint8 mouse_state_last_frame;
EXTERN int input_handle( void );
EXTERN void save_mouse_state(void);
EXTERN void init_keyboard_input_array(void);

EXTERN int MouseWheelUpPressed(void);
EXTERN int MouseWheelDownPressed(void);
EXTERN int GetMousePos_x(void);
EXTERN int GetMousePos_y(void);
EXTERN int MouseRightPressed(void); 
EXTERN int MouseLeftPressed(void);
EXTERN int MouseLeftClicked(void);
EXTERN int MouseRightClicked(void);
EXTERN int MouseLeftUnclicked(void);
EXTERN int MouseRightUnclicked(void);
EXTERN int LeftPressed (void) ;
EXTERN int RightPressed (void) ;
EXTERN int UpPressed (void) ;
EXTERN int DownPressed (void) ;
EXTERN int SpacePressed (void) ;
EXTERN int EnterPressed (void) ;
EXTERN int EscapePressed (void) ;
EXTERN int LeftCtrlPressed (void) ;
EXTERN int CtrlPressed (void) ;
EXTERN int ShiftPressed (void) ;
EXTERN int AltPressed (void) ;
EXTERN int APressed (void) ;
EXTERN int HPressed (void) ;
EXTERN int MPressed (void) ;
EXTERN int QPressed (void) ;
EXTERN int SPressed (void) ;
EXTERN int WPressed (void) ;
EXTERN int XPressed (void) ;

#undef EXTERN
#ifdef _keyboard_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int input_key_press(SDLKey, SDLMod);
EXTERN void keychart(void);
EXTERN void input_set_default(void);
EXTERN void input_keyboard_init(void);
EXTERN int getchar_raw (void);
EXTERN void input_get_keybind(char *cmdname, SDLKey *key, SDLMod *mod);

// menu.c 
#undef EXTERN
#ifdef _menu_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int ResolveDialogSectionToChatFlagsIndex ( char* SectionName );
EXTERN void clear_player_inventory_and_stats ( void );
EXTERN void StoreMenuBackground ( int backup_slot );
EXTERN void RestoreMenuBackground ( int backup_slot );
EXTERN int DoMenuSelection( char* InitialText , char *MenuTexts[], int FirstItem , int background_code , void* MenuFont );
EXTERN int ChatDoMenuSelectionFlagged( char* InitialText , char** MenuTexts, 
				       unsigned char * Chat_Flags, int FirstItem , 
				       int background_code , void* MenuFont , enemy* ChatDroid );
EXTERN int ChatDoMenuSelection( char **MenuTexts, int FirstItem , void* MenuFont , enemy* ChatDroid );
EXTERN void StartupMenu (void);
EXTERN void InitiateMenu( int background_code );
EXTERN void Cheatmenu (void);
EXTERN void EscapeMenu (void);
EXTERN int GetNumberOfTextLinesNeeded ( char* GivenText, SDL_Rect GivenRectangle , float text_stretch );

// misc.c 
#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif
#define CURLEVEL (curShip . AllLevels [ Me . pos . z ])
EXTERN void print_trace ( int signum );
EXTERN void implant_backtrace_into_signal_handlers ( void ) ;
EXTERN void adapt_button_positions_to_screen_resolution( void );
EXTERN void ErrorMessage ( const char* FunctionName , const char* ProblemDescription, int InformDevelopers , int IsFatal, ... );
EXTERN void UpdateScreenOverButtonFromList ( int ButtonIndex );
EXTERN void ShowGenericButtonFromList ( int ButtonIndex );
EXTERN int MouseCursorIsInRect ( SDL_Rect* our_rect , int x , int y );
EXTERN int MouseCursorIsOnButton( int ButtonIndex , int x , int y );
EXTERN void *MyMemmem ( char *haystack, size_t haystacklen, char *needle, size_t needlelen);
EXTERN char* ReadAndMallocStringFromData ( char* SearchString , const char* StartIndicationString , const char* EndIndicationString );
EXTERN char* ReadAndMallocStringFromDataOptional ( char* SearchString , const char* StartIndicationString , const char* EndIndicationString, char Terminator );
EXTERN int CountStringOccurences ( char* SearchString , const char* TargetString ) ;
EXTERN void ReadValueFromStringWithDefault( char* SearchBeginPointer , const char* ValuePreceedText , const char* FormatString , const char * DefaultValueString, void* TargetValue , char* EndOfSearchSectionPointer );
EXTERN void ReadValueFromString( char* SearchBeginPointer , const char* ValuePreceedText , const char* FormatString , void* TargetValue , char* EndOfSearchSectionPointer );
EXTERN char* ReadAndMallocAndTerminateFile( char* filename , const char* File_End_String ) ;
EXTERN char* LocateStringInData ( char* SearchBeginPointer, const char* SearchTextPointer ) ;
EXTERN int find_file (const char *fname, const char *datadir, char * File_Path, int silent);
EXTERN void CheckForTriggeredEventsAndStatements (void);
EXTERN void Pause (void);
EXTERN void ComputeFPSForThisFrame(void);
EXTERN void StartTakingTimeForFPSCalculation(void);
EXTERN int Get_Average_FPS ( void );
EXTERN float Frame_Time (void);
EXTERN void Activate_Conservative_Frame_Computation(void);
EXTERN void gotoxy (int, int);
EXTERN int MyRandom (int);
EXTERN void Teleport ( int LNum , float X , float Y , int WithSound ) ;
EXTERN int SaveGameConfig (void);
EXTERN int LoadGameConfig (void);
EXTERN void InsertNewMessage (void);
EXTERN void Terminate (int);
EXTERN void ShowDebugInfos (void);
EXTERN int GiveNumberToThisActionLabel ( char* ActionLabel );
EXTERN void ExecuteActionWithLabel ( char* ActionLabel ) ;
EXTERN void ExecuteEvent ( int EventNumber );
EXTERN char *va (char *format, ...);
EXTERN Sint16 ReadSint16 (void * memory);
EXTERN void endian_swap(char * pdata, size_t dsize, size_t nelements);

// enemy.c 
#undef EXTERN
#ifdef _enemy_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void robot_group_turn_hostile ( enemy * );
EXTERN void SetRestOfGroupToState ( Enemy ThisRobot , short NewState );
EXTERN int MakeSureEnemyIsInsideHisLevel ( Enemy ThisRobot );
EXTERN void ShuffleEnemys ( int LevelNum );
EXTERN int CheckEnemyEnemyCollision (enemy *);
EXTERN void MoveEnemys (void);
EXTERN void ClearEnemys (void);
EXTERN void InitEnemy (enemy *);
EXTERN void start_gethit_animation_if_applicable ( enemy* ThisRobot ) ;
EXTERN void animate_enemy ( enemy * our_bot ) ;
EXTERN void hit_enemy ( enemy * target, float hit, char givexp, short int killertype, char mine);
EXTERN enemy * enemy_resolve_address ( short int enemy_number, enemy ** enemy_addr );
EXTERN void enemy_set_reference ( short int * enemy_number, enemy ** enemy_addr, enemy * addr);
EXTERN void enemy_generate_level_lists ();

#define BROWSE_ALIVE_BOTS_SAFE(X,Y) list_for_each_entry_safe(X, Y, &alive_bots_head, global_list)
#define BROWSE_ALIVE_BOTS(X) list_for_each_entry(X, &alive_bots_head, global_list)
#define BROWSE_DEAD_BOTS_SAFE(X,Y) list_for_each_entry_safe(X, Y, &dead_bots_head, global_list)
#define BROWSE_DEAD_BOTS(X) list_for_each_entry(X, &dead_bots_head, global_list)
#define BROWSE_LEVEL_BOTS_SAFE(X,Y,L) list_for_each_entry_safe(X,Y, &level_bots_head[(L)], level_list)
#define BROWSE_LEVEL_BOTS(T,L) list_for_each_entry(T, &level_bots_head[(L)], level_list)


// ship.c 
#undef EXTERN
#ifdef _ship_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ShowDeckMap (void);
EXTERN void ShowItemInfo ( item* ShowItem , int page , char ShowArrows , int background_code , int title_text_flag );
EXTERN void EnterLift (void);
EXTERN void EnterItemIdentificationBooth( void );
EXTERN void EnterCodepanel (void);
EXTERN void EnterConsole (void);
EXTERN void AlleLevelsGleichFaerben (void);
void ShowDroidInfo (int droidtype, int page , char ShowArrows );
void PaintConsoleMenu (int menu_pos);
EXTERN void write_full_item_name_into_string ( item* ShowItem , char* full_item_name ) ;

// text.c 
#undef EXTERN
#ifdef _text_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void show_backgrounded_label_at_map_position ( char* LabelText , float fill_status , float pos_x , float pos_y , int zoom_is_on );
EXTERN void show_backgrounded_text_rectangle ( char* text , int x , int y , int w , int h );
EXTERN char * GetEditableStringInPopupWindow ( int MaxLen , char* PopupWindowTitle , char* DefaultString );
EXTERN void GiveMouseAlertWindow ( const char* WindowText ) ;
EXTERN int CutDownStringToMaximalSize ( char* StringToCut , int LengthInPixels );
EXTERN void SetNewBigScreenMessage( char* ScreenMessageText );
EXTERN void DisplayBigScreenMessage( void );
EXTERN char* GetChatWindowInput( SDL_Surface* Background , SDL_Rect* Chat_Window_Pointer );
EXTERN void ChatWithFriendlyDroid( Enemy ChatDroid );
EXTERN void EnemyHitByBulletText( enemy * );
EXTERN void EnemyInfluCollisionText ( enemy * );

EXTERN void SetTextCursor (int x, int y);
EXTERN void SetLineLength (int);

EXTERN int DisplayText ( const char *text, int startx, int starty, const SDL_Rect *clip , float text_stretch );
EXTERN void display_current_chat_protocol ( int background_picture_code , enemy* ChatDroid , int with_update );

EXTERN void DisplayChar (unsigned char c);
EXTERN int ScrollText (char *text, int startx, int starty, int background_code );

EXTERN int ImprovedCheckLineBreak(char *text, const SDL_Rect *clip , float text_stretch );
EXTERN char *PreviousLine (char *textstart, char *text);
EXTERN char *NextLine (char *text);
EXTERN char *GetString ( int max_len , int background_code , const char* text_for_overhead_promt ) ;
EXTERN void printf_SDL (SDL_Surface *screen, int x, int y, const char *fmt, ...);

// text_public.c 
#undef EXTERN
#ifdef _text_public_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void LoadChatRosterWithChatSequence ( char* FullPathAndFullFilename );
EXTERN void delete_one_dialog_option ( int i , int FirstInitialisation );
EXTERN void InitChatRosterForNewDialogue( void );
EXTERN void DebugPrintf (int db_level, const char *fmt, ...);
EXTERN void *MyMalloc (long);
EXTERN void PrepareMultipleChoiceDialog ( Enemy ChatDroid , int with_flip );
EXTERN int FS_filelength (FILE *f);
EXTERN void inflate_stream(FILE *, unsigned char **, int *);

// hud.c 
#undef EXTERN
#ifdef _hud_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void GiveItemDescription ( char* ItemDescText , item* CurItem , int ForShop );
EXTERN void DisplayBanner ( void );
EXTERN int get_days_of_game_duration ( float current_game_date );
EXTERN int get_hours_of_game_duration ( float current_game_date );
EXTERN int get_minutes_of_game_duration ( float current_game_date );
EXTERN void append_new_game_message ( char* game_message_text );
EXTERN void display_current_game_message_window ( void );
EXTERN void toggle_game_config_screen_visibility ( int screen_visible );

// shop.c 
#undef EXTERN
#ifdef _shop_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ShowRescaledItem ( int position , int TuxItemRow , item* ShowItem );
EXTERN int TryToIntegrateItemIntoInventory ( item* BuyItem , int AmountToBuyAtMost );
EXTERN int AssemblePointerListForChestShow ( item** ItemPointerListPointer , moderately_finepoint chest_pos );
EXTERN int AssemblePointerListForItemShow ( item** ItemPointerListPointer , int IncludeWornItems );
EXTERN void InitTradeWithCharacter( int CharacterCode ) ;

// takeover.c 
#undef EXTERN
#ifdef _takeover_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int Takeover (enemy *);
EXTERN void ChooseColor (void);
EXTERN void PlayGame (void);
EXTERN void EnemyMovements (void);

EXTERN int GetTakeoverGraphics (void);
EXTERN void ShowPlayground (void);
EXTERN void InventPlayground (void);

EXTERN void ProcessPlayground (void);
EXTERN void ProcessDisplayColumn (void);
EXTERN void ProcessCapsules (void);
EXTERN void AnimateCurrents (void);

EXTERN void ClearPlayground (void);
EXTERN int IsActive (int color, int row);

// BFont.c
#undef EXTERN
#ifdef _bfont_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN Uint32 FdGetPixel32 (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN Uint32 FdGetPixel24 (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN Uint16 FdGetPixel16 (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN Uint8 FdGetPixel8 (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN void PutPixel32 (SDL_Surface * surface, int x, int y, Uint32 pixel);
EXTERN void PutPixel24 (SDL_Surface * surface, int x, int y, Uint32 pixel);
EXTERN void PutPixel16 (SDL_Surface * surface, int x, int y, Uint32 pixel);
EXTERN void PutPixel8 (SDL_Surface * surface, int x, int y, Uint32 pixel);
EXTERN Uint32 FdGetPixel (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN void PutPixel (SDL_Surface * surface, int x, int y, Uint32 pixel);

#undef EXTERN
#ifdef _LISTS_C
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void list_add(list_head_t *new, list_head_t * head);
EXTERN void list_add_tail(list_head_t *new, list_head_t * head);
EXTERN void list_del(list_head_t *entry);
EXTERN void list_del_init(list_head_t *entry);
EXTERN void list_move(list_head_t *list, list_head_t *head);
EXTERN void list_move_tail(list_head_t *list, list_head_t *head);
EXTERN int list_empty(const list_head_t *head);
EXTERN void list_splice(list_head_t *list, list_head_t *head);
EXTERN void list_splice_init(list_head_t *list, list_head_t *head);

#undef EXTERN
#ifdef _open_gl_atlas_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int load_texture_atlas ( const char *, const char *, char *filenames[], iso_image *, int ); 

//--------------------
// Leave this final endif in here!  It's the wrapper of the whole
// file to prevent double-definitions.
//
#endif
