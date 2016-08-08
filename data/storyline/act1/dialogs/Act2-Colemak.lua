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
	FirstTime = function()

		Npc:set_rush_tux(false) -- While we could check for RT state, this NPC should never be called without it.

	end,

	EveryTime = function()
		if (not Act2_TalkedToColemak) then
			next("intro")
		else
			next("node99") -- Player should not leave this dialog. It's not my fault if you pressed 'ESC'.
		end

	end,

	{
		id = "intro",
		text = _"Who are you?",
		code = function()
			--; TRANSLATORS: %s = Tux:get_player_name()
			Npc:says(_"Hello, my name is Colemak. I am an old friend of yours, %s.", Tux:get_player_name())
			Act2_TalkedToColemak=true
			show("guyinfo","youinfo","bossinfo")
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
			Npc:says(_"Thankfully the plan failed, the bots turned out to be too perfect. So he used...")
			Tux:says(_"The Great Assault. Thanks, I understand it now.")
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
			Npc:says(_"Thankfully they never figured out about what I truly knew. I was cryonized for figuring out that Hell Fortress Factory Boss had a pessoal teletransport network and used it to grab the money and goon vacations in tropical islands.")
			Tux:says(_"Ironic.")
			hide("youinfo")
		end,
	},

	{
		id = "lhcinfo",
		text = _"High Council? Tell me more...",
		code = function()
			Npc:says(_"I thought you already knew all that, being a former ship commander and all...?", "NO_WAIT")
			Tux:says(_"I was cryonized, so I forgot. Would you mind helping me to find my memories?")
			Npc:says(_"You should find Dvorak first, he was a good friend of yours. Like, your guide.")
			Npc:says(_"He surely will know what you should do next.")
			hide("lhcinfo")
			show("node99")
		end,
	},


	{
		id = "node99",
		text = _"Let's find Dvorak! (end game)",
		code = function()
			-- Switch music back to town.ogg -- not needed (yet)
			--switch_background_music("town.ogg") 

			-- We don't have content from here on. The end.
			hide("node99")
			display_big_message(_"--- Continues ---")
			win_game()
			end_dialog()
		end,
	},
}
