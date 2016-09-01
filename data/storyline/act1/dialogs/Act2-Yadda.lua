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
PERSONALITY = { "Arrogant", "Wise" },
PURPOSE = "$$NAME$$ is a strange monk who follows the path of light. In future may require good Karma to engage conversation with. Gives quest to slay evil viruses. Give Source Book of Light, Light Saber and Light Staff. Sells player Laser Power Pack and small plasma grenades. Is a pacifist with a weak resolve, thus why he sells Tux grenades and etc. May summon droids to help Tux in certain quests."
WIKI]]--

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {
	EveryTime = function()
		-- People say I am the wisest man alive on the world, but I think I am the best on the universe!
		Npc:says(_"Greetings, I'm Master Yadda. I teach the path of light.")

		-- Those two conditions make Yadda hostile and wanting to kill tux at once.
		if (Tux:has_item_equipped("Nobody's edge")) then
			next("evil")
		elseif (Tux:has_item_backpack("Pandora's Cube")) then
			next("pandora")
		end

		-- Show player bad options, like showing Nobody's Edge and such.
		if (Tux:has_item_backpack("Nobody's edge")) then
			show("evil")
		end

		show("node99")
	end,

	{
		id = "evil",
		text = _"I have this nice weapon here, want to take a look?",
		code = function()
			Npc:says(_"Nobody's Edge! How do you dare to bring such evil pulsing weapon to those lands!!", "NO_WAIT")
			--; TRANSLATORS: woza futhi zisize mina → Come and aid me (Zulu, Google Translated)
			Npc:says(_"Woza futhi zisize mina, minions! Let's kill this... this... heretic!")
			Npc:set_faction("ms")
			create_droid("Act2-YaddaRef1", "GUB", "ms", "AfterTakeover", "radar")
			create_droid("Act2-YaddaRef2", "GUA", "ms", "AfterTakeover", "radar")
			end_dialog()
		end,
	},
	{
		id = "pandora",
		text = _"I found this small cube on a desert.",
		code = function()
			Npc:says(_"The Pandora Box! How do you dare to bring doom to those lands!!", "NO_WAIT")
			--; TRANSLATORS: woza futhi zisize mina → Come and aid me (Zulu, Google Translated)
			Npc:says(_"Woza futhi zisize mina, minions! Let's kill this doombringer!")
			Npc:set_faction("ms")
			create_droid("Act2-YaddaRef1", "GUB", "ms", "AfterTakeover", "radar")
			create_droid("Act2-YaddaRef2", "GUA", "ms", "AfterTakeover", "radar")
			end_dialog()
		end,
	},
	{
		id = "node99",
		text = _"Erm... That was very wise, thank you. I'll think about what you said.",
		code = function()
			Npc:says_random(_"Stay on the path of light.",
							_"I'm pure wisdom. If you follow my words you'll have success.",
							_"Wisdom is more worth than rubies. But if it's my wisdom, it'll be more worth than the whole universe!",
							_"Take as much time you need to reflect on what I said, and you'll become wise. Because I am the wisest man on this universe.")
			end_dialog()
		end,
	},
}
