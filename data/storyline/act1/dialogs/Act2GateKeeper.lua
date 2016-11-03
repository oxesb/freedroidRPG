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
PERSONALITY = { "Novice" },
PURPOSE = "$$NAME$$ is part of Act2. He is an unexperienced member who Spencer told to transport player to RR Resorts but ends up crashing the spaceship. He teaches tux to never try to pilot a spaceship when drunk."
WIKI]]--

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {
	EveryTime = function()
		if (Act2_opengate) then
				-- Maybe this is the first time you've met him. In such case, we must fix some storyline.
				if (not Tux:has_met("Act2GateKeeper")) then
					Npc:says(_"I found a drink near the cockpit, it looked OK so I drank from it.")
					Tux:says(_"Hey, are you sure you're not drunk nor anything?")
					Npc:says(_"Do not worry: There's nothing which could possibly go wrong!")
				end
				Npc:says(_"Sooo, Spencer told me to transport you to RR Resorts. Tell me when you're ready.")
				show("node0", "node99")
		elseif (HF_FirmwareUpdateServer_uploaded_faulty_firmware_update) then
			Npc:says(_"Heya, congratulations!")
			if (Tux:has_met("Act2GateKeeper")) then
				Tux:says(_"You're still alive, it seems.")
				Npc:says(_"Yes, I told you, there was nothing which could possibly go wrong!")
				Tux:says(_"So, what are you doing here?")
			else
				Tux:says(_"Thanks. So, what's up?")
				Npc:says(_"Oh nothing. I went to the MS bar, found a drink near some blood strains. It looked OK so I drank it.")
				Tux:says(_"Hm...")
				Npc:says(_"Oh, but do not worry! Nothing could possibly go wrong. Also, I have a special mission!")
			end
			Npc:says(_"Right now Spencer instructed me to inspect the Landing Zone. See if I find any ships.")
			Tux:says(_"I'll let you do your work alone. Just, eh, be sure to don't drink anything you find inside the ship, ok?")
			Npc:says(_"Ok, do not worry with it!") -- He'll drink anyway...
			end_dialog()
		else
			--; TRANSLATORS: %s = Tux:get_player_name()
			Npc:says(_"Hello, %s. Spencer told me to try to assist you on the Hell Fortress, but I'm scared of the bots.", Tux:get_player_name())
			Tux:says(_"How come they haven't attacked you yet?")
			Npc:says(_"Hmm, I'm not sure. Maybe it had something to do with that drink I found abandoned on the bar near some blood strains?")
			Tux:says(_"You go out drinking everything you see? Really??")
			Npc:says(_"Sure, what could possibly go wrong?")
			Tux:says(_"...")
			Tux:says(_"This way you'll die soon, young boy.") -- Tux refuses his help: too inexperienced.
			end_dialog()
		end
	end,

	{
		id = "node0",
		text = _"Yes. To the next act!",
		code = function()
			Npc:says(_"Good luck on the unknown.") -- TODO: This part is to be reworked.
			delay_game(0.2) -- for effect
			Tux:teleport("Act2StartGameSquare") -- TODO: This must be included on endact_1() function and called here...
			revive_faction("ms") -- fixme: This is to be removed.
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
