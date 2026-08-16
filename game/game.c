#include "../common/types.h"
#include "game.h"
#include "hisc.h"
#include "../platform/platform.h"

/*
*   Alex2: Platform agnostic game logic.
*   Last Update: Aug 15, 2026
*   Author: Scott Harner
*/

static Thisc *hisc;						// a hiscore table
static int sound_vol, music_vol;

void game_load_sound_config() {
	platform_load_sound_config(&sound_vol, &music_vol);
}

// platform agnostic primary game logic
void game_initialize()
{
    platform_initialize();
   	hisc = makeTable();
	if (!loadTable(hisc)) resetScores(hisc);
}

void game_configure_sound()
{
    game_load_sound_config();
    platform_configure_sound(sound_vol, music_vol);
}

void game_shutdown()
{
    saveTable(hisc);
    platform_shutdown(&sound_vol, &music_vol);
}

int game_play()
{
    return platform_play(hisc, sound_vol);
}

int game_title()
{
    return platform_title(&sound_vol, &music_vol);
}

void game_show_high_scores() 
{
    platform_show_high_scores(hisc);
}