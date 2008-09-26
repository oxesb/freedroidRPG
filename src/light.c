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

/**
 * This file contains all the functions managing the lighting inside
 * the freedroidRPG game window, i.e. the 'light radius' of the Tux and
 * of other light emanating objects.
 */

#define _light_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "map.h"
#include "proto.h"
#include "SDL_rotozoom.h"

//--------------------
// 
#define MAX_NUMBER_OF_LIGHT_SOURCES 100
moderately_finepoint light_sources [ MAX_NUMBER_OF_LIGHT_SOURCES ] ;
int light_source_strengthes [ MAX_NUMBER_OF_LIGHT_SOURCES ] ;
int light_strength_buffer [ 64 ] [ 48 ] ;

/**
 * There might be some obstacles that emit some light.  Yet, we can't
 * go through all the obstacle list of a level every frame at sufficiently
 * low cost.  Therefore we create a reduced list of light emitters for
 * usage in the light computation code.
 */
void 
update_light_list ( )
{
    int i;
    Level light_level = curShip . AllLevels [ Me . pos . z ] ;
    int map_x, map_y, map_x_end, map_y_end, map_x_start, map_y_start ;
    int glue_index;
    int obs_index;
    int next_light_emitter_index;
    obstacle* emitter;
    int blast;

    //--------------------
    // At first we fill out the light sources array with 'empty' information,
    // i.e. such positions, that won't affect our location for sure.
    //
    for ( i = 0 ; i < MAX_NUMBER_OF_LIGHT_SOURCES ; i ++ )
    {
	light_sources [ i ] . x = -200 ;
	light_sources [ i ] . y = -200 ;
	light_source_strengthes [ i ] = 0 ;
    }
    
    //--------------------
    // Now we fill in the Tux position as the very first light source, that will
    // always be present.
    //
    light_sources [ 0 ] . x = Me . pos . x ;
    light_sources [ 0 ] . y = Me . pos . y ;
    light_source_strengthes [ 0 ] = 
	light_level -> light_radius_bonus + Me . light_bonus_from_tux  ;
    //--------------------
    // We must not in any case tear a hole into the beginning of
    // the list though...
    //
    if ( light_source_strengthes [ 0 ] <= 0 )
	light_source_strengthes [ 0 ] = 1;
    next_light_emitter_index = 1 ;

    //--------------------
    // Now we can fill in any explosions, that are currently going on.
    // These will typically emanate a lot of light.
    //
    for ( blast = 0 ; blast < MAXBLASTS ; blast ++ )
    {
	if ( ! ( AllBlasts [ blast ] . type == DRUIDBLAST ) ) continue ;

	light_sources [ next_light_emitter_index ] . x = AllBlasts [ blast ] . pos . x ;
	light_sources [ next_light_emitter_index ] . y = AllBlasts [ blast ] . pos . y ;

	//--------------------
	// We add some light strength according to the phase of the blast
	//
	light_source_strengthes [ next_light_emitter_index ] = 10 - AllBlasts [ blast ] . phase / 2 ;
	if ( light_source_strengthes [ next_light_emitter_index ] < 0 )
	    continue;
	next_light_emitter_index ++ ;
	
	//--------------------
	// We must not write beyond the bounds of our light sources array!
	//
	if ( next_light_emitter_index >= MAX_NUMBER_OF_LIGHT_SOURCES - 1 )
	{
	    ErrorMessage ( __FUNCTION__  , "\
WARNING!  End of light sources array reached!",
				       NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	    return;
	}
    }

    //--------------------
    // Now we can fill in the remaining light sources of this level.
    // First we do all the obstacles:
    //
    map_x_start = Me . pos . x - 12 ;
    map_y_start = Me . pos . y - 12 ;
    map_x_end = Me . pos . x + 12 ;
    map_y_end = Me . pos . y + 12 ;
    if ( map_x_start < 0 ) map_x_start = 0 ;
    if ( map_y_start < 0 ) map_y_start = 0 ;
    if ( map_x_end >= light_level -> xlen ) map_x_end = light_level -> xlen - 1 ;
    if ( map_y_end >= light_level -> ylen ) map_y_end = light_level -> ylen - 1 ;
    for ( map_x = map_x_start ; map_x < map_x_end ; map_x ++ )
    {
	for ( map_y = map_y_start ; map_y < map_y_end ; map_y ++ )
	{
	    for ( glue_index = 0 ; glue_index < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; glue_index ++ )
	    {
		//--------------------
		// end of obstacles glued to here?  great->next square
		//
		if ( light_level -> map [ map_y ] [ map_x ] . obstacles_glued_to_here [ glue_index ] == (-1) )
		    break;

		obs_index = light_level -> map [ map_y ] [ map_x ] . obstacles_glued_to_here [ glue_index ] ;
		emitter = & ( light_level -> obstacle_list [ obs_index ] );

		if ( obstacle_map [ emitter -> type ] . emitted_light_strength == 0 )
		    continue;

		//--------------------
		// Now we know that this one needs to be inserted!
		//
		light_sources [ next_light_emitter_index ] . x = emitter -> pos . x ;
		light_sources [ next_light_emitter_index ] . y = emitter -> pos . y ;
		light_source_strengthes [ next_light_emitter_index ] = 
		    obstacle_map [ emitter -> type ] . emitted_light_strength ;
		next_light_emitter_index ++ ;

		//--------------------
		// We must not write beyond the bounds of our light sources array!
		//
		if ( next_light_emitter_index >= MAX_NUMBER_OF_LIGHT_SOURCES - 1 )
		{
		    ErrorMessage ( __FUNCTION__  , "\
WARNING!  End of light sources array reached!",
					       NO_NEED_TO_INFORM, IS_WARNING_ONLY );
		    return;
		}
	    }
	}
    }
   
    enemy *erot;
    BROWSE_LEVEL_BOTS(erot, Me.pos.z) 
    {
	if ( fabsf ( Me . pos . x - erot->pos . x ) >= 12 ) 
	    continue;

	if ( fabsf ( Me . pos . y - erot->pos . y ) >= 12 )
	    continue;

	//--------------------
	// Now we know that this one needs to be inserted!
	//
	light_sources [ next_light_emitter_index ] . x = erot->pos . x ;
	light_sources [ next_light_emitter_index ] . y = erot->pos . y ;
	light_source_strengthes [ next_light_emitter_index ] = -14 ;
	next_light_emitter_index ++ ;

	//--------------------
	// We must not write beyond the bounds of our light sources array!
	//
	if ( next_light_emitter_index >= MAX_NUMBER_OF_LIGHT_SOURCES - 1 )
	{
	    ErrorMessage ( __FUNCTION__  , "\
WARNING!  End of light sources array reached!",
				       NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	    return;
	}
	
    }
    
}; // void update_light_list ( )

/**
 * This function is used to find the light intensity at any given point
 * on the map.
 */
static int 
calculate_light_strength ( moderately_finepoint target_pos )
{
    int final_darkness = NUMBER_OF_SHADOW_IMAGES;
    int i;
    float xdist;
    float ydist;
    float light_vec_len;

    //--------------------
    // Now that the light sources array is fully set up, we can start
    // to compute the individual light strength at any given position
    //
    for ( i = 0 ; i < MAX_NUMBER_OF_LIGHT_SOURCES ; i ++ )
    {
	//--------------------
	// Now if we've reached the end of the current list of light
	// sources, then we can stop immediately.
	//
	if ( light_source_strengthes [ i ] == 0 )
	{
	    if ( final_darkness < -curShip . AllLevels [ Me . pos . z ] -> minimum_light_value )
		return ( final_darkness );
	    else
		return ( -curShip . AllLevels [ Me . pos . z ] -> minimum_light_value );
	}

	//--------------------
	// We could of course use a maximum function to find out the proper light at
	// any place.  But maybe addition of light would be better, so we use the latter
	// code.
	//
	// If the sign of the inequality determines the outcome already, then we need not
	// do anything else.  This might save a lot of computation time... and also it 
	// allows to take the square of the inequality after that SAFELY!!!
	//
	if ( final_darkness + light_source_strengthes [ i ] <= 0 ) continue;

	//--------------------
	// This function is time-critical, simply because it's used a lot every frame for 
	// the light radius.  Therefore we avoid square roots here and use squared for the
	// inequality now.  Let's see if that has some numerical advantage already...
	//
	xdist = light_sources [ i ] . x - target_pos . x ;
	ydist = light_sources [ i ] . y - target_pos . y ;

	if (  ( xdist * xdist + ydist * ydist ) * 4.0 * 4.0 
	     < 
 	     ( final_darkness + light_source_strengthes [ i ] ) *  
  	     ( final_darkness + light_source_strengthes [ i ] ) )
	{

	    //--------------------
	    // Let's try something different here:  We now do some passability
	    // checking as well!  Cool!  But only for the very first light source
	    //
	    /*float */light_vec_len = sqrt(xdist * xdist + ydist * ydist);

	if ( light_vec_len > 0.5 )
	    {
		if ( curShip . AllLevels [ Me . pos . z ] -> use_underground_lighting )
		{
		    if ( ! DirectLineColldet( light_sources [ i ] . x , light_sources [ i ] . y , 
					       target_pos . x , target_pos . y , 
					       Me . pos . z, &FilterVisible ) )
			continue;
		}
	    }
	    final_darkness = (int) ( light_vec_len * 4.0 ) - light_source_strengthes [ i ] ;
	}
    }
    
    if ( final_darkness < -curShip . AllLevels [ Me . pos . z ] -> minimum_light_value )
	return ( final_darkness );
    else
	return ( -curShip . AllLevels [ Me . pos . z ] -> minimum_light_value );
    
}; // int calculate_light_strength ( moderately_finepoint target_pos )

/**
 * When the light radius (i.e. the shadow values for the floor) has been
 * set up, the shadows usually are very 'hard' in the sense that extreme
 * darkness can be right next to very bright light.  This does not look
 * very real.  Therefore we 'soften' the shadow a bit, by allowing only
 * a limited step size from one shadow square to the next.  Of course 
 * these changes have to be propagated, so we run through the whole
 * shadow grid twice and propagate in 'both' directions.  
 * The hardness of the shadow can be controlled in the definition of
 * MAX_LIGHT_STEP.  Higher values will lead to harder shadows, while 
 * lower values will give very smooth and flourescent shadows propagating
 * even a bit under walls (which doesn't look too good).  4 seems like
 * a reasonable setting.
 */
static void
soften_light_distribution ( void )
{
    int x , y ;
#define MAX_LIGHT_STEP 3

    //--------------------
    // Now that the light buffer has been set up properly, we can start to
    // smoothen it out a bit.  We do so in the direction of more light.
    //
    for ( x = 1 ; x < (64-1) ; x ++ )
    {
	for ( y = 1 ; y < (48-1) ; y ++  )
	{
	    if ( light_strength_buffer [ x + 1 ] [ y ] > light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP )
		light_strength_buffer [ x + 1 ] [ y ] = light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP ;
	    if ( light_strength_buffer [ x ] [ y + 1 ] > light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP )
		light_strength_buffer [ x ] [ y + 1 ] = light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP ;
	    if ( light_strength_buffer [ x + 1 ] [ y + 1 ] > light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP )
		light_strength_buffer [ x + 1 ] [ y + 1 ] = light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP ;
	}
    }
    // now the same again, this time from right to left
    for ( x = (64-2) ; x > 1 ; x -- )
    {
	for ( y = ( 48 - 2 ) ; y > 1 ; y -- )
	{
	    if ( light_strength_buffer [ x - 1 ] [ y ] > light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP )
		light_strength_buffer [ x - 1 ] [ y ] = light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP ;
	    if ( light_strength_buffer [ x ] [ y - 1 ] > light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP )
		light_strength_buffer [ x ] [ y - 1 ] = light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP ;
	    if ( light_strength_buffer [ x - 1 ] [ y - 1 ] > light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP )
		light_strength_buffer [ x - 1 ] [ y - 1 ] = light_strength_buffer [ x ] [ y ] + MAX_LIGHT_STEP ;
	}
    }
}; // void soften_light_distribution ( void )

/**
 * This function is used to find the light intensity at any given point
 * on the map.
 */
void 
set_up_light_strength_buffer ( void )
{
    int x ;
    int y ;
    moderately_finepoint target_pos ;
    int screen_x ;
    int screen_y ;
    const float xrat = ( GameConfig . screen_width / 64 ) ;
    const float yrat = ( GameConfig . screen_height / 48 ) ;

	for ( x = 0 ; x < 64 ; x ++ )
    {
	for ( y = 0 ; y < 48 ; y ++  )
	{
	    screen_x = x * xrat - UserCenter_x;
	    screen_y = y * yrat - UserCenter_y;

	    target_pos . x = translate_pixel_to_map_location ( screen_x , screen_y , TRUE ) ;
	    target_pos . y = translate_pixel_to_map_location ( screen_x , screen_y , FALSE ) ;

	    light_strength_buffer [ x ] [ y ] = 
		calculate_light_strength ( target_pos );
	}
    }
    
    soften_light_distribution();

}; // void set_up_light_strength_buffer ( void )

/**
 * This function is used to find the light intensity at any given point
 * on the screen.
 */
int 
get_light_strength_screen ( int x, int y )
{

    x = ( x * 64 ) / GameConfig . screen_width ;
    y = ( y * 48 ) / GameConfig . screen_height ;

    if ( ( x < 0 ) || ( y < 0 ) || ( x >= 64 ) || ( y >= 48 ) )
    {
	//--------------------
	// If a request reaches outside the prepared buffer, we use the
	// nearest point, if we can get it easily, otherwise we'll use
	// blackness by default.
	//

	if ( ( x >= 0 ) && ( x < 64 ) )
	{
	    if ( y >= 48 )
		return ( light_strength_buffer [ x ] [ 47 ] );
	    if ( y < 0 )
		return ( light_strength_buffer [ x ] [ 0 ] );
	}
	if ( ( y >= 0 ) && ( y < 48 ) )
	{
	    if ( x >= 64 )
		return ( light_strength_buffer [ 63 ] [ y ] );
	    if ( x < 0 )
		return ( light_strength_buffer [ 0 ] [ y ] );
	}
	return ( NUMBER_OF_SHADOW_IMAGES );
    }

    return ( light_strength_buffer [ x ] [ y ] );
    
}; // int get_light_screen_strength ( moderately_finepoint target_pos )


/**
 * This function is used to find the light intensity at any given point
 * on the map.
 */
int 
get_light_strength ( moderately_finepoint target_pos )
{
   int x , y ;

   x = translate_map_point_to_screen_pixel_x ( target_pos . x ,  target_pos . y );
   y = translate_map_point_to_screen_pixel_y ( target_pos . x ,  target_pos . y );

   return get_light_strength_screen ( x, y );
}

/**
 * This function should blit the shadows on the floor, that are used to
 * generate the impression of a 'light radius' around the players 
 * character.
 */
void
blit_classic_SDL_light_radius( void )
{
    static int first_call = TRUE ;
    int i, j ;
char fpath[2048];
    char constructed_file_name[2000];
    int our_height, our_width, our_max_height, our_max_width;
    int light_strength;
    static int pos_x_grid [ (int)(MAX_FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ] [ (int)(MAX_FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2 ) ] ;
    static int pos_y_grid [ (int)(MAX_FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ] [ (int)(MAX_FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2 ) ] ;
    static SDL_Rect target_rectangle;
    int chunk_size_x;
    int chunk_size_y;
    SDL_Surface* tmp;
    
    //--------------------
    // If the darkness chunks have not yet been loaded, we load them...
    //
    if ( first_call )
    {
	first_call = FALSE;
	for ( i = 0 ; i < NUMBER_OF_SHADOW_IMAGES ; i ++ )
	{
	    sprintf ( constructed_file_name , "light_radius_chunks/iso_light_radius_darkness_%04d.png" , i + 1 );
	    find_file (constructed_file_name , GRAPHICS_DIR , fpath, 0 );
	    get_iso_image_from_file_and_path ( fpath , & ( light_radius_chunk [ i ] ) , TRUE ) ;
	    tmp = light_radius_chunk [ i ] . surface ;
	    light_radius_chunk [ i ] . surface = SDL_DisplayFormatAlpha ( light_radius_chunk [ i ] . surface ) ; 
	    SDL_FreeSurface ( tmp ) ;
	}
	
	pos_x_grid [ 0 ] [ 0 ] = translate_map_point_to_screen_pixel_x ( Me . pos . x - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) , Me . pos . y - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) ) - 10 ;
	pos_y_grid [ 0 ] [ 0 ] = translate_map_point_to_screen_pixel_y ( Me . pos . x - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) , Me . pos . y - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) ) - 42 ;
	
	chunk_size_x = 26 /2 + 1 ;
	chunk_size_y = 14 /2 ; 
	
	for ( i = 0 ; i < (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ; i ++ )
	{
	    for ( j = 0 ; j < (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ; j ++ )
	    {
		pos_x_grid [ i ] [ j ] = pos_x_grid [ 0 ] [ 0 ] + ( i - j ) * chunk_size_x ;
		pos_y_grid [ i ] [ j ] = pos_y_grid [ 0 ] [ 0 ] + ( i + j ) * chunk_size_y ;
	    }
	}
    }
    
    //--------------------
    // Now it's time to apply the light radius
    //
    our_max_width = FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2 ;
    our_max_height = our_max_width;

    for ( our_height = 0 ; our_height < our_max_height ; our_height ++ )
    {
	for ( our_width = 0 ; our_width < our_max_width ; our_width ++ )
	{
	    if ( our_width % LIGHT_RADIUS_CRUDENESS_FACTOR ) continue;
	    if ( our_height % LIGHT_RADIUS_CRUDENESS_FACTOR ) continue;
	    
	    target_rectangle . x = pos_x_grid [ our_width ] [ our_height ] ;
	    target_rectangle . y = pos_y_grid [ our_width ] [ our_height ] ;
	    light_strength = get_light_strength_screen ( target_rectangle . x, target_rectangle . y ) ;
	    
	    if ( light_strength >= NUMBER_OF_SHADOW_IMAGES ) light_strength = NUMBER_OF_SHADOW_IMAGES -1 ;
	    if ( light_strength <= 0 ) continue ;

	    our_SDL_blit_surface_wrapper( light_radius_chunk [ light_strength ] . surface , NULL , Screen, &target_rectangle );
	}
    }
}; // void blit_classic_SDL_light_radius( void )

/**
 * FreedroidRPG does some light/shadow computations and then draws some
 * shadows/light on the floor.  There is a certain amount of light around
 * the Tux.  This light is called the 'light radius'.  
 * 
 * This function is supposed to compute all light/shadow values for the
 * the floor and then draw the light radius on the floor.  Note, that 
 * this is something completely different from the prerendered shadows
 * that are to be blitted for obstacles, when they are exposed to 
 * daylight.
 */
void
blit_light_radius ( void )
{

    //--------------------
    // Before making any reference to the light, it's best to 
    // first calculate the values in the light buffer, because
    // those will be used when drawing the light radius.
    //
    set_up_light_strength_buffer ( );

    if ( use_open_gl )
    {
	// blit_open_gl_light_radius ();
	// blit_open_gl_cheap_light_radius ();
	blit_open_gl_stretched_texture_light_radius ();
    }
    else
    {
	blit_classic_SDL_light_radius();
    }
    
}; // void blit_light_radius ( void )

#undef _light_c
