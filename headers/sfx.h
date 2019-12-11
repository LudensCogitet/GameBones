#ifndef SFX_H_INCLUDED
#define SFX_H_INCLUDED

#include "./enums.h"

void gb_sfx_init();
void gb_sfx_teardown();

int gb_sfx_sound_load(char* filename, GB_SFX_SOUND sound);
void gb_sfx_sound_unload(GB_SFX_SOUND sound);

int gb_sfx_sound_play(GB_SFX_SOUND sound, int loop);
void gb_sfx_sound_stop(int channel);

#endif
