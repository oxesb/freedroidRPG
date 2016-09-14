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
	FirstTime = function()
		Yadda_times=0
	end,

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

		-- Yadda's wise advices
		Yadda_times=Yadda_times + 1
		if (Yadda_times==1) then
			Npc:says(_"Come and seek my lightly advices, and you'll find out truth and light.")
		elseif (Yadda_times==7) then
			Npc:says(_"Persistence is the way to perfection.")
		elseif (Yadda_times==13) then
			--; TRANSLATORS: 1337 => 13 times to get this message, 37 times to get secret. (and also a word: leet, from leet hacker)
			Npc:says(_"I have secrets, but you need to be very 1337 to understand it.")
			Npc:says(_"But this is just the 13rd time you ask for my advice. So, bad luck.")
		elseif (Yadda_times==37) then
			Npc:says(_"Hey, did you knew this is the 37th time you ask for my enlightenment? I'll teach you a secret. There is a mini boss on this area. Might be worth checking out.")
			show("C64gate")
		elseif (Yadda_times < 20) then
			Npc:says_random(_"In matters of destroying bots, style, not brute force, is the vital thing.", --after Oscar Wilde's The Importance of Being Earnest (1895)
							_"Any fool can bust a bot open; the art consists in knowing how to exploit it.", --after quote attributed to Frank Wedekind
							_"The knife is without measure, for if you hit a bot right, out will come treasure.",
							_"When the linarian fall, the bots reigns.",
							_"Yadda Yadda Yadda.",
							_"Basically, it's just a jump to the left, and a step to the right... You see?") --after "Time Warp" lyrics from Rocky Horror Show (1973)
		elseif (Yadda_times < 40) then
			Npc:says_random(_"No one knows what the future awaits. Except Master Yadda.",
							_"Yadda Yadda Yadda.",
							_"The secret of the sure victory is siding with Master Yadda.",
							_"Kill bots first, ask questions later.",
							_"There was the Experimental Alpha Class. If you meet one, your death is close.", -- Bots 0xx, relies on special skills.
							_"If you can think, you're sentient. This won't make you a lifeform though.",
							_"Lamps are nice. Always carry one with you. You never know when you'll need them.",
							_"Bots will aim whatever is in front of them. Which usually is you.")
		else
			Npc:says(_"You're very dedicated, seeking my advice, and wise words that much. But I have nothing else to tell you. Besides that: ")
			--; TRANSLATORS: %d = number of times talked to Yadda
			Npc:says(_"You may ask me %d times and I still won't have anything new to teach you.", Yadda_times)
		end


		show("node99")
	end,

	{
		id = "C64gate",
		text = _"I heard there was a dangerous bot which needs killing. Where is it?",
		code = function()
			Npc:says(_"Indeed, an evil bot there is, a virus from the original Paradroid game threatens this lands.", "NO_WAIT")
			Npc:says(_"If you defeat it, the fictional survivors which aren't here because the Great Assault would be very grateful. You will also gain a nice amount of experience, and who knows what it will drop?", "NO_WAIT")
			Tux:says(_"Seems interesting. Where can I find it?")
			Npc:says(_"Just follow the water, young apprentice of Yaddawan. Only the ones with enough faith can walk over water and save the world!")
			del_obstacle("Act2-ArtificialPassage-1")
			del_obstacle("Act2-ArtificialPassage-2")
			hide("C64gate")
		end,
	},
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
