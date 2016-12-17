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
PERSONALITY = { "Enigmatic", "Friendly", "Helpful" },
MARKERS = { NPCID1 = "Dvorak" },
PURPOSE = "$$NAME$$ is a human who worked for $$NPCID1$$ before the Great Assault. He joined MS and helped $$NPCID1$$ to figure
	 out that the only way to save the planet was after the Great Assault. While $$NPCID1$$ thought $$NAME$$ died, $$NAME$$ was
	 cryonized on RR Resorts. $$NAME$$ helps tux to figure out (and lastly save) their old friend, $$NPCID1$$.",
RELATIONSHIP = {
	{
		actor = "$$NPCID1$$",
		text = "$$NAME$$ knows $$NPCID1$$, but maybe a little more than $$NPCID1$$ would like."
	}
}
WIKI]]--

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {

	EveryTime = function()

-- 		"I have secrets to tell, let me get close to you first so no one will overhear us."

		if (Npc:get_rush_tux()) then
			Npc:set_rush_tux(false)
		end	

		if (not Act2_TalkedToColemak) then -- First time, jump to intro. Last time, endgame
			next("intro")
		elseif (not Act2_DvorakPlanDownload) then
			next("endgame")
		else
			Npc:says(_"Please go to the Terminal you used to uncryonize me and issue the [b]download[/b] command.")
			end_dialog()
		end

	

	end,

	{
		id = "intro",
		text = _"Who are you?",
		code = function()
			--; TRANSLATORS: %s = Tux:get_player_name()
			Npc:says(_"Hello, my name is Colemak. I am an old friend of yours, %s.", Tux:get_player_name())
			show("nameinfo","youinfo")
			hide("intro")
		end,
	},

	{
		id = "guyinfo",
		text = _"You just killed two guys!!",
		code = function()
			Npc:says(_"Well, as you aren't trying to kill me as some irrational birds I've met, I'll say to you.")
			Npc:says(_"They aren't humans. They were part of the devious plan of the MS big boss.", "NO_WAIT")
			Npc:says(_"They are bots, which thinks they are humans. What better way to kill humans that with humans?")
			Npc:says(_"Thankfully the plan failed, the bots turned out to be too human for his liking. So he used...")
			Tux:says(_"The Great Assault. Thanks, I understand now.")
			hide("guyinfo")
		end,
	},

	{
		id = "bossinfo",
		text = _"The former Chief Programming Officer spoke about... linarians...",
		code = function()
			Npc:says(_"Ah yes, I heard that from Dvorak. Apparently MS big boss was born in Linarius.", "NO_WAIT")
			Npc:says(_"I've heard his nickname a few times now: [b]Agent Zero[/b].")
			Npc:says(_"Apparently, from what I collected while I was on MS, he was one of the first agents sent by the High Council.")
			hide("bossinfo") show("lhcinfo")
		end,
	},

	{
		id = "youinfo",
		text = _"Why are you cryonized?",
		code = function()
			Npc:says(_"Well, as you might be aware, if you figure out too much you're cryonized.", "NO_WAIT")
			Npc:says(_"Only the ones who figures out about MS big boss are actually killed.")
			Npc:says(_"The reason is simple. Killing would be too noisy, while cryonizing keeps them undercover.")
			Npc:says(_"Thankfully they never figured out about what I truly knew. I was cryonized for figuring out that Hell Fortress Factory Boss had a teletransport network only for him which he used to grab the money and go on vacations in tropical islands.")
			Tux:says(_"Ironic.")
			hide("youinfo")
		end,
	},

	{
		id = "nameinfo",
		text = _"HOW DO YOU KNOW MY NAME?!",
		code = function()
			Npc:says(_"Why, Dvorak told me, of course.")
			Tux:says(_"Ok, what's going on here?!")
			hide("nameinfo") show("bossinfo", "guyinfo")
		end,
	},

	{
		id = "lhcinfo",
		text = _"High Council? Tell me more...",
		code = function()
			Npc:says(_"I thought you already knew all that, being a former ship commander and all...?")
			Tux:says(_"I was cryonized, so I forgot. Would you mind helping me to find my memories?")
			Npc:says(_"You should find Dvorak first, he was a good friend of yours. Like, your guide.")
			Npc:says(_"He surely will know what you should do next.")
			hide("lhcinfo")
			show("node99")
		end,
	},

	{
		id = "node99",
		text = _"That's too much for me. Give me a minute or two to process all this information, please.",
		code = function()
			Npc:says(_"Don't worry my friend. Take all time you need, and then come back to talk with me.")
			Tux:says(_"Thanks, Colemak. I'll talk to you as soon as I'm ready.")
			Npc:says("", "NO_WAIT") -- White division line
			Npc:says(_"[b]Important: Colemak locked the Power Grid gates to prevent enemies from coming from the Factory.[/b]")
			Npc:says("", "NO_WAIT") -- White division line
			Npc:says(_"[b]Notice: Area south of RR Resorts is now available.[/b]")
			Tux:update_quest("A New Mission From Spencer", _"I ended up waking Colemak, which seems to be an old friend from the enigmatic entity which entitles itself as \"Dvorak\". Apparently this entity can help me recover my memories. I should talk to Colemak again soon.")
			-- Normalizes music
			switch_background_music("town.ogg") 
			Act2_TalkedToColemak=true
			change_obstacle_state("Act2CityGate", "closed")
			change_obstacle_state("RRResorts-NorthGate", "closed")
			change_obstacle_state("RRResorts-SouthGate", "opened")
			change_obstacle_state("Act2-IcySummerGate", "opened")
			hide("node99")
			end_dialog()
		end,
	},

	{
		id = "endgame",
		text = _"Let's find Dvorak!",
		code = function()
			Npc:says(_"So, are you ready?")
			Tux:says(_"Yes I am!")
			Npc:says(_"As you must be aware, bots were on Dvorak's track, and he had to hide.", "NO_WAIT")
			Npc:says(_"It would be impossible to locate him now, however he uploaded a detailed plan for you on my Cryo terminal.")
			Npc:says(_"I'll now unlock the [b]download[/b] command. Download the plans for me, pretty please.", "NO_WAIT")
			Npc:says(_"It's encrypted and requires a key, but I'm sure you'll find it. Inform me about what Dvorak left there!")
			Act2_DvorakPlanDownload=true
			Tux:update_quest("A New Mission From Spencer", _"Dvorak has foreseen my arrival and left some data at the terminal which I used to uncryonize Colemak. I must download these files no matter what it takes!")

			-- This was a fix for credits not being played (thanks wlan2)
			-- Cursor state was not reset on win_game() so you had to interact with terminal.
			end_dialog()
		end,
	},
}
