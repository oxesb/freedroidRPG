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
PERSONALITY = { "Brilliant" },
PURPOSE = "$$NAME$$ will help to improve Tux\'s abilities. Will also give Tux some more background on Act2.",
BACKSTORY = "$$NAME$$ is part of Act2. He is an ex-Megasys Programming Chief Officer, which can improve Tux's programming. However he requires previous knowledge which can be obtained with Soreson."
WIKI]]--

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {
	FirstTime = function()
		Tux:says(_"Um... Hello?")
		Npc:says(_"What do you want, linarian?")
		Tux:says(_"Isn't it strange? I mean, have you met a linarian before?")
		Npc:says(_"[b]*chuckles*[/b] No, not at all. When your boss is a linarian, it's hard to miss one.", "NO_WAIT")
		Tux:says(_"Boss?")
		Npc:says(_"He owned MegaSystem. He kept his face hidden all the time, but I'm pretty sure he was a Linarian.", "NO_WAIT")
		Npc:says(_"He left me and that other guy here to take care of this facility. Thankfully there are delicious military food here.")
		Tux:says(_"But the military food is disguising!")
		Npc:says(_"Really? I like it.")
		Npc:says(_"Anyway, it's a pleasure to meet you, Linarian. I don't have a name. Neither have you.")
		show("node20", "node50")
	end,

	EveryTime = function()
		Npc:says(_"I hope you enjoy being here, Linarian.")
		show("node99")
	end,

	{
		id = "node20",
		text = _"But I have a name. You must have one too.",
		code = function()
			Npc:says(_"Listen, Linarian:")
			Npc:says(_"When you know the deep codes and see light of all chips...") -- quote from Soreson the coder.
			Npc:says(_"When your boss is the dark shadow of dread and hate on a tiny computer screen...")
			Npc:says(_"A name is something you don't bother in remembering. Call me CPO, a short for Chief Programming Officer.")
			Tux:says(_"Can you tell me a little more about this linarian? The president, I mean.") -- This should be secret, it's just here for outlining purposes.
			Npc:says(_"No, I can't. He wouldn't leave me alive if he couldn't kill me, and I would rather stay alive.")
			Tux:says(_"Which remembers, why are you alive, if everyone else is dead?")
			Npc:says(_"There are no dead here. Only sleepers. Well, except for the few ones killed by the bots, some of the staff managed to survive...")
			Tux:says(_"This is interesting. So basically the MS was ruled by a Linarian.")
			Npc:says(_"Yes.")
			Tux:says(_"Besides, you take care of the facility...")
			Npc:says(_"Yes. I must be sure all the cryonized on upper floors are losing memory, and the ones on underground won't wake up and provoke havoc by not remembering anything.")
			Npc:says(_"Which also means that if you do anything funny, I'll erase you from existence.")
			Npc:set_name("CPO - ex-Programmer Officer")
			hide("node20")
		end,
	},
	{
		id = "node50",
		text = _"Can you help me to improve computer skills?",
		code = function()
			if (Tux:get_skill("programming") < 1) then
				Npc:says(_"No. Man, you don't know the basic. It's hard to teach a linarian, you know?")
				Npc:says(_"Either you learn some programming first, or you get out of my face!")
			else
				Npc:says(_"Hum. You look like someone who will have no problem in learning programming.")
				Npc:says(_"I charge a lot, though. 10,000 circuits, and one training point. It's a bargain if you think well, you surely is rich. And besides, coding can overheat you, take care!")
				hide("node50") show("node60")
			end
		end,
	},
	{
		id = "node60",
		text = _"Sure. Make me your apprentice.",
		code = function()
				if (Tux:train_skill(10000, 1, "programming")) then -- Not much AP because Tux probably is a high-level linarian by now.
					Tux:heat(110) -- Ping-pong is the hardest game ever made. It only loses to Nethack.
					Npc:says(_"Let us begin then.")
					Npc:says(_"Come closer...")
					Npc:says(_"Do you see this screen?")
					Tux:says(_"Yes. I see a ball going up and down, hitting two moving walls.")
					Npc:says(_"This is called ping-pong. It's a very nice game.")
					Tux:says(_"Seems simple.")
					Npc:says(_"Yes. but it's much more complex than you think. It's not the code I'm talking about, or even the ball going up and down. The important is not the program on itself. It's the art of programming.")
					Npc:says(_"Once you understand how the bytes flows, how they sum and they bitwise, how to use the Tachyons and to transform bits in bytes and bytes in bits, you'll become a true programming master.")
					Tux:says(_"What should I do?")
					Npc:says(_"Now, look at source code, and replicate the ping pong, but not with the simple AI I included. I want a sentient ping pong field, with air friction, gravity, virtual reality, I want also to you consider the solar effect over tachyon particles and the E-particle emission on field due to atrict with two antigrav rackets.") -- Are tachyons the way to contact Linarius?
					Tux:says(_"Yes sir! Will be done sir!")
					Npc:says(_"[b]...some time later...[/b]")
					Npc:says(_"Very good. The E-particle emission is a little off, but this can be forgiven.")
					Npc:says(_"You're good to go.")
					Npc:says(_"By the way, you've learnt very fast the exponential overflow over E-particles. You have an unnatural talent to it, even bigger than MS President. Did you sold your soul by any chance?")
					Npc:says(_"This is enough training for today.")
					hide("node60")
				else
					if (Tux:get_gold() >= 10000) then
						Npc:says(_"You don't have enough experience. I can't teach you anything more right now.")
						Npc:says(_"It might be hard to find new bots, but there is nothing I can do for you right now.")
					else
						Npc:says(_"You must cover the risk I'll be taking in teaching you a secret from MegaSys. After all, the right to program was copyrighted by them, so you must pay me if I ever need a lawyer.")
					end
				end
		end,
	},
	{
		id = "node99",
		text = _"I'll take a look on the fine resort of yours. Please excuse me.",
		code = function()
			Npc:says(_"Just don't touch anything and you'll be fine.")
			end_dialog()
		end,
	},
}
