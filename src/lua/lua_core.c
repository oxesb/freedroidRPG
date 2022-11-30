/*
 *
 *   Copyright (c) 2008-2010 Arthur Huillet
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
 * This file contains core functions to interface and use lua in freedroidRPG
 */

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#include "lua_core.h"

// forward def
static void pretty_print_lua_error(lua_State*, const char*, const char*, int, const char*);

// extern defs
extern void init_lua_config_loader(void);
extern void init_lua_game_scripting(void);

void init_lua(void)
{
	init_lua_config_loader();
	init_lua_game_scripting();
}

void close_lua(void)
{
	for (int target = 0; target < LUA_LAST; target++) {
		delete_lua_state(target);
	}
}

// Reload the Lua VM used for game scripting, when a new game is loaded
void reset_lua_game_scripting()
{
	init_lua_game_scripting();
}

////////////////////////////////////////////////////////////
// Manage Lua states (i.e. Lua VMs)
////////////////////////////////////////////////////////////

/* Lua states, once for game config loading and once for dialog execution */
static lua_State *lua_states[LUA_LAST] = { NULL };

static int lua_src_gettext(lua_State *L)
{
	char *text = (char *)luaL_checkstring(L, 1);
	lua_pushstring(L, _(text));
	return 1;
}

static int lua_data_gettext(lua_State *L)
{
	char *text = (char *)luaL_checkstring(L, 1);
	lua_pushstring(L, D_(text));
	return 1;
}

static int lua_dialogs_gettext(lua_State *L)
{
	char *text = (char *)luaL_checkstring(L, 1);
	lua_pushstring(L, L_(text));
	return 1;
}

lua_State *create_lua_state(enum lua_target target)
{
	if (target < 0 || target >= LUA_LAST) {
		error_message(__FUNCTION__, "Tried to create an unknown lua_state #%d. Expect some errors", PLEASE_INFORM, target);
		return NULL;
	}

	delete_lua_state(target);
	lua_states[target] = luaL_newstate();
	luaL_openlibs(lua_states[target]);

	// Add context specific lua gettexts
	luaL_Reg lua_gettext[] = {
			{ "_",  lua_dialogs_gettext },
			{ "S_", lua_src_gettext     },
			{ "D_", lua_data_gettext    },
			{ NULL, NULL }
	};
	for (int i = 0; lua_gettext[i].name != NULL; i++) {
		lua_pushcfunction(lua_states[target], lua_gettext[i].func);
		lua_setglobal(lua_states[target], lua_gettext[i].name);
	}

	return lua_states[target];
}

void delete_lua_state(enum lua_target target)
{
	if (target < 0 || target >= LUA_LAST) {
		error_message(__FUNCTION__, "Tried to delete an unknown lua_state #%d.", PLEASE_INFORM, target);
		return;
	}

	if (lua_states[target])
		lua_close(lua_states[target]);
}

lua_State *get_lua_state(enum lua_target target)
{
	if (target >= 0 && target < LUA_LAST)
		return lua_states[target];

	// If we were requesting an unknown lua state, return the first one
	// as a fall-back, but it will probably lead to some errors.
	error_message(__FUNCTION__, "Tried to get an unknown lua_state #%d. Expect some errors...", PLEASE_INFORM, target);
	return lua_states[0];
}

/*
 * Load a lua module in a Lua state.
 * The directory containing the module is added to the package.path lua global
 * variable.
 * The module is loaded by calling 'require(module)'.
 */
void load_lua_module(enum lua_target target, int subdir, const char *module)
{
	char fpath[PATH_MAX];
	lua_State *L = get_lua_state(target);

	/*
	 * Add the module's dir to the Lua package.path
	 */

	if (find_file(fpath, subdir, module, ".lua", PLEASE_INFORM)) {

		// Use the dirname of the module and add the search pattern
		find_file(fpath, subdir, "?.lua", NULL, SILENT);

		// Get current Lua package.path
		lua_getglobal(L, "package");
		lua_getfield(L, 1, "path");
		const char *package_path = lua_tostring(L, -1);
		lua_pop(L, 2);

		// Add the search path, if needed
		if (!strstr(package_path, fpath)) {
			lua_getglobal(L, "package"); /* -> stack: package */
			lua_getfield(L, 1, "path");  /* -> stack: package.path < package */
			lua_pushliteral(L, ";");     /* -> stack: ";" < package.path < package */
			lua_pushstring(L, fpath);    /* -> stack: fpath < ";" < package.path < package */
			lua_concat(L, 3);            /* -> stack: package.path;fpath < package */
			lua_setfield(L, 1, "path");  /* package.path = package.path;fpath -> stack: package */
			lua_pop(L, 1);
		}
	}

	/*
	 * Call "require(module)" to load the module
	 */

	call_lua_func(target, NULL, "require", "s", NULL, module);
}

////////////////////////////////////////////////////////////
// Call of a Lua func from a C code
////////////////////////////////////////////////////////////

/*
 * Prepare a lua function call by pushing on the Lua stack the name of
 * the function to call and the arguments of the call (see comment of call_lua_func()).
 */
static int push_func_and_args(lua_State *L, const char *module, const char *func, const char *sig, va_list *vl)
{
	 /* push function */
	if (module) {
		lua_getglobal(L, module);
		lua_getfield(L, -1, func);
		lua_remove(L, -2);
	} else {
		lua_getglobal(L, func);
	}

	while (sig && *sig) { /* repeat for each argument */
		switch (*sig++) {
		case 'f': /* double argument */
			lua_pushnumber(L, va_arg(*vl, double));
			break;
		case 'd': /* int argument */
			lua_pushinteger(L, (lua_Integer)va_arg(*vl, int));
			break;
		case 's': /* string argument */
			lua_pushstring(L, va_arg(*vl, char *));
			break;
		case 'S': /* dynarray of strings : push a table containing the strings */
			{
				int i;
				struct dynarray *array = va_arg(*vl, struct dynarray *);
				lua_createtable(L, array->size, 0);
				for (i=0; i<array->size; i++)
				{
					char *text = ((char **)(array->arr))[i];
					lua_pushstring(L, text);
					lua_rawseti(L, -2, i+1); // table[i+1] = text
				}
			}
			break;
		default:
			DebugPrintf(-1, "call_lua_func: invalid input option (%c).", *(sig - 1));
			return 0;
		}
	}

	return 1;
}

/*
 * Retrieve the returned values of a lua function call.
 * The results are stored in the locations pointed to by the pointer arguments
 * that follow the signature string (see comment of call_lua_func()).
 */
static int pop_results(lua_State *L, const char *sig, va_list *vl)
{
	if (!sig)
		return 1;

	int nres = strlen(sig);
	int index;
	int rc = 0;

	for (index = -nres; *sig; index++) { /* repeat for each result */
		int ltype = lua_type(L, index);
		switch (*sig++) {
		case 'f': /* double result */
			if (ltype != LUA_TNUMBER) {
				DebugPrintf(-1, "call_lua_func: wrong result type for #%d returned value (number expected)", index);
				goto pop_and_return;
			}
			*va_arg(*vl, double *) = lua_tonumber(L, index);
			break;
		case 'd': /* int result */
			if (ltype != LUA_TNUMBER) {
				DebugPrintf(-1, "call_lua_func: wrong result type for #%d returned value (number expected)", index);
				goto pop_and_return;
			}
			*va_arg(*vl, int *) = lua_to_int(lua_tointeger(L, index));
			break;
		case 's': /* string result */
			if (ltype != LUA_TSTRING) {
				DebugPrintf(-1, "call_lua_func: wrong result type for #%d returned value (string expected)", index);
				goto pop_and_return;
			}
			*va_arg(*vl, const char **) = strdup(lua_tostring(L, index));
			break;
		case 'S': /* dynarray of strings result */
			{
				int i;
				if (ltype != LUA_TTABLE) {
					DebugPrintf(-1, "call_lua_func: wrong result type for #%d returned value (table expected)", index);
					goto pop_and_return;
				}
				struct dynarray *array = va_arg(*vl, struct dynarray *);
				dynarray_init(array, lua_rawlen(L, index), sizeof(char *)); // the dynarray was reset by the caller
				for (i=1; i<=lua_rawlen(L, index); i++) {
					lua_rawgeti(L, index, i);
					if (!lua_isstring(L, -1)) {
						DebugPrintf(-1, "call_lua_func: wrong result type for #%d:%d returned value (string expected).\nSkeeping that value", index, i);
					} else {
						char *nodename = strdup(lua_tostring(L, -1));
						dynarray_add(array, &nodename, sizeof(string));
					}
					lua_pop(L, 1);
				}
			}
			break;
		default:
			DebugPrintf(-1, "call_lua_func: invalid output option (%c)", *(sig - 1));
			goto pop_and_return;
		}
	}

	rc = 1;

pop_and_return:
	lua_pop(L, nres);
	return rc;
}

/**
 * \brief Helper function to call a Lua function.
 *
 * \details Call a Lua function passing parameters and retrieving results through the
 * Lua stack.
 * (inspired from a code found in "Programming in Lua, 2ed").
 *
 * Usage:
 *
 * To execute a Lua function call such as "var1, var2 = my_module.my_func(param1)",
 * use "call_lua_func(lua_target, "my_module", "my_func", insig, outsig, param1, &var1, var2)"
 *
 * C being a strong typed language, the type of the input parameters and of the
 * returned values has to be defined, through 'insig' (for input parameters)
 * and 'outsig' (for returned values). A type is defined by a single specific
 * character, and 'insig' (resp. 'outsig') is a string containing a list of
 * those characters, one character per parameter (resp. returned value).
 *
 * Known types are:
 *   'f' for double type
 *   'd' for int type
 *   's' for string type (i.e. char*)
 *   'S' for dynarray of strings
 *
 * Following 'insig' and 'outsig' is a list of data to be used as parameters for
 * the Lua function (one data per character in 'insig'), followed by a list
 * of pointers to store the returned values (one pointer per character in 'outsig').
 * Note 1: memory of string returned values is allocated by call_lua_func().
 * Note 2: memory of string dynarray slots is allocated by call_lua_func(),
 *         and the dynarray has to be freed before the call.
 *
 * If the Lua function is not inside a module, set 'module' to NULL.
 * If the Lua function has no parameters, set 'insig' to NULL.
 * If the Lua does not return values, set 'outsig' to NULL.
 *
 * Exemples (of doubtful utility...):
 * double sine;
 * call_lua_func(LUA_DIALOG, "math", "sin", "f", "f", 3.14, &sine);
 * char *tmpname;
 * call_lua_func(LUA_DIALOG, "os", "tmpname", NULL, "s", tmpname);
 *
 * \param target A lua_target enum value defining the Lua context to use
 * \param module Name of the module containing the Lua function, or NULL if none
 * \param func   Name of the Lua function to call
 * \param insig  Signature string for the input parameters, or NULL
 * \param outsig Signature string for the returned values
 * \param ...    Input parameters data and pointers to returned value storages
 *
 * \return TRUE if the function call succeeded, else return FALSE
 */
int call_lua_func(enum lua_target target, const char *module, const char *func, const char *insig, const char *outsig, ...)
{
	int narg = (insig) ? strlen(insig) : 0;   /* number of arguments */
	int nres = (outsig) ? strlen(outsig) : 0; /* number of results */

	lua_State *L = get_lua_state(target);

	va_list vl;
	va_start(vl, outsig);

	/* do the call */
	if (!push_func_and_args(L, module, func, insig, &vl)) {
		error_message(__FUNCTION__, "Aborting lua function call.", NO_REPORT);
		va_end(vl);
		return FALSE;
	}

	if (lua_pcall(L, narg, nres, 0) != LUA_OK) {
		DebugPrintf(-1, "call_lua_func: Error calling ’%s’: %s", func, lua_tostring(L, -1));
		lua_pop(L, 1);
		error_message(__FUNCTION__, "Aborting lua function call.", NO_REPORT);
		va_end(vl);
		return FALSE;
	}

	if (!pop_results(L, outsig, &vl)) {
		error_message(__FUNCTION__, "Aborting lua function call.", NO_REPORT);
		va_end(vl);
		return FALSE;
	}

	va_end(vl);
	return TRUE;
}

////////////////////////////////////////////////////////////
// Execute a single Lua func or a whole Lua code in a co-routine
////////////////////////////////////////////////////////////

/**
 * \brief Prepare to call a lua function from a module in a coroutine
 *
 * \details Create a new lua thread, and fill the lua stack with the function to
 * call and its argument, in preparation to a call to resume_coroutine, which will
 * actually start the coroutine.
 * (See call_lua_func() for an explanation of the parameters, given that there are
 * no returned values for coroutine)
 *
 * \param target A lua_target enum value defining the Lua context to use
 * \param module Name of the module containing the Lua function, or NULL if none
 * \param func   Name of the Lua function to call
 * \param insig  Signature string for the input parameters, or NULL
 * \param ...    Input parameters data
 *
 * \return Pointer to a lua_coroutine struct holding the data needed to start (resume) the coroutine
 */
struct lua_coroutine *prepare_lua_coroutine(enum lua_target target, const char *module, const char *func, const char *insig, ...)
{
	lua_State *L = get_lua_state(target);
	lua_State *co_L = lua_newthread(L);

	struct lua_coroutine *new_coroutine = (struct lua_coroutine *)MyMalloc(sizeof(struct lua_coroutine));
	new_coroutine->thread = co_L;
	new_coroutine->nargs = (insig) ? strlen(insig) : 0;

	va_list vl;
	va_start(vl, insig);

	push_func_and_args(co_L, module, func, insig, &vl);

	va_end(vl);

	return new_coroutine;
}

/**
 * \brief Prepare to call a lua function, given in a source code, in a coroutine
 *
 * \details Create a new lua thread, and fill the lua stack with the function to
 * call and its argument, in preparation to a call to resume_coroutine, which will
 * actually start the coroutine.
 *
 * \param target A lua_target enum value defining the Lua context to use
 * \param code   The code of the function to execute
 *
 * \return Pointer to a lua_coroutine struct holding the data needed to start (resume) the coroutine
 */
struct lua_coroutine *load_lua_coroutine(enum lua_target target, const char *code)
{
	lua_State *L = get_lua_state(target);
	lua_State *co_L = lua_newthread(L);

	if (luaL_loadstring(co_L, code)) {
		pretty_print_lua_error(co_L, lua_tostring(co_L, -1), code, 2, __FUNCTION__);
		lua_pop(L, -1);
		return NULL;
	}
	struct lua_coroutine *new_coroutine = (struct lua_coroutine *)MyMalloc(sizeof(struct lua_coroutine));
	new_coroutine->thread = co_L;
	new_coroutine->nargs = 0;

	return new_coroutine;
}

int resume_lua_coroutine(struct lua_coroutine *coroutine)
{
	int rtn = lua_resume(coroutine->thread, NULL, coroutine->nargs);

	switch (rtn) {
		case 0:
			// The lua script has ended
			return TRUE;
		case LUA_YIELD:
			// The lua script is 'pausing'. Next resume will be without arguments
			coroutine->nargs = 0;
			return FALSE;
		default:
			// Any other return code is an error.
			break;
	}

	// On error:
	// Use the lua debug API to get information about the code of the current script
	char *error_msg = strdup(lua_tostring(coroutine->thread, -1));

	lua_Debug ar;
	lua_getstack(coroutine->thread, 0, &ar);
	lua_getinfo(coroutine->thread, "nS", &ar);

	if (ar.what[0] == 'C') {
		// Error caught in a C function.
		// Try to get the lua calling code from the call stack.
		if (!lua_getstack(coroutine->thread, 1, &ar)) {
			// Nothing in the call stack. Display an error msg and exit.
			error_message(__FUNCTION__, "Error in a lua API call in function '%s()': %s.",
					PLEASE_INFORM, ar.name, error_msg);
			goto EXIT;
		}
		// Get the info of the lua calling code, and continue to display it
		lua_getinfo(coroutine->thread, "nS", &ar);
	}

	if (ar.source[0] != '@') {
		// ar.source contains the script code
		pretty_print_lua_error(coroutine->thread, error_msg, ar.source, 2, __FUNCTION__);
	} else {
		// The script code is in an external file
		// Extract the erroneous function's code from the source file
		FILE *src = fopen(&ar.source[1], "r");

		if (!src) {
			error_message(__FUNCTION__,
					"Error detected in a lua script, but we were not able to open its source file (%s).\n"
					"This should not happen !\n"
					"Lua error: %s",
					PLEASE_INFORM, &ar.source[1], error_msg);
			goto EXIT;
		}

		struct auto_string *code = alloc_autostr(256);
		char buffer[256] = "";
		char *ptr = buffer;
		int lc = 1;
		for (;;) {
			if (*ptr == '\0') {
				if (feof(src)) break;
				size_t nbc = fread(buffer, 1, 255, src);
				buffer[nbc] = '\0';
				ptr = buffer;
			}
			if (lc > ar.lastlinedefined) break;
			if (lc >= ar.linedefined) {
				// The use of autostr_append to add a single character is
				// not efficient, but this code is used only in case of a
				// script error, so we do not really care of efficiency
				autostr_append(code, "%c", *ptr);
			}
			if (*ptr == '\n') lc++;
			ptr++;
		}

		pretty_print_lua_error(coroutine->thread, error_msg, code->value, ar.linedefined, __FUNCTION__);
		free_autostr(code);
		fclose(src);
	}

EXIT:
	free(error_msg);
	lua_pop(coroutine->thread, 1);

	return TRUE; // Pretend the lua script has ended
}

////////////////////////////////////////////////////////////
// High levels functions to run a Lua code
////////////////////////////////////////////////////////////

// Run the code from a string
int run_lua(enum lua_target target, const char *code)
{
	lua_State *L = get_lua_state(target);

	int rtn = luaL_dostring(L, code);
	if (rtn) {
		pretty_print_lua_error(L, lua_tostring(L, -1), code, 2, __FUNCTION__);
		lua_pop(L, -1);
	}

	return rtn;
}

// Run the code from a file
void run_lua_file(enum lua_target target, const char *path)
{
	lua_State *L = get_lua_state(target);

	if (luaL_dofile(L, path)) {
		error_message(__FUNCTION__, "Cannot run script file %s: %s.",
		         PLEASE_INFORM | IS_FATAL, path, lua_tostring(L, -1));
	}
}

////////////////////////////////////////////////////////////
// Misc functions
////////////////////////////////////////////////////////////

static void pretty_print_lua_error(lua_State* L, const char* error_msg, const char *code, int cur_line, const char *funcname)
{
	int err_line = 0;
	struct auto_string *erronous_code;

	erronous_code = alloc_autostr(16);

	// Find which line the error is on (if there is a line number in the error message)
	const char *error_ptr = error_msg;

	while (*error_ptr != 0 && *error_ptr != ':') {
		error_ptr++;
	}
	if (*error_ptr != 0) {
		// Line number found
		error_ptr++;
		err_line = strtol(error_ptr, NULL, 10);
	}

	// Break up lua code by newlines then insert line numbers & error notification,
	// and display +/- 5 lines of code around the error.
	// Note: strtok() can not be used to split display_code, because a sequence
	// of two or more contiguous delimiter bytes ('\n' in our case) in the parsed
	// string is considered to be a single delimiter. So, we would miss all
	// blank lines.
	char *display_code = strdup(code);
	char *ptr = display_code;

	for (;;) {
		char *line = ptr;

		if (*line == '\0')
			break;

		ptr = strchr(line, '\n');
		if (ptr)
			*ptr = '\0';

		if (cur_line >= (err_line-5) && cur_line <= (err_line+5)) {
			if (err_line != cur_line) {
				autostr_append(erronous_code, "%d  %s\n", cur_line, line);
			} else if (term_has_color_cap) { //color highlighting for Linux/Unix terminals
				autostr_append(erronous_code, "\033[41m>%d %s\033[0m\n", cur_line, line);
			} else {
				autostr_append(erronous_code, ">%d %s\n", cur_line, line);
			}
		}

		if (!ptr) {
			// We just inserted the last line
			break;
		}

		// Prepare for next line output
		*ptr = '\n';
		ptr++;
		cur_line++;
	}

	fflush(stdout);
	error_message(funcname, "Error running Lua code: %s.\nErroneous LuaCode={\n%s}",
			 PLEASE_INFORM, error_msg, erronous_code->value);

	free(display_code);
	free_autostr(erronous_code);
}

int lua_to_int(lua_Integer value)
{
	if (sizeof(lua_Integer) > sizeof(int)) {
		if (value > (lua_Integer)INT_MAX)
			return INT_MAX;
		if (value < (lua_Integer)INT_MIN)
			return INT_MIN;
		return (int)value;
	}
	return (int)value;
}

short lua_to_short(lua_Integer value)
{
	if (sizeof(lua_Integer) > sizeof(short)) {
		if (value > (lua_Integer)SHRT_MAX)
			return SHRT_MAX;
		if (value < (lua_Integer)SHRT_MIN)
			return SHRT_MIN;
		return (short)value;
	}
	return (short)value;
}

/**
 * Save Lua variables as lua code.
 * Variables prefixed with '_' are omitted because these are Lua predefined variables.
 */
void write_lua_variables(struct auto_string *dst)
{
	int boolean;
	const char *value;
	lua_State *L = get_lua_state(LUA_DIALOG);

	// Push global table on the stack
	lua_pushglobaltable(L);

	// Loop over the global table content
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		int value_type = lua_type(L, -1);
		int key_type = lua_type(L, -2);

		if (key_type != LUA_TSTRING) {
			lua_pop(L, 1);
			continue;
		}

		const char *name = lua_tostring(L, -2);
		if (name[0] == '_') {
			lua_pop(L, 1);
			continue;
		}

		switch (value_type)
		{
			case LUA_TBOOLEAN:
				boolean = lua_toboolean(L, -1);
				autostr_append(dst, "_G[\"%s\"] = %s\n", name, boolean ? "true" : "false");
				break;
			case LUA_TSTRING:
				value = lua_tostring(L, -1);
				autostr_append(dst, "_G[\"%s\"] = \"%s\"\n", name, value);
				break;
			case LUA_TNUMBER:
				value = lua_tostring(L, -1);
				autostr_append(dst, "_G[\"%s\"] = %s\n", name, value);
				break;
			default:
				break;
		}

		lua_pop(L, 1);
	}

	autostr_append(dst, "\n");

	// Pop global table from the stack
	lua_pop(L, 1);
}

void set_lua_ctor_upvalue(enum lua_target target, const char *fn, void *p)
{
	lua_State *L = get_lua_state(target);

	lua_getglobal(L, fn);
	lua_pushlightuserdata(L, p);
	if (!lua_setupvalue(L, -2, 1)) {
		lua_pop(L, 2);
		error_message(__FUNCTION__, "No upvalue defined for %s closure.",
		             PLEASE_INFORM | IS_FATAL, fn);
	}
	lua_pop(L, 1);
}

