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
 * This file contains restricted version of some 'dangerous' Lua functions,
 * to implement a kind of a 'Lua sandbox'
 */

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "lua/lua_core.h"

#ifdef __WIN32__
#define realpath(N,R) _fullpath((R),(N), _MAX_PATH)
#endif

//
// Helper for limited_dofile(), called when the Lua code yields
//
static int dofilecont(lua_State *L, int d1, lua_KContext d2)
{
	// code from lua/lbaselib.c
	(void)d1;  (void)d2;
	return lua_gettop(L) - 1;
}

//
// 'Jailed' version of luaB_dofile(): the requested file must be in the data dir
//
int lua_restricted_dofile(lua_State *L)
{
	const char *fname = luaL_optstring(L, 1, NULL);
	lua_settop(L, 1);

	// Check if the requested file can be loaded
	// Return 'nil' is not

	char *abs_fname = realpath(fname,  NULL);
	if (!abs_fname) {
		// Was not able to resolve the filename
		lua_pushnil(L);
		return 1;
	} else {
		int can_load = TRUE;
		char *data_root = realpath(data_dirs[DATA_ROOT].path, NULL);
		struct auto_string *data_dir = alloc_autostr(PATH_MAX);
		autostr_append(data_dir, "%s/data", data_root);
		if (strncmp(abs_fname, data_dir->value, data_dir->length) != 0) {
			// The requested file is not in the data dir
			can_load = FALSE;
		}
		free_autostr(data_dir);
		free(data_root);
		free(abs_fname);
		if (!can_load) {
			lua_pushnil(L);
			return 1;
		}
	}

	// Load the script
	// (code from lua/lbaselib.c)

	if (luaL_loadfile(L, fname) != LUA_OK)
		return lua_error(L);
	lua_callk(L, 0, LUA_MULTRET, 0, dofilecont);
	return dofilecont(L, 0, 0);
}

// Restricted version of setmetatable() which only set an __index table
// as a metamethod
// Used to implement a kind of inheritance from a base table (set of attributes)
// Input: -1: a table to set as the __index (we call it a 'base class')
//        -2: a table to which the __index is added (we call it a 'derived class')

int lua_restricted_setmetatable(lua_State *L)
{
	// Only a table can be set as the __index
	luaL_checktype(L, -1, LUA_TTABLE);

	lua_newtable(L);                // -1: new, -2: base, -3: derived
	lua_rotate(L, -2, 1);           // -1: base, -2: new, -3: derived
	lua_setfield(L, -2, "__index"); // new["__index"] = base -> -1: new, -2: derived
	lua_setmetatable(L, -2);        // -1: derived
	return 1;
}

// Replace luaL_openlibs() with a more restrictive version
// Note: the module list is extracted from linit.c, version 5.3
//       to be adapted, potentially, to each Lua version

struct restricted_spec {
	luaL_Reg lib;
	const char **whitelist;
};

int blacklisted_func(lua_State *L)
{
	lua_Debug ar0, ar1;
	lua_getstack(L, 0, &ar0);
	lua_getinfo(L, "nS", &ar0);
	lua_getstack(L, 1, &ar1);
	lua_getinfo(L, "Sl", &ar1);

	if (ar1.source[0] == '=') {
		DebugPrintf(-1, "Attempt to call the blacklisted function '%s' from an unknown source\n", ar0.name);
	} else {
		struct auto_string *error_msg = alloc_autostr(64);
		if (ar1.source[0] == '@') {
			autostr_append(error_msg, "%s:%d: attempt to call the blacklisted function '%s'", &ar1.source[1], ar1.currentline, ar0.name);
		} else {
			autostr_append(error_msg, "at line:%d: attempt to call the blacklisted function '%s'", ar1.currentline, ar0.name);
		}
		struct auto_string *erroneous_code = extract_lua_error_from_stack(L, error_msg->value);
		if (erroneous_code) {
			fflush(stdout);
			error_message(__FUNCTION__, "Error running Lua code: %s.\nErroneous LuaCode={\n%s}",
					PLEASE_INFORM, error_msg->value, erroneous_code->value);
			free_autostr(erroneous_code);
		}
		free_autostr(error_msg);
	}

	return 0;
}

void apply_white_list(lua_State *L, const char **whitelist)
{
	// Iterate over each field of the table defining the module,
	// and then remove un-allowed functions
	lua_pushnil(L);
	while (lua_next(L, 1) != 0) { /* 'key' is at index -2, and 'value' is at index -1 */

		// If the current key is not a string, it does not reference a function
		// Restriction is only about functions, so if the value is not a function
		// we keep it.
		if (lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TFUNCTION) {

			const char *key = lua_tolstring(L, -2, NULL);
			// Check if the function is allowed, remove it if not
			int keep = FALSE;
			for (const char** whitekey = whitelist; *whitekey != NULL; whitekey++) {
				if (!strcmp(key, *whitekey)) {
					keep= TRUE;
					break;
				}
			}
			if (!keep) {
				// Replace by a function that reports the error
				lua_pushcclosure(L, blacklisted_func, 0);
				lua_setfield(L, -4, key);
			}

		}

		/* removes 'value'; keeps 'key' for next iteration of lua_next() */
		lua_pop(L, 1);
	}
}

void lua_restricted_openlibs(lua_State *L)
{
	// For each module, a white list of allowed functions will be
	// defined.
	// Ideally, a restricted Lua env containing only the allowed entries
	// should be set, and used to replace the _ENV upvalue before to
	// execute a Lua chunk with luaL_pcall().
	// We here use an easier, but conceptually identical, method :
	// all modules are fully loaded, and then the functions not listed
	// in the withelists are removed.

	static const char *full_list[] = { "*", NULL };
	static const char *none_list[] = { NULL };

	static const char *io_whitelist[] = {
		"lines", "output", "write",
		// "close", "flush", "input", "open", "output", "popen", "read", "tmpfile", "type"
		NULL
	};
	static const char *os_whitelist[] = {
		"clock", "date", "difftime", "time",
		// "execute", "exit", "getenv", "remove", "rename", "setlocale", "tmpname"
		NULL
	};
	static const char *debug_whitelist[] = {
		"getinfo",
		// "debug", "getuservalue", "gethook", "getlocal", "getregistry",
		// "getmetatable", "getupvalue", "upvaluejoin", "upvalueid", "setuservalue",
		// "sethook", "setlocal", "setmetatable", "setupvalue", "traceback",
		NULL
	};
	static const struct restricted_spec restricted_libs[] = {
			{ {"_G",            luaopen_base},      full_list },
			{ {LUA_LOADLIBNAME, luaopen_package},   none_list },
			{ {LUA_COLIBNAME,   luaopen_coroutine}, full_list },
			{ {LUA_TABLIBNAME,  luaopen_table},     full_list },
			{ {LUA_IOLIBNAME,   luaopen_io}, 		io_whitelist },
			{ {LUA_OSLIBNAME,   luaopen_os},        os_whitelist },
			{ {LUA_STRLIBNAME,  luaopen_string},    full_list },
			{ {LUA_MATHLIBNAME, luaopen_math},      full_list },
			// { {LUA_UTF8LIBNAME, luaopen_utf8},   full_list }, // Unused
			{ {LUA_DBLIBNAME,   luaopen_debug},     debug_whitelist },
#if defined(LUA_COMPAT_BITLIB)
			// { {LUA_BITLIBNAME, luaopen_bit32},   full_list }, // Unused
#endif
			{ {NULL, NULL}, NULL }
	};

	// Load each module and only keep whitelisted content

	for (const struct restricted_spec *lib_spec = restricted_libs; lib_spec->lib.func; lib_spec++) {
		// Load and register a module content
		luaL_requiref(L, lib_spec->lib.name, lib_spec->lib.func, 1);
		// If all functions are allowed, nothing to do
		if (lib_spec->whitelist == full_list) {
			// the 'module' is no more needed on the stack (it is now registered)
			lua_pop(L, 1);
			continue;
		}
		apply_white_list(L, lib_spec->whitelist);
		// the 'module' is no more needed on the stack (it is now registered)
		lua_pop(L, 1);
	}

	// Now apply white list to the global table
	// Note: some libs add global functions, so we need to apply the
	// global white list after all libs are loaded.

	static const char *global_whitelist[] = {
		"assert", "error", "ipairs", "next", "pairs", "pcall", "print",
		"rawequal", "rawlen", "rawget",	"select",
		"tonumber", "tostring", "xpcall", "type",
		NULL
	};

	lua_pushglobaltable(L);
	apply_white_list(L, global_whitelist);
	lua_pop(L, 1);

	// Add restricted implementation of some Lua funtions

	static const luaL_Reg lua_restricted_funcs[] = {
			{ "FDdofile", lua_restricted_dofile },
			{ "FDinherit", lua_restricted_setmetatable },
			{ NULL, NULL }
	};

	for (int i = 0; lua_restricted_funcs[i].name != NULL; i++) {
		lua_pushcfunction(L, lua_restricted_funcs[i].func);
		lua_setglobal(L, lua_restricted_funcs[i].name);
	}

#if 0
	// Display the content of a table
	// Used to check if only white-listed funcs are registered

	// Use one of the next two lines to push a table on the stack :
	//    lua_pushglobaltable(L); // Use this one to push the global table
	// or
	//    lua_getglobal(L, "<module_name>"; // Change '<module_name>' to whatever you want
	lua_getglobal(L, "package");

	printf("-------------------------------------\n");
    /* table is in the stack at index 't' */
    lua_pushnil(L);  /* first key */
    while (lua_next(L, 1) != 0) {
      /* uses 'key' (at index -2) and 'value' (at index -1) */
      printf("%15s - %s\n",
    		  lua_tolstring(L, -2, NULL),
			  lua_typename(L, lua_type(L, -1)));
      /* removes 'value'; keeps 'key' for next iteration */
      lua_pop(L, 1);
    }
	lua_pop(L, 1);
	printf("-------------------------------------\n");
#endif
}
