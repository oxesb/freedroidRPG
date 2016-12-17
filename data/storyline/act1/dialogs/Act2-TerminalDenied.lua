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
			cli_says(_"Access Denied.")
		else
			Npc:says(_"[b]No instructions to load this terminal.[/b]")
			Tux:says(_"help", "NO_WAIT")
			Npc:says(_"This terminal needs instructions to be loaded. Instructions can be requested at last floor of factory north from town.")
			Npc:says(_"[b]WARNING:[/b] People capable of issuing authorization are all dead, killed by at weakest Battle droids.") 
			-- Not everyone survives: Look for dead bodies. Battle necessary: Battle Droids. Auth -- You won't find it at last floor.
		end
		end_dialog()
		show("node99") -- Better safe than sorry.
	end,

	{
		id = "node99",
		--; TRANSLATORS: command, use lowercase here
		text = _"logout",
		code = function()
			cli_says(_"Goodbye")
			play_sound("effects/Menu_Item_Selected_Sound_1.ogg")
			end_dialog()
		end,
	},
}
