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
PERSONALITY = { Pushy", "Obsessed" },
PURPOSE = "$$NAME$$ is an Act 2 NPC. He is isolated with a C-64 virus south of R&R Resorts. He initially sells Tux somewhat useless items, but then Tux can exchange hundred of items to unlock options on his shop in three main branches. When Tux complete each branch, a new, special action becomes available, like crafting add-ons. When Tux completes all branches, he may do the Barrett M82 quest, which is the most powerful gun on the game."
WIKI]]--

local Npc = FDrpg.get_npc()
local Tux = FDrpg.get_tux()

return {
	FirstTime = function()
		show("node0")
	end,

	EveryTime = function()
		show("node99")
	end,

	{
		id = "node0",
		text = _"Uhm... Hello?",
		code = function()
			Npc:says(_"Hello, I'm Fred. I make bombs and ammo. I can sell some for you.")
			Tux:says(_"I would like to give a look at it later, thanks.")
			hide("node0") show("node1")
		end,
	},
	{
		id = "node1",
		text = _"Let me see what you're selling.",
		code = function()
			Npc:says(_"Sure, please take a look.")
			hide("node1") show("node10", "buy")
			trade_with("Act2-Fred")
		end,
	},
	{
		id = "node10",
		text = _"What is that? You call that bombs and ammo??",
		code = function()
			Npc:says(_"Sure I do.")
			Tux:says(_"That can't even be called a grenade. Also, what's that ammo useful for? Could you sell me the gun, at least?")
			Npc:says(_"Oh, my dear penguin-- I mean, linarian, of course I have more things to sell. I just don't want to sell them to you, at least, not yet.")
			Tux:says(_"I'm a rich bird. Just name a price and I'll buy.")
			Npc:says(_"Then let's do a game. You bring me a hundred of something which you think I need, and in return, I unlock new things so you can buy them. I won't accept repeated items. Deal?")
			Tux:says(_"Deal.")
			hide("node10") show("vmx", "smallemp", "762ammo")
		end,
	},
	{
		id = "buy",
		text = _"Let me see what you're selling.",
		code = function()
			Npc:says_random(_"Sure, please take a look.",
							_"Maybe you'll like something.",
							_"In time of apocalypse, only I can supply you.",
							_"My wares are your only hope of survival out there.",
							_"Grenades and Ammo. Everything you'll ever need.",
							_"So, are you enjoying those lands so far?",
							_"I hope you have a gun to use my ammo. Otherwise, it's useless.",
							_"Please buy my grenades. You won't regret.",
							_"Nice shopping!")
			trade_with("Act2-Fred")
		end,
	},
	{ -- Unlock special items (eg. Teleport Homing Beacon)
	  -- When finishing this branch, Tux may to include add-ons to equipment.
		id = "vmx",
		text = _"Do you sell VMX grenades? I'll pay you with 100 Tachyon Condensators.",
		code = function()
			Npc:says(_"Oh, VMX, those bombs to kill biological beings... And commodore 64 viruses.")
			if (Tux:count_item_backpack("Tachyon Condensator") >= 100) then
				Npc:says(_"Deal. You can now buy [b]VMX Gas Grenade[/b].")
				Tux:del_item_backpack("Tachyon Condensator", 100)
				sell_item("VMX Gas Grenade", 1, "Act2-Fred")
				hide("vmx")
				show("addonequip") -- Finished SPECIAL branch.
			else
				Npc:says(_"But you need the 100 Tachyon Condensators. I won't start selling the VMX grenade without them.")
			end
		end,
	},
	{ -- Unlocks further grenades and bombs
	  -- When finishing this branch, Tux may require to craft add-ons.
		id = "smallemp",
		text = _"Do you sell EMP grenades? I'll pay you with 100 Antimatter Converters.",
		code = function()
			Npc:says(_"Oh, EMP, Electromagnetic Pulse Grenades. Yes, I do. I amuse myself everyday that they cannot hurt the C-64 virus anomaly over there.")
			if (Tux:count_item_backpack("Antimatter-Matter Converter") >= 100) then
				Npc:says(_"Deal. You can now buy [b]Small EMP Shockwave Generator[/b].")
				Tux:del_item_backpack("Antimatter-Matter Converter", 100)
				sell_item("Small EMP Shockwave Generator", 1, "Act2-Fred")
				hide("smallemp") -- show("bigemp")
				show("addoncraft") -- Finished GRENADES branch.
			else
				Npc:says(_"But you need the 100 Antimatter Converters. I won't start selling the grenades without them.")
			end
		end,
	},
	{ -- Unlocks further ammo (and possibly guns)
	  -- When finishing this branch, Tux may require Barrett.
		id = "762ammo",
		text = _"Do you sell 7.62x39mm Ammunition for the popular AK-47? I'll pay you with 100 Plasma Transistors.",
		code = function()
			Npc:says(_"So you found an AK-47, uhm? It's a good weapon, I doubt you'll find a better one so soon on this part of the country.")
			if (Tux:count_item_backpack("Plasma Transistor") >= 100) then
				Npc:says(_"Deal. You can now buy [b]7.62x39mm Ammunition[/b].")
				Tux:del_item_backpack("Plasma Transistor", 100)
				sell_item("7.62x39mm Ammunition", 1, "Act2-Fred")
				hide("762ammo")
				show("barrett") -- Finished AMMO/GUN branch.
			else
				Npc:says(_"But you need the 100 Plasma Transistors. I won't start selling the ammo without them.")
			end
		end,
	},
	{ -- TODO Requires maxing out every node and something absurd for an absurd weapon (40~240 damage!!)
		id = "barrett",
		text = _"Can you sell me a weapon for the .50 BMG Ammo?",
		code = function()
			Npc:says(_"A weapon for the .50 Browning Machine Gun Ammo. Only the Barrett M82 Sniper Rifle use that junk. I mean, they don't even stack, you need a slot on inventory for each ammo.")
			Tux:says(_"So, if I was interested in such gun. How much would it cost?")
			Npc:says(_"I won't sell you. You see, the Barrett is powerful enough to stop a truck, literally, and the likelihood of even a battle droid surviving more than 2 hits by this weapon is quite slim.")
			Npc:says(_"So unless you're military or something, I won't be selling it for you.")
			Tux:says(_"I have many circuits, you know.")
			Npc:says(_"And I have a Barrett M82. I bet a few direct hits from this gun, and you'll leave life to become history.") 
			Tux:says(_"...You've got a point there.")
		end,
	},
	{
		id = "addoncraft",
		text = _"Could you craft some add-ons for me?",
		code = function()
			Npc:says(_"Of course I can craft add-ons for my best customer.")
			Tux:says(_"If not the only.")
			craft_addons()
		end,
	},
	{
		id = "addonequip",
		text = _"Can you assemble add-ons on my equip?",
		code = function()
			Npc:says(_"Of course I can assemble add-ons for my best customer.")
			Tux:says(_"If not the only.")
			upgrade_items()
		end,
	},
	{
		id = "node99",
		text = _"I'll leave this strange man to his... evildoings.",
		code = function()
			Npc:says_random(_"...",
							_"You joke now, but a bot soon will teach you to be more serious.",
							_"Evildoer? You don't even know me.",
							_"...But it's you who is killing bots left and right...",
							_"Having survived the Great Assault doesn't makes me an evildoer.")
			end_dialog()
		end,
	},
}
