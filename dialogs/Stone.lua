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

return {
	FirstTime = function()
		show("node0")
	end,

	EveryTime = function()
		if (has_quest("Saving the shop")) then
			show_node_if((get_gold() > 1400), "node23")
			if (done_quest("Saving the shop")) then
				hide("node23") show("node10")
			end
		end

		if (Stone_talk_again_later) then
			show("node4")
		end

		show("node99")
	end,

	{
		id = "node0",
		text = _"Hi! I'm new here.",
		code = function()
			npc_says(_"Welcome to our town! I'm Ms. Stone. I run this shop.")
			set_bot_name(_"Stone - Shop owner")
			hide("node0") show("node1", "node3", "node10")
		end,
	},
	{
		id = "node1",
		text = _"I would like to trade with you.",
		code = function()
			npc_says(_"Good! Here is the selection of items I have for sale.")
			show_node_if((not Stone_asked_for_guns), "node2")
			trade_with("Stone")
		end,
	},
	{
		id = "node2",
		text = _"Swords, clubs and maces? What is this? I want guns!",
		code = function()
			if (tux_has_joined_guard) then
				npc_says(_"I think the Red Guard has their own store somewhere.")
				npc_says(_"At least, they somehow get supplement I don't sell.")
				npc_says(_"However, I'm quite sure, they don't offer the same range of products that I do.")
			else
				npc_says(_"Sorry, this is all we have for now. The Red Guard took away all of our poppers.", "NO_WAIT")
				npc_says(_"For our own good. Of course.")
			end
			Stone_asked_for_guns = true
			hide("node2")
		end,
	},
	{
		id = "node3",
		text = _"What can you tell me about the guards here?",
		code = function()
			npc_says(_"Ah, the Red Guard.", "NO_WAIT")
			if (guard_follow_tux) then -- tux followed by guard
				npc_says(_"Well, they keep the town safe.")
				npc_says(_"Without them, everything would drown in chaos, but...")
				show("node4")
			elseif (tux_has_joined_guard) then -- tux joined guard/not followed by guard
				npc_says(_"Well, you ARE a red guard.")
				npc_says(_"What should I tell you about them that you don't know yet?")
				tux_says(_"Uhm, right.")
			else
				npc_says(_"Stranger, best if you don't ask such questions.", "NO_WAIT")
				npc_says(_"They keep us alive and safe from the bots, but...")
				show("node4")
			end
			hide("node3")
		end,
	},
	{
		id = "node4",
		text = _"But...?",
		code = function()
			if (guard_follow_tux) then
				npc_says(_"But nothing.")
				npc_says(_"I'm busy right now.")
				npc_says(_"We should talk again later...")
				Stone_talk_again_later = true -- EveryTime Code: if var then show("node4")
			elseif (tux_has_joined_guard) then -- tux joined guard, not followed
				npc_says(_"But nothing!")
				npc_says(_"I don't know what you are talking about.")
				Stone_talk_again_later = false -- Stone rejects to tell more because Tux is RG now
			else
				npc_says(_"The Guards run the town with very strict laws and demand total obedience.", "NO_WAIT")
				npc_says(_"We all need to obey their rules. We need to work hard or they hurt us.")
				npc_says(_"I guess I should not complain. The bots are far worse than the guards can be, but...")
				npc_says(_"Sucked dry by a bloodthirsty regime over many years or just sliced into bits by our former servants...", "NO_WAIT")
				npc_says(_"... What's the difference?")
				Stone_talk_again_later = false
				show("node8")
			end
			hide("node4")
		end,
	},
	{
		id = "node8",
		text = _"If things are so bad right now, why don't you leave?",
		echo_text = false,
		code = function()
			tux_says(_"If things are so bad right now, why --")
			npc_says(_"I don't want to hear this jabberwocky.", "NO_WAIT")
			npc_says(_"A hint, Linarian: We don't have much, but we're alive and we'd rather stay that way.", "NO_WAIT")
			if (tux_has_joined_guard) then
				npc_says(_"You are a Red Guardian now, feel free to make attempts on making our situation better here in this town.")
			else
				npc_says(_"If you want to start a revolution, you're on your own.")
			end
			hide("node8")
		end,
	},
	{
		id = "node10",
		text = _"How is your business going?",
		code = function()
			if (done_quest("Saving the shop")) then
				npc_says(_"Thanks to you I have a chance now.", "NO_WAIT")
				npc_says(_"Ha! It's time to show the world that Lily Stone is not going down without putting up a REAL good fight.")
				set_bot_name(_"Lily Stone - Shop owner")
			else
				npc_says(_"I'll be finished in about a week.", "NO_WAIT")
				npc_says(_"The Stone family always bought cheap and sold at a low profit. The times have changed, but not my prices.")
				npc_says(_"The new taxes, the worldwide economic collapse and other factors are slowly murdering the family store.")
				show("node13", "node16")
			end
			hide("node10")
		end,
	},
	{
		id = "node13",
		text = _"Can I help you somehow, Ms. Stone?",
		code = function()
			npc_says(_"No, there's nothing that can be done anymore. I don't have enough money to lift the shop from the downward spiral.")
			npc_says(_"I'd need thousands to just survive the month.", "NO_WAIT")
			npc_says(_"The shop is dying, and there's nothing either of us can do about it.")
			hide("node13", "node16") show("node15")
		end,
	},
	{
		id = "node15",
		text = _"Maybe there is still hope, Ms. Stone. How much exactly do you need?",
		code = function()
			npc_says(_"I couldn't get myself to look at my finance logs for a while now. I guess about two thousand circuits.")
			add_quest(_"Saving the shop", _"Ms. Stone can't afford to pay her taxes, and it looks like the family business will be going under soon. She needs 2000 in cash to stay afloat. I could try to help her out.")
			show_node_if((get_gold() > 2000), "node23")
			hide("node15")
		end,
	},
	{
		id = "node16",
		text = _"A pity that nothing can be done about your store.",
		code = function()
			npc_says(_"Yes. A tradition of six generations will die with this shop...", "NO_WAIT")
			npc_says(_"I guess it just had to happen one day...")
			hide("node13", "node16")
		end,
	},
	{
		id = "node23",
		text = _"I have the money you need Ms. Stone. You can have it.",
		code = function()
			npc_says(_"Is... Is this a joke? So much money? And you just give it to me?", "NO_WAIT")
			npc_says(_"Thank you! Thank you! Thank you! Savior!")
			if (tux_has_joined_guard) then -- joined guard
				npc_says(_"Are you sure you really have the sum of 2000 circuits?")
				if (del_gold(2000)) then
					tux_says(_"Yes, I have it.")
					npc_says(_"Oh thank you!")
					end_quest(_"Saving the shop", _"Ms. Stone seemed relieved as I gave her the money. I hope she won't need to file bankruptcy.")
				else -- joined guard + not enough money
					npc_says(_"Hold on a minute, you are still a bit short on money.", "NO_WAIT")
					npc_says(_"Don't try to trick me.")
				end
			else -- not joined guard, Stone will be more kind
				npc_says(_"But this isn't fair to you. The Stones are a family of fair merchants and I intend to uphold that tradition.")
				npc_says(_"I need 1,547 circuits to get through the month. I will accept only that and not a transistor more.")
				if (del_gold(1547)) then
					npc_says(_"Now it's fair. Thank you.", "NO_WAIT")
					npc_says(_"You've given me a bit of hope.. ", "NO_WAIT")
					npc_says(_"Wait a second... I think I recognize you...")
					npc_says(_"You are %s, aren't you...?", get_player_name())
					npc_says(_"The hero...")
					show("node31", "node32", "node33")
					push_topic("Stone's Hero")
				else -- not joined guard + not enough money
					npc_says(_"Hold on a minute, you are still a bit short on money.", "NO_WAIT")
					npc_says(_"Please return with the full amount if you want to help.", "NO_WAIT")
					npc_says(_"Otherwise you're throwing good money after bad, and I couldn't let you do that.")
				end
			end
			hide("node23")
		end,
	},
	{
		id = "node31",
		text = _"Yes, it's me.",
		topic = "Stone's Hero",
		code = function()
			npc_says(_"I thought so. My grandfather used to tell me stories about you. He said you would come here in a time of despair.", "NO_WAIT")
			npc_says(_"And here you are.")
			npc_says(_"I hope your arrival changes the tide of this war...", "NO_WAIT")
			npc_says(_"I have something here for you. I managed to hide this Shotgun from the Red Guard.")
			npc_says(_"It used to belong to my grandfather, so it's very dear to me, but I'm sure he would have like you to have it on your journey to save us all.")
			npc_says(_"%s, if you ever find out who started the Great Assault... GIVE THEM A SWIFT KICK FROM LILY STONE!!!", get_player_name())
			set_bot_name(_"Lily Stone - Shop owner")
			add_item("Two Barrel shotgun", 1)
			add_item("Shotgun shells", 20)
			end_quest(_"Saving the shop", _"Ms. Stone was really grateful for my help. She took only what she needed from me, and still rewarded me with a shotgun she had been hiding from the Red Guard.")
			hide("node31", "node32", "node33")
			pop_topic() -- "Stone's Hero"
		end,
	},
	{
		id = "node32",
		text = _"It's not important who I am.",
		topic = "Stone's Hero",
		code = function()
			npc_says(_"If you want to remain a stranger, I will respect your wishes.", "NO_WAIT")
			npc_says(_"So be it.")
			end_quest(_"Saving the shop", _"Ms. Stone was really grateful for my help. She took only what she needed from me.")
			hide("node31", "node32", "node33")
			pop_topic() -- "Stone's Hero"
		end,
	},
	{
		id = "node33",
		text = _"Ha ha ha, you fell for it! You get nothing from me! Hahahahah!",
		topic = "Stone's Hero",
		code = function()
			npc_says(_"You... monster...", "NO_WAIT")
			npc_says(_"No one makes fun of Lily Stone's family store!", "NO_WAIT")
			npc_says(_"Say your prayers, you oversized duck!!!")
			add_gold(1547)
			npc_faction("crazy", _"Lily Stone - Bird Hunter")
			end_quest(_"Saving the shop", _"I tricked that Ms. Stone into believing I was going to help her out. It was funny to see her get angry.")
			hide("node31", "node32", "node33")
			pop_topic() -- "Stone's Hero"
			end_dialog()
		end,
	},
	{
		id = "node99",
		text = _"See you later.",
		code = function()
			npc_says_random(_"Have a nice day!",
				_"Take care that you don't void any warranties.",
				_"Please come back if you have any questions regarding my goods.",
				_"I hope to see you again, soon!")
			end_dialog()
		end,
	},
}
