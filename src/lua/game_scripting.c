/*
 *
 *   Copyright (c) 2008-2010 Arthur Huillet
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
 * This file contains functions related to the Lua scripting interface of FreedroidRPG
 * It contains functions bound to lua, that we call 'bfuncs' (bounded functions).
 * They are mainly used in dialogs and event triggers.
 * Those functions are step by step replaced by 'cfuncs' in lua bindings (see
 * src/lua/bindings/luaFD_bindings.h, and see the bfuncs array in this file).
 * We however keep them for backward compatibilty, but they should be considered as
 * deprecated.
 */

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#include "lua_core.h"

#include "lvledit/lvledit_actions.h"
#include "lvledit/lvledit_map.h"

#include "widgets/widgets.h"

/* Pointer to the Lua state for game scripting execution */
static lua_State *dialog_lua_state;

/**
 * Retrieve current chat context, and fail with error if there is no dialog
 * currently running.
 */
static struct chat_context *__get_current_chat_context(const char *funcname)
{
	struct chat_context *current_chat_context = chat_get_current_context();
	if (!current_chat_context)
		error_message(funcname, _("No chat context available on the context stack."), PLEASE_INFORM | IS_FATAL);
	return current_chat_context;
}

#define GET_CURRENT_CHAT_CONTEXT() __get_current_chat_context(__FUNCTION__)

/** Helper to retrieve the enemy a Lua function must act upon.
  * An optional dialog name can be provided, by default the function will act
  * upon the current chat droid.
  */
static enemy *get_enemy_opt(lua_State *L, int param_number, int optional)
{
	const char *dialog = luaL_optstring(L, param_number, NULL);

	if (!dialog) {
		struct chat_context *current_chat_context = GET_CURRENT_CHAT_CONTEXT();
		return current_chat_context->partner;
	}

	enemy *en = get_enemy_with_dialog(dialog);
	if (!optional && !en)
		error_message(__FUNCTION__, "Could not find a droid corresponding to the dialog \"%s\".", PLEASE_INFORM | IS_FATAL, dialog);
	return en;
}

static enemy *get_enemy_arg(lua_State *L, int param_number)
{
	return get_enemy_opt(L, param_number, FALSE);
}

//=====================================================================
// bfuncs, to use in dialogs scripts
//=====================================================================

static int bfunc_teleport(lua_State * L)
{
	gps stop_pos = { -1, -1, -1 };
	const char *label = luaL_checkstring(L, 1);
	gps teleport_pos = get_map_label_center(label);
	reset_visible_levels();
	Teleport(teleport_pos.z, teleport_pos.x, teleport_pos.y, TRUE, TRUE);
	Me.speed.x = 0.0;
	Me.speed.y = 0.0;
	clear_out_intermediate_points(&stop_pos, Me.next_intermediate_point, MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX);
	clear_active_bullets();
	return 0;
}

static int bfunc_teleport_npc(lua_State * L)
{
	const char *label = luaL_checkstring(L, 1);
	enemy *en = get_enemy_arg(L, 2);
	gps teleport_pos = get_map_label_center(label);
	teleport_enemy(en, teleport_pos.z, teleport_pos.x, teleport_pos.y);
	return 0;
}

static int bfunc_teleport_home(lua_State * L)
{
	TeleportHome();

	return 0;
}

static int bfunc_has_teleport_anchor(lua_State * L)
{
	if (Me.teleport_anchor.z != -1)
		lua_pushboolean(L, TRUE);
	else
		lua_pushboolean(L, FALSE);

	return 1;
}

static int bfunc_display_big_message(lua_State * L)
{
	const char *msg = luaL_checkstring(L, 1);
	SetNewBigScreenMessage(msg);
	return 0;
}

static int bfunc_display_console_message(lua_State * L)
{
	const char *msg = luaL_checkstring(L, 1);
	append_new_game_message("%s", msg);
	return 0;
}

static int bfunc_change_obstacle(lua_State * L)
{
	const char *obslabel = luaL_checkstring(L, 1);
	int type = lua_to_int(luaL_checkinteger(L, 2));
	change_obstacle_type(obslabel, type);
	return 0;
}

static int bfunc_get_obstacle_type(lua_State * L)
{
	const char *obslabel = luaL_checkstring(L, 1);

	obstacle *our_obstacle = give_pointer_to_obstacle_with_label(obslabel, NULL);

	lua_pushinteger(L, (lua_Integer)our_obstacle->type);
	return 1;
}

static int bfunc_delete_obstacle(lua_State * L)
{
	const char *obslabel = luaL_checkstring(L, 1);
	change_obstacle_type(obslabel, -1);
	return 0;
}

static int bfunc_obstacle_message(lua_State *L)
{
	const char *obslabel = luaL_checkstring(L, 1);
	const char *message = luaL_checkstring(L, 2);
	int obstacle_level_num;
	obstacle *o = give_pointer_to_obstacle_with_label(obslabel, &obstacle_level_num);
	level *l = curShip.AllLevels[obstacle_level_num];

	message = strdup(message);

	del_obstacle_extension(l, o, OBSTACLE_EXTENSION_SIGNMESSAGE);
	add_obstacle_extension(l, o, OBSTACLE_EXTENSION_SIGNMESSAGE, (void *)message);

	return 0;
}

static int bfunc_heal_tux(lua_State * L)
{
	if (Me.energy > 0) {
		Me.energy = Me.maxenergy;
		play_sound("effects/new_healing_sound.ogg");
	}
	return 0;
}

static int bfunc_kill_tux(lua_State * L)
{
	Me.energy = -100;
	return 0;
}

static int bfunc_hurt_tux(lua_State * L)
{
	int hp = lua_to_int(luaL_checkinteger(L, 1));

	if (hp < 0)
		play_sound("effects/new_healing_sound.ogg");

	hit_tux(hp);
	return 0;
}

static int bfunc_get_tux_hp(lua_State * L)
{
	lua_pushinteger(L, (lua_Integer)Me.energy);
	return 1;
}

static int bfunc_get_tux_max_hp(lua_State * L)
{
	lua_pushinteger(L, (lua_Integer)Me.maxenergy);
	return 1;
}

static int bfunc_heat_tux(lua_State * L)
{
	int temp = lua_to_int(luaL_checkinteger(L, 1));
	Me.temperature += temp;
	return 0;
}

static int bfunc_get_tux_cool(lua_State * L)
{
	lua_pushinteger(L, (lua_Integer)(Me.max_temperature - Me.temperature));
	return 1;
}

static int bfunc_improve_skill(lua_State * L)
{
	const char *skilltype = luaL_checkstring(L, 1);
	int *skillptr = NULL;
	if (!strcmp(skilltype, "melee")) {
		skillptr = &Me.melee_weapon_skill;
		SetNewBigScreenMessage(_("Melee fighting ability improved!"));
	} else if (!strcmp(skilltype, "ranged")) {
		skillptr = &Me.ranged_weapon_skill;
		SetNewBigScreenMessage(_("Ranged combat ability improved!"));
	} else if (!strcmp(skilltype, "programming")) {
		skillptr = &Me.spellcasting_skill;
		SetNewBigScreenMessage(_("Programming ability improved!"));
	} else {
		error_message(__FUNCTION__,
			     "Lua script called me with an incorrect parameter. Accepted values are \"melee\", \"ranged\", and \"programming\".",
			     PLEASE_INFORM);
	}

	if (skillptr) {
		ImproveSkill(skillptr);
	}
	return 0;
}

static int bfunc_get_skill(lua_State * L)
{
	const char *skilltype = luaL_checkstring(L, 1);
	int *skillptr = NULL;
	if (!strcmp(skilltype, "melee")) {
		skillptr = &Me.melee_weapon_skill;
	} else if (!strcmp(skilltype, "ranged")) {
		skillptr = &Me.ranged_weapon_skill;
	} else if (!strcmp(skilltype, "programming")) {
		skillptr = &Me.spellcasting_skill;
	} else {
		error_message(__FUNCTION__,
			     "Lua script called me with an incorrect parameter. Accepted values are \"melee\", \"ranged\", and \"programming\".",
			     PLEASE_INFORM);
	}

	if (skillptr) {
		lua_pushinteger(L, (lua_Integer)*skillptr);
	} else
		lua_pushinteger(L, (lua_Integer)0);

	return 1;
}

static int bfunc_improve_program(lua_State * L)
{
	const char *pname = luaL_checkstring(L, 1);
	improve_program(get_program_index_with_name(pname));
	return 0;
}

static int bfunc_downgrade_program(lua_State * L)
{
	const char *pname = luaL_checkstring(L, 1);
	downgrade_program(get_program_index_with_name(pname));
	return 0;
}

static int bfunc_get_program_revision(lua_State * L)
{
	const char *pname = luaL_checkstring(L, 1);
	lua_pushinteger(L, (lua_Integer)Me.skill_level[get_program_index_with_name(pname)]);
	return 1;
}

static int bfunc_delete_item(lua_State * L)
{
	const char *item_id = luaL_checkstring(L, 1);
	int mult = lua_to_int(luaL_optinteger(L, 2, 1));
	DeleteInventoryItemsOfType(get_item_type_by_id(item_id), mult);
	return 0;
}

static int bfunc_give_item(lua_State * L)
{
	const char *itemname = luaL_checkstring(L, 1);
	int mult = lua_to_int(luaL_optinteger(L, 2, 1));

	if (!mult) { error_message(__FUNCTION__, "Tried to give %s with multiplicity 0.", PLEASE_INFORM, itemname); return 0; }

	item NewItem;
	NewItem = create_item_with_id(itemname, TRUE, mult);

	// Either we put the new item directly into inventory or we issue a warning
	// that there is no room and then drop the item to the floor directly under
	// the current Tux position.  That can't fail, right?
	char msg[1000];
	if (!give_item(&NewItem)) {
		sprintf(msg, _("Received item: %s (on floor)"), itemname);
	} else {
		sprintf(msg, _("Received item: %s"), itemname);
	}
	SetNewBigScreenMessage(msg);
	return 0;
}

static int bfunc_sell_item(lua_State *L)
{
	struct chat_context *current_chat_context = GET_CURRENT_CHAT_CONTEXT();

	const char *itemname = luaL_checkstring(L, 1);
	int weight = lua_to_int(luaL_optinteger(L, 2, 1));
	const char *charname = luaL_optstring(L, 3, current_chat_context->partner->dialog_section_name);

	npc_add_shoplist(charname, itemname, weight);

	return 0;
}

static int bfunc_count_item_backpack(lua_State * L)
{
	const char *item_id = luaL_checkstring(L, 1);

	lua_pushinteger(L, (lua_Integer)CountItemtypeInInventory(get_item_type_by_id(item_id)));

	return 1;
}

static int bfunc_has_item_equipped(lua_State * L)
{
	const char *item_id = luaL_checkstring(L, 1);
	int item_idx = get_item_type_by_id(item_id);
	if ((item_idx != -1) && ((Me.weapon_item.type == item_idx) || (Me.drive_item.type == item_idx)
		|| (Me.armour_item.type == item_idx) || (Me.shield_item.type == item_idx)
		|| (Me.special_item.type == item_idx))) {
		lua_pushboolean(L, TRUE);
	} else {
		lua_pushboolean(L, FALSE);
	}
	return 1;
}

static int bfunc_equip_item(lua_State * L)
{
	const char *item_name = luaL_checkstring(L, 1);

	if (!item_name) {
		error_message(__FUNCTION__, "Tried to add item without a name", PLEASE_INFORM);
		return 0;
	}
	item new_item = create_item_with_id(item_name, TRUE, 1);
	equip_item(&new_item);
	SetNewBigScreenMessage(_("1 item received!"));
	return 0;
}

static int bfunc_death_item(lua_State * L)
{
	const char *item_id = luaL_checkstring(L, 1);
	enemy *en = get_enemy_arg(L, 2);
	if (!strcmp(item_id, "NONE"))
		en->on_death_drop_item_code = -1;
	else
		en->on_death_drop_item_code = get_item_type_by_id(item_id);
	return 0;
}

static int bfunc_add_diary_entry(lua_State * L)
{
	const char *mis_name = luaL_checkstring(L, 1);
	const char *text = luaL_checkstring(L, 2);

	mission_diary_add(mis_name, text);
	return 0;
}

static int bfunc_has_met(lua_State *L)
{
	const char *npc_name = luaL_checkstring(L, 1);
	struct npc *used_npc = npc_get(npc_name);
	lua_pushboolean(L, used_npc->chat_character_initialized);
	return 1;
}

static int bfunc_assign_mission(lua_State * L)
{
	const char *misname = luaL_checkstring(L, 1);
	const char *diarytext = luaL_optstring(L, 2, NULL);

	assign_mission(misname);
	if (diarytext != NULL)
		mission_diary_add(misname, diarytext);

	return 0;
}

static int bfunc_complete_mission(lua_State * L)
{
	const char *misname = luaL_checkstring(L, 1);
	const char *diarytext = luaL_optstring(L, 2, NULL);

	complete_mission(misname);
	if (diarytext != NULL)
		mission_diary_add(misname, diarytext);

	return 0;
}

static int bfunc_is_mission_assigned(lua_State * L)
{
	const char *misname = luaL_checkstring(L, 1);
	struct mission *quest = (struct mission *)dynarray_member(&Me.missions, get_mission_index_by_name(misname), sizeof(struct mission));

	lua_pushboolean(L, quest->MissionWasAssigned);

	return 1;
}

static int bfunc_is_mission_complete(lua_State * L)
{
	const char *misname = luaL_checkstring(L, 1);
	struct mission *quest = (struct mission *)dynarray_member(&Me.missions, get_mission_index_by_name(misname), sizeof(struct mission));

	lua_pushboolean(L, quest->MissionIsComplete);

	return 1;
}

static int bfunc_give_xp(lua_State * L)
{
	int xp = lua_to_int(luaL_checkinteger(L, 1)) * Me.experience_factor;
	char tmpstr[150];
	Me.Experience += xp;
	sprintf(tmpstr, _("+%d experience points"), xp);
	SetNewBigScreenMessage(tmpstr);
	return 0;
}

static int bfunc_eat_training_points(lua_State * L)
{
	int nb = lua_to_int(luaL_checkinteger(L, 1));
	Me.points_to_distribute -= nb;
	return 0;
}

static int bfunc_get_training_points(lua_State * L)
{
	lua_pushinteger(L, (lua_Integer)Me.points_to_distribute);
	return 1;
}

static int bfunc_add_gold(lua_State * L)
{
	int nb = lua_to_int(luaL_checkinteger(L, 1));
	char tmpstr[150];

	if (nb < 0 && -nb > Me.Gold) {
		error_message(__FUNCTION__, "Tried to remove %d gold from the player that only has %d!", PLEASE_INFORM,
			     -nb, Me.Gold);
		nb = -Me.Gold;
	}

	Me.Gold += nb;

	if (nb > 0)
		sprintf(tmpstr, _("Gained %d valuable circuits!"), nb);
	else
		sprintf(tmpstr, _("Lost %d valuable circuits!"), -nb);

	SetNewBigScreenMessage(tmpstr);
	return 0;
}

static int bfunc_get_gold(lua_State * L)
{
	lua_pushinteger(L, (lua_Integer)Me.Gold);
	return 1;
}

static int bfunc_change_stat(lua_State * L)
{
	const char *characteristic = luaL_checkstring(L, 1);
	int nb = lua_to_int(luaL_checkinteger(L, 2));
	int *statptr = NULL;

	if (!strcmp(characteristic, "strength")) {
		statptr = &Me.base_strength;
	} else if (!strcmp(characteristic, "dexterity")) {
		statptr = &Me.base_dexterity;
	} else if (!strcmp(characteristic, "CPU")) {
		statptr = &Me.base_cooling;
	} else if (!strcmp(characteristic, "vitality")) {
		statptr = &Me.base_physique;
	} else {
		error_message(__FUNCTION__,
			     "I was called with characteristic name %s - accepted values are \"strength\", \"dexterity\", \"CPU\", and \"vitality\".",
			     PLEASE_INFORM, characteristic);
		return 0;
	}

	*statptr += nb;
	return 0;
}

static int bfunc_respawn_level(lua_State * L)
{
	int lnb = lua_to_int(luaL_checkinteger(L, 1));

	respawn_level(lnb);

	return 0;
}

static int bfunc_trade_with(lua_State * L)
{
	const char *cname = luaL_checkstring(L, 1);

	struct npc *n = npc_get(cname);
	init_trade_with_character(n);

	return 0;
}

static int bfunc_upgrade_items(lua_State * L)
{
	item_upgrade_ui();

	return 0;
}

static int bfunc_craft_addons(lua_State * L)
{
	addon_crafting_ui();

	return 0;
}

static int bfunc_heal_npc(lua_State * L)
{
	enemy *en = get_enemy_arg(L, 1);
	en->energy = Droidmap[en->type].maxenergy;
	return 0;
}

static int bfunc_get_npc_damage_amount(lua_State * L)
{
	enemy *en = get_enemy_arg(L, 1);
	lua_pushinteger(L, (lua_Integer)(Droidmap[en->type].maxenergy - en->energy));
	return 1;
}

static int bfunc_get_npc_max_health(lua_State * L)
{
	enemy *en = get_enemy_arg(L, 1);
	lua_pushinteger(L, (lua_Integer)(Droidmap[en->type].maxenergy));
	return 1;
}

static int bfunc_npc_dead(lua_State *L)
{
	const char *cname = luaL_checkstring(L, 1);
	enemy *erot;
	int dead = 0;

	BROWSE_DEAD_BOTS(erot) {
		if (!strcmp(erot->dialog_section_name, cname)) {
			dead = 1;
			break;
		}
	}

	lua_pushboolean(L, dead);
	return 1;
}

static int bfunc_freeze_tux_npc(lua_State * L)
{
	float duration = luaL_checknumber(L, 1);
	enemy *en = get_enemy_arg(L, 2);
	en->paralysation_duration_left = duration;
	Me.paralyze_duration = duration;
	return 0;
}


static int bfunc_get_player_name(lua_State * L)
{
	if (Me.character_name)
		lua_pushstring(L, Me.character_name);
	else
		lua_pushstring(L, "");
	return 1;
}

static int bfunc_chat_says(lua_State * L)
{
	const char *answer = luaL_checkstring(L, 1);
	int no_wait = !strcmp(luaL_optstring(L, 2, "WAIT"), "NO_WAIT");

	chat_add_response(answer);

	if (no_wait)
		return 0;

	struct chat_context *current_chat_context = GET_CURRENT_CHAT_CONTEXT();
	current_chat_context->wait_user_click = TRUE;
	// The Lua manual says that:
	// "lua_yield() should only be called as the return expression of a C function"
	// But the "should" is actually a "must"...
	return lua_yield(L, 0);
}

static int bfunc_start_chat(lua_State * L)
{
	int called_from_dialog;
	struct enemy *partner;
	struct npc *used_npc;
	struct chat_context *chat_context;

	// This function can be called from an event lua script or from a dialog
	// lua script.
	// In the first case, we have to open the chat screen and launch the chat
	// engine.
	// In the second case, a dialog is already running, so we have to interrupt
	// it (yield the lua coroutine) to let the chat engine run the new dialog.

	// To know if the function is called from a dialog script, we check if
	// there is already something on the chat context stack.
	called_from_dialog = (chat_get_current_context() != NULL);

	// Create a chat context and push it on the satck
	// Get the enemy to chat with from its name, get associated npc and
	// dialog, and create a chat context
	partner = get_enemy_arg(L, 1);
	used_npc = npc_get(partner->dialog_section_name);
	if (!used_npc)
		return 0;

	chat_context = chat_create_context(partner, used_npc);
	if (!chat_push_context(chat_context)) {
		chat_delete_context(chat_context);
		return 0;
	}

	if (!called_from_dialog) {
		// Open the chat screen and run the chat engine.
		chat_run();
	} else {
		// Yield the current dialog script, to let the chat engine run the
		// new dialog.
		return lua_yield(L, 0); // lua_yield must be called in a return statement
	}

	return 0;
}

static int bfunc_end_chat(lua_State * L)
{
	struct chat_context *current_chat_context = GET_CURRENT_CHAT_CONTEXT();
	current_chat_context->end_dialog = 1;
	return 0;
}

static int bfunc_partner_started(lua_State * L)
{
	struct chat_context *current_chat_context = GET_CURRENT_CHAT_CONTEXT();
	lua_pushboolean(L, current_chat_context->partner_started);
	return 1;
}

static int bfunc_drop_dead(lua_State * L)
{
	enemy *en = get_enemy_arg(L, 1);
	hit_enemy(en, en->energy + 1, 0, Droidmap[en->type].is_human - 2, 0);

	struct chat_context *current_chat_context = GET_CURRENT_CHAT_CONTEXT();

	if (en == current_chat_context->partner)
		current_chat_context->end_dialog = 1;
	return 0;
}

static int bfunc_set_bot_state(lua_State * L)
{
	const char *cmd = luaL_checkstring(L, 1);
	enemy *en = get_enemy_arg(L, 2);
	enemy_set_state(en, cmd);
	return 0;
}

static int bfunc_broadcast_bot_state(lua_State * L)
{
	const char *cmd = luaL_checkstring(L, 1);

	struct chat_context *current_chat_context = GET_CURRENT_CHAT_CONTEXT();

	const char *dialogname = current_chat_context->partner->dialog_section_name;
	enemy *en;
	BROWSE_LEVEL_BOTS(en, current_chat_context->partner->pos.z) {
		if (!strcmp(en->dialog_section_name, dialogname) && (is_friendly(en->faction, FACTION_SELF))) {
			enemy_set_state(en, cmd);
		}
	}
	return 0;
}

static int bfunc_set_bot_destination(lua_State * L)
{
	const char *label = luaL_checkstring(L, 1);
	enemy *en = get_enemy_arg(L, 2);
	enemy_set_destination(en, label);
	return 0;
}

static int bfunc_set_rush_tux(lua_State * L)
{
	const uint8_t cmd = (luaL_checkinteger(L, 1) != FALSE);
	enemy *en = get_enemy_arg(L, 2);
	en->will_rush_tux = cmd;
	return 0;
}

static int bfunc_will_rush_tux(lua_State * L)
{
	enemy *en = get_enemy_arg(L, 1);
	lua_pushboolean(L, en->will_rush_tux);
	return 1;
}

static int bfunc_takeover(lua_State * L)
{
	int opponent_capsules = lua_to_int(luaL_checkinteger(L, 1));
	int player_capsules = 2 + Me.skill_level[get_program_index_with_name("Hacking")];
	int game_length = lua_to_int(luaL_optinteger(L, 2, (lua_Integer)100));

	int won = do_takeover(player_capsules, opponent_capsules, game_length, NULL);

	lua_pushboolean(L, won);

	return 1;
}

static int bfunc_bot_exists(lua_State *L)
{
	int exists = get_enemy_opt(L, 1, TRUE) != NULL;
	lua_pushboolean(L, exists);
	return 1;
}

static int bfunc_get_bot_type(lua_State * L)
{
	enemy *en = get_enemy_arg(L, 1);
	lua_pushstring(L, Droidmap[en->type].droidname);
	return 1;
}

static int bfunc_get_bot_class(lua_State * L)
{
       enemy *en = get_enemy_arg(L, 1);
       lua_pushinteger(L, (lua_Integer)Droidmap[en->type].class);
       return 1;
}

static int bfunc_get_bot_name(lua_State * L)
{
	enemy *en = get_enemy_arg(L, 1);
	lua_pushstring(L, en->short_description_text);
	return 1;
}

static int bfunc_get_bot_translated_name(lua_State * L)
{
	enemy *en = get_enemy_arg(L, 1);
	lua_pushstring(L, D_(en->short_description_text));
	return 1;
}

static int bfunc_set_bot_name(lua_State * L)
{
	const char *bot_name = luaL_checkstring(L, 1);
	enemy *en = get_enemy_arg(L, 2);
	free(en->short_description_text);
	en->short_description_text = strdup(bot_name);
	return 0;
}

static int bfunc_difficulty_level(lua_State * L)
{
	lua_pushnumber(L, GameConfig.difficulty_level);
	return 1;
}

static int bfunc_set_npc_faction(lua_State *L)
{
	const char *fact = luaL_checkstring(L, 1);
	enemy *en = get_enemy_arg(L, 2);
	en->faction = get_faction_id(fact);
	return 0;
}

static int bfunc_kill_faction(lua_State *L)
{
	const char *fact = luaL_checkstring(L, 1);
	const char *respawn = luaL_optstring(L, 2, "");
	if (strcmp(respawn, "no_respawn") && (strcmp(respawn, "")))
		error_message(__FUNCTION__, "\
				Received optional second argument \"%s\". Accepted value is \"no_respawn\".\n\
				Faction \"%s\" will now be killed and will respawn as usual.", PLEASE_INFORM, respawn, fact);

	int faction_id = get_faction_id(fact);
	int no_respawn = !strcmp(respawn, "no_respawn");
	enemy *erot, *nerot;

	// Kill alive bots of that faction and set their respawn attribute
	BROWSE_ALIVE_BOTS_SAFE(erot, nerot) {
		if (erot->faction != faction_id)
			continue;
		hit_enemy(erot, erot->energy + 1, 0, -2, 0);
		if (no_respawn)
			erot->will_respawn = FALSE;
	}

	// Perhaps there are also some dead bots in the faction that we
	// do not want to be respawned
	if (no_respawn) {
		BROWSE_DEAD_BOTS(erot) {
			if (erot->faction != faction_id)
				continue;
			erot->will_respawn = FALSE;
		}
	}

	return 0;
}

static int bfunc_user_input_string(lua_State *L)
{
	const char *title = luaL_checkstring(L, 1);
	const char *default_str = luaL_optstring(L, 2, "");

	const char *str = get_editable_string_in_popup_window(100, title, default_str);

	if (!str)
		str = strdup("");

	lua_pushstring(L, str);

	free((void *)str);
	return 1;
}

static int bfunc_set_faction_state(lua_State *L)
{
	const char *fact_name = luaL_checkstring(L, 1);
	const char *state_str = luaL_checkstring(L, 2);
	const char *fact2_name = luaL_optstring(L, 3, "self");

	enum faction_state state;
	enum faction_id fact_id = get_faction_id(fact_name);
	enum faction_id fact2_id = get_faction_id(fact2_name);

	if (!strcmp(state_str, "hostile"))
		state = HOSTILE;
	else if (!strcmp(state_str, "friendly"))
		state = FRIENDLY;
	else {
		error_message(__FUNCTION__, "Unknown faction state %s.", PLEASE_INFORM, state_str);
		return 0;
	}

	set_faction_state(fact_id, fact2_id, state);

	return 0;
}

static int bfunc_create_droid(lua_State *L)
{
	const char *label = luaL_checkstring(L, 1);
	const char *type_name = luaL_checkstring(L, 2);
	const char *fact_name = luaL_optstring(L, 3, "ms");
	const char *dialog    = luaL_optstring(L, 4, "AfterTakeover");
	const char *Sensor_ID = luaL_optstring(L, 5, NULL);
	gps pos = get_map_label_center(label);
	int type;

	type = get_droid_type(type_name);

	enemy *en = enemy_new(type);
	enemy_reset(en);
	en->pos.x = pos.x;
	en->pos.y = pos.y;
	en->pos.z = pos.z;
	en->faction = get_faction_id(fact_name);
	en->dialog_section_name = strdup(dialog);
	if (Sensor_ID != NULL)
		en->sensor_id = get_sensor_id_by_name(Sensor_ID);
	enemy_insert_into_lists(en, TRUE);

	return 0;
}

static int bfunc_get_game_time(lua_State *L)
{
	lua_pushinteger(L, (lua_Integer)(Me.current_game_date));

	return 1;
}

static int bfunc_get_game_date(lua_State *L)
{
	// This function retrieves the ingame date, using C functions defined in hud.c
	// It returns in sequence: days, hours and minutes.
	lua_pushinteger(L, get_days_of_game_duration(Me.current_game_date));
	lua_pushinteger(L, get_hours_of_game_duration(Me.current_game_date));
	lua_pushinteger(L, get_minutes_of_game_duration(Me.current_game_date));

	return 3;
}

static int bfunc_win_game(lua_State *L)
{
	ThouHastWon();

	return 0;
}

static int bfunc_jump_to_game_act(lua_State *L)
{
	const char *act_id = luaL_checkstring(L, 1);
	game_act_set_next(act_id);
	return 0;
}

static int bfunc_play_sound(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);

	play_sound(filename);
	return 0;
}

static int bfunc_freeze_tux(lua_State * L)
{
	float duration = luaL_checknumber(L, 1);
	Me.paralyze_duration = duration;
	return 0;
}

static int bfunc_freeze_npc(lua_State * L)
{
	float duration = luaL_checknumber(L, 1);
	enemy *en = get_enemy_arg(L, 2);
	en->paralysation_duration_left = duration;
	return 0;
}

static int bfunc_add_obstacle(lua_State *L)
{
	int levelnum = lua_to_int(luaL_checkinteger(L, 1));

#ifdef __clang_analyzer__
	// Avoid Clang Static Analyser to report a possible OOB access
	// on curShip.AllLevels[levelnum]
	if (levelnum < 0 || levelnum >= MAX_LEVELS) return 0;
#endif

	if (!level_exists(levelnum)) {
		error_message(__FUNCTION__, "Requested level num (%d) does not exists. Can not add the obstacle.", PLEASE_INFORM, levelnum);
		return 0;
	}
	struct level *lvl = curShip.AllLevels[levelnum];
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	int type = luaL_checknumber(L, 4);

	add_obstacle(lvl, x, y, type);

	return 0;
}

static int bfunc_add_volatile_obstacle(lua_State *L)
{
	int levelnum = lua_to_int(luaL_checkinteger(L, 1));

#ifdef __clang_analyzer__
	// Avoid Clang Static Analyser to report a possible OOB access
	// on curShip.AllLevels[levelnum]
	if (levelnum < 0 || levelnum >= MAX_LEVELS) return 0;
#endif

	if (!level_exists(levelnum)) {
		error_message(__FUNCTION__, "Requested level num (%d) does not exists. Can not add the obstacle.", PLEASE_INFORM, levelnum);
		return 0;
	}
	struct level *lvl = curShip.AllLevels[levelnum];
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	int type = luaL_checknumber(L, 4);

	struct obstacle_spec *obs_spec = get_obstacle_spec(type);
	add_volatile_obstacle(lvl, x, y, type, obs_spec->vanish_delay + obs_spec->vanish_duration);

	return 0;
}

static int bfunc_meters_traveled(lua_State *L)
{
	lua_pushinteger(L, (lua_Integer)Me.meters_traveled);
	return 1;
}

static int bfunc_run_from_dialog(lua_State *L)
{
	lua_pushboolean(L, (chat_get_current_context() != NULL));
	return 1;
}

static int bfunc_running_benchmark(lua_State *L)
{
	lua_pushboolean(L, (do_benchmark) != NULL);
	return 1;
}

static int bfunc_reprogramm_bots_after_takeover(lua_State *L)
{
	int rvat = lua_to_int(luaL_checkinteger(L, 1));
	GameConfig.talk_to_bots_after_takeover = rvat;

	return 0;
}

static int bfunc_switch_background_music_to(lua_State *L)
{
	char *filename = (char *)luaL_checkstring(L, 1);
	switch_background_music(filename);
	return 0;
}

static int bfunc_exit_game(lua_State *L)
{
	lua_Integer exit_status = luaL_checkinteger(L, 1);

	if (exit_status == 1) {
		Terminate(EXIT_FAILURE);
		return 0;
	} else {
		Terminate(EXIT_SUCCESS);
		return 0;
	}
}

static int bfunc_find_file(lua_State *L)
{
	const char *filename = (char *)luaL_checkstring(L, 1);
	if (lua_type(L, 2) != LUA_TTABLE) {
		error_message(__FUNCTION__,
		              "Unexpected data type for the second parameter. A table is expected.",
		              PLEASE_INFORM);
	} else {
		// Loop on each subdir, stopping as soon as the requested file is found
		for (int i = 0; i < lua_rawlen(L, -1); i++) {
			lua_rawgeti(L, 2, i+1);
			int subdir_handle = lua_to_int(luaL_checkinteger(L, -1));
			lua_pop(L, 1);

			if (subdir_handle >= 0 && subdir_handle < LAST_DATA_DIR) {
				char fpath[PATH_MAX];
				if (find_file(fpath, subdir_handle, filename, NULL, SILENT)) {
					lua_pushstring(L, fpath);
					return 1;
				}
			}
		}
		error_message(__FUNCTION__, "Dialog file %s was not found.", PLEASE_INFORM, filename);
	}

	lua_pushnil(L); /* return nil on error */
	return 1;
}

static int bfunc_term_has_color_cap(lua_State *L)
{
	lua_pushboolean(L, (term_has_color_cap == TRUE));
	return 1;
}

static int bfunc_dir(lua_State *L)
{
	/* Note: Code taken (and adapted) from "Programming in Lua, 2nd edition" */

	DIR *dir = NULL;
	struct dirent *entry = NULL;
	int i;
	int subdir_handle = lua_to_int(luaL_checkinteger(L, 1));

	if (subdir_handle < 0 || subdir_handle >= LAST_DATA_DIR) {
		lua_pushnil(L); /* return nil on error */
		return 1;
	}

	/* open directory */
	dir = opendir(data_dirs[subdir_handle].path);

	/* create the returned result table */
	lua_newtable(L);
	i = 1;
	while ((entry = readdir(dir)) != NULL) {
		lua_pushnumber(L, i++); /* push key */
		lua_pushstring(L, entry->d_name); /* push value */
		lua_settable(L, -3);
	}

	closedir(dir);
	return 1;
}

static int bfunc_set_mouse_move_target(lua_State *L)
{
	/* USE WITH CARE!
	 * I made this function so we could automatize some tests on level 24
	 * This is not supposed to be used in the "real game"
	 */
	Me.mouse_move_target.x = luaL_checknumber(L, 1);
	Me.mouse_move_target.y = luaL_checknumber(L, 2);
	Me.mouse_move_target.z = luaL_checknumber(L, 3);

	move_tux();

	return 1;
}

static int bfunc_get_game_version(lua_State *L)
{
	lua_pushstring(L, freedroid_version);
	return 1;
}

static int bfunc_event_trigger_disable(lua_State *L)
{
	char *name = (char *)luaL_checkstring(L, 1);
	if (!event_trigger_set_enable(name, FALSE)) {
		error_message(__FUNCTION__, "Event trigger %s was not found.", PLEASE_INFORM, name);
	}
	return 0;
}

static int bfunc_event_trigger_enable(lua_State *L)
{
	char *name = (char *)luaL_checkstring(L, 1);
	if (!event_trigger_set_enable(name, TRUE)) {
		error_message(__FUNCTION__, "Event trigger %s was not found.", PLEASE_INFORM, name);
	}
	return 0;
}

static int bfunc_event_trigger_enabled(lua_State *L)
{
	char *name = (char *)luaL_checkstring(L, 1);
	uint32_t state;
	if (!event_trigger_get_state(name, &state)) {
		error_message(__FUNCTION__, "Event trigger %s was not found.", PLEASE_INFORM, name);
		lua_pushnil(L); /* return nil on error */
		return 1;
	}
	lua_pushboolean(L, state & TRIGGER_ENABLED);
	return 1;
}

static int bfunc_dispatch_event(lua_State *L)
{
	const char *trigger_name = luaL_checkstring(L, 1);
	float time = luaL_checknumber(L, 2);
	dispatch_event_timer(trigger_name, Me.current_game_date + time);
	return 0;
}

luaL_Reg bfuncs[] = {
	/* teleport(string map_label)
	 * Teleports the player to the given map label.
	 */
	{"teleport", bfunc_teleport} // -> FDtux:teleport
	,
	/* teleport_npc(string map_label, [dialog name])
	 * Teleports the current npc, or named npc to the given map label
	 */
	{"teleport_npc", bfunc_teleport_npc} // -> FDnpc:teleport
	,
	/* teleport_home(string map_label)
	 * Teleports the player to the home.
	 */
	{"teleport_home", bfunc_teleport_home} // -> FDtux:teleport_home
	,
	/* has_teleport_anchor()
	 * Return true if a teleport anchor is active.
	 */
	{"has_teleport_anchor", bfunc_has_teleport_anchor} // -> FDtux:has_teleport_anchor
	,
	/* display_big_message(string msg)
	 * Displays a big vanishing message on screen (seen in game, not in the dialog).
	 */
	{"display_big_message", bfunc_display_big_message}
	,
	/* use display_console_message(string msg), supports [b] and [/b]
	 * Displays a message on the game console.
	 */
	{"event_display_console_message", bfunc_display_console_message}
	,

	/* change_obstacle_type(string obstacle_label, int obstacle_type)
	 * Changes the obstacle to the given state.
	 */
	{"change_obstacle_type", bfunc_change_obstacle},
	{"get_obstacle_type", bfunc_get_obstacle_type}
	,
	/* del_obstacle(string obstacle_label)
	 * Delete the given obstacle
	 */
	{"del_obstacle", bfunc_delete_obstacle}
	,

	/* change_obstacle_message(string obstacle_label, string message)
	 * Change the SIGNMESSAGE of the given obstacle.
	 */
	{"change_obstacle_message", bfunc_obstacle_message}
	,

	/* kill_tux() - kills Tux
	 * heal_tux() - heal_tux completely heals Tux
	 * hurt_tux(int how_many_hp_to_remove) - removes the given number of health points.
	 * 	This number can obviously be negative.
	 * heat_tux(int amount)	- Increases temperature (=removes cooling), number can be negative.
	 */
	{"kill_tux", bfunc_kill_tux} // -> FDtux:kill
	,
	{"heal_tux", bfunc_heal_tux} // -> FDtux:heal
	,
	{"hurt_tux", bfunc_hurt_tux} // -> FDtux:hurt
	,
	{"heat_tux", bfunc_heat_tux} // -> FDtux:heat
	,
	/* get_tux_hp()             - Returns Tux's current health
	 * get_tux_max_hp()         - Returns Tux's current maximum health
	 * see also: tux_hp_ratio() - Returns the ratio of the two
	 */
	{"get_tux_hp", bfunc_get_tux_hp} // -> FDtux:get_hp
	,
	{"get_tux_max_hp", bfunc_get_tux_max_hp} // -> FDtux:get_max_hp
	,
	/* get_tux_cool()		- Returns Tux's current remaining heat absorbing capabilities
	 */
	{"get_tux_cool", bfunc_get_tux_cool} // -> FDtux:get_cool
	,
	/* improve_skill(string skill_name)
	 * get_skill()
	 * improve_skill improves one of the three "melee", "ranged" and "programming" skills
	 * by one level.
	 * get_skill returns the current level (as an integer) of one of the three skills.
	 */
	{"improve_skill", bfunc_improve_skill} // -> FDtux:improve_skill
	,
	{"get_skill", bfunc_get_skill} // -> FDtux:get_skill
	,

	/* improve_program(string program_name)
	 * Improve the program given by one level.
	 * get_program_revision(string program_name) returns current program revision level
	 */
	{"improve_program", bfunc_improve_program} // -> FDtux:improve_program
	,
	{"downgrade_program", bfunc_downgrade_program} // -> FDtux:downgrade_program
	,
	{"get_program_revision", bfunc_get_program_revision} // -> FDtux:get_program_revision
	,
	/* del_item_backpack(string item_name[, int multiplicity = 1])
	 * add_item(string item_name, int multiplicity)
	 * - Deletes or adds the given number of items from/to the inventory.
	 *
	 * count_item_backpack(string item_name)
	 * - returns the number of items of the given name currently in the inventory.
	 *
	 * has_item_equipped(string item_name)
	 * - returns true when the item is equipped
	 */
	{"del_item_backpack", bfunc_delete_item} // -> FDtux:del_item_backpack
	,
	{"add_item", bfunc_give_item} // -> FDtux:add_item
	,
	{"count_item_backpack", bfunc_count_item_backpack} // -> FDtux:count_item_backpack
	,
	{"has_item_equipped", bfunc_has_item_equipped} // -> FDtux:has_item_equipped
	,
	{"equip_item", bfunc_equip_item} // -> FDtux:equip_item
	,
	{"sell_item", bfunc_sell_item}
	,
	/* set_death_item(string item_name [, string  npc])
	 * changes the item dropped when the droid dies
	*/
	{"set_death_item", bfunc_death_item} // -> FDnpc:set_death_item
	,
	{"add_diary_entry", bfunc_add_diary_entry}
	,
	{"has_met", bfunc_has_met} // -> FDtux:has_met
	,
	{"assign_quest", bfunc_assign_mission} // -> FDtux:assign_quest
	,
	{"has_quest", bfunc_is_mission_assigned} // -> FDtux:has_quest
	,
	{"complete_quest", bfunc_complete_mission} // -> FDtux:complete_quest
	,
	{"done_quest", bfunc_is_mission_complete} // -> FDtux:done_quest
	,
	{"add_xp", bfunc_give_xp} // -> FDtux:add_xp
	,
	{"del_training_points", bfunc_eat_training_points} // -> FDtux:del_training_points
	,
	{"get_training_points", bfunc_get_training_points} // -> FDtux:get_training_points
	,
	{"add_gold", bfunc_add_gold} // -> FDtux:add_gold
	,
	{"get_gold", bfunc_get_gold} // -> FDtux:get_gold
	,
	{"change_stat", bfunc_change_stat} // -> FDtux:change_stat
	,
	{"respawn_level", bfunc_respawn_level}
	,
	{"trade_with", bfunc_trade_with}
	,
	{"upgrade_items", bfunc_upgrade_items}
	,
	{"craft_addons", bfunc_craft_addons}
	,
	{"get_player_name", bfunc_get_player_name} // -> FDtux:get_player_name
	,
	{"chat_says", bfunc_chat_says}
	,
	{"start_chat", bfunc_start_chat}
	,
	{"end_dialog", bfunc_end_chat}
	,
	/* NOTE:  if (partner_started())  will always be true
	 * if rush_tux is 1
	 */
	{"partner_started", bfunc_partner_started}
	,
	{"drop_dead", bfunc_drop_dead}  // -> FDnpc:drop_dead
	,
	{"bot_exists", bfunc_bot_exists} // <Fluzz> Is that really needed ?
	,
	{"set_bot_state", bfunc_set_bot_state} // -> FDnpc:set_state
	,
	{"set_bot_destination", bfunc_set_bot_destination} // -> FDnpc:set_destination
	,
	{"broadcast_bot_state", bfunc_broadcast_bot_state} // <Fluzz> Broadcast only to bots with same dialog. Intended ?
	,
	/* set_rush_tux()   - Sets or unsets if the NPC should rush and talk to Tux
	 * will_rush_tux() - Checks if the NPC is planning on rushing Tux
	 */
	{"set_rush_tux", bfunc_set_rush_tux} // -> FDnpc:set_rush_tux
	,
	{"will_rush_tux", bfunc_will_rush_tux} // -> FDnpc:get_rush_tux
	,
	{"takeover", bfunc_takeover}
	,
	/* heal_npc([dialog])			- Returns the NPC's current health
	 * npc_damage_amount([dialog])		- Returns the current damage for the NPC
	 * npc_max_health([dialog])		- Returns the max possible health for the NPC
	 * see also: npc_damage_ratio([dialog]) - Returns the ratio of the two
	 */
	{"heal_npc", bfunc_heal_npc} // -> FDnpc:heal
	,
	{"npc_damage_amount", bfunc_get_npc_damage_amount} // -> FDnpc:get_damage
	,
	{"npc_max_health", bfunc_get_npc_max_health} // -> FDnpc:get_max_health
	,
	{"freeze_tux_npc", bfunc_freeze_tux_npc}
	,
	{"npc_dead", bfunc_npc_dead},  // -> FDnpc:is_dead
	/* bot_type() tells you what model
	   bot_class() tells you the class of a bot
	   bot_name() tells you what name it displays
	   set_bot_name() puts a new name in
	 */
	{"bot_type", bfunc_get_bot_type}, // -> FDnpc:get_type

	{"bot_class", bfunc_get_bot_class}, // -> FDnpc:get_class

	{"bot_name", bfunc_get_bot_name}, // -> FDnpc:get_name
	{"bot_translated_name", bfunc_get_bot_translated_name}, // -> FDnpc:get_translated_name

	{"set_bot_name", bfunc_set_bot_name}, // -> FDnpc:set_name

	{"difficulty_level", bfunc_difficulty_level},

	{"set_npc_faction", bfunc_set_npc_faction}, // -> FDnpc:set_faction
	{"set_faction_state", bfunc_set_faction_state},
	/*
	  kill_faction() kills all enemies belonging
	  to a specified faction. The second argument is
	  optional, and specifies whether or not the faction
	  will respawn. It can only be the string "no_respawn".
	*/
	{"kill_faction", bfunc_kill_faction},

	{"user_input_string", bfunc_user_input_string},

	{"create_droid", bfunc_create_droid},

	{"win_game", bfunc_win_game},
	// Finish the game.

	// Record that the current game act is finished and that a new game act
	// is to be started (will be executed once returned in the main loop)
	{"jump_to_game_act", bfunc_jump_to_game_act},

	{"game_time", bfunc_get_game_time},
	{"game_date", bfunc_get_game_date},
	/* play_sound("file")
	 * path has to originate from /sound , e.g.
	 * play_sound("effects/No_Ammo_Sound_0.ogg")
	 */
	{"play_sound", bfunc_play_sound},
	// freeze_tux() freezes tux for the given amount of seconds
	{"freeze_tux", bfunc_freeze_tux}, // -> FDtux:freeze
	// freeze_npc() freezes the npc for the given amount of seconds
	{"freeze_npc", bfunc_freeze_npc}, // -> FDnpc:freeze
	/* add_obstacle(lvl, x, y, obst_ID) add obstacles to maps at given position
	 * add_obstacle(8, 41.4, 51.5, 100)
	 * where 8 is the level number, x and y are the coordinates and 100
	 * is the obstacle ID (see defs.h)
	 */
	{"add_obstacle", bfunc_add_obstacle},
	{"add_volatile_obstacle", bfunc_add_volatile_obstacle},
	// meters_traveled() returns ingame meters tux has traveled
	{"meters_traveled", bfunc_meters_traveled}, // -> FDtux:get_meters_traveled
	// if (run_from_dialog()) then
	// to check if certain code was run from inside a dialog or not
	{"run_from_dialog", bfunc_run_from_dialog},
	// returns if we are running a benchmark e.g. the dialog validator
	// or not
	// USE WITH CARE
	{"running_benchmark", bfunc_running_benchmark},
	/* switch_background_music("file")
	 * path has to originate from /sound/music , e.g.
	 * play_sound("menu.ogg")
	 */
	{"switch_background_music", bfunc_switch_background_music_to},
	// 1 = true,  0 = false
	{"reprogramm_bots_after_takeover", bfunc_reprogramm_bots_after_takeover},

	{"exit_game", bfunc_exit_game},

	{"find_file", bfunc_find_file},
	{"dir", bfunc_dir},

	{"term_has_color_cap", bfunc_term_has_color_cap },
	/* USE WITH CARE!
	 * This function was made so that we could automate some tests on level 24
	 * This is not supposed to be used in the "real game"
	 */
	{"set_mouse_move_target", bfunc_set_mouse_move_target},

	{"get_game_version", bfunc_get_game_version},

	{"disable_event_trigger", bfunc_event_trigger_disable},
	{"enable_event_trigger", bfunc_event_trigger_enable},
	{"event_trigger_enabled", bfunc_event_trigger_enabled},

	{"dispatch_event", bfunc_dispatch_event},

	{NULL, NULL}
};

/** Auto-create instance of NPCs binding
 * (This avoid the need to explicitly do it in the dialog scripts)
 * The dialog's name of the NPC is used to create the Lua instance.
 * It must univoquely identify one single NPC. Failing to respect
 * that constraint can lead to some strange behaviors...
 * You were warned !
 */
extern int luaFD_npc_get_instance(lua_State *L);

static int auto_create_npc(lua_State *L)
{
	// params : table ("_G"), name
	const char *name = luaL_checkstring(L, 2);
	lua_pop(L, 2);

	// This function is called when 'name' is not known.
	// If it is an npc, then create it, register it in the global environment,
	// and return it.
	// Else, return nil
	if (get_enemy_with_dialog(name)) {
		lua_pushstring(L, name);
		luaFD_npc_get_instance(L);
		lua_remove(L, -2); // remove 'name' from the stack, keep the npc

		lua_pushvalue(L, -1); // setglobal will consume the npc, so we make a copy
		lua_setglobal(L, name);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

/**
 * Reset (or create) the Lua state used to load and execute the dialogs
 */

void init_lua_game_scripting(void)
{
	dialog_lua_state = create_lua_state(LUA_DIALOG);

	// Bind the 'out-of-module' functions
	for (int i = 0; bfuncs[i].name != NULL; i++) {
		lua_pushcfunction(dialog_lua_state, bfuncs[i].func);
		lua_setglobal(dialog_lua_state, bfuncs[i].name);
	}

	// Bindings
	luaFD_init(dialog_lua_state);

	// Load and initialize some Lua modules
	load_lua_module(LUA_DIALOG, LUA_MOD_DIR, "FDutils");
	load_lua_module(LUA_DIALOG, LUA_MOD_DIR, "FDdialog");
	call_lua_func(LUA_DIALOG, "FDdialog", "set_dialog_dirs", "dd", NULL, MAP_DIALOG_DIR, BASE_DIALOG_DIR);

	// Set the __index metamethod of the global environment,
	// by adding a metatable containing a __index closure
	// (used to auto-create NPCs, see the comment in index_metamethod())
	lua_getglobal(dialog_lua_state, "_G");
	lua_newtable(dialog_lua_state);
	lua_pushcfunction(dialog_lua_state, auto_create_npc);
	lua_setfield(dialog_lua_state, -2, "__index");
	lua_setmetatable(dialog_lua_state, -2);
	lua_pop(dialog_lua_state, 1);

	// Finally load the script helpers Lua functions
	char fpath[PATH_MAX];
	find_file(fpath, LUA_MOD_DIR, "script_helpers.lua", NULL, PLEASE_INFORM | IS_FATAL);
	run_lua_file(LUA_DIALOG, fpath);
}
