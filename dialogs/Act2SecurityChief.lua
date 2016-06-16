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
PERSONALITY = { "Unfriendly" },
PURPOSE = "$$NAME$$ helps improve Tux\'s skill. He is very unfriendly to the player."
WIKI]]--

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {
	FirstTime = function()
		show("node0", "node1", "node2")
	end,

	EveryTime = function()

		Npc:says(_"A linarian. Perfect, my bad day just got worse.")

		show("node99")

	end,

	{
		id = "node0",
		text = _"Hi! I'm new here.",
		code = function()
			Npc:says(_"Good. Just select an empty hibernation chamber and go sleep.")
			Npc:says(_"I really don't care to whatever you do here, I wish I was just sleeping like the others which knows too much, but the guy at my side had to have someone to help him.")
			Npc:says(_"I know nothing. So if you're thinking in doing me silly questions, forget it, I won't answer.") -- Maybe later allow the player to make questions and enrage this NPC...
			hide("node0")
		end,
	},
	{
		id = "node1",
		text = _"Could you teach me how to fight?",
		code = function()
				Npc:says(_"Sure. I'm bored anyway.", "NO_WAIT")
				Npc:says(_"But for a price. A thousand circuits should be enough. A training point as well, or you won't remember what I'm about to teach you.")
				Npc:says(_"Also, I don't care if you get hurt.")
				hide("node1") show("node12")
		end,
	},
	{
		id = "node2",
		text = _"Do you have a name?",
		code = function()
			Npc:says(_"Who cares? I long forgot my name. All I know is the work I must do.")
			Npc:says(_"Call me 'pitiable man'. Because this is what I am.")
			Npc:set_name("Pitiable Man - Ex-Security Officer")
			hide("node2")
		end,
	},
	{
		id = "node12",
		text = _"Yes, teach me how to smash concrete with my bare hands! (costs 1000 valuable circuits, 1 training points)",
		code = function()
			if (Tux:get_skill("melee") >= 1) then
				if (Tux:train_skill(1000, 1, "melee")) then
					Npc:says(_"Let us begin then.")
					Npc:says(_"Please equip this screwdriver. I don't want to get hurt.") -- Maybe give tux a screwdriver latter, as a token?
					Npc:says(_"Also, put all your possessions near the tree, no cheating.")
					Tux:says(_"Done. What I do now? Should I try to hit you?")
					Npc:says(_"No. Nothing so dangerous.")
					Npc:says(_"I'll spawn a 883 Droid to you fight with.")
					Tux:says(_"Are you crazy?? With a screwdriver! What about 'dangerous'??")
					Npc:says(_"Nothing dangerous for [b]my[/b] health, of course. Good luck! Don't die!")
					Tux:del_health(100) -- Do not move from here, sound effect reasons.
					Npc:says(_"[b]...some painful time latter...[/b]")
					Tux:says(_"Ugh. Done, the 883 is dead, and surprisingly I'm alive. Who changed that thing to see me while invisible??")
					Npc:says(_"I did the best 883 I could. To defeat it with a screwdriver, you must be very special. You are my apprentice now.")
					Npc:says(_"However, this is enough training for now. I don't have more bots to you do insane fights with.")
					hide("node12")
				else
					if (Tux:get_gold() >= 1000) then
						Npc:says(_"You don't have enough experience. I can't teach you anything more right now.")
						Npc:says(_"It might be hard to find new bots, but there is nothing I can do for you right now.")
					else
						Npc:says(_"You can be poor. But you will be poor [b]and[/b] an untrained Linarian.")
					end
				end
			else
				Npc:says(_"Hey, wake up! if you challenge a bee, you'll die. You must have learned some melee fighting earlier in order to understand what I'm about to tell you.")
			end
		end,
	},
	{
		id = "node99",
		text = _"See you later.",
		code = function()
			Npc:says_random(_"Finally, peace.",
								_"Leaving me at peace, finally.")
			end_dialog()
		end,
	}
}
