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
		Npc:says(_"[b]Loading terminal...[/b]")
		if (Tux:has_item("PC LOAD LETTER")) then
			Npc:says(_"[b]Loading biologic credentials.[/b]")
			if (not RRTerminal_connected) then
				if (takeover(12)) then -- I prefer a fixed number of charges...
					show("node0")
					RRTerminal_connected = true
					Npc:says(_"[b]Welcome to this terminal.[/b]")
				else
					Npc:says(_"[b]Access Denied.[/b]")
				end
			else
				show("node0")
				Npc:says(_"[b]Welcome to this terminal.[/b]")
			end

		else
			Npc:says(_"[b]No instructions to load this terminal.[/b]")
			Tux:says(_"help", "NO_WAIT")
			Npc:says(_"This terminal needs instructions to be loaded. Instructions can be requested at last floor of factory north from town.")
			Npc:says(_"[b]WARNING:[/b] Not everyone comes back alive. You also need to have special authorization from big boss to go there.")
			end_dialog()
		end
		show("node99")
	end,

	{
		id = "node0",
		--; TRANSLATORS: command, user lowercase here
		text = _"help",
		code = function()
			Npc:says(_"[b]Available commands: help, freeze, unfreeze, logout[/b]")
			show("node1", "node2")
		end,
	},
	{
		id = "node1",
		--; TRANSLATORS: command, user lowercase here
		text = _"freeze",
		code = function()
			Npc:says(_"[b]Failure: Either the capsule is in use or too damaged to follow proper freezing protocols.[/b]")
		end,
	},
	{
		id = "node2",
		--; TRANSLATORS: command, user lowercase here
		text = _"unfreeze",
		code = function()
			Npc:says(_"[b]WARNING: Decryonization in progress![/b]")
			Npc:says(_"[b]...[/b]")
			Npc:says(_"[b]...[/b]")
			--; TRANSLATORS: Keep First Letter (Of Word) Case Please.
			Npc:says(_"[b]...Initiating Final Unfreezing Sequences...[/b]")
			Npc:says(_"[b]Closing oxygen vial...[/b]", "NO_WAIT") -- Can't tell what's their definition of vial.
			Npc:says(_"[b]Closing water vial...[/b]", "NO_WAIT")
			Npc:says(_"[b]Closing memory wiper vial...[/b]")
			Npc:says(_"[b]WARNING: PATIENT ID #1337 'Colemak' HAS BEEN SUCCESSFULLY UNCRYONIZED![/b]")
			Npc:says(_"Current Memory Loss: 0.00%%") -- oO He didn't had his memory wipped! Of course *someone* disrupted the "memory wipper" fluids...

			-- hostilize the cryo keepers
			Act2SecurityChief:set_faction("rr")
			Act2ProgrammingChief:set_faction("rr")

			--teletransport them, with delay
			play_sound("effects/new_teleporter_sound.ogg")
			delay_game(0.5)
			Act2SecurityChief:teleport("75-KillTuxPos01", "Act2SecurityChief")
			delay_game(0.5)
			Act2ProgrammingChief:teleport("75-KillTuxPos02", "Act2ProgrammingChief")
			delay_game(1.0)

			-- Colemak kill them. This is unexplained, but the game is terminated... for now...
			Act2SecurityChief:drop_dead()
			delay_game(0.5)
			Act2ProgrammingChief:drop_dead()
			delay_game(0.5)

			 -- Sorry, nothing more to see here! Finishes the game.
			display_big_message(_"--- Continues ---")
			win_game()
			end_dialog()
		end,
	},
	{
		id = "node99",
		--; TRANSLATORS: command, user lowercase here
		text = _"logout",
		code = function()
			Npc:says(_"[b]Goodbye[/b]")
			play_sound("effects/Menu_Item_Selected_Sound_1.ogg")
			end_dialog()
		end,
	},
}
