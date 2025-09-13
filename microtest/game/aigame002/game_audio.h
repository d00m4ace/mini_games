#ifndef GAME_AUDIO_H
#define GAME_AUDIO_H

// Sound system globals
Sound sound_shoot;
Sound sound_hit;
Sound sound_explosion;
bool sounds_loaded;

// Function declarations
void load_game_sounds(void);
void unload_game_sounds(void);

// Implementation
void load_game_sounds(void)
{
	if(sounds_loaded) return;

	InitAudioDevice();

	sound_shoot = LoadSound("resources/shoot01.wav");
	sound_hit = LoadSound("resources/hit01.wav");
	sound_explosion = LoadSound("resources/explosion01.wav");

	SetSoundVolume(sound_shoot, 0.5f);
	SetSoundVolume(sound_hit, 0.6f);
	SetSoundVolume(sound_explosion, 0.7f);

	sounds_loaded = true;
}

void unload_game_sounds(void)
{
	if(!sounds_loaded) return;

	UnloadSound(sound_shoot);
	UnloadSound(sound_hit);
	UnloadSound(sound_explosion);

	CloseAudioDevice();
	sounds_loaded = false;
}

#endif // GAME_AUDIO_H