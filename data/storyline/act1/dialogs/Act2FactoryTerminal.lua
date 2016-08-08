---------------------------------------------------------------------
-- This file is part of Freedroid
--
-- Freedroid is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- Freedroid is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with Freedroid; see the file COPYING. If not, write to the
-- Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
-- MA 02111-1307 USA
----------------------------------------------------------------------

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {
	EveryTime = function()
		play_sound("effects/Menu_Item_Deselected_Sound_0.ogg")
		Act2BotFactory_prompt = "tux@gateserv.rr: ~ #"

		if (not Tux:has_item("Arcane Lore")) then
			cli_says(_"Connection refused. ", "NO_WAIT")
			Npc:says(_"This is a RR1979 secure terminal immune to hack attempts.") -- RR 1979 is ACT2 Review Request on Review Board...
			end_dialog()
		else
			Npc:says(_"[b]Arcane Lore[/b] - Special disk to hack secure RR1979 terminals.", "NO_WAIT")
			Npc:says(_"[b]User tux created.[/b] Enjoy!", "NO_WAIT")
			Npc:says("")
			cli_says(_"Login : ", "NO_WAIT")
			--; TRANSLATORS: username, maybe this should stay in lowercase letters?
			Tux:says(_"tux", "NO_WAIT")
			cli_says(_"Entering as tux", "NO_WAIT")
			Npc:says("", "NO_WAIT")

			if (cmp_obstacle_state("Act2BotFactory", "closed")) then
				Npc:says(_"R&R Factory gate status: CLOSED", "NO_WAIT")
				show("node0")
			elseif (cmp_obstacle_state("Act2BotFactory", "opened")) then
				Npc:says(_"R&R Factory gate status: OPEN", "NO_WAIT")
				show("node10")
			else
				Npc:says("GAME BUG. PLEASE REPORT, RRGATE_TERMINAL EveryTime LuaCode")
			end
			cli_says(Act2BotFactory_prompt, "NO_WAIT")
			show("node99")
		end
	end,

	{
		id = "node0",
		--; TRANSLATORS: command,  use lowercase here
		text = _"open gate",
		echo_text = false,
		code = function()
			--; TRANSLATORS: command,  use lowercase here
			Tux:says(_"open gate", "NO_WAIT")


			-- The challenge to open the gate may provoke EXTREME DEATH
			number_one=math.random(2,5)
			number_two=math.random(1,number_one-1)
			captcha = number_one + number_two
			if (captcha == 3) then
				captcha = _"three"
			elseif (captcha == 4) then
				captcha = _"four"
			elseif (captcha == 5) then
				captcha = _"five"
			elseif (captcha == 6) then
				captcha = _"six"
			elseif (captcha == 7) then
				captcha = _"seven"
			elseif (captcha == 8) then
				captcha = _"eight"
			elseif (captcha == 9) then
				captcha = _"nine"
			end
			response = user_input_string(string.format(_"CAPTCHA: Please write the lowercase word that answers the following: %d + %d = ?", number_one, number_two))


			if (captcha ~= response) then
				Npc:says(_"Access Denied. Electrical discharge in progress.")
				Npc:says(_"TIP OF THE DAY: make sure you enter a word and not digits.")
				freeze_tux_npc(2)
				Tux:hurt(30)
				Tux:heat(60)
				play_sound("effects/Menu_Item_Selected_Sound_1.ogg")
				end_dialog()
			else
				Npc:says(_"Access granted. Opening gate ...")
				Npc:says(_"Gate status: OPEN")
				change_obstacle_state("Act2BotFactory", "opened")
				cli_says(Act2BotFactory_prompt, "NO_WAIT")
				hide("node0") show("node10")
			end
		end,
	},
	{
		id = "node10",
		--; TRANSLATORS: command,  use lowercase here
		text = _"close gate",
		echo_text = false,
		code = function()
			--; TRANSLATORS: command,  use lowercase here
			Tux:says(_"close gate", "NO_WAIT")
			Npc:says(_"Access granted. Closing gate ...")
			Npc:says(_"Gate status: CLOSED")
			change_obstacle_state("Act2BotFactory", "closed")
			cli_says(Act2BotFactory_prompt, "NO_WAIT")
			hide("node10") show("node0")
		end,
	},
	{
		id = "node99",
		--; TRANSLATORS: command,  use lowercase here
		text = _"logout",
		echo_text = false,
		code = function()
			--; TRANSLATORS: command,  use lowercase here
			Tux:says(_"logout", "NO_WAIT")
			Npc:says(_"Exiting...")
			play_sound("effects/Menu_Item_Selected_Sound_1.ogg")
			end_dialog()
		end,
	},
}
