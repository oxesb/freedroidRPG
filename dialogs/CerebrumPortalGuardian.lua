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
BACKSTORY = "$$NAME$$ was part of Act 2. Asks for some addons in exchange of more useful ones and some experience."
WIKI]]--

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {
	FirstTime = function()
		Npc:says(_"Intruder detected!! Identifying unknown object...")
		Npc:says(_"...")
		Npc:says(_"...")
		Npc:says(_"Object identification failed. Attack!")
		play_sound("effects/Influencer_Scream_Sound_0.ogg")
		Tux:says(_"Hey! That hurts...")
		Npc:says(_"Intruder responds... Analysing... Intruder is an life form. Aborting attack.")
		Npc:says(_"Excuse me for attacking you.")
		Tux:says(_"Why did you attack me?")
		Npc:says(_"[b]ORDER #4[/b]: If intruder can not be identified, it must be terminated.")
		Npc:says(_"Sorry, it was my fault, duck.")
		Npc:says(_"Can I somehow help you?")
		Tux:says(_"No, ugh... I am still alive, don't worry...")
		Npc:says(_"Good to hear that.")
		Npc:says(_"My identificator is... [b]NO ID FOUND[/b]... Just call me Cerebrum. And who are you?")
		Npc:set_name(_"Cerebrum - Portal Guardian")
			--; TRANSLATORS: %s = Tux:get_player_name()
		Tux:says(_"My name is %s.", Tux:get_player_name())
			--; TRANSLATORS: %s = Tux:get_player_name()
		Npc:says(_"Hello %s!", Tux:get_player_name())
		show("node0", "node1", "node3", "node5", "node6")
		Npc:set_rush_tux(false)
	end,

	EveryTime = function()
		if (Tux:has_met("CerebrumPortalGuardian")) then
			Tux:says_random(_"Hello, sentient bot!",
					_"Hi!",
					_"Hi Cerebrum!")
			Npc:says_random(_"Welcome back.",
					_"Hostile droids didn't kill you? That's good. Hi then!",
					_"Hello duck!")
            show("node1")
		end
		if ((Tux:has_quest("Bot in need")) and 
		   (not Tux:done_quest("Bot in need")) and 
		   (Tux:has_item_backpack("Tinfoil patch")) and 
		   (Tux:has_item_backpack("Laser sight")) and 
		   (Tux:has_item_backpack("Peltier element"))) then
			show("node4")
		else
			hide("node4")
		end
		show("node99")
	end,

	{
		id = "node0",
		text = _"Why did you call me a duck?",
		echo_text = false,
		code = function()
			Tux:says(_"Why did you call me a duck? I'm not a duck! I'm a Linarian!")
			Npc:says(_"I know. But calling you a duck is funny!")
			hide("node0")
		end,
	},

	{
		id = "node1",
		text = _"What are you doing here?",
		code = function()
			Npc:says(_"[b]PRIMARY OBJECTIVES[/b]:", "NO_WAIT")
			Npc:says(_"1) Guard the portal;", "NO_WAIT")
			Npc:says(_"2) Configure computers;", "NO_WAIT")
			Npc:says(_"3) Watch the reactor's state.")
			Tux:says(_"Do you mean this big reactor?")
			Npc:says(_"Yeah. You've guessed.")
			Tux:says(_"And is it stable?")
			if (not Tux:done_quest("Bot in need")) then
				Npc:says(_"Not really, but don't worry. Just don't touch any buttons.")
			else 
				Npc:says(_"Yeah! Thank you for Peltier Element! It is cooling it perfectly!")
			end
			Npc:says(_"Ah! I forgot to tell you that I am also reading books.")
			Tux:says(_"You are reading books? Hmmm...")
			show("node2") hide("node1")
		end,
	},

	{
		id = "node2",
		text = _"Are you sentient?",
		code = function()
			Npc:says(_"Maybe you will find answer in my description. Should I show it to you?")
			Tux:says(_"Yes please.")
			Npc:says(_"[b]LOADING DESCRIPTION[/b]")
			Npc:says(_"...")
			Npc:says(_"...")
			Npc:says(_"Here is it:")
			Npc:says(_"Experimental command cyborg. Fitted with a new type of brain. Mounted on a security droid anti-grav unit for convenience.")
			hide("node2")
		end,
	},

	{
		id = "node3",
		text = _"How is it going?",
		code = function()
			Npc:says(_"[b]STATUS[/b]: Bad. I am in need of many addons. My targetting system is outaded, armor is too, and reactor is unstable...")
			Tux:says(_"Do you mean this big reactor?")
			Npc:says(_"Yes. That one.")
			Tux:says(_"Maybe I can help you somehow? Which addons do you need?")
			Npc:says(_"I need these ones:","NO_WAIT")
			Npc:says(_"[b]Laser Sight[/b] - it'll make my targetting much easier;","NO_WAIT")
			Npc:says(_"[b]Tinfoil Patch[/b] - it'll raise my armor;","NO_WAIT")
			Npc:says(_"[b]Peltier Element[/b] - I'll use this one for stabilizing the reactor's cooling.")
			--; TRANSLATORS: %s = Tux:get_player_name()
			Npc:says(_"So will you help me %s? I will give you some required components.", Tux:get_player_name())
			Tux:says(_"Of course I will help.")
			Npc:says(_"Thank you, duck.")
			if (difficulty() == "easy") then
				Tux:add_item("Plasma Transistor", 15)
				Tux:add_item("Entropy Inverter", 15)
				Tux:add_item("Antimatter-Matter Converter", 3)
				Tux:add_item("Tachyon Condensator", 10)
			elseif (difficulty() == "normal") then
				Tux:add_item("Plasma Transistor", 10)
				Tux:add_item("Entropy Inverter", 10)
				Tux:add_item("Antimatter-Matter Converter", 2)
				Tux:add_item("Tachyon Condensator", 10)
			else
				Tux:add_item("Plasma Transistor", 5)
				Tux:add_item("Entropy Inverter", 5)
				Tux:add_item("Antimatter-Matter Converter", 1)
				Tux:add_item("Tachyon Condensator", 5)
			end
			Tux:add_quest("Bot in need", _"999 Cerebrum bot asked me for Laser Sight, Tinfoil Patch and Peltier Element. I want to help my new friend so I will bring these addons him.")
			hide("node3")
		end,
	},

	{
		id = "node4",
		text = _"I brought your addons. It was very long way!",
		code = function()
			Npc:says(_"Thank you! I was fearing that you took my components and happily made addons for yourself.")
			Npc:says(_"For that I will award you! While you were searching for addons I found two Brain Stimulators. You may have them.")
			Npc:says(_"Brain Stimulators are very special addons. They speed up learning making you gain more experience from killing bots. But don't use them too often! I have heard that they can burn human's brain!")
			Tux:says(_"Don't worry, I have Linarian brain.")
			Tux:del_item("Laser sight")
			Tux:del_item("Tinfoil patch")
			Tux:del_item("Peltier element")
			Tux:add_item("Brain stimulator")
			Tux:add_item("Brain stimulator")
			Tux:end_quest("Bot in need", _"I brought addons to Cerebrum. He gave me pair of Brain Stimulators as reward. They will be useful.")
			hide("node4")
		end,
	},

	{
		id = "node5",
		text = _"What are these two bots?",
		code = function()
			Npc:says(_"629 guards the gate, and 476 watches for the cables state.")
			Tux:says(_"Why does he watch the cables? Is there something wrong with them?")
			Npc:says(_"No. It just has an error or something. Just ignore that.")
			hide("node5")
		end,
	},

	{
		id = "node6",
		text = _"Why are you not hostile?",
		code = function()
			Npc:says(_"I was not active during the day, when all bots went insane.")
			Npc:says(_"I was activated just few weeks ago, so I could survive.")
			Tux:says(_"Who activated you?")
			Npc:says(_"Who knows... The archive that contains this information can only be accessed through network connection, but it's broken. Sorry duck, I don't know.")
			hide("node6")
		end,
	},

	{
		id = "node99",
		text = _"See you later.",
		code = function()
			Npc:says_random(_"Goodbye.",
					_"See you later duck!",
					_"If hostile droids won't kill you, then of course we'll see!",
					_"Good luck, funny duck.")
			end_dialog()
		end,
	},
}
