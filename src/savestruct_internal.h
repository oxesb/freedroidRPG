/*
 *
 *   Copyright (c) 2004-2010 Arthur Huillet
 *   Copyright (c) 2011 Samuel Degrande
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
 * \file savestruct_internal.h
 * \brief Lua based save/load subsystem's definitions
 */

/// \defgroup luasaveload Lua based save/load subsystem
///
/// \par General overview
///   \n
///   The save/load subsystem is based on a set of functions automatically
///   generated by reading the \e struct.h file. The saved data is a Lua script
///   containing a set of Lua tables (one table per 'root' data structure).\n
///   The Lua parser is used to load the saved file, and calls table
///   constructors written in C.\n
///   \n
///   A python script (gen_savestruct.py) scans the struct.h file, and creates
///   RW functions (RW = Read/Write) in savestruct.[c,h] for each data
///   structure found in its \e dump_structs list, as well as for the data
///   types found in those structures.\n
///   \n
///   For each structure, the script creates a \e read_<struct>() and a
///   \e write_<struct>(). Those functions read and write each field of the
///   structure using calls to \e read_<fieldtype>() and \e write_<fieldtype>().\n
///   \n
///   For instance, for the following structure:
///   \code
/// struct my_struct {
///   int var1;
///   float var2;
/// };
///   \endcode
///   the python script creates:
///   \code
/// void read_my_struct(lua_State* L, int index, struct my_struct *data)
/// {
///   ...
///   read_int((L, -1, &data->var1);
///   ...
///   read_float(L, -1, &data->var2);
///   ...
/// }
///
/// void write_my_struct(struct auto_string *strout, struct my_struct *data)
/// {
///   ...
///   write_int(strout, &data->var1);
///   ...
///   write_float(strout, &data->var2);
/// }
///   \endcode
///
///   \par
///   \b Note: Actually, the script uses \e read_int32_t() and \e write_int32_t(),
///   see the \e special_types list used by the script to define type aliases.\n
///   \n
///   So, we end up with calls to read and write of simple types (int, float,
///   or 'user defined' types, i.e. types not defined in struct.h).\n
///   Those \e terminal RW functions are 'hardcoded' in savestruct_internal.[c,h],
///   via a macro (see for instance \e define_read_xxx(uint32_t)) or in
///   extension (see for instance \e write_SDL_Rect()).\n
///   \n
///   As a consequence, when a new structure is introduced in \e struct.h,
///   nothing is to be written in order to RW it, apart from adding it to the
///   script's \e dump_structs list, in case of a 'root' data structure.\n
///   \n
///   However, if a new \e terminal type is used in a structure, the RW
///   functions of that type have to be added in savestruct_internal.[c,h].
///   If those functions have to be defined in another file (due to access to
///   static data, for instance), they have to be declared in savestruct_internal.h\n
///   \n
///   \b Note: For the python script to work, C structures used as field in other
///   structures have to be typedef-ed. Example:
///   \code
/// typedef struct one_struct {
/// 	...
/// } one_struct;
///
/// struct struct_to_be_saved {
/// 	...
///		one_struct my_data;
/// }
///   \endcode
///
///  \par Pointer fields
///    \n
///    If a field in a structure is a pointer, it will not be saved (written),
///    and it will be initialized to NULL when the structure is read.\n
///    It does, indeed, not make sense to save a pointer (i.e. a memory address),
///    because nothing ensures that the pointed object will be stored at the
///    same address when the savegame will be reloaded.\n
///    An other write strategy is to be used, usually meaning that a 'user type'
///    is to be defined, with its own RW functions (see for instance, the
///    \e 'string' type mentionned in \b Special \b types).\n
///
///  \par Aggregate types
///    \n
///    An \b aggregate type is an array, a dynarray, a sparse dynarray or a list.\n
///    They need a special naming in order for the python script to know what
///    kind of aggregate is to be used, and what type the aggregate contains:
///    \b <element_type>_<aggregate_type>\n
///    \n
///    Typedefs have to be set in \e struct.h to create those names. For instance:
///    \code
/// typedef struct item {
///   ...
/// } item;
/// typedef struct dynarray item_dynarray;
///
/// typedef struct melee_shot {
///   ...
/// } melee_shot;
/// typedef struct sparse_dynarray melee_shot_sparse_dynarray;
///
/// typedef struct timed_event {
///   ...
///   list_head_t node;
/// } timed_event;
/// typedef struct list_head timed_event_list;
///     \endcode
///
/// \par Special types
///   - The \e 'char' type MUST NOT be used for fields of structures that are to
///     be written in a savegame:
///     - for a single char, use \b 'uint8_t'.
///     - for an array of char containing a text, use \b 'string'.
///     - for an array of char containing flags, use \b 'uint8_t[]'.
///
///     \n
///     This rule ensures that char-arrays are saved using the right encoding. The
///     python script will complain if it finds a \e 'char' type, and it will exit
///     with failure (see the \e forbidden_types list in the python script).\n
///
///   - If a field is a pointer to a static data (such as a static text), i.e.
///     if the pointed value is hardcoded in the sources, it MUST NOT be saved.
///     \n
///     Thus, it cannot be defined using the \e 'string' type, because \e 'string'
///     data are saved. It can also not be a \e 'char*', because \e 'char' is a
///     \e 'forbidden' type.
///     \n
///     As a trick, we introduced the \e 's_char' type (which is defined to be a
///     \e 'char').
///
/// \par User defined RW functions
///   \n
///   It is sometimes not possible or optimal to use the automatically created
///   RW functions (for instance, if elements of an array flagged as deleted
///   are not to be saved). Pair of specific RW functions can be written (see
///   savestruct_internal.c for some examples).\n
///   They can be defined in any source file, but they need to be declared in
///   savestruct_internal.h\n
///   \n
///   They also have to be added to the \e 'harcdoded_types' list of the python
///   script, to avoid the script to create its own version of those functions.
///
/// \par Guide to add a new data to save/load
///   \n
///   As an example, we add a new list of structs:
///   - In struct.h, add the declaration of the structure and define a specific
///     type for the list:
///     \code
/// typedef struct my_element {
///   ...
/// } my_element;
/// typedef struct list_head my_element_list;
///     \endcode
///     \n
///   - In gen_savestruct.py, add the typedefed name of the struct in
///     \e 'dump_structs', and the typedefed name of the list in \e 'additional_types'.\n
///     \n
///   - In global.h, declare the global variable of the new list type:
///     \code
/// EXTERN my_element_list the_elements_head;
///     \endcode
///     (for a dynarray, for instance, it would be: EXTERN my_element_dynarray all_elements)\n
///     \n
///   - That global variable is to be initialized in one source file. For a
///     list, it can be, for instance:
///     \code
/// LIST_INIT(the_elements_head);
///     \endcode
///     \n
///   - If needed, write the RW functions wherever they fit, and define them in
///     savestruct_internal.h (write_my_element(...), read_my_element(...),
///     write_my_element_list(...), read_my_element_list(...))\n
///     \n
///   - In savestruct_internal.c, add the calls to the RW functions of the list:
///     \code
/// static int the_elements_ctor(lua_State *L)
/// {
///   read_my_element_list(L, 1, &the_elements_head);
///   return 0;
/// }
///
/// void save_game_data(struct auto_string *strout)
/// {
///   ...
///   autostr_append(strout, "the_elements");
///   write_my_element_list(strout, &the_elements_head);
///   autostr_append(strout, "\n");
/// }
///
/// void load_game_data(char *strin)
/// {
///   luaL_Reg lfuncs[] = {
///     ...
///     {"the_elements", the_elements_ctor},
///     ...
///   };
/// }
///     \endcode

#ifndef _savestruct_internal_h
#define _savestruct_internal_h

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "lua.h"

#include "savestruct.h"

/// \defgroup helpers Functions or macros helpers
/// \ingroup luasaveload

/**
 * Try to get a named field and output a warning if it fails.
 * \ingroup helpers
 *
 * The macro is implemented with a compound statement expression, so that it can
 * be used as a predicate in a conditional statement.
 * \param L Lua state
 * \param n Index on the stack
 * \param t Name of the field
 */

#define lua_getfield_or_warn(L, n, t)\
({ \
	lua_getfield(L, n, t);\
	int rc = !lua_isnil(L, -1);\
	if (!rc) {\
		error_once_message(ONCE_PER_GAME, __FUNCTION__, "Field \"%s\" not found", PLEASE_INFORM, t);\
		alert_once_window(ONCE_PER_GAME, _("An error occurred when trying to load the savegame.\n"\
		                                   "A common reason for this is that FreedroidRPG has been "\
		                                   "updated to a newer version since the save was made, in "\
		                                   "which case the savegame is very likely not compatible.\n"\
		                                   "If you see this message and you have not updated the game, "\
		                                   "make sure to report this to the developers.\n"\
		                                   "Thanks!"));\
		lua_pop(L, 1);\
	}\
	rc;\
})

/**
 * Check the type of a data on Lua stack, and aborts if type is wrong.
 * \ingroup helpers
 *
 * \param L Lua state
 * \param n Index on the stack
 * \param t Lua type to check against
 */

#define lua_is_of_type_or_abort(L, n, t)\
do {\
	if (lua_type(L, n) != t) {\
		error_message(__FUNCTION__, "Unexpected data type\n", PLEASE_INFORM);\
		alert_window(_("An error occurred when trying to load the savegame.\n"\
		               "A data type was found to be incompatible with the "\
		               "expected one. Your savegame could be corrupted and so "\
		               "its loading is aborted.\n"\
		               "If you see this message and you have not manually modified "\
		               "your savegame, make sure to report this to the developers.\n"\
		               "Thanks!"));\
		longjmp(saveload_jmpbuf, 1);\
	}\
} while(0)

/// \defgroup arraymacros Save/load array macros
/// \ingroup luasaveload
///
/// Functions needed to write/read arrays (or dynarrays) are defined using
/// a set of macros. The gen_savestruct python script adds them to
/// savestruct.c when it detects that the field of a structure is an
/// array (or dynarray).
/// You should thus rarely have to used them directly.

/**
 * Define a function to write an array of type X.
 * \ingroup arraymacros
 *
 * The generated function will write a Lua table containing all elements of
 * a C array.
 * \param X Data type
 */

#define define_write_xxx_array(X)\
void write_##X##_array(struct auto_string *strout, X *data, int size)\
{\
	autostr_append(strout, "{\n");\
	int i;\
	for (i = 0; i < size; i++) {\
		write_##X(strout, &data[i]);\
		autostr_append(strout, ",\n");\
	}\
	autostr_append(strout, "}\n");\
}

/**
 * Define a function to write a dynarray of type X.
 * \ingroup arraymacros
 *
 * The generated function will write a Lua table containing all elements of
 * a C dynarray.
 * \param X Data type
 */

#define define_write_xxx_dynarray(X)\
void write_##X##_dynarray(struct auto_string *strout, X##_dynarray *data)\
{\
	autostr_append(strout, "{\n");\
	int i;\
	for (i = 0; i < data->size; i++) {\
		write_##X(strout, &((X *)data->arr)[i]);\
		autostr_append(strout, ",\n");\
	}\
	autostr_append(strout, "}\n");\
}

/**
 * Define a function to write a sparse dynarray of type X.
 * \ingroup arraymacros
 *
 * The generated function will write a Lua table containing all elements of
 * a C dynarray. Only used slots are written.
 * \param X Data type
 */

#define define_write_xxx_sparse_dynarray(X)\
void write_##X##_sparse_dynarray(struct auto_string *strout, X##_sparse_dynarray *data)\
{\
	autostr_append(strout, "{\n");\
	int i;\
	for (i = 0; i < data->size; i++) {\
		if (data->used_members[i]) {\
			write_##X(strout, &((X *)data->arr)[i]);\
			autostr_append(strout, ",\n");\
		}\
	}\
	autostr_append(strout, "}\n");\
}

/**
 * Define a function to write a list of type X.
 * \ingroup arraymacros
 *
 * The generated function will write a Lua table containing all elements of
 * a C list.
 * \param X Data type
 */

#define define_write_xxx_list(X)\
void write_##X##_list(struct auto_string *strout, X##_list *data)\
{\
	autostr_append(strout, "{\n");\
	X *elt;\
	list_for_each_entry(elt, data, node) {\
		write_##X(strout, elt);\
		autostr_append(strout, ",\n");\
	}\
	autostr_append(strout, "}\n");\
}

/**
 * Define a function to read an array of type X.
 * \ingroup arraymacros
 *
 * The generated function will read a Lua table, and store each read element
 * in a C array.
 * \param X Data type
 */

#define define_read_xxx_array(X)\
void read_##X##_array(lua_State *L, int index, X *result, int array_size)\
{\
	lua_is_of_type_or_abort(L, index, LUA_TTABLE);\
	int i;\
	for (i = 0; i < lua_rawlen(L, -1) && i < array_size; i++) {\
		lua_rawgeti(L, index, i+1);\
		read_##X(L, -1, &result[i]);\
		lua_pop(L, 1);\
	}\
}

/**
 * Define a function to read a dynarray of type X.
 * \ingroup arraymacros
 *
 * The generated function will read a Lua table, and append each read element
 * into a C dynarray.
 * \param X Data type
 */

#define define_read_xxx_dynarray(X)\
void read_##X##_dynarray(lua_State *L, int index, X##_dynarray *result)\
{\
	lua_is_of_type_or_abort(L, index, LUA_TTABLE);\
	int array_size = lua_rawlen(L, index);\
	if (array_size != 0) {\
		dynarray_init((struct dynarray *)result, array_size, sizeof(X));\
		int i;\
		X data;\
		for (i = 0; i < array_size; i++) {\
			lua_rawgeti(L, index, i+1);\
			read_##X(L, -1, &data);\
			dynarray_add((struct dynarray *)result, &data, sizeof(X));\
			lua_pop(L, 1);\
		}\
	} else {\
		dynarray_init((struct dynarray *)result, 0, sizeof(X));\
	}\
}

/**
 * Define a function to read a sparse dynarray of type X.
 * \ingroup arraymacros
 *
 * The generated function will read a Lua table, and append each read element
 * into a C dynarray. The array is initialized as being a sparse dynarray.
 * \param X Data type
 */

#define define_read_xxx_sparse_dynarray(X)\
void read_##X##_sparse_dynarray(lua_State *L, int index, X##_sparse_dynarray *result)\
{\
	lua_is_of_type_or_abort(L, index, LUA_TTABLE);\
	int array_size = lua_rawlen(L, index);\
	if (array_size != 0) {\
		sparse_dynarray_init((struct sparse_dynarray *)result, array_size, sizeof(X));\
		int i;\
		X data;\
		for (i = 0; i < array_size; i++) {\
			lua_rawgeti(L, index, i+1);\
			read_##X(L, -1, &data);\
			sparse_dynarray_add((struct sparse_dynarray *)result, &data, sizeof(X));\
			lua_pop(L, 1);\
		}\
	} else {\
		sparse_dynarray_init((struct sparse_dynarray *)result, 0, sizeof(X));\
	}\
}

/**
 * Define a function to read a list of type X.
 * \ingroup arraymacros
 *
 * The generated function will read a Lua table, and add each read element
 * on the tail of a C list.
 * \param X Data type
 */

#define define_read_xxx_list(X)\
void read_##X##_list(lua_State *L, int index, X##_list *result)\
{\
	lua_is_of_type_or_abort(L, index, LUA_TTABLE);\
	int array_size = lua_rawlen(L, index);\
	INIT_LIST_HEAD(result);\
	if (array_size != 0) {\
		for (int i = 0; i < array_size; i++) {\
			lua_rawgeti(L, index, i+1);\
			X *data = MyMalloc(sizeof(X));\
			read_##X(L, -1, data);\
			list_add_tail(&data->node, result);\
			lua_pop(L, 1);\
		}\
	}\
}

/// \defgroup simplerw Read/write of simple types
/// \ingroup luasaveload
///
/// Functions used to read and write simple types

void read_uint8_t(lua_State *L, int index, uint8_t *data);
void write_uint8_t(struct auto_string *strout, uint8_t *data);
void read_uint16_t(lua_State *L, int index, uint16_t *data);
void write_uint16_t(struct auto_string *strout, uint16_t *data);
void read_int16_t(lua_State *L, int index, int16_t *data);
void write_int16_t(struct auto_string *strout, int16_t *data);
void read_uint32_t(lua_State *L, int index, uint32_t *data);
void write_uint32_t(struct auto_string *strout, uint32_t *data);
void read_int32_t(lua_State *L, int index, int32_t *data);
void write_int32_t(struct auto_string *strout, int32_t *data);
void read_float(lua_State *L, int index, float *data);
void write_float(struct auto_string *strout, float *data);
void read_double(lua_State *L, int index, double *data);
void write_double(struct auto_string *strout, double *data);
void read_string(lua_State *L, int index, string *data);
void write_string(struct auto_string *strout, string *data);

/// \defgroup userrw Read/write of 'user' types
/// \ingroup luasaveload
///
/// Functions used to read and write 'user' types.
/// 'User' types are compound types or C structures not defined in struct.h, and
/// thus needing hardcoded read/write functions.

void read_luacode(lua_State *L, int index, luacode *data);
void write_luacode(struct auto_string *strout, luacode *data);
void read_SDL_Rect(lua_State *L, int index, SDL_Rect *data);
void write_SDL_Rect(struct auto_string *strout, SDL_Rect *data);
void read_automap_data_t(lua_State *L, int index, automap_data_t *data);
void write_automap_data_t(struct auto_string *strout, automap_data_t *data);
void read_list_head_t(lua_State *L, int index, list_head_t *data);
void write_list_head_t(struct auto_string *strout, list_head_t *data);
void read_game_config(lua_State *L, int index);
void write_game_config(struct auto_string *strout);

/// \defgroup overloadrw Overloaded read/write functions
/// \ingroup luasaveload
///
/// Some types can not be saved using the 'standard' scheme, and need specific
/// read/write functions.

void read_keybind_t_array(lua_State *L, int index, keybind_t *result, int size);
void write_keybind_t_array(struct auto_string *strout, keybind_t *data, int size);
void write_event_triggers_dynarray(struct auto_string *);
void read_event_triggers_dynarray(lua_State *, int);

/// \defgroup externalrw Declaration of external read/write functions
/// \ingroup luasaveload
///
/// Some read/write functions use private data defined outside of the saveload
/// subsystem. They however have to be declared here.

/**
 * Read a 'faction'.
 * \ingroup externalrw
 *
 * \param L           Current Lua State
 * \param index       Lua stack index of the data
 * \param faction_idx Pointer to the index of the faction to be read (a pointer is used
 *                    for compatibility with standard read function's signature)
 */
void read_faction(lua_State *L, int index, int *faction_idx);

/**
 * Write a 'faction'.
 * \ingroup externalrw
 *
 * \param strout The auto_string to be filled
 * \param faction_idx Pointer to the index of the faction to be saved (a pointer is used
 *                    for compatibility with standard write function's signature)
 */
void write_faction(struct auto_string *strout, int *faction_idx);

/// \defgroup toprw 'Root' save/load functions
/// \ingroup luasaveload
///
/// Functions used by the game's core to save/load a whole
/// set of data.

void save_game_data(struct auto_string *strout);
void load_game_data(char *strin);
void save_freedroid_configuration(struct auto_string *strout);
void load_freedroid_configuration(char *strin);

#endif // _savestruct_internal_h
