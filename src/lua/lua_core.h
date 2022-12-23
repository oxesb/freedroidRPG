/*
 *
 *   Copyright (c) 2022 Samuel Degrande
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
 * This file contains the declarations of core functions to interface and use
 * lua in freedroidRPG
 *
 * It declares the "internal" lua core functions used by lua/config_loader.c and
 * lua/game_scripting.c
 * The "external" api is declared in proto.h
 */

#ifndef _lua_core_h_
#define _lua_core_h_

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

lua_State *create_lua_state(enum lua_target);
void delete_lua_state(enum lua_target);

void load_lua_module(enum lua_target, int, const char*);
int call_lua_func(enum lua_target, const char*, const char*, const char*, const char*, ...);
struct lua_coroutine *prepare_lua_coroutine(enum lua_target, const char*, const char*, const char*, ...);
struct lua_coroutine *load_lua_coroutine(enum lua_target, const char*);
int resume_lua_coroutine(struct lua_coroutine*);

struct auto_string *extract_lua_error_from_stack(lua_State*, const char *);
void dump_lua_stack(lua_State*);

#endif /* _lua_core_h_ */
