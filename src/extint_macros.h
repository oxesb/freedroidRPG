/*
 *
 *   Copyright (c) 2016 Samuel Degrande
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

#if !defined(INTERN_FOR)
#	undef EXTERN
#	undef INITIALIZER
#elif INTERN_FOR == 1
#	undef  EXTERN
#	define EXTERN
#	undef  INITIALIZER
#	define INITIALIZER(v) = v
#else
#	undef  EXTERN
#	define EXTERN extern
#	undef  INITIALIZER
#	define INITIALIZER(v)
#endif

#undef INTERN_FOR
