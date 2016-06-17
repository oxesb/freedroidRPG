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
--[[WIKI
PERSONALITY = { "Robotic" },
PURPOSE = "$$NAME$$ is part of Act2. His only purpose is to dump the player on Act 2 lands by Spencer orders."
WIKI]]--

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {
	EveryTime = function()
		if (Act2_opengate) then
				Npc:says(_"Hello linarian. Do you want to go to the Resorts? Please note that once you go you won't be able to come back.")
				show("node0", "node99")
		else
			Npc:says(_"[b]INTRUDER,[/b] leave the premises at once. You are not allowed here.")
			end_dialog()
		end
	end,

	{
		id = "node0",
		text = _"Yes. To the next act!",
		code = function()
			Npc:says(_"Good luck on the unknown.")
			delay_game(0.2) -- for effect
			Tux:teleport("Act2StartGameSquare") -- TODO: This must be included on endact_1() function and called here...
			end_dialog()
		end,
	},
	{
		id = "node99",
		text = _"Not yet. But I'll be back.",
		code = function()
			end_dialog()
		end,
	},
}
