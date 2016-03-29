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
PURPOSE = "$$NAME$$ is part of Act2. He explains the player why he cannot cross the Act 2 gates, and open then when it's time."
WIKI]]--

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {
	EveryTime = function()
		Npc:says(_"No one shall enter or leave these gates.")
		show("node0", "node99")
	end,

	{
		id = "node0",
		text = _"Why I can't cross this gate?",
		code = function()
			Npc:says(_"The droid out there are too dangerous.")
			if (HF_FirmwareUpdateServer_uploaded_faulty_firmware_update) then
				Tux:says(_"But I disabled all the bots!")
				Npc:says(_"Incorrect. The RR Factory is still active.")
				Tux:says(_"Then let me disable the RR! I've done it before!", "NO_WAIT")
				Npc:says(_"No. You do not have proper authorization.")
				if (Act2_opengate) then
					Tux:says(_"But I have authorization!")
					Npc:says(_"Can you proof it?")
					Tux:says(_"Erm, no. Spencer said he authorized me to go there, but-", "NO_WAIT")
					Npc:says(_"Don't worry with details, I trust you. Gate is now open. Try to don't die.")
					Tux:says(_"Whoa, thank you!")
					change_obstacle_state("Act2MainAcessGate", "opened") -- Open main gate, all others are locked down.

				else
					Tux:says(_"And where can I obtain such authorization?", "NO_WAIT")
					Npc:says(_"With a blue, small-sized card with the following inscription: Arcane Lore") -- See graphics/item/script/portrait_0010.jpg --> Replace ugly LUA Token.
					Tux:says(_"You didn't answered my question.", "NO_WAIT") -- Spencer haves it.
					Npc:says(_"Don't worry, I'm sure you'll find it when it's time.", "NO_WAIT")
				end
			else
				Tux:says(_"Just how dangerous?")
				Npc:says(_"Extremely dangerous.")
			end
			Npc:says(_"Please excuse me, I have a gate to keep.")
			end_dialog()
		end,
	},
	{
		id = "node99",
		text = _"...",
		code = function()
			end_dialog()
		end,
	},
}
