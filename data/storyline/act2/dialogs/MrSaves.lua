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
PERSONALITY = { "Weird", "Mysterious", "Riddles", "Omniscient?", "Crypt" },
PURPOSE = "$$NAME$$ warns Tux in advance about some events which are about to happen - if Tux can understand his weird analogies, anyway. Otherwise, he's just an useless character which spawn between Acts and player will wonder why he does that and what he's talking about.",
BACKSTORY = "$$NAME$$ is an anomaly. No one knows where he comes from, why he is here, or even if he should be in FreedroidRPG to begin with. He was not added by FreedroidRPG staff, and seems pretty happy in playing mind games with Tux. But, who is this man, who all he does all day is staring the sky? He might be holding a big secret, but you'll never know. All you can do for now is fry your brain, trying to understand why he just stares the sky, without doing anything, as if everything is going as it should be going... including you."
WIKI]]--

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

-- Before we begin my script, I want to apologize. You mortals will never understand the joy of winning my mental games, so, please, try harder.
-- As we're at that, please do not read the comments from now on, they explain how my riddles work, and bring the answers. They're consistent.
-- I talk using sky-related things. I start with a hello in three different forms (my dear, hi there and oh a linarian) and the past, then I explain the current situation, and I finally excuse myself in the same way, with a warning about the future.
-- Please keep this logical sequence, and HAVE FUN! Signed, Mr. Saves, the Watcher Of Stories.
return {
	FirstTime = function()
		Saves_times=0
	end,

	EveryTime = function()
		Saves_times=Saves_times + 1

		-- I like to salute people correctly, so get the correct game time
		local day, hour, minute = game_date()
		if (hour >= 18 or hour < 6) then
			Npc:says(_"Good evening!")
		elseif (hour < 12 and hour >= 6) then
			Npc:says(_"Good morning!")
		elseif (hour < 18 and hour >= 12) then
			Npc:says(_"Good afternoon!")
		end

		if (not has_met("MrSaves")) then
			Tux:says(_"...Hello.")
			Npc:says(_"I am Mr. Saves, the Watcher of Stories!")
			Npc:set_name(_"Mr. Saves, Watcher of Stories")
			Npc:says(_"The weather is grim again, don't you think? Ah, I love mental games. All the better when no one can understand them.")
			--Tux:says(_"...No one asked.")
			Tux:says(_"Uhm... great, I think?")
			show("weird", "alive", "lazy", "node99")
		elseif (Saves_times == 8) then
			Npc:says(_"You don't have anything better to do, do you?")
			Npc:says(_"I repeated the same thing seven times and you still come to bother me.")
			Npc:says(_"Seven is a number of luck, though. Here, take this program update and don't show up again.")
			Tux:improve_program("Animal Magnetism")
			Npc:says(_"[b]Animal Magnetism skill updated![/b]")
			Npc:says(_"If you do, I'll repeat myself until I manage to bore you out.")
			Tux:says(_"Uhm... Thanks?")
			Npc:says(_"Now get going. You have more important things to do.")
			end_dialog()
		else
			next("act2a")
		end
	end,
	-- The "actXY" or "actX" nodes are the "not-understandable mental games" from Mr. Saves. Please do not read the comments!
	{
		id = "act1",
		text = _"Exit",
		echo_text = false,
		code = function()
			-- Mr. Saves is not talking about stars falling - he is talking about Linarians going against the LHC to help humanity - namely, Tux
			Npc:says_random(_"Hello, my dear. I've just saw a falling star the other day.",
							_"Hi there. Do you like star shower? But, so far, only one has fallen...",
							_"Oh, if it isn't a linarian! The sky is silent, only one star has fallen thus far.")
			-- Mr. Saves is saying that when Tux awoke (or rather, went against MS) he impacted the humans (clouds).
			-- He hopes they rise in revolt (rain) soon.
			Npc:says(_"During it's fall, it pierced the clouds. I hope it may rain soon.")
			-- He is saying to be aware of Linarians (stars) in Earth, which did not felt.
			Npc:says(_"Sorry, I would love to talk more, however, it's not only the stars in sky I must watch. There are stars in Earth too, did you knew that?")
			next('node99')
		end,
	},
	{
		id = "act2a",
		text = _"Exit",
		echo_text = false,
		code = function()
			-- Mr. Saves is not talking about weather and rain, but about the great assault (weather) and humans rising in revolt (rain)
			Npc:says_random(_"Hello, my dear. Did you noticed any rain during your travels?",
							_"Hi there. Do you think it'll rain today? It haven't rained well lately.",
							_"Oh, if it isn't a linarian! The weather is almost perfect, except it almost never rains.")
			-- During the start of the Great Assault there was heavy human resistance, but now they're getting used to it.
			-- Mr. Saves is currently checking for surviving humans which could rise in revolt (clouds).
			Npc:says(_"It rained a lot during the first days, but now it's very dry. Actually, I'm just staring at the clouds.")
			-- Umbrella can block the rain. They're MegaSys contigencies being prepared against a revolt, and Tux is now warned about that.
			Npc:says(_"Sorry, I would love to talk more, however, I can feel umbrellas being open. Be prepared, Linarian.")
			next('node99')
		end,
	},
	{
		id = "act2b",
		text = _"Exit",
		echo_text = false,
		code = function()
			-- Mr. Saves is not talking about the sun, he is talking about Dvorak and the High Council.
			Npc:says_random(_"Hello, my dear. The sun is brighting more every day, to burn the humans.",
							_"Hi there. If you're soaking, it's because the sun is more intense, to burn us all.",
							_"Oh, if it isn't a linarian! The sun is pretty bright, does it want to burn humans?")
			-- If nothing is done about the Linarian High Council, all humans will die before they can revolt (so, bots are not your only worry)
			Npc:says(_"If things are left unchecked, the clouds will vanish before it rain.")
			-- He is saying that he saw Dvorak, who could block the High Council influence (sunlight). He also leaves out that Dvorak actually needs Tux, thus, it 'orbited' around the star which felt the other day (Tux).
			Npc:says(_"Sorry, I would love to talk more, however, I think I've seen a crypt planet, which could block the sunlight. Did this planet orbited around the star which felt on the other day?")
			next('node99')
		end,
	},
	-- The first time you talk to Mr. Saves, you are presented the opportunity to do questions (most IRC-quotes based)
	{
		id = "alive",
		text = _"Why are you alive? Why are the bots not attacking you?",
		code = function()
			Npc:says(_"I don't exist under normal circumstances.") -- IRC quote
			Tux:says(_"Makes sense, you cannot attack something which doesn't exist.")
			Tux:says(_"...No wait. It doesn't. If you don't exist, I can't talk to you either!")
			Npc:says(_"Well, with strange aeons, even Mr. Saves may die.") -- IRC quote from quote “with strange aeons, even death may die”
			Tux:says(_"...You know what? You're weird. I bet the bots are just afraid of getting close to you.")
			hide('alive')
		end,
	},
	{
		id = "weird",
		text = _"You're weird.",
		code = function()
			Npc:says(_"That's my purpose in life.") -- IRC quote
			Tux:says(_"That only makes you weirder!")
			Npc:says(_"If you think too much about it, you'll end up overheating.")
			Npc:says(_"I advice you to drop this subject.")
			hide('weird')
		end,
	},
	{
		id = "lazy",
		text = _"What are you doing?",
		code = function()
			Npc:says(_"Volunteer work.")
			Tux:says(_"I don't see you working.")
			Npc:says(_"Well, that's because we volunteer for things exactly so that we can be lazy on them.") -- IRC quote
			Tux:says("...")
			hide('lazy')
		end,
	},
	{
		id = "node99",
		text = _"Er, uhm... Good bye?",
		code = function()
			play_sound("effects/Menu_Item_Selected_Sound_1.ogg")
			end_dialog()
		end,
	},
}
