/*
 *
 *   Copyright (c) 2006 Arvid Picciani
 *   Copyright (c) 2004-2007 Arthur Huillet
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 */

#include "struct.h"
#include "global.h"
#include "proto.h"

void Play_Spell_ForceToEnergy_Sound(void)
{
	play_sound("effects/Spell_ForceToEnergy_Sound_0.ogg");
}

/**
 * This function plays a voice sample, stating that not enough power
 * (strength) is available to use a certain item.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 */
void No_Ammo_Sound(void)
{
	static Uint32 PreviousSound = (-1);
	Uint32 now;

	now = SDL_GetTicks();
	if (SDL_GetTicks() - PreviousSound >= 0.25 * 1000) {
		play_sound("effects/No_Ammo_Sound_0.ogg");
		PreviousSound = now;
	}
}

/**
 * This function plays a voice sample, stating that not enough power
 * (strength) is available to use a certain item.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 */
void Not_Enough_Power_Sound(void)
{
	static Uint32 PreviousSound = (-1);
	Uint32 now;

	now = SDL_GetTicks();
	if (SDL_GetTicks() - PreviousSound >= 1.15 * 1000) {
		play_sound("effects/tux_ingame_comments/Not_Enough_Power_Sound_0.ogg");
		PreviousSound = now;
	}
}

/**
 * This function plays a voice sample, stating that not enough power
 * distribution (dexterity) is available to use a certain item.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 */
void Not_Enough_Dist_Sound(void)
{
	static Uint32 PreviousSound = (-1);
	Uint32 now;

	now = SDL_GetTicks();
	if (SDL_GetTicks() - PreviousSound >= 1.15 * 1000) {
		play_sound("effects/tux_ingame_comments/Not_Enough_Dist_Sound_0.ogg");
		PreviousSound = now;
	}
}

/**
 * Whenever the Tux meets someone in the game for the very first time,
 * this enemy or friend will issue the first-time greeting.
 * Depending on whether that is typically a hostile unit or not, we play
 * the corresponding sound from preloaded memory or, if there is no combat
 * to expect, by loading it freshly from disk and then forgetting about it
 * again.
 */
void play_droid_greeting_sound(enemy *ThisRobot)
{
	if (!Droidmap[ThisRobot->type].greeting_sound)
		return;
	play_sound_at_position(Droidmap[ThisRobot->type].greeting_sound /*sounds[SoundCode]*/, &Me.pos, &ThisRobot->pos);
}

/**
 * Whenever a bot dies, that should create a dying sound.  But so far,
 * this will be done only for fully animated bots, since the other bots
 * just explode and that has a sound of it's own.
 */
void play_droid_death_sound(enemy * ThisRobot)
{
	if (!Droidmap[ThisRobot->type].death_sound)
		return;
	play_sound_at_position(Droidmap[ThisRobot->type].death_sound, &Me.pos, &ThisRobot->pos);
}

/**
 * Whenever a bot starts to attack the Tux, he'll issue the attack cry.
 * Either we output a standard sound, either we output a special voice sample such as "drill eyes"
 * This is done here, and no respect to loading time issues for now...
 */
void play_droid_attack_sound(enemy *ThisRobot)
{
	char *path = Droidmap[ThisRobot->type].voice_samples_path;
	int first = Droidmap[ThisRobot->type].voice_samples_first;
	int last = Droidmap[ThisRobot->type].voice_samples_last;
	int probability = Droidmap[ThisRobot->type].voice_samples_probability;

	if (!path || (probability == 0) || (MyRandom(100) > probability)) {
		// Play the attack sound if the random draw is high enough, or if no voice samples are set
		play_sound_at_position(Droidmap[ThisRobot->type].attack_sound, &Me.pos, &ThisRobot->pos);

	} else {

		char sample_path[1024];
		sprintf(sample_path, "%s/%d.ogg", path, MyRandom(last - first) + first);
		play_sound_at_position(sample_path, &Me.pos, &ThisRobot->pos);

	}
}

/**
 * Whenever an item is placed or taken, we'll issue a sound attached to
 * that item.
 */
void play_item_sound(item_t item_type, struct gps *item_pos)
{
	char final_file_name[5000];

	// First some sanity check...
	//
	if (item_type < 0) {
		error_message(__FUNCTION__, "\
		                           negative item type received (%d)!", PLEASE_INFORM | IS_FATAL, item_type);
	}
	// Now we can proceed and just play the sound associated with that item...
	//
	strcpy(final_file_name, "effects/item_sounds/");
	strcat(final_file_name, ItemMap[item_type].item_drop_sound_file_name);
	play_sound_at_position(final_file_name, &Me.pos, item_pos);

}

/**
 * This function generates a voice output stating that the influencer
 * can't carry any more right now.  Also this function will see to it,
 * that the sentence is not repeated until 4 seconds after the previous
 * cant-carry-sentence have passed.
 */
void CantCarrySound(void)
{
	static Uint32 CurrentTicks = 0;

	if ((SDL_GetTicks() - CurrentTicks) > 2 * 1000) {
		CurrentTicks = SDL_GetTicks();
		switch (MyRandom(2)) {
		case 0:
			play_sound("effects/tux_ingame_comments/ICantCarryAnyMore_Sound_0.ogg");
			break;
		case 1:
			play_sound("effects/tux_ingame_comments/ICantCarryAnyMore_Sound_1.ogg");
			break;
		case 2:
			play_sound("effects/tux_ingame_comments/ICantCarryAnyMore_Sound_2.ogg");
			break;
		default:
			break;
		}
	}
}


/**
 * When the Tux uses the 'teleport home' spell, this sound will be played.
 */
void teleport_arrival_sound(void)
{
	play_sound("effects/new_teleporter_sound.ogg");
}

/**
 * Whenever the Tux gets hit, we *might* play a sound sample for the hit,
 * depending on random results.  Since we don't want to hear the same
 * sound sample all the time, there is a selection of possible files
 * one of which will be selected at random.
 */
void tux_scream_sound(void)
{
	switch (MyRandom(4)) {
	case 0:
		play_sound("effects/Influencer_Scream_Sound_0.ogg");
		break;
	case 1:
		play_sound("effects/Influencer_Scream_Sound_1.ogg");
		break;
	case 2:
		play_sound("effects/Influencer_Scream_Sound_2.ogg");
		break;
	case 3:
		play_sound("effects/Influencer_Scream_Sound_3.ogg");
		break;
	case 4:
		play_sound("effects/Influencer_Scream_Sound_4.ogg");
		break;
	default:
		break;
	}
}

/**
 * We add a matching group of sounds here for the menu movements.  It's
 * a 'ping-ping' sound, well, not super, but where do we get a better one?
 */
void MenuItemSelectedSound(void)
{
	play_sound_v("effects/Menu_Item_Selected_Sound_1.ogg", 0.5);
}

void MenuItemDeselectedSound(void)
{
	play_sound_v("effects/Menu_Item_Deselected_Sound_0.ogg", 0.5);
}

void MoveMenuPositionSound(void)
{
	play_sound_v("effects/MenuClick.ogg", 0.5);
}

void ThouArtDefeatedSound(void)
{
	if (!sound_on)
		return;
	play_sound("effects/ThouArtDefeated_Sound_0.ogg");
}

/**
 * When the Tux makes a weapon swing, this will either cause a swinging
 * sound and then a 'hit' sound or it will just be a swinging sound.  The
 * following functions do this, also creating some variation in the choice
 * of sample used.
 */
void play_melee_weapon_hit_something_sound(void)
{
	const char *sounds[] = {
		"effects/swing_then_hit_1.ogg",
		"effects/swing_then_hit_2.ogg",
		"effects/swing_then_hit_3.ogg",
		"effects/swing_then_hit_4.ogg",
		"effects/swing_then_hit_5.ogg",
	};
	int sound_index = MyRandom(sizeof(sounds) / sizeof(sounds[0]) - 1);

	// The target of the attack is very near Tux, so no need to play
	// a positional sound.
	play_sound(sounds[sound_index]);
}

void play_melee_weapon_missed_sound(struct gps *attacker_pos)
{
	const char *sounds[] = {
		"effects/swing_then_nohit_1.ogg",
		"effects/swing_then_nohit_2.ogg",
		"effects/swing_then_nohit_3.ogg",
		"effects/swing_then_nohit_4.ogg",
	};
	int SoundCode = MyRandom(sizeof(sounds) / sizeof(sounds[0]) - 1);

	play_sound_at_position(sounds[SoundCode], &Me.pos, attacker_pos);
}

/**
 * This function should generate the sound that belongs to a certain
 * (ranged) weapon.
 *
 * This does not include the Tux swinging/swinging_and_hit
 * sounds, when Tux is using melee weapons, but it does include ranged
 * weapons and the non-animated bot weapons too.
 */
void fire_bullet_sound(int BulletType, struct gps *shooter_pos)
{
	if (!sound_on)
		return;

	struct bulletspec *bullet_spec = dynarray_member(&bullet_specs, BulletType, sizeof(struct bulletspec));

	if (!bullet_spec->sound) {
		play_melee_weapon_missed_sound(shooter_pos);
		return;
	}

	char sound_file[100] = "effects/bullets/";
	strcat(sound_file, bullet_spec->sound);

	play_sound_at_position(sound_file, &Me.pos, shooter_pos);
}

/**
 * For the takeover game, there are 4 main sounds.  We handle them from
 * the cache, even if that might also be possible as 'once_needed' type
 * sound samples...
 */
void Takeover_Set_Capsule_Sound(void)
{
	play_sound("effects/TakeoverSetCapsule_Sound_0.ogg");
}

void Takeover_Game_Won_Sound(void)
{
	play_sound("effects/Takeover_Game_Won_Sound_0.ogg");
}

void Takeover_Game_Deadlock_Sound(void)
{
	play_sound("effects/Takeover_Game_Deadlock_Sound_0.ogg");
}

void Takeover_Game_Lost_Sound(void)
{
	play_sound("effects/Takeover_Game_Lost_Sound_0.ogg");
}

void play_blast_sound(char *blast_sound, struct gps *blast_pos)
{
	char fpath[PATH_MAX];

	if (!blast_sound)
		return;

	strcpy(fpath, "effects/");
	strcat(fpath, blast_sound);
	play_sound_at_position(fpath, &Me.pos, blast_pos);
}

/**
 * This function plays a sound of a bullet being reflected. It is only
 * used, when a bullets is compensated by the tux armor.
 */
void BulletReflectedSound(void)
{
	play_sound("effects/Bullet_Reflected_Sound_0.ogg");
}

void play_open_chest_sound(void)
{
	play_sound("effects/open_chest_sound.ogg");
}

void play_read_sign_sound()
{
	play_sound("effects/read_sign_sound.ogg");
}
