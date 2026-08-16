#ifndef PLATFORM_H
#define PLATFORM_H

#include "../common/types.h"
#include "../game/hisc.h"
#include "../game/game.h"

int platform_load_score_table(Thisc *);
void platform_save_score_table(Thisc *);
void * platform_memory_allocate(unsigned int);
void platform_copy_string(char *, char *);
void platform_initialize();
void platform_load_sound_config(int *, int *);
int platform_play(Thisc *, int);
void platform_shutdown(int *, int *);
int platform_title(int *, int *);
void platform_show_high_scores(Thisc *hisc);
void platform_configure_sound(int sound_vol, int music_vol);

#endif