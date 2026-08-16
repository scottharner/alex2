#ifndef GAME_H
#define GAME_H

#include "../common/types.h"

void game_initialize();
void game_load_sound_config();
void game_shutdown();
int game_play();
int game_title();
void game_show_high_scores();
void game_configure_sound();

#endif