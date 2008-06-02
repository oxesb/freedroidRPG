/***********************************************************/
/*                                                         */
/*   BFONT.c v. 1.0.3 - Billi Font Library by Diego Billi  */
/*   Heavily modified for FreedroidRPG needs over years    */
/*                                                         */
/***********************************************************/

#define _bfont_c

#include "system.h"
#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "SDL_rotozoom.h"


/* Current font */
static BFont_Info *CurrentFont;

/**
 *
 *
 */
static void
InitFont (BFont_Info * Font)
{
  unsigned int x = 0, i = 0;
  Uint32 sentry;
  SDL_Surface* tmp_char1;

  Font->h = Font->Surface->h;

  i = '!';
  sentry = SDL_MapRGB(Font->Surface->format, 255, 0, 255); 

  if (Font->Surface == NULL)
    {
      fprintf (stderr, "BFont: The font has not been loaded!\n");
      exit (1);
    }
  if (SDL_MUSTLOCK (Font->Surface))
    SDL_LockSurface (Font->Surface);
  x = 0;
  while (x < (Font->Surface->w - 1) && i < MAX_CHARS_IN_FONT)
    {
      if (FdGetPixel (Font->Surface, x, 0) != sentry)
	{
	  Font->Chars[i].x = x;
	  Font->Chars[i].y = 1;
	  Font->Chars[i].h = Font->Surface->h;
          while( x < (Font->Surface->w)){
               if(FdGetPixel (Font->Surface, x, 0) == sentry)
                   break;
               x++;
          }
	  Font->Chars[i].w = (x - Font->Chars[i].x);

	  //--------------------
	  // Now we make a copy for later reference when we do OpenGL based
	  // output of this character.
	  //
	  Font->number_of_chars = i + 1;
	  tmp_char1 = SDL_CreateRGBSurface( 0 , CharWidth ( Font , i ) , FontHeight (Font) -1 , 32, 
					    //0xFF000000 , 0x00FF0000  , 0x0000FF00 , 0x000000FF );
					    0x000000FF , 0x0000FF00  , 0x00FF0000 , 0xFF000000 );

	  SDL_SetAlpha( Font->Surface , 0 , 255 );
	  SDL_SetColorKey( Font->Surface , 0 , 0 );
	  Font -> char_iso_image [ i ] . surface = SDL_DisplayFormatAlpha ( tmp_char1 ) ;
	  Font -> char_iso_image [ i ] . texture_has_been_created = FALSE ;
	  our_SDL_blit_surface_wrapper ( Font->Surface, & ( Font -> Chars [ i ] ) , 
					 Font -> char_iso_image [ i ] . surface , NULL );
	  SDL_SetAlpha( Font -> char_iso_image [ i ] . surface , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
	  SDL_SetColorKey ( Font -> char_iso_image [ i ] . surface , 0 , 0 );

	  flip_image_vertically ( Font -> char_iso_image [ i ] . surface ) ;
	  
	  SDL_FreeSurface ( tmp_char1 );
	  //--------------------
	  // Now we can go on to the next char
	  //
	  i++;
	}
      else
	{
	  x++;
	}

    }
  Font->Chars[' '].x = 0;
  Font->Chars[' '].y = 0;
  Font->Chars[' '].h = Font->Surface->h;
  Font->Chars[' '].w = Font->Chars['!'].w;
  Font->number_of_chars = i;

#ifdef HAVE_LIBGL
if ( use_open_gl ) 
  Font->list_base = glGenLists(i);
#endif
  if (SDL_MUSTLOCK (Font->Surface))
    SDL_UnlockSurface (Font->Surface);

  SDL_SetColorKey ( Font->Surface, 0 , 0 );
  SDL_SetAlpha( Font -> Surface , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
}; // void InitFont (BFont_Info * Font)

/**
 * Load the font and stores it in the BFont_Info structure 
 */
BFont_Info *
LoadFont (const char *filename)
{
  int x;
  BFont_Info *Font = NULL;
  SDL_Surface *tmp = NULL ;

  if (filename != NULL)
    {
      Font = (BFont_Info *) MyMalloc (sizeof (BFont_Info));
      if (Font != NULL)
	{
	  //--------------------
	  // Here we cannot use our image loading wrapper, cause that one
	  // would also flip the image horizontally, which would destroy the
	  // top character info!
	  // 
	  // Too bad!
	  //
	  // tmp = (SDL_Surface *) our_IMG_load_wrapper (filename);
	  //
	  tmp = (SDL_Surface *) IMG_Load (filename);

	  if (tmp != NULL)
	    {
	      Font->Surface = SDL_DisplayFormatAlpha ( tmp ) ;
	      SDL_FreeSurface ( tmp ) ;
	      for (x = 0; x < 256; x++)
		{
		  Font->Chars[x].x = 0;
		  Font->Chars[x].y = 0;
		  Font->Chars[x].h = 0;
		  Font->Chars[x].w = 0;
		}
	      /* Init the font */
	      InitFont (Font);
	      /* Set the font as the current font */
	      SetCurrentFont (Font);
	      
	    }
	  else
	    {
	      /* free memory allocated for the BFont_Info structure */
	      free (Font);
	      Font = NULL;
	    }
	}
    }

  return Font;
}

/** 
 *
 *
 */
void
FreeFont (BFont_Info * Font)
{
  SDL_FreeSurface (Font->Surface);
  free (Font);
}; // void FreeFont (BFont_Info * Font)

/**
 * Set the current font 
 */
void
SetCurrentFont (BFont_Info * Font)
{
  CurrentFont = Font;
}; // void SetCurrentFont (BFont_Info * Font)

/**
 * Returns the pointer to the current font strucure in use 
 */
BFont_Info *
GetCurrentFont (void)
{
  return CurrentFont;
}; // BFont_Info * GetCurrentFont (void)

/**
 * Return the font height 
 */
int
FontHeight (BFont_Info * Font)
{
  return (Font->h);
}; // int FontHeight (BFont_Info * Font)


/* Return the width of the "c" character */
int
CharWidth (BFont_Info * Font, unsigned char c)
{

  if ( c < ' ' || c > Font->number_of_chars-1 ) c = '.';
  return Font->Chars[c].w;
}

/**
 * Puts a single char on the surface with the specified font 
 */
int
PutCharFont (SDL_Surface * Surface, BFont_Info * Font, int x, int y, unsigned char c)
{
  SDL_Rect dest;
  SDL_Rect clipping_rect;
  dest.w = CharWidth (Font, ' ');
  dest.h = FontHeight (Font) ;
  dest.x = x;
  dest.y = y;

  if ( c<' ' || c>Font->number_of_chars-1 ) c = '.';
  if ( ( c != ' ' ) && ( c != '\n' ) )
    {
      if ( use_open_gl )
	{
	//--------------------
	// We need to simulate the clipping is would be done by SDL.
	// We simply do "all or none" clipping for left and right, but for the top/bottom of the screen,
	// we use GL to do proper clipping in order not to see a whole line disappear at once.

	// glClipPlane simply does a little math.

        SDL_GetClipRect ( Surface , & clipping_rect );

	#ifdef HAVE_LIBGL
	GLdouble eqntop[4] = { 0.0, 1.0, 0.0, - clipping_rect . y };
	GLdouble eqnbottom[4] = { 0.0, -1.0, 0.0, clipping_rect . y + clipping_rect . h};
        glClipPlane(GL_CLIP_PLANE0, eqntop);
        glEnable(GL_CLIP_PLANE0);
        glClipPlane(GL_CLIP_PLANE1, eqnbottom);
        glEnable(GL_CLIP_PLANE1);
	#endif
	if ( ( dest . x < clipping_rect . x + clipping_rect . w ) &&
		( dest . x >= clipping_rect . x ) )
	    {
	    if ( ! Font -> char_iso_image [ c ] . texture_has_been_created )
		{
		// If the character is not ready to be printed on screen
		if ( Font -> char_iso_image [ c ] . surface == NULL )
		    {
		    ErrorMessage ( __FUNCTION__  , "Surface for character %d was NULL pointer!",
			    PLEASE_INFORM, IS_FATAL, c );
		    }
		make_texture_out_of_surface ( & ( Font -> char_iso_image [ c ] ) ) ;
#ifdef HAVE_LIBGL
		glNewList(Font->list_base + c, GL_COMPILE);

		glBindTexture(GL_TEXTURE_2D, (Font->char_iso_image[c] . texture));
		glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
		glEnable ( GL_BLEND );

		glBegin(GL_QUADS);

		glTexCoord2f( 0.0, 1.0 );
		glVertex2i( 0 , 0 );
		glTexCoord2f( 0.0, 0.0 );
		glVertex2i( 0 , Font->char_iso_image[c] . texture_height );
		glTexCoord2f( 1.0, 0.0);
		glVertex2i( Font->char_iso_image[c] . texture_width , Font->char_iso_image[c] . texture_height );
		glTexCoord2f( 1.0, 1.0 );
		glVertex2i(  Font->char_iso_image[c] . texture_width , 0 );

		glEnd( );
		glEndList();
		glDisable ( GL_BLEND );

#endif
		}

#ifdef HAVE_LIBGL
	    glPushMatrix();
	    glMatrixMode(GL_MODELVIEW);
	    glTranslated(dest.x, dest.y, 0);
	    glCallList(Font->list_base + c );
	    glPopMatrix();
	    glDisable(GL_CLIP_PLANE0);
	    glDisable(GL_CLIP_PLANE1);
#endif

	    }
	}
      else
	{
	  our_SDL_blit_surface_wrapper ( Font -> Surface, & Font -> Chars [ c ] , Surface, &dest);
	}
    }

  return CharWidth ( Font , c ) ;

}; // int PutCharFont (SDL_Surface * Surface, BFont_Info * Font, int x, int y, int c)

/**
 *
 *
 */
void
PutString (SDL_Surface * Surface, int x, int y, char *text)
{
  PutStringFont (Surface, CurrentFont, x, y, text);
}; // void PutString (SDL_Surface * Surface, int x, int y, char *text)

/**
 *
 *
 */
void
PutStringFont (SDL_Surface * Surface, BFont_Info * Font, int x, int y, char *text)
{
  int i = 0;
  //--------------------
  // I added little hack to kern MenuFont..
  // This basicly just prints them more tight on the screen.
  // basse, 15.2.03
  //
  int kerning = 0;
  if (Font==Menu_BFont) kerning = -4;

  if (Font==FPS_Display_BFont || Font==Blue_BFont || Font==Red_BFont) kerning = -2;

  while (text[i] != '\0')
    {
      //--------------------
      // Here I've added some hack to allow to give a font switching
      // directive with a text through various menus and therefore
      // switch the font even multiple times in one big text.
      //                                          jp, 27.7.2002
      //
      switch ( text [ i ] )
	{
	case 1: Font = Red_BFont; kerning = -2; break;
	case 2: Font = Blue_BFont; kerning = -2; break;
	case 3: Font = FPS_Display_BFont; kerning = -2; break;
	default:
	      x += PutCharFont (Surface, Font, x, y, text[i]) + kerning;
	}
      i++;
    }
}; // void PutStringFont (SDL_Surface * Surface, BFont_Info * Font, int x, int y, char *text)

/**
 *
 *
 */
int
TextWidth (char *text)
{
  return ( TextWidthFont (CurrentFont, text) ) ;
}; // int TextWidth (char *text)

/**
 *
 *
 */
int
TextWidthFont (BFont_Info * Font, char *text)
{
  int i = 0, x = 0;
  //--------------------
  // Based on Bastians hack: 
  // 
  // 'I added little hack to kern MenuFont..
  // This basicly just prints them more tight on the screen.
  // basse, 15.2.03'
  //
  // I extend this to give new text width results...
  //
  int kerning = 0;
  if ( Font == Menu_BFont ) kerning = -4;
  if ( Font==FPS_Display_BFont || Font==Blue_BFont || Font==Red_BFont) kerning = -2;

  while (text[i] != '\0')
    {
      switch ( text [ i ] )
	{
	case 1: Font = Red_BFont; kerning = -2; break;
	case 2: Font = Blue_BFont; kerning = -2; break;
	case 3: Font = FPS_Display_BFont; kerning = -2; break;
	}
      x += CharWidth (Font, text[i]) + kerning ;
      i++;
    }
  return x;
}; // int TextWidthFont (BFont_Info * Font, char *text)

/* counts the spaces of the strings */
int
count (char *text)
{
  char *p = NULL;
  int pos = -1;
  int i = 0;
  /* Calculate the space occupied by the text without spaces */
  while ((p = strchr (&text[pos + 1], ' ')) != NULL)
    {
      i++;
      pos = p - text;
    }
  return i;
}

void
JustifiedPutString (SDL_Surface * Surface, int y, char *text)
{
  JustifiedPutStringFont (Surface, CurrentFont, y, text);
}

void
JustifiedPutStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
			char *text)
{
  int spaces = 0;
  int gap;
  int single_gap;
  int dif;

  char *strtmp;
  char *p;
  int pos = -1;
  int xpos = 0;


  if (strchr (text, ' ') == NULL)
    {
      PutStringFont (Surface, Font, 0, y, text);
    }
  else
    {
      gap = (Surface->w - 1) - TextWidthFont (Font, text);

      if (gap <= 0)
	{
	  PutStringFont (Surface, Font, 0, y, text);
	}
      else
	{
	  spaces = count (text);
	  dif = gap % spaces;
	  single_gap = (gap - dif) / spaces;
	  xpos = 0;
	  pos = -1;
	  while (spaces > 0)
	    {
	      p = strstr (&text[pos + 1], " ");
	      strtmp = NULL;
	      strtmp =
		(char *) calloc ((p - &text[pos + 1]) + 1, sizeof (char));
	      if (strtmp != NULL)
		{
		  strncpy (strtmp, &text[pos + 1], (p - &text[pos + 1]));
		  PutStringFont (Surface, Font, xpos, y, strtmp);
		  xpos =
		    xpos + TextWidthFont (Font,
					  strtmp) + single_gap +
		    CharWidth (Font, ' ');
		  if (dif >= 0)
		    {
		      xpos++;
		      dif--;
		    }
		  pos = p - text;
		  spaces--;
		  free (strtmp);
		}
	    }
	  strtmp = NULL;
	  strtmp =
	    (char *) calloc (strlen (&text[pos + 1]) + 1, sizeof (char));

	  if (strtmp != NULL)
	    {
	      strncpy (strtmp, &text[pos + 1], strlen (&text[pos + 1]));
	      PutStringFont (Surface, Font, xpos, y, strtmp);
	      free (strtmp);
	    }
	}
    }
}

void
CenteredPutString (SDL_Surface * Surface, int y, char *text)
{
  CenteredPutStringFont (Surface, CurrentFont, y, text);
}

void
CenteredPutStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
		       char *text)
{
  PutStringFont (Surface, Font,
		 Surface->w / 2 - TextWidthFont (Font, text) / 2, y, text);
}

void
RightPutString (SDL_Surface * Surface, int y, char *text)
{
  RightPutStringFont (Surface, CurrentFont, y, text);
}

void
RightPutStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
		    char *text)
{
  PutStringFont (Surface, Font, Surface->w - TextWidthFont (Font, text) - 1,
		 y, text);
}

void
LeftPutString (SDL_Surface * Surface, int y, char *text)
{
  LeftPutStringFont (Surface, CurrentFont, y, text);
}

void
LeftPutStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
		   char *text)
{
  PutStringFont (Surface, Font, 0, y, text);
}

/******/

void
PrintString (SDL_Surface * Surface, int x, int y, const char *fmt, ...)
{
  va_list args;
  char *temp;
  va_start (args, fmt);

  if ((temp = (char *) malloc (1000 + 1)) != NULL)
    {
      vsprintf (temp, fmt, args);

      PutStringFont (Surface, CurrentFont, x, y, temp);

      free (temp);
    }
  va_end (args);
}

void
PrintStringFont (SDL_Surface * Surface, BFont_Info * Font, int x, int y,
		 const char *fmt, ...)
{
  va_list args;
  char *temp;
  va_start (args, fmt);

  if ((temp = (char *) malloc (1000 + 1)) != NULL)
    {
      vsprintf (temp, fmt, args);
      PutStringFont (Surface, Font, x, y, temp);
      free (temp);
    }
  va_end (args);
}

void
CenteredPrintString (SDL_Surface * Surface, int y, const char *fmt, ...)
{
  va_list args;
  char *temp;
  va_start (args, fmt);

  if ((temp = (char *) malloc (1000 + 1)) != NULL)
    {
      vsprintf (temp, fmt, args);
      CenteredPutString (Surface, y, temp);
      free (temp);
    }
  va_end (args);
}

void
CenteredPrintStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
			 const char *fmt, ...)
{
  va_list args;
  char *temp;
  va_start (args, fmt);

  if ((temp = (char *) malloc (1000 + 1)) != NULL)
    {
      vsprintf (temp, fmt, args);
      CenteredPutStringFont (Surface, Font, y, temp);
      free (temp);
    }
  va_end (args);

}

void
RightPrintString (SDL_Surface * Surface, int y, const char *fmt, ...)
{
  va_list args;
  char *temp;
  va_start (args, fmt);

  if ((temp = (char *) malloc (1000 + 1)) != NULL)
    {
      vsprintf (temp, fmt, args);
      RightPutString (Surface, y, temp);
      free (temp);
    }
  va_end (args);
}

void
RightPrintStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
		      const char *fmt, ...)
{
  va_list args;
  char *temp;
  va_start (args, fmt);

  if ((temp = (char *) malloc (1000 + 1)) != NULL)
    {
      vsprintf (temp, fmt, args);
      RightPutStringFont (Surface, Font, y, temp);
      free (temp);
    }
  va_end (args);
}

void
LeftPrintString (SDL_Surface * Surface, int y, const char *fmt, ...)
{
  va_list args;
  char *temp;
  va_start (args, fmt);

  if ((temp = (char *) malloc (1000 + 1)) != NULL)
    {
      vsprintf (temp, fmt, args);
      LeftPutString (Surface, y, temp);
      free (temp);
    }
  va_end (args);
}

void
LeftPrintStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
		     const char *fmt, ...)
{
  va_list args;
  char *temp;
  va_start (args, fmt);

  if ((temp = (char *) malloc (1000 + 1)) != NULL)
    {
      vsprintf (temp, fmt, args);
      LeftPutStringFont (Surface, Font, y, temp);
      free (temp);
    }
  va_end (args);
}

void
JustifiedPrintString (SDL_Surface * Surface, int y, const char *fmt, ...)
{
  va_list args;
  char *temp;
  va_start (args, fmt);

  if ((temp = (char *) malloc (1000 + 1)) != NULL)
    {
      vsprintf (temp, fmt, args);
      JustifiedPutString (Surface, y, temp);
      free (temp);
    }
  va_end (args);
}

void
JustifiedPrintStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
			  const char *fmt, ...)
{
  va_list args;
  char *temp;
  va_start (args, fmt);

  if ((temp = (char *) malloc (1000 + 1)) != NULL)
    {
      vsprintf (temp, fmt, args);
      JustifiedPutStringFont (Surface, Font, y, temp);
      free (temp);
    }
  va_end (args);
}


/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/

inline void
PutPixel32 (SDL_Surface * surface, int x, int y, Uint32 pixel)
{  
  Uint8 *p;
  p = (Uint8 *) surface->pixels + y * surface->pitch + x * 4;
 
  *(Uint32 *) p = pixel;
 
} // void PutPixel ( ... )


inline void
PutPixel24 (SDL_Surface * surface, int x, int y, Uint32 pixel)
{  
  Uint8 *p;
  p = (Uint8 *) surface->pixels + y * surface->pitch + x * 3;
 
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	{
	  p[0] = (pixel >> 16) & 0xff;
	  p[1] = (pixel >> 8) & 0xff;
	  p[2] = pixel & 0xff;
	}
      else
	{
	  p[0] = pixel & 0xff;
	  p[1] = (pixel >> 8) & 0xff;
	  p[2] = (pixel >> 16) & 0xff;
	}
 
} // void PutPixel ( ... )

inline void
PutPixel16 (SDL_Surface * surface, int x, int y, Uint32 pixel)
{  
  Uint8 *p;
  p = (Uint8 *) surface->pixels + y * surface->pitch + x * 2;
 
  *(Uint16 *) p = pixel;
 
} // void PutPixel ( ... )

inline void
PutPixel8 (SDL_Surface * surface, int x, int y, Uint32 pixel)
{  
  Uint8 *p;
  p = (Uint8 *) surface->pixels + y * surface->pitch + x;
 
  *p = pixel;
 
} // void PutPixel ( ... )

void
PutPixel (SDL_Surface * surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  Uint8 *p;

  if ( use_open_gl )
    {
      if ( surface == Screen ) 
	{
	  PutPixel_open_gl ( x , y , pixel ) ;
	  return;
	}
    }


  //--------------------
  // Here I add a security query against segfaults due to writing
  // perhaps even far outside of the surface pixmap data.
  //
  if ( ( x < 0 ) || ( y < 0 ) || ( x >= surface->w ) || ( y >= surface->h ) ) return;

  /* Here p is the address to the pixel we want to set */
  p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp)
    {
    case 1:
      *p = pixel;
      break;

    case 2:
      *(Uint16 *) p = pixel;
      break;

    case 3:
      // pixel = pixel & 0x0ffffff ;
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	{
	  p[0] = (pixel >> 16) & 0xff;
	  p[1] = (pixel >> 8) & 0xff;
	  p[2] = pixel & 0xff;
	}
      else
	{
	  p[0] = pixel & 0xff;
	  p[1] = (pixel >> 8) & 0xff;
	  p[2] = (pixel >> 16) & 0xff;
	}
      break;

    case 4:
      *(Uint32 *) p = pixel;
      break;
    }

}; // void PutPixel ( ... )

/**
 * NOTE:  I THINK THE SURFACE MUST BE LOCKED FOR THIS!
 *
 */
inline Uint32
FdGetPixel32 (SDL_Surface * Surface, Sint32 X, Sint32 Y)
{
  return *((Uint32 *) Surface->pixels + Y * Surface->pitch / 4 + X);
}

inline Uint32
FdGetPixel24 (SDL_Surface * Surface, Sint32 X, Sint32 Y)
{

  Uint8 *bits = ((Uint8 *) Surface->pixels) + Y * Surface->pitch + X * 3;

  return SDL_MapRGB (Surface->format, *((bits) + Surface->format->Rshift / 8), *((bits) + Surface->format->Gshift / 8), *((bits) + Surface->format->Bshift / 8));

}

inline Uint16
FdGetPixel16 (SDL_Surface * Surface, Sint32 X, Sint32 Y)
{
  return *((Uint16 *) Surface->pixels + Y * Surface->pitch / 2 + X);
}

inline Uint8
FdGetPixel8 (SDL_Surface * Surface, Sint32 X, Sint32 Y)
{
  return *((Uint8 *) Surface->pixels + Y * Surface->pitch + X);
}

Uint32
FdGetPixel (SDL_Surface * Surface, Sint32 X, Sint32 Y)
{

  Uint8 *bits;
  Uint32 Bpp;

  Bpp = Surface->format->BytesPerPixel;

  bits = ((Uint8 *) Surface->pixels) + Y * Surface->pitch + X * Bpp;

  // Get the pixel
  switch (Bpp)
    {
    case 1:
      return *((Uint8 *) Surface->pixels + Y * Surface->pitch + X);
      break;
    case 2:
      return *((Uint16 *) Surface->pixels + Y * Surface->pitch / 2 + X);
      break;
    case 3:
      {				// Format/endian independent
	Uint8 r, g, b;
	r = *((bits) + Surface->format->Rshift / 8);
	g = *((bits) + Surface->format->Gshift / 8);
	b = *((bits) + Surface->format->Bshift / 8);
	return SDL_MapRGB (Surface->format, r, g, b);
      }
      break;
    case 4:
      return *((Uint32 *) Surface->pixels + Y * Surface->pitch / 4 + X);
      break;
    }

  return -1;
}

#undef _bfont_c
