#include "../headers/sfx.h"
#include "../headers/enums.h"

#include <SDL2/SDL_mixer.h>
#include <stdio.h>

static Mix_Chunk *gb_sfx_sounds[SFX_SOUND_NUM_SOUNDS];

void gb_sfx_init() {
    if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        exit(1);
    }

    for (unsigned int i = 0; i < SFX_SOUND_NUM_SOUNDS; i++)
        gb_sfx_sounds[i] = 0;
}

void gb_sfx_sound_unload(GB_SFX_SOUND sound);
void gb_sfx_teardown() {
    for (unsigned int i = 0; i < SFX_SOUND_NUM_SOUNDS; i++) {
        gb_sfx_sound_unload(i);
    }

    Mix_Quit();
}


void gb_sfx_sound_unload(GB_SFX_SOUND sound) {
    if (gb_sfx_sounds[sound]) {
        Mix_FreeChunk(gb_sfx_sounds[sound]);
        gb_sfx_sounds[sound] = 0;
    }
}

int gb_sfx_sound_load(char* filename, GB_SFX_SOUND sound) {
    if (gb_sfx_sounds[sound]) {
        gb_sfx_sound_unload(sound);
    }

    gb_sfx_sounds[sound] = Mix_LoadWAV(filename);
    if (!gb_sfx_sounds[sound]) {
        fprintf(stderr, "Failed to load sound from file: %s", Mix_GetError());
        return 0;
    }

    return 1;
}

int gb_sfx_sound_play(GB_SFX_SOUND sound, int loop) {
    return Mix_PlayChannel(-1, gb_sfx_sounds[sound], loop);
}

void gb_sfx_sound_stop(int channel) {
    Mix_HaltChannel(channel);
}
