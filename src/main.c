/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2017, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <jo/jo.h>
// #include <stdio.h>
// #include "allegro.h"
#include "main.h"
#include "hisc.h"
// #include "../assets/data.h"	
#include "pcmsys.h"

/*
 *  Alex the Allegator 2
 *
 *  Written for Allegro Speedhack 2001
 *
 *  Johan Peitz (c) 2001
 *  d98peitz@dtek.chalmers.se
 *  http://www.dtek.chalmers.se/~fld/
 */

#define TITLE_TRACKID 6
#define SONG1_TRACKID 2
#define SONG2_TRACKID 3
#define SONG3_TRACKID 4
#define SONG4_TRACKID 5
#define BACKGROUND_ZINDEX 500
#define DONKEY_ZINDEX 450
#define TEXT_ZINDEX 400
#define AA2_ZINDEX 390
#define LOGO_ZINDEX 380
#define PLACEING_ZINDEX 400
#define POINTER_ZINDEX 350
#define INTRO_SCALE_GRAPHIC_TIME 120
#define INTRO_STILL_GRAPHIC_TIME 120
#define INTRO_FADE_GRAPHIC_TIME 120
#define INTRO_FADE_TEXT_TIME 60
#define INTRO_STILL_TEXT_TIME 60
#define INTRO_BLANK_TEXT_TIME 60
#define INTRO_TEXT_COUNT 6
#define CREDITS_TEXT_COUNT 10
#define INSTRUCTIONS_PAGE_COUNT 9
#define INSTRUCTIONS_LINE_COUNT 10
#define MAX_ACTION_CYCLES 10000
#define AA2_FINAL_X 240
#define POINTER_WIDTH 16
#define POINTER_HEIGHT 16
#define GAME_FONT_WIDTH 16
#define GAME_FONT_HEIGHT 32
#define GAME_FONT_MAPPING "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"?=',.()*-/ "
#define HOF_MAX_INDEX 3
#define HOF_CHARS_COUNT 28
#define MAX_COOLDOWN_COUNT 20
#define FADE_INTERVAL 3

Thisc *hisc;						// a hiscore table
Ttoken board[8][8];					// the board
Tparticle dust[MAX_PARTICLES];		// particles for the particle engine
Tplayer ply[3];						// 2 players, ignore ply[0]

// // global stuff
int playing;			// can the player interact?
int scrolling;			// are we scrolling the board?
int scroll_dir;			// 1 up, 2 right, 3 down, 4 left
int scroll_x, scroll_y;	// scrolling offset
int placeing, place_type, place_x, place_y;	// place_token stuff
byte player;			// current player
int winner;				// who won? 3=draw
int winner_presses;			// button presses after winner message is shown
byte sound_vol, music_vol;
int locked_row, locked_col; // current locked row/col
int hint, hint_x, hint_y;   // hint stuff
int cpu;				  // 0 = none, 1 = ply1, 2= ply2
int thinking;				// cpu moves counter

static mode current_game_mode;
static fade_state current_fade_state;
static game_type current_game_type;
static mode load_game_mode;
static void (*load_action)(void);
static int shlogo_sprite_id;
static int title_sprite_id;
static int aa2_sprite_id;
static int aalogo_sprite_id;
static int vol1_sprite_id;
static int vol2_sprite_id;
static int vol3_sprite_id;
static int pointer_sprite_id;
static int emptytkn_sprite_id;
static int greentkn_sprite_id;
static int bluetkn_sprite_id;
static int multitkn_sprite_id;
static int deadtkn_sprite_id;
static int hint_sprite_id;
static int board_sprite_id;
static int p1status_sprite_id;
static int p2status_sprite_id;
static int arrow1_sprite_id;
static int arrow2_sprite_id;
static int arrow3_sprite_id;
static int arrow4_sprite_id;
static int darrow1_sprite_id;
static int darrow2_sprite_id;
static int darrow3_sprite_id;
static int darrow4_sprite_id;
static int player11_sprite_id;
static int player12_sprite_id;
static int player13_sprite_id;
static int player14_sprite_id;
static int player21_sprite_id;
static int player22_sprite_id;
static int player23_sprite_id;
static int player24_sprite_id;
static int notkn_sprite_id;
static int dust000_sprite_id;
static int dust001_sprite_id;
static int dust002_sprite_id;
static int dust003_sprite_id;
static int dust004_sprite_id;
static int donkey1_sprite_id;
static int donkey2_sprite_id;
static int donkey3_sprite_id;
static int donkey4_sprite_id;
static int end_sprite_id;
static int endh_sprite_id;
static int tglup_sprite_id;
static int tgluph_sprite_id;
static int tgldn_sprite_id;
static int tgldnh_sprite_id;
static int action_counter;
static int fade_brightness;
static int fade_cooldown;
static jo_font *game_white_font;
static jo_font *game_black_font;
static int current_intro_text_index = 0;
static int current_instructions_page_index = 0;
static bool intro_mode_started = false;
static bool intro_graphic_scaled = false;
static bool intro_graphic_shown = false;
static bool intro_graphic_faded = false;
static bool intro_text_shown = false;
static bool is_instructions_selected = false;
static bool is_credits_selected = false;
static bool is_high_scores_selected = false;
static bool is_hvc_selected = false;
static bool is_cvh_selected = false;
static bool is_hvh_selected = false;
static int aa2_x;
static int pointer1_x;
static int pointer1_y;
static int pointer2_x;
static int pointer2_y;
static bool is_showing_main_menu_options = false;
static bool is_showing_start_game_options = false;
static bool title_did_set_seed = false;
static short select_sound_id;
static short plmulti_sound_id;
static short pldead_sound_id;
static short remove_sound_id;
static short rotate_sound_id;
static int title_menu_x, title_menu_y;
static int token_count = 0;
static int stone_count = 0;
static int hof_p = 0;
static int hof_score = 0;
static int hof_selected_index = 0;
static bool game_sprites_loaded = false;
static bool did_play_game = false;

static const char hof_chars[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','.',' ',};

static const char* intro_text[] =
{
	"JOHAN PEITZ",
	"PRESENTS",
	"THE RETURN OF",
	"ALEX",
	"THE ALLEGATOR",
	"IN"
};

static const char* credits_text[] = 
{
	"GAME ENGINEER - JOHAN PEITZ",
    "PORT ENGINEER - SCOTT HARNER",
    "QA TESTER - EVAN HARNER",
    "AUDIO DRIVER ENGINEER - PONUT64",
    "ADVISOR - HASSMASCHINE",
    "ADVISOR - TREKKIESUNITE118",
    "ADVISOR - REYEME",
	"",
	"",
	"POWERED BY - JO ENGINE"
};

static const char* instructions_titles[] = 
{
	"PLOT",
	"RULES",
	"MOVES",
	"SPECIAL TOKENS",
	"",
	"SCORING",
	"",
	"WINNING",
	""
};

static const char* instructions_lines[INSTRUCTIONS_PAGE_COUNT][INSTRUCTIONS_LINE_COUNT] = 
{
	{
		"ONE DAY, ALEX AND HIS BROTHER AARON",
		"DECIDED TO PLAY A GAME. THEY FOUND",
		"A BOARD AND A HANDFUL OF TOKENS AND",
		"SAT DOWN TO MAKE UP SOME RULES.",
		"",
		"",
		"",
		"",
		"",
		"USE D-PAD TO BROWSE, START QUITS."
	},
	{
		"EACH HAD AN UNLIMITED SET OF TOKENS",
		"THAT COULD BE PLAYED ON THE BOARD.",
		"WHEN A SQUARE OF 4 TOKENS OF THE",
		"SAME COLOR WAS PLAYED, THEY WERE",
		"ALL REMOVED. THE ONE WHO FIRST",
		"REMOVED ALL HIS TOKENS FROM THE",
		"BOARD WON THE GAME.",
		"",
		"",
		""
	},
	{
		"EVERY TURN THEY COULD EITHER PLACE",
		"A TOKEN, OR THEY COULD SLIDE A ROW",
		"OR COLUMN OF THE BOARD ONE SLOT. A",
		"SLIDED ROW OR COLUMN COULD NOT BE",
		"MOVED THE NEXT TURN BY THE OTHER",
		"PLAYER.",
		"",
		"",
		"",
		""
	},
	{
		"MORE TOKENS WERE ADDED. A YELLOW ONE",
		"WAS USED AS A MULTI TOKEN. IT COULD",
		"BE USED AS ANY COLOR BY ANY ONE. A",
		"GRAY TOKEN WAS ALSO USED. TO REMOVE",
		"A GRAY TOKEN YOU HAD TO SURROUND IT",
		"WITH THREE YELLOW ONES.",
		"",
		"",
		"",
		""
	},
	{
		"ABOUT EVERY TENTH TIME A TOKEN WAS",
		"PLAYED ON THE BOARD, A GRAY TOKEN",
		"APPEARED AT A RANDOM PLACE.",
		"",
		"IF ANYONE MANAGED TO REMOVE MORE",
		"THEN 4 TOKENS IN ONE GO, HE WAS",
		"REWAREDED WITH A MULTI TOKEN, THAT",
		"HE COULD PLAY AT ANY TIME.",
		"",
		""
	},
	{
		"TO MAKE THE GAME MORE INTERESTING",
		"THEY ADDED A SCORING SYSTEM. NORMAL",
		"TOKENS HAD A VALUE OF 10. MULTI ",
		"TOKENS SCORES 50 AND THE GREY TOKENS",
		"SCORED 100. YOU RECEIVED THE SCORE",
		"WHEN THE TOKENS WHERE TAKEN OF THE",
		"BOARD. YOU ALSO GOT SCORE FOR THE",
		"NUMBER OF TOKENS YOU COULD REMOVE",
		"IN ONE SWEEP. THE MORE TOKENS IN ONE",
		"GO, THE HIGHER THE SCORE."
	},
	{
		"LATER THEY FOUND OUT THAT IT WAS A", 
		"LOT MORE FUN IF THE PLAYER WHO'S",
		"TURN IT WAS, RECEIVED ALL SCORE, EVEN",
		"THE SCORE FOR ANY QUADRUPLES OF THE",
		"OTHER COLOR.",
		"",
		"",
		"",
		"",
		""
	},
	{
		"HAVING ADDED THE SCORING SYSTEM, THE",
		"OBJECT WAS NO LONGER ONLY TO WIPE",
		"THE BOARD. TO WIN YOU ALSO HAD TO",
		"HAVE THE HIGHEST SCORE.",
		"",
		"",
		"",
		"",
		"",
		""
	},
	{
		"NOW IT'S YOUR TURN TO HELP ALEX AND",
		"AARON TO PLAY THEIR GAME.",
		"",
		"GOOD LUCK, AND MOST OF ALL",
		"HAVE FUN!",
		"",
		"",
		"",
		"",
		""
	}
};

static int token_sprite_ids[5];
static int p1_anim_sprite_ids[4];
static int p2_anim_sprite_ids[4];
static int hof_char_indexes[3];
static int dust_sprite_ids[5];
static int donkey_sprite_ids[4];

// track button changes for better title menu input handling
static bool current_pad1_input_states[INPUT_TYPE_COUNT];
static bool previous_pad1_input_states[INPUT_TYPE_COUNT];
static bool current_pad2_input_states[INPUT_TYPE_COUNT];
static bool previous_pad2_input_states[INPUT_TYPE_COUNT];
static bool is_pad2_available = false;

void reset_scores(Thisc *table) {
	reset_table(table,"JOHAN PEITZ",1000,0);
	strcpy(table[0].name, "ALEX THE ALLEGATOR");
	strcpy(table[1].name, "AARON THE ALLEGATOR");
	strcpy(table[2].name, "JOHAN PEITZ");
	strcpy(table[3].name, "- - -");
	strcpy(table[4].name, "HAVE YOU TRIED");
	strcpy(table[5].name, "OUR DONKEY SPAM?");
	strcpy(table[6].name, "IT'S THE BEST!");
	strcpy(table[7].name, "ORDER NOW AT:");
	strcpy(table[8].name, "1-800-SPAM-R-US");
	strcpy(table[9].name, "WE TAKE VISA.");
}

int get_random(int max)
{
    if (max <= 0) return 0;
    return (jo_random(max)); // jo_random requires passing parm so we cant use modulo
}

void load_sound_config() 
{
	// start sound at full volume and music at half volume
	// sound and music are on a different scale with ponesound
	// sound is 0-7
	// music is 0-7
	sound_vol=6;
 	music_vol=4;
}

void reset_title_props()
{
	current_game_type = GAME_TYPE_HVC;
	title_menu_x=-200, title_menu_y=144;
	is_showing_start_game_options = false;
	is_instructions_selected = false;
	is_credits_selected = false;
	is_high_scores_selected = false;
	is_hvc_selected = false;
	is_cvh_selected = false;
	is_hvh_selected = false;
}

void load_intro_sprites()
{
	shlogo_sprite_id = jo_sprite_add_tga(NULL, "SHLOGO.TGA", JO_COLOR_Transparent);
}

void unload_intro_sprites()
{
	jo_sprite_free_from(shlogo_sprite_id);
}

void load_pregame_assets()
{
	select_sound_id = load_8bit_pcm((Sint8 *)"SELECT.PCM", 15360); // using ponesound due to issues with jo engine audio
	plmulti_sound_id = load_8bit_pcm((Sint8 *)"PLMULTI.PCM", 15360);
	pldead_sound_id = load_8bit_pcm((Sint8 *)"PLDEAD.PCM", 15360);
	remove_sound_id = load_8bit_pcm((Sint8 *)"REMOVE.PCM", 15360);
	rotate_sound_id = load_8bit_pcm((Sint8 *)"ROTATE.PCM", 15360);

	game_black_font = jo_font_load(NULL, "GAMEBLK.TGA", JO_COLOR_RGB(255,0,255),GAME_FONT_WIDTH, GAME_FONT_HEIGHT, 0, GAME_FONT_MAPPING);
	game_black_font->z_index = TEXT_ZINDEX;

	load_title_sprites();
	load_intro_sprites(); // load last since we will unload these which frees memory after
}

void load_title_sprites()
{
	title_sprite_id = jo_sprite_add_tga(NULL, "TITLE.TGA", JO_COLOR_Transparent);
	aa2_sprite_id = jo_sprite_add_tga(NULL, "AA2.TGA", JO_COLOR_Black);
	aalogo_sprite_id = jo_sprite_add_tga(NULL, "AALOGO.TGA", JO_COLOR_RGB(255,0,255));
	vol1_sprite_id = jo_sprite_add_tga(NULL, "VOL1.TGA", JO_COLOR_Black);
	vol2_sprite_id = jo_sprite_add_tga(NULL, "VOL2.TGA", JO_COLOR_Black);
	vol3_sprite_id = jo_sprite_add_tga(NULL, "VOL3.TGA", JO_COLOR_Black);
	pointer_sprite_id = jo_sprite_add_tga(NULL, "POINTER.TGA", JO_COLOR_Black);
	donkey1_sprite_id = jo_sprite_add_tga(NULL, "DONKEY1.TGA", JO_COLOR_Black);
	donkey2_sprite_id = jo_sprite_add_tga(NULL, "DONKEY2.TGA", JO_COLOR_Black);
	donkey3_sprite_id = jo_sprite_add_tga(NULL, "DONKEY3.TGA", JO_COLOR_Black);
	donkey4_sprite_id = jo_sprite_add_tga(NULL, "DONKEY4.TGA", JO_COLOR_Black);

	donkey_sprite_ids[0] = donkey1_sprite_id;
	donkey_sprite_ids[1] = donkey2_sprite_id;
	donkey_sprite_ids[2] = donkey3_sprite_id;
	donkey_sprite_ids[3] = donkey4_sprite_id;
}

void load_game_sprites()
{
	if (!game_sprites_loaded)
	{
		emptytkn_sprite_id = jo_sprite_add_tga(NULL, "EMPTYTKN.TGA", JO_COLOR_Black);
		greentkn_sprite_id = jo_sprite_add_tga(NULL, "GREENTKN.TGA", JO_COLOR_Transparent);
		bluetkn_sprite_id = jo_sprite_add_tga(NULL, "BLUETKN.TGA", JO_COLOR_Transparent);
		multitkn_sprite_id = jo_sprite_add_tga(NULL, "MULTITKN.TGA", JO_COLOR_Black);
		deadtkn_sprite_id = jo_sprite_add_tga(NULL, "DEADTKN.TGA", JO_COLOR_Black);
		board_sprite_id = jo_sprite_add_tga(NULL, "BOARD.TGA", JO_COLOR_Transparent);
		p1status_sprite_id = jo_sprite_add_tga(NULL, "P1STATUS.TGA", JO_COLOR_Transparent);
		p2status_sprite_id = jo_sprite_add_tga(NULL, "P2STATUS.TGA", JO_COLOR_Transparent);
		arrow1_sprite_id = jo_sprite_add_tga(NULL, "ARROW1.TGA", JO_COLOR_Black);
		arrow2_sprite_id = jo_sprite_add_tga(NULL, "ARROW2.TGA", JO_COLOR_Black);
		arrow3_sprite_id = jo_sprite_add_tga(NULL, "ARROW3.TGA", JO_COLOR_Black);
		arrow4_sprite_id = jo_sprite_add_tga(NULL, "ARROW4.TGA", JO_COLOR_Black);
		darrow1_sprite_id = jo_sprite_add_tga(NULL, "DARROW1.TGA", JO_COLOR_Black);
		darrow2_sprite_id = jo_sprite_add_tga(NULL, "DARROW2.TGA", JO_COLOR_Black);
		darrow3_sprite_id = jo_sprite_add_tga(NULL, "DARROW3.TGA", JO_COLOR_Black);
		darrow4_sprite_id = jo_sprite_add_tga(NULL, "DARROW4.TGA", JO_COLOR_Black);
		player11_sprite_id = jo_sprite_add_tga(NULL, "PLAYER11.TGA", JO_COLOR_Black);
		player12_sprite_id = jo_sprite_add_tga(NULL, "PLAYER12.TGA", JO_COLOR_Black);
		player13_sprite_id = jo_sprite_add_tga(NULL, "PLAYER13.TGA", JO_COLOR_Black);
		player14_sprite_id = jo_sprite_add_tga(NULL, "PLAYER14.TGA", JO_COLOR_Black);
		player21_sprite_id = jo_sprite_add_tga(NULL, "PLAYER21.TGA", JO_COLOR_Black);
		player22_sprite_id = jo_sprite_add_tga(NULL, "PLAYER22.TGA", JO_COLOR_Black);
		player23_sprite_id = jo_sprite_add_tga(NULL, "PLAYER23.TGA", JO_COLOR_Black);
		player24_sprite_id = jo_sprite_add_tga(NULL, "PLAYER24.TGA", JO_COLOR_Black);
		notkn_sprite_id = jo_sprite_add_tga(NULL, "NOTKN.TGA", JO_COLOR_Black);
		dust000_sprite_id = jo_sprite_add_tga(NULL, "DUST000.TGA", JO_COLOR_White);
		dust001_sprite_id = jo_sprite_add_tga(NULL, "DUST001.TGA", JO_COLOR_Transparent);
		dust002_sprite_id = jo_sprite_add_tga(NULL, "DUST002.TGA", JO_COLOR_Transparent);
		dust003_sprite_id = jo_sprite_add_tga(NULL, "DUST003.TGA", JO_COLOR_White);
		dust004_sprite_id = jo_sprite_add_tga(NULL, "DUST004.TGA", JO_COLOR_White);
		hint_sprite_id = jo_sprite_add_tga(NULL, "HINT.TGA", JO_COLOR_Black);
		end_sprite_id = jo_sprite_add_tga(NULL, "END.TGA", JO_COLOR_RGB(255,0,255));
		tglup_sprite_id = jo_sprite_add_tga(NULL, "TGLUP.TGA", JO_COLOR_RGB(255,0,255));
		tgldn_sprite_id = jo_sprite_add_tga(NULL, "TGLDN.TGA", JO_COLOR_RGB(255,0,255));
		endh_sprite_id = jo_sprite_add_tga(NULL, "ENDH.TGA", JO_COLOR_RGB(255,0,255));
		tgluph_sprite_id = jo_sprite_add_tga(NULL, "TGLUPH.TGA", JO_COLOR_RGB(255,0,255));
		tgldnh_sprite_id = jo_sprite_add_tga(NULL, "TGLDNH.TGA", JO_COLOR_RGB(255,0,255));
		game_sprites_loaded = true;

		token_sprite_ids[0] = emptytkn_sprite_id;
		token_sprite_ids[1] = greentkn_sprite_id;
		token_sprite_ids[2] = bluetkn_sprite_id;
		token_sprite_ids[3] = multitkn_sprite_id;
		token_sprite_ids[4] = deadtkn_sprite_id; 
		
		p1_anim_sprite_ids[0] = player11_sprite_id;
		p1_anim_sprite_ids[1] = player12_sprite_id;
		p1_anim_sprite_ids[2] = player13_sprite_id;
		p1_anim_sprite_ids[3] = player14_sprite_id;

		p2_anim_sprite_ids[0] = player21_sprite_id;
		p2_anim_sprite_ids[1] = player22_sprite_id;
		p2_anim_sprite_ids[2] = player23_sprite_id;
		p2_anim_sprite_ids[3] = player24_sprite_id;

		dust_sprite_ids[0] = dust000_sprite_id;
		dust_sprite_ids[1] = dust001_sprite_id;
		dust_sprite_ids[2] = dust002_sprite_id;
		dust_sprite_ids[3] = dust003_sprite_id;
		dust_sprite_ids[4] = dust004_sprite_id;
	}
}

void manage_game_assets()
{
	unload_intro_sprites(); // dump the intro sprites to save on memory
	load_game_sprites();
}

void reset_game()
{
	action_counter = 0;
	current_game_mode = MODE_LOAD;
	load_game_mode = MODE_INTRO;
	load_action = load_pregame_assets;
}

// some of the logic was written to work with allegro binary angles but we need radian angles for jo engine
jo_fixed get_radian_angle(jo_fixed binary_angle)
{
    return binary_angle * ((2 * JO_PI) / 256);
}

void init() 
{
	jo_core_init(JO_COLOR_Black);

	// initialize sound
	load_drv(ADX_MASTER_2304);
	CDDA_SetVolume(4);

	// initialize fonts - load just what's needed for loading screen
	game_white_font = jo_font_load(NULL, "GAMEWHT.TGA", JO_COLOR_RGB(255,0,255),GAME_FONT_WIDTH, GAME_FONT_HEIGHT, 0, GAME_FONT_MAPPING);
	game_white_font->z_index = TEXT_ZINDEX;

	hisc = make_table();
	reset_scores(hisc);
	load_sound_config();

	reset_game();
}

// check if input was newly pressed
bool pad_input_pressed(int pad, input_type candidate_input)
{
    switch (pad)
	{
		case 2:
			return current_pad2_input_states[candidate_input] && !previous_pad2_input_states[candidate_input];

			break;

		default:
			return current_pad1_input_states[candidate_input] && !previous_pad1_input_states[candidate_input];
		
			break;
	}
}

void reset_pad_input_states(int pad)
{
    switch (pad)
	{
		case 2:
			for (int i = 0; i < INPUT_TYPE_COUNT; i++)
			{
				previous_pad2_input_states[i] = false;
				current_pad2_input_states[i] = false;
			}

			break;

		default:
			for (int i = 0; i < INPUT_TYPE_COUNT; i++)
			{
				previous_pad1_input_states[i] = false;
				current_pad1_input_states[i] = false;
			}

			break;
	}
}

void save_previous_pad_inputstates(int pad)
{
    // save previous state
	switch (pad)
	{
		case 2: 
			for (int i = 0; i < INPUT_TYPE_COUNT; i++)
				previous_pad2_input_states[i] = current_pad2_input_states[i];

			break;

		default:
			for (int i = 0; i < INPUT_TYPE_COUNT; i++)
				previous_pad1_input_states[i] = current_pad1_input_states[i];

			break;
	}
}

// track all current and previous input states so we can check on input presses
static void update_pad_input_states(int pad)
{
    save_previous_pad_inputstates(pad);

    // read current state
    switch (pad)
	{
		case 2:
			current_pad2_input_states[INPUT_TYPE_UP] = jo_is_input_key_pressed(6, JO_KEY_UP);
			current_pad2_input_states[INPUT_TYPE_DOWN] = jo_is_input_key_pressed(6,JO_KEY_DOWN);
			current_pad2_input_states[INPUT_TYPE_LEFT] = jo_is_input_key_pressed(6,JO_KEY_LEFT);
			current_pad2_input_states[INPUT_TYPE_RIGHT] = jo_is_input_key_pressed(6,JO_KEY_RIGHT);
			current_pad2_input_states[INPUT_TYPE_START] = jo_is_input_key_pressed(6,JO_KEY_START);    
			current_pad2_input_states[INPUT_TYPE_A] = jo_is_input_key_pressed(6,JO_KEY_A);
			current_pad2_input_states[INPUT_TYPE_C] = jo_is_input_key_pressed(6,JO_KEY_C);
			current_pad2_input_states[INPUT_TYPE_Z] = jo_is_input_key_pressed(6,JO_KEY_Z);

			break;

		default:
			current_pad1_input_states[INPUT_TYPE_UP] = jo_is_pad1_key_pressed(JO_KEY_UP);
			current_pad1_input_states[INPUT_TYPE_DOWN] = jo_is_pad1_key_pressed(JO_KEY_DOWN);
			current_pad1_input_states[INPUT_TYPE_LEFT] = jo_is_pad1_key_pressed(JO_KEY_LEFT);
			current_pad1_input_states[INPUT_TYPE_RIGHT] = jo_is_pad1_key_pressed(JO_KEY_RIGHT);
			current_pad1_input_states[INPUT_TYPE_START] = jo_is_pad1_key_pressed(JO_KEY_START);    
			current_pad1_input_states[INPUT_TYPE_A] = jo_is_pad1_key_pressed(JO_KEY_A);
			current_pad1_input_states[INPUT_TYPE_C] = jo_is_pad1_key_pressed(JO_KEY_C);
			current_pad1_input_states[INPUT_TYPE_Z] = jo_is_pad1_key_pressed(JO_KEY_Z);

			break;
	}
}

bool is_pad_key_pressed(int pad, int key)
{
	switch(pad)
	{
		case 2:
			return jo_is_input_key_pressed(6,key);
			break;

		default:
			return jo_is_pad1_key_pressed(key);
			break;
	}
}

bool is_pad_available(int pad)
{
	switch (pad)
	{
		case 2:
			return jo_is_input_available(6); // seems that port 1 is 0-5 and port 2 is probably 6-11
			break;

		default:
			return jo_is_pad1_available();
			break;
	}
}

// retrieve the input type from the user
input_type get_pad_input_type(mode game_mode, int pad)
{
    input_type current_pad_input = INPUT_TYPE_NOTHING;
	if (is_pad_available(pad))
	{
		update_pad_input_states(pad);
		switch(game_mode)
		{
			case MODE_INTRO:

				if (pad_input_pressed(pad, INPUT_TYPE_START)) current_pad_input = INPUT_TYPE_START;
				else if (pad_input_pressed(pad, INPUT_TYPE_A)) current_pad_input = INPUT_TYPE_A;
				else if (pad_input_pressed(pad, INPUT_TYPE_C)) current_pad_input = INPUT_TYPE_C;

				break;

			case MODE_INSTRUCTIONS:
				if (pad_input_pressed(pad, INPUT_TYPE_START)) current_pad_input = INPUT_TYPE_START;  
				else if (pad_input_pressed(pad, INPUT_TYPE_A)) current_pad_input = INPUT_TYPE_A;
				else if (pad_input_pressed(pad, INPUT_TYPE_C)) current_pad_input = INPUT_TYPE_C;
				else if (pad_input_pressed(pad, INPUT_TYPE_LEFT)) current_pad_input = INPUT_TYPE_LEFT;
				else if (pad_input_pressed(pad, INPUT_TYPE_RIGHT)) current_pad_input = INPUT_TYPE_RIGHT;

				break;

			case MODE_HOF:
				if (pad_input_pressed(pad, INPUT_TYPE_START)) current_pad_input = INPUT_TYPE_START;  
				else if (pad_input_pressed(pad, INPUT_TYPE_A)) current_pad_input = INPUT_TYPE_A;
				else if (pad_input_pressed(pad, INPUT_TYPE_C)) current_pad_input = INPUT_TYPE_C;
				else if (pad_input_pressed(pad, INPUT_TYPE_LEFT)) current_pad_input = INPUT_TYPE_LEFT;
				else if (pad_input_pressed(pad, INPUT_TYPE_RIGHT)) current_pad_input = INPUT_TYPE_RIGHT;
				else if (pad_input_pressed(pad, INPUT_TYPE_UP)) current_pad_input = INPUT_TYPE_UP;
				else if (pad_input_pressed(pad, INPUT_TYPE_DOWN)) current_pad_input = INPUT_TYPE_DOWN;

				break;

			default:

				if (pad_input_pressed(pad, INPUT_TYPE_START)) current_pad_input = INPUT_TYPE_START;  
				else if (pad_input_pressed(pad, INPUT_TYPE_A)) current_pad_input = INPUT_TYPE_A;
				else if (pad_input_pressed(pad, INPUT_TYPE_C)) current_pad_input = INPUT_TYPE_C;
				else if (pad_input_pressed(pad, INPUT_TYPE_Z)) current_pad_input = INPUT_TYPE_Z;
				else if (is_pad_key_pressed(pad, JO_KEY_UP) && is_pad_key_pressed(pad, JO_KEY_LEFT)) current_pad_input = INPUT_TYPE_UP_LEFT;
				else if (is_pad_key_pressed(pad, JO_KEY_UP) && is_pad_key_pressed(pad, JO_KEY_RIGHT)) current_pad_input = INPUT_TYPE_UP_RIGHT;
				else if (is_pad_key_pressed(pad, JO_KEY_DOWN) && is_pad_key_pressed(pad, JO_KEY_LEFT)) current_pad_input = INPUT_TYPE_DOWN_LEFT;
				else if (is_pad_key_pressed(pad, JO_KEY_DOWN) && is_pad_key_pressed(pad, JO_KEY_RIGHT)) current_pad_input = INPUT_TYPE_DOWN_RIGHT;
				else if (is_pad_key_pressed(pad, JO_KEY_DOWN)) current_pad_input = INPUT_TYPE_DOWN;
				else if (is_pad_key_pressed(pad, JO_KEY_UP)) current_pad_input = INPUT_TYPE_UP;
				else if (is_pad_key_pressed(pad, JO_KEY_LEFT)) current_pad_input = INPUT_TYPE_LEFT;
				else if (is_pad_key_pressed(pad, JO_KEY_RIGHT)) current_pad_input = INPUT_TYPE_RIGHT;

				break;
		}
	}
	else
	{
		reset_pad_input_states(pad);
	}

    return current_pad_input;
}

void make_bg() {
	int x;

	// draw game board
	jo_set_default_background_color(JO_COLOR_RGB(199,125,125)); // pink
	jo_sprite_draw3D2(board_sprite_id, 17, 17, BACKGROUND_ZINDEX);
	jo_sprite_draw3D2(p1status_sprite_id, 240, 17, BACKGROUND_ZINDEX);
	jo_sprite_draw3D2(p2status_sprite_id, 240, 121, BACKGROUND_ZINDEX);

	// draw arrows
	for(x=0;x<8;x++) {
		jo_sprite_draw3D2(arrow1_sprite_id, 26+x*24, 6, BACKGROUND_ZINDEX);
		jo_sprite_draw3D2(arrow3_sprite_id, 26+x*24, 213, BACKGROUND_ZINDEX);
		jo_sprite_draw3D2(arrow2_sprite_id, 213, 26+x*24, BACKGROUND_ZINDEX);
		jo_sprite_draw3D2(arrow4_sprite_id, 6, 26+x*24, BACKGROUND_ZINDEX);
	}
}

void create_particle(int x,int y,int im) 
{	
   int i=0;
   while(dust[i].exist && i<MAX_PARTICLES-1) i++;  // find available i

   dust[i].x = jo_int2fixed(x); 
   dust[i].y = jo_int2fixed(y);
   dust[i].dx = jo_fixed_cos(get_radian_angle(jo_int2fixed(get_random(256)-1))); // this likely does not match what was in allegro
   dust[i].dy = jo_fixed_sin(get_radian_angle(jo_int2fixed(get_random(256)-1))); // this likely does not match what was in allegro
   dust[i].image = im;
   dust[i].exist = 1;
}

void draw_particles() {
	int i,y;

	for(i=0;i<MAX_PARTICLES;i++)
		if (dust[i].exist) {
			jo_sprite_draw3D2(dust_sprite_ids[dust[i].image], jo_fixed2int(dust[i].x)-2, jo_fixed2int(dust[i].y)-2, BACKGROUND_ZINDEX);
			// draw_sprite(swap_screen, data[DUST000+dust[i].image].dat, jo_fixed2int(dust[i].x)-2, jo_fixed2int(dust[i].y)-2);
			dust[i].x += dust[i].dx;
			dust[i].y += dust[i].dy;
			dust[i].dy += jo_fixed_sin(get_radian_angle(jo_int2fixed(2)));
			y = jo_fixed2int(dust[i].y);
			if (y > 240) dust[i].exist = 0;
		}
}

int get_right_aligned_x_coord(const jo_font *font, int right_x, float scale, char *text)
{
	return right_x - (strlen(text) * font->spacing * scale);
}

int get_center_aligned_x_coord(const jo_font *font, float scale, char *text)
{
	return JO_TV_WIDTH_2 - (strlen(text) * font->spacing * scale)/2;
}

void set_pointer_position(int pad, input_type current_pad_input)
{
	if (current_pad_input == INPUT_TYPE_LEFT || current_pad_input == INPUT_TYPE_UP_LEFT || current_pad_input == INPUT_TYPE_DOWN_LEFT)
	{
		switch(pad)
		{
			case 2:
				pointer2_x -= 2;
				if (pointer2_x < 0)
					pointer2_x = 0;

				break;

			default:
				pointer1_x -= 2;
				if (pointer1_x < 0)
					pointer1_x = 0;

				break;
		}
	}
	
	if (current_pad_input == INPUT_TYPE_RIGHT || current_pad_input == INPUT_TYPE_UP_RIGHT || current_pad_input == INPUT_TYPE_DOWN_RIGHT)
	{
		switch(pad)
		{
			case 2:
				pointer2_x += 2;
				if (pointer2_x > (JO_TV_WIDTH - 1 - POINTER_WIDTH))
					pointer2_x = JO_TV_WIDTH - 1 - POINTER_WIDTH;

				break;

			default:
				pointer1_x += 2;
				if (pointer1_x > (JO_TV_WIDTH - 1 - POINTER_WIDTH))
					pointer1_x = JO_TV_WIDTH - 1 - POINTER_WIDTH;

				break;
		}
	}
	
	if (current_pad_input == INPUT_TYPE_UP || current_pad_input == INPUT_TYPE_UP_LEFT || current_pad_input == INPUT_TYPE_UP_RIGHT)
	{
		switch(pad)
		{
			case 2: 
				pointer2_y -= 2;
				if (pointer2_y < 0)
					pointer2_y = 0;

				break;

			default:
				pointer1_y -= 2;
				if (pointer1_y < 0)
					pointer1_y = 0;

				break;
		}
	}

	if (current_pad_input == INPUT_TYPE_DOWN || current_pad_input == INPUT_TYPE_DOWN_LEFT || current_pad_input == INPUT_TYPE_DOWN_RIGHT)
	{
		switch(pad)
		{
			case 2:
				pointer2_y += 2;
				if (pointer2_y > (JO_TV_HEIGHT - 1 - POINTER_HEIGHT))
					pointer2_y = JO_TV_HEIGHT - 1 - POINTER_HEIGHT;

				break;
			
			default:
				pointer1_y += 2;
				if (pointer1_y > (JO_TV_HEIGHT - 1 - POINTER_HEIGHT))
					pointer1_y = JO_TV_HEIGHT - 1 - POINTER_HEIGHT;

				break;
		}
	}
}

void draw_game(int show_pointer) {
	int x,y;
	int anim_col = -1;
	int anim_row = -1;

	if (placeing) 
	{
		// draw animation when placing tile
		// draw small square centered and progressively get bigger
		float scale = (23.0f - placeing) / 23.0f;
		jo_sprite_change_sprite_scale_xy(scale, scale);
		int size = (int)(23.0f * scale);
		size = size > 11 ? 11 : size;
		int sprite_x = (21 + place_x * 24) + (11 - size) / 2;
		int sprite_y = (21 + place_y * 24) + (11 - size) / 2;
		jo_sprite_draw3D2(token_sprite_ids[place_type], sprite_x, sprite_y, PLACEING_ZINDEX);
		jo_sprite_restore_sprite_scale();
	}

	if (scrolling) 
	{
		// draw animation when scrolling column or row
		// scrolling is countdown offset to draw at
		// scroll_x is starting point for current column or row
		// scroll_y is starting point for current column or row
		jo_sprite_set_clipping_area(21, 21, 191, 191, BACKGROUND_ZINDEX);
		jo_sprite_enable_clipping(false);
		if (scroll_dir==1) { // up - drawing a column
			anim_col = (scroll_x-21)/24;
			for (y=0;y<8;y++)
			{
				if (board[anim_col][y].token)
				{
					jo_sprite_draw3D2(token_sprite_ids[board[anim_col][y].token], scroll_x, 21+y*24+scrolling, BACKGROUND_ZINDEX);
				}
			}
		}
		if (scroll_dir==2) { // right - drawing a row
			anim_row = (scroll_y-21)/24;
			for (x=0;x<8;x++)
			{
				if (board[x][anim_row].token)
				{
					jo_sprite_draw3D2(token_sprite_ids[board[x][anim_row].token], 21+x*24-scrolling, scroll_y, BACKGROUND_ZINDEX);
				}
			}
		}
		if (scroll_dir==3) { // down - drawing a column
			anim_col = (scroll_x-21)/24;
			for (y=0;y<8;y++)
			{
				if (board[anim_col][y].token)
				{
					jo_sprite_draw3D2(token_sprite_ids[board[anim_col][y].token], scroll_x, 21+y*24-scrolling, BACKGROUND_ZINDEX);
				}
			}
		}
		if (scroll_dir==4) { // left - drawing a row
			anim_row = (scroll_y-21)/24;
			for (x=0;x<8;x++)
			{
				if (board[x][anim_row].token)
				{
					jo_sprite_draw3D2(token_sprite_ids[board[x][anim_row].token], 21+x*24+scrolling, scroll_y, BACKGROUND_ZINDEX);
				}
			}
		}

		jo_sprite_disable_clipping();
	}

	for(x=0;x<8;x++)
		for(y=0;y<8;y++)
			if (board[x][y].token)
			{
				if (x != anim_col && y != anim_row && (!placeing || (!(x == place_x && y == place_y))))
					jo_sprite_draw3D2(token_sprite_ids[board[x][y].token], 21+x*24, 21+y*24, BACKGROUND_ZINDEX);
			}

	// draw arrows - replace if disabled
	for(x=0;x<8;x++) {
		if (x == locked_col) {
			jo_sprite_draw3D2(darrow1_sprite_id, 26+x*24, 6, BACKGROUND_ZINDEX);
			jo_sprite_draw3D2(darrow3_sprite_id, 26+x*24, 213, BACKGROUND_ZINDEX);
		}
		if (x == locked_row) {
			jo_sprite_draw3D2(darrow2_sprite_id, 213, 26+x*24, BACKGROUND_ZINDEX);
			jo_sprite_draw3D2(darrow4_sprite_id, 6, 26+x*24, BACKGROUND_ZINDEX);
		}
	}

 	// draw hint
 	if (hint & 8) {
		jo_sprite_draw3D2(hint_sprite_id, 21+hint_x*24, 21+hint_y*24, BACKGROUND_ZINDEX);
 	}

	// draw scores
	char score_string[10];
	sprintf(score_string, "%d", ply[1].score);
	int right_aligned_x = get_right_aligned_x_coord(game_white_font, 314, 0.99f, score_string);
	jo_font_print(game_black_font, right_aligned_x-1,20,0.99f, score_string);
	jo_font_print(game_white_font, right_aligned_x,19,0.99f, score_string);
	sprintf(score_string, "%d", ply[2].score);
	right_aligned_x = get_right_aligned_x_coord(game_white_font, 314, 0.99f, score_string);
	jo_font_print(game_black_font, right_aligned_x-1,122,0.99f, score_string);
	jo_font_print(game_white_font, right_aligned_x,121,0.99f, score_string);

	// draw characters
	if (ply[1].anim && !winner) 
	{
		int p1_anim_sprite = ply[1].anim_offset + (ply[1].anim&8?0:1);
		jo_sprite_draw3D2(p1_anim_sprite_ids[p1_anim_sprite], 280, 68, BACKGROUND_ZINDEX);
	}
	else 
	{
		jo_sprite_draw3D2(player11_sprite_id, 280, 68, BACKGROUND_ZINDEX);
	}
	if (ply[2].anim && !winner)
	{
		int p2_anim_sprite = ply[2].anim_offset + (ply[2].anim&8?0:1);
		jo_sprite_draw3D2(p2_anim_sprite_ids[p2_anim_sprite], 280, 180, BACKGROUND_ZINDEX);
	}
	else 
	{
		jo_sprite_draw3D2(player21_sprite_id, 280, 180, BACKGROUND_ZINDEX);
	}

	// draw avail. multi tokens
	jo_sprite_draw3D2((ply[1].multi?multitkn_sprite_id:notkn_sprite_id),245,73, BACKGROUND_ZINDEX);
	jo_sprite_draw3D2((ply[2].multi?multitkn_sprite_id:notkn_sprite_id),245,185, BACKGROUND_ZINDEX);
	
	if (ply[1].multi>1)
	{
		sprintf(score_string, "%d", ply[1].multi);
		jo_font_print(game_white_font, 245, 76, 0.5f, score_string);
	}

	if (ply[2].multi>1)
	{
		sprintf(score_string, "%d", ply[2].multi);
		jo_font_print(game_white_font, 245, 188, 0.5f, score_string);
	}

		if (show_pointer) draw_particles();

	if (show_pointer)
	{
		int mx = pointer1_x;
		int my = pointer1_y;
		bool is_player_turn = (player == 1 || (current_game_type == GAME_TYPE_CVH && player == 2));
		if (!is_player_turn)
			jo_sprite_enable_half_transparency();

		if (ply[1].carry || 
			(current_game_type == GAME_TYPE_CVH && ply[2].carry)) // pointer1 is 2nd player in this mode
			jo_sprite_draw3D2(multitkn_sprite_id, mx-11, my-11, POINTER_ZINDEX);

		jo_sprite_draw3D2(pointer_sprite_id, mx-1, my-1, POINTER_ZINDEX);
		jo_sprite_draw3D2(current_game_type == GAME_TYPE_CVH ? dust002_sprite_id:dust001_sprite_id, mx+9, my+11, POINTER_ZINDEX);

		if (!is_player_turn)
			jo_sprite_disable_half_transparency();

		if (current_game_type == GAME_TYPE_HVH)
		{
			is_player_turn = (player == 2);
			mx = pointer2_x;
			my = pointer2_y;
			if (!is_player_turn)
				jo_sprite_enable_half_transparency();

			if (ply[2].carry)
				jo_sprite_draw3D2(multitkn_sprite_id, mx-11, my-11, POINTER_ZINDEX);

			jo_sprite_draw3D2(pointer_sprite_id, mx-1, my-1, POINTER_ZINDEX);
			jo_sprite_draw3D2(dust002_sprite_id, mx+9, my+11, POINTER_ZINDEX);

			if (!is_player_turn)
				jo_sprite_disable_half_transparency();
		}
	}
}

void reset_particles() 
{
	int i;
	for(i=0;i<MAX_PARTICLES;i++)
		dust[i].exist = 0;
}

void reset_fade()
{
	current_fade_state = FADE_STATE_IN;
	fade_brightness = -255;
	fade_cooldown = MAX_COOLDOWN_COUNT;
}

void process_fade(void (*draw)(void), void (*end)(void))
{
	switch (current_fade_state)
	{
		case FADE_STATE_IN:

			if (fade_brightness < 0)
			{
				fade_brightness+=FADE_INTERVAL;
				if (fade_brightness > 0)
					fade_brightness = 0;
			}

			jo_set_screen_color_filter_a(JO_ALL_SCROLL_SCREEN, fade_brightness, fade_brightness, fade_brightness);
			(*draw)();

			if (fade_brightness == 0)
			{
				current_fade_state = FADE_STATE_NONE;
				jo_disable_all_screen_color_filter();
			}

			break;

		case FADE_STATE_OUT:
			if (fade_brightness > -255)
			{
				fade_brightness-=FADE_INTERVAL;
				if (fade_brightness < -255)
					fade_brightness = -255;
			}

			jo_set_screen_color_filter_a(JO_ALL_SCROLL_SCREEN, fade_brightness, fade_brightness, fade_brightness);
			(*draw)();

			if (fade_brightness == -255)
			{
				current_fade_state = FADE_STATE_COOLDOWN;
				jo_clear_screen();
			}

			break;

		case FADE_STATE_COOLDOWN:
			if (fade_cooldown > 0)
				fade_cooldown--;
			else
			{
				current_fade_state = FADE_STATE_DONE;
				(*end)();
			}

			break;

		default:
			(*draw)();

			break;
	}
}

void draw_title() 
{
	if (action_counter <= 1)
	{
		aa2_x = JO_TV_WIDTH;
		jo_set_default_background_color(JO_COLOR_RGB(73,97,40)); // light green
		is_showing_main_menu_options = true;
		is_showing_start_game_options = false;
	}
	else if (aa2_x > AA2_FINAL_X)
	{
		aa2_x--;
	}

	// title logos and text
	jo_sprite_draw3D2(title_sprite_id, 0, 16, TEXT_ZINDEX);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(aa2_sprite_id, aa2_x, 8, AA2_ZINDEX);
	jo_sprite_disable_half_transparency();
	jo_sprite_draw3D2(aalogo_sprite_id, 64, 48, LOGO_ZINDEX);

	// title menu options
	if (is_showing_main_menu_options || title_menu_y < JO_TV_HEIGHT)
	{
		jo_font_print(game_black_font, 39, title_menu_y + 1, 0.99f, "START GAME");
		jo_font_print(game_white_font, 40, title_menu_y, 0.99f, "START GAME");
		jo_font_print(game_black_font, 39, title_menu_y + 21, 0.99f, "HIGH SCORES");
		jo_font_print(game_white_font, 40, title_menu_y + 20, 0.99f, "HIGH SCORES");
		jo_font_print(game_black_font, 39, title_menu_y + 41, 0.99f, "INSTRUCTIONS");
		jo_font_print(game_white_font, 40, title_menu_y + 40, 0.99f, "INSTRUCTIONS");
		jo_font_print(game_black_font, 39, title_menu_y + 61, 0.99f, "CREDITS");
		jo_font_print(game_white_font, 40, title_menu_y + 60, 0.99f, "CREDITS");
	}
	
	if (is_showing_start_game_options)
	{
		jo_font_print(game_black_font, title_menu_x-1, 144 + 1, 0.99f, "HUMAN VS AARON");
		jo_font_print(game_black_font, title_menu_x-1, 164 + 1, 0.99f, "ALEX VS HUMAN");
		jo_font_print(game_white_font, title_menu_x, 144, 0.99f, "HUMAN VS AARON");
		jo_font_print(game_white_font, title_menu_x, 164, 0.99f, "ALEX VS HUMAN");
		
		if (!is_pad2_available)
			jo_sprite_enable_half_transparency();
		
		jo_font_print(game_black_font, title_menu_x-1, 184 + 1, 0.99f, "HUMAN VS HUMAN");
		jo_font_print(game_white_font, title_menu_x, 184, 0.99f, "HUMAN VS HUMAN");
		
		if (!is_pad2_available)
			jo_sprite_disable_half_transparency();
	}

	// volume controls
	jo_sprite_draw3D2(vol3_sprite_id, 280, 152, TEXT_ZINDEX);
	jo_sprite_draw3D2(vol3_sprite_id, 304, 152, TEXT_ZINDEX);
	jo_sprite_draw3D2(vol1_sprite_id, 276, 197-(sound_vol*7), TEXT_ZINDEX);
	jo_sprite_draw3D2(vol2_sprite_id, 300, 197-(music_vol*7), TEXT_ZINDEX);

	draw_donkeys();
	jo_sprite_draw3D2(pointer_sprite_id, pointer1_x, pointer1_y, POINTER_ZINDEX);
}

void draw_donkeys() 
{
	int i,x;

	for(i=0;i<MAX_PARTICLES;i++)
		if (dust[i].exist) {
			jo_sprite_draw3D2(donkey_sprite_ids[dust[i].image], jo_fixed2int(dust[i].x), jo_fixed2int(dust[i].y), DONKEY_ZINDEX);
			dust[i].x += dust[i].dx;
			dust[i].y += dust[i].dy;
			dust[i].dy += jo_fixed_sin(get_radian_angle(jo_int2fixed(2)));
			if (dust[i].dy > jo_int2fixed(2)) dust[i].dy = -dust[i].dy;
			x = jo_fixed2int(dust[i].x);
			if (x > 320) dust[i].exist = 0;
		}
}

void create_donkey(int x,int y,int im) 
{
   int i=0;

   while(dust[i].exist && i<MAX_PARTICLES-1) i++;  // find available i

   dust[i].x = jo_int2fixed(x); 
   dust[i].y = jo_int2fixed(y);
   dust[i].dx = jo_int2fixed(get_random(2)+1); // 2 or 3
   dust[i].dy = jo_int2fixed(get_random(2)); // 1 or 2
   dust[i].image = im;
   dust[i].exist = 1;
}

bool pointer_on_hvc_game_option()
{
	return (pointer1_x >= title_menu_x && 
		pointer1_x <= (title_menu_x + GAME_FONT_WIDTH * 14)) && // 14 chars
		(pointer1_y >= (144 + 8) && 
		pointer1_y <= (144 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_cvh_game_option()
{
	return (pointer1_x >= title_menu_x && 
		pointer1_x <= (title_menu_x + GAME_FONT_WIDTH * 14)) && // 14 chars
		(pointer1_y >= (164 + 8) && 
		pointer1_y <= (164 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_hvh_game_option()
{
	return (pointer1_x >= title_menu_x && 
		pointer1_x <= (title_menu_x + GAME_FONT_WIDTH * 14)) && // 14 chars
		(pointer1_y >= (184 + 8) && 
		pointer1_y <= (184 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_start_game_option(int menu_y)
{
	return (pointer1_x >= 40 && 
		pointer1_x <= (40 + GAME_FONT_WIDTH * 10)) && // 10 chars
		(pointer1_y >= (menu_y + 8) && 
		pointer1_y <= (menu_y + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_high_scores_option(int menu_y)
{
	return (pointer1_x >= 40 && 
		pointer1_x <= (40 + GAME_FONT_WIDTH * 11)) && // 11 chars
		(pointer1_y >= (menu_y + 28) && 
		pointer1_y <= (menu_y + 20 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_instructions_option(int menu_y)
{
	return (pointer1_x >= 40 && 
		pointer1_x <= (40 + GAME_FONT_WIDTH * 11)) && // 12 chars but subtract one for going too far
		(pointer1_y >= (menu_y + 48) && 
		pointer1_y <= (menu_y + 40 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_credits_option(int menu_y)
{
	return (pointer1_x >= 40 && 
		pointer1_x <= (40 + GAME_FONT_WIDTH * 7)) && // 7 chars
		(pointer1_y >= (menu_y + 68) && 
		pointer1_y <= (menu_y + 60 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_sound_vol()
{
	return (pointer1_x>275 && pointer1_x<290 && pointer1_y>149 && pointer1_y<201);
}

bool pointer_on_music_vol()
{
	return (pointer1_x>295 && pointer1_x<310 && pointer1_y>149 && pointer1_y<201);
}

void process_game_option_select()
{
	pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
	
	// only set seed once per game to maintain randomness
	if (!title_did_set_seed)
	{
		jo_random_seed = action_counter;
		title_did_set_seed = true;
	}

	current_fade_state = FADE_STATE_OUT;
}

void end_title()
{
	action_counter = 0;
	if (is_instructions_selected)
	{
		current_game_mode = MODE_INSTRUCTIONS;
	}
	else if (is_credits_selected)
	{
		current_game_mode = MODE_CREDITS;
	}
	else if (is_high_scores_selected)
	{
		current_game_mode = MODE_HIGH_SCORES;
	}
	else if (is_hvc_selected || is_cvh_selected || is_hvh_selected)
	{
		if (!game_sprites_loaded)
		{
			current_game_mode = MODE_LOAD;
			load_game_mode = MODE_GAME;
			load_action = manage_game_assets;
		}
		else
			current_game_mode = MODE_GAME;

		if (is_hvc_selected)
			current_game_type = GAME_TYPE_HVC;
		else if (is_cvh_selected)
			current_game_type = GAME_TYPE_CVH;
		else if (is_hvh_selected)
			current_game_type = GAME_TYPE_HVH;
	}
}

void title() 
{
	int x=320, y=10;

	if (action_counter <= 1)
	{
		if (did_play_game)
		{
			// we are coming off a play of the game where other music was playing
			// stop game music and start title music
			CDDA_Stop();
			CDDA_PlaySingle(TITLE_TRACKID, true);
			did_play_game = false;
		}

		reset_title_props();
		pointer1_x=JO_TV_WIDTH_2;
		pointer1_y=JO_TV_HEIGHT_2; 
		pointer2_x=JO_TV_WIDTH_2;
		pointer2_y=JO_TV_HEIGHT_2;
		//clicked;
		reset_particles();
		reset_fade();
	}

	process_fade(draw_title, end_title);
	
	if (current_fade_state == FADE_STATE_NONE)
	{
		is_pad2_available = is_pad_available(2);
		input_type current_pad1_input = get_pad_input_type(current_game_mode, 1);
		set_pointer_position(1, current_pad1_input);

		if (is_showing_start_game_options && title_menu_y<JO_TV_HEIGHT) title_menu_y+=4;
		if (is_showing_start_game_options && title_menu_x < 40) title_menu_x+= 4;

		if ((get_random(500)-1)<5) create_donkey(-40,(get_random(220)-1)+20,get_random(4)-1);

		// check if a, c, or start was pressed and the pointer is on key location
		if ((current_pad1_input == INPUT_TYPE_START || 
			current_pad1_input == INPUT_TYPE_A || 
			current_pad1_input == INPUT_TYPE_C))
		{
			if (pointer_on_sound_vol())
			{
				int sv=sound_vol;
				sound_vol=(pointer1_y >= 197) ? 0 : (int)((197 - pointer1_y) / 7);	// fx
				if (sound_vol!=sv) 
				{
					pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
				}
			}
			else if (pointer_on_music_vol())
			{
				int mv=music_vol;
				music_vol=(pointer1_y >= 197) ? 0 : (int)((197 - pointer1_y) / 7);	// music
				if (mv!=music_vol) {
					CDDA_SetVolume(music_vol);
				}
			}
			else if (is_showing_main_menu_options)
			{
				if (pointer_on_start_game_option(title_menu_y))
				{
					pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
					is_showing_start_game_options = true;
					is_showing_main_menu_options = false;
				}
				else if (pointer_on_instructions_option(title_menu_y))
				{
					pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
					current_fade_state = FADE_STATE_OUT;
					is_instructions_selected = true;
				}
				else if (pointer_on_credits_option(title_menu_y))
				{
					pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
					current_fade_state = FADE_STATE_OUT;
					is_credits_selected = true;
				}
				else if (pointer_on_high_scores_option(title_menu_y))
				{
					pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
					current_fade_state = FADE_STATE_OUT;
					is_high_scores_selected = true;
				}
			}
			else if (is_showing_start_game_options)
			{
				if (pointer_on_hvc_game_option(title_menu_y))
				{
					process_game_option_select();
					is_hvc_selected = true;
					cpu = 2;
				}
				else if (pointer_on_cvh_game_option(title_menu_y))
				{
					process_game_option_select();
					is_cvh_selected = true;
					cpu = 1;
				}
				else if (pointer_on_hvh_game_option(title_menu_y) && is_pad2_available)
				{
					process_game_option_select();
					is_hvh_selected = true;
					cpu = 0;
				}
			}
		}
	}

}

void start_new_game() {
	int x,y;

	for(x=0;x<8;x++)
		for(y=0;y<8;y++)
			board[x][y] = empty_square;

	board[3][3] = green_token;
	board[4][4] = green_token;
	board[3][4] = blue_token;
	board[4][3] = blue_token;

	playing = 1;
	player = 1;
	// scrolling = 0;
	winner = 0;
	winner_presses = 0;
	locked_col = locked_row = -1;
	hint = 0;

	ply[1] = ply[2] = reset_player;

	reset_particles();
}

int rotate_row(int row, int go_left) {
	int i;
	Ttoken tmp;

	// move tokens
	if (go_left) {
		tmp = board[0][row];
		for(i=0;i<7;i++) board[i][row] = board[i+1][row];
		board[7][row] = tmp;
	}
	else {
		tmp = board[7][row];
		for(i=7;i>0;i--) board[i][row] = board[i-1][row];
		board[0][row] = tmp;
	}

	return 1;
}

int anim_rotate_row(int row, int go_left) {
	// setup scrolling area
	scroll_dir = (go_left?4:2);

	scroll_x = 20;
	scroll_y = 21+row*24;
	scrolling = 24;

	rotate_row(row,go_left);
		
	playing = 0;
	pcm_play(rotate_sound_id, PCM_PROTECTED, sound_vol);
	locked_row = row;
	locked_col = -1;

	return 1;
}

int rotate_column(int col, int go_up) 
{
	int i;
	Ttoken tmp;

	// move tokens
	if (go_up) {
		tmp = board[col][0];
		for(i=0;i<7;i++) board[col][i] = board[col][i+1];
		board[col][7] = tmp;
	}
	else {
		tmp = board[col][7];
		for(i=7;i>0;i--) board[col][i] = board[col][i-1];
		board[col][0] = tmp;
	}

	return 1;
}

int anim_rotate_column(int col, int go_up) 
{
	// setup scrolling area
	scroll_dir = (go_up?1:3);

	scroll_x = 21+col*24;
	scroll_y = 20;
	scrolling = 24;

	rotate_column(col,go_up);

	playing=0;
	locked_col = col;
	locked_row = -1;
	
	pcm_play(rotate_sound_id, PCM_PROTECTED, sound_vol);

	return 1;
}

int place_token(int x, int y, int type) {
	if (board[x][y].token != 0) return 0;

	if (type == BLUETOKEN) board[x][y] = blue_token;
	else if (type == GREENTOKEN) board[x][y] = green_token;
	else if (type == MULTITOKEN) board[x][y] = multi_token;
	else if (type == DEADTOKEN) board[x][y] = dead_token;

	return 1;
}

int anim_place_token(int x, int y, int type) {
	if (!place_token(x,y,type)) return 0;

	short sound_id = (type<3 ? select_sound_id : (type==3 ? plmulti_sound_id : pldead_sound_id));
	pcm_play(sound_id, PCM_PROTECTED, sound_vol);

	place_x = x;
	place_y = y;
	placeing = 24;
	place_type = type;
	playing = 0;

	return 1;
}

int get_token(int x, int y, int p) 
{
	int t = board[x][y].token;
	if (t==DEADTOKEN) t = 0;
	if (t==MULTITOKEN) t = p;
	return t;
}

void flag_square(int x, int y) 
{
	board[x][y].flag = 
	board[x+1][y].flag = 
	board[x+1][y+1].flag = 
	board[x][y+1].flag = 1;
}

int check_board_score() 
{
	int x,y;
	int me;
	int acc_score = 0;

	// unflag tokens
	for(x=0;x<8;x++)
		for(y=0;y<8;y++) 
			board[x][y].flag = 0;

	// flag all tokens that are in a four
	for(x=0;x<8;x++)
		for(y=0;y<8;y++) {
			me = board[x][y].token;
			if (me) 
			{
				if (x<7 && y<7)
					if (get_token(x+1,y,me) == me && get_token(x+1,y+1,me) == me && get_token(x,y+1,me) == me) flag_square(x,y);
				if (x>0 && y<7)
					if (get_token(x-1,y,me) == me && get_token(x-1,y+1,me) == me && get_token(x,y+1,me) == me) flag_square(x-1,y);
				if (x<7 && y>0)
					if (get_token(x+1,y,me) == me && get_token(x+1,y-1,me) == me && get_token(x,y-1,me) == me) flag_square(x,y-1);
				if (x>0 && y>0)
					if (get_token(x-1,y,me) == me && get_token(x-1,y-1,me) == me && get_token(x,y-1,me) == me) flag_square(x-1,y-1);
			}
		}

	// score flagged tokens
	me = 0;
	for(x=0;x<8;x++)
		for(y=0;y<8;y++) 
			if (board[x][y].flag) 
			{
				acc_score += board[x][y].score;
				me++;
			}

	// get plenty-bonus
	if (me) acc_score += (me-1)*(me+1);

	return acc_score;
}

int check_end() {
	int x,y;
	int p1=0,p2=0;

	check_board_score(); // force flagging of tokens are to be removed
	for(x=0;x<8;x++)
		for(y=0;y<8;y++) {
			if (board[x][y].token==1 && !board[x][y].flag) p1++;
			if (board[x][y].token==2 && !board[x][y].flag) p2++;
		}

	if (!p1 && !p2) return 3;
	if (!p2) return 2;
	if (!p1) return 1;
	return 0;
}

void check_board(byte player) 
{
	int x,y;
	int me,i;
	int acc_score;
	int p1=0,p2=0;

	// unflag tokens
	for(x=0;x<8;x++)
		for(y=0;y<8;y++) 
			board[x][y].flag = 0;

	acc_score = check_board_score();			// flags tokens
	
	// remove flagged tokens
	me = 0;
	for(x=0;x<8;x++)
		for(y=0;y<8;y++) 
			if (board[x][y].flag) 
			{
				for(i=0;i<10;i++) create_particle(20+x*24+(get_random(24)-1),20+y*24+(get_random(24)-1),board[x][y].token);
				board[x][y] = empty_square;
				me++;
			}

	if (me) 
	{
		pcm_play(remove_sound_id, PCM_PROTECTED, sound_vol);
		ply[player].anim=100;
		ply[player].anim_offset=0;
		if (me>4) {
			ply[player].multi++;
			ply[player].anim_offset=2;
			ply[player].anim=200;
		}
	}

	ply[player].score += acc_score;

	for(x=0;x<8;x++)
		for(y=0;y<8;y++) 
		{
			if (board[x][y].token==1 && !board[x][y].flag) p1++;
			if (board[x][y].token==2 && !board[x][y].flag) p2++;
		}

	if (!p1 || !p2) winner=1;
}

void draw_high_scores() {
	int i;	

	jo_sprite_draw3D2(title_sprite_id, 0, 16, TEXT_ZINDEX);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(aa2_sprite_id, AA2_FINAL_X, 8, AA2_ZINDEX);
	jo_sprite_disable_half_transparency();

	int title_y = -60;
	jo_font_print_centered(game_black_font, -1, title_y+1, 0.99f, "ALL TIME HIGH");
	jo_font_print_centered(game_white_font, 0, title_y, 0.99f, "ALL TIME HIGH");

	char score_string[10];
	for(i=0;i<MAX_SCORES;i++) {
		jo_font_print(game_black_font, 3,66+i*16,0.5f, hisc[i].name);
		jo_font_print(game_white_font, 4,65+i*16,0.5f, hisc[i].name);
		sprintf(score_string, "%d", hisc[i].score);
		int right_aligned_x = get_right_aligned_x_coord(game_white_font, 300, 0.5f, score_string);
		jo_font_print(game_black_font, right_aligned_x-1,66+i*16,0.5f, score_string);
		jo_font_print(game_white_font, right_aligned_x,65+i*16,0.5f, score_string);
	}
	
	draw_donkeys();
}

void end_high_scores()
{
	action_counter = 0;
	current_game_mode = MODE_TITLE;
}

void high_scores() 
{
	if (action_counter <= 1)
	{
		reset_particles();
		jo_clear_screen();
		jo_set_default_background_color(JO_COLOR_RGB(40,81,97)); // blue
		reset_fade();
	}
	
	process_fade(draw_high_scores, end_high_scores);

	if (current_fade_state == FADE_STATE_NONE)
	{
		if ((get_random(500)-1)<5) create_donkey(-40,(get_random(220)-1)+20,get_random(4)-1);

		input_type current_pad1_input = get_pad_input_type(current_game_mode, 1);
		input_type current_pad2_input = get_pad_input_type(current_game_mode, 2);
		if (current_pad1_input == INPUT_TYPE_START ||
			current_pad1_input == INPUT_TYPE_A || 
			current_pad1_input == INPUT_TYPE_C || 
			(did_play_game && current_game_type == GAME_TYPE_HVH && current_pad2_input == INPUT_TYPE_START) || 
			(did_play_game && current_game_type == GAME_TYPE_HVH && current_pad2_input == INPUT_TYPE_A) || 
			(did_play_game && current_game_type == GAME_TYPE_HVH && current_pad2_input == INPUT_TYPE_C))
		{
			// user wants to return to title so start fading out
			current_fade_state = FADE_STATE_OUT;
		}
	}
}

bool does_any_score_hof_qualify()
{
	Thisc tmp;
	bool player_hof_qualified = false;
	tmp.score = ply[1].score;
	if (qualify_table(hisc, tmp) && cpu!=1) player_hof_qualified = true;
	if (!player_hof_qualified)
	{
		tmp.score = ply[2].score;
		if (qualify_table(hisc, tmp) && cpu!=2) player_hof_qualified = true;
	}

	return player_hof_qualified;
}

void draw_hof()
{
	jo_sprite_draw3D2(title_sprite_id, 0, 16, TEXT_ZINDEX);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(aa2_sprite_id, AA2_FINAL_X, 8, AA2_ZINDEX);
	jo_sprite_disable_half_transparency();

	int title_y = -60;
	jo_font_print_centered(game_black_font, -1, title_y+1, 0.99f, "ALL TIME HIGH");
	jo_font_print_centered(game_white_font, 0, title_y, 0.99f, "ALL TIME HIGH");

	jo_sprite_draw3D2(token_sprite_ids[hof_p], 0, 80, TEXT_ZINDEX);
	jo_sprite_draw3D2(token_sprite_ids[hof_p], 320-24, 80, TEXT_ZINDEX);
	char score_string[35];
	sprintf(score_string, "PLAYER %d, YOU GOT A HIGH SCORE!", hof_p);
	jo_font_print_centered(game_black_font, -1, -28+1, 0.50f, score_string);
	jo_font_print_centered(game_white_font, 0, -28, 0.50f, score_string);

	int center_x_coord = get_center_aligned_x_coord(game_white_font, 0.99f, "AAAA");
	jo_sprite_draw3D2(hof_selected_index == 0 ? tgluph_sprite_id : tglup_sprite_id, center_x_coord, 104, TEXT_ZINDEX);
	jo_sprite_draw3D2(hof_selected_index == 1 ? tgluph_sprite_id : tglup_sprite_id, center_x_coord + 18, 104, TEXT_ZINDEX);
	jo_sprite_draw3D2(hof_selected_index == 2 ? tgluph_sprite_id : tglup_sprite_id, center_x_coord + 36, 104, TEXT_ZINDEX);
	jo_font_printf(game_black_font, center_x_coord-1, 117, 0.99f, "%c", hof_chars[hof_char_indexes[0]]);
	jo_font_printf(game_white_font, center_x_coord, 116, 0.99f, "%c", hof_chars[hof_char_indexes[0]]);		
	jo_font_printf(game_black_font, center_x_coord+18-1, 117, 0.99f, "%c", hof_chars[hof_char_indexes[1]]);
	jo_font_printf(game_white_font, center_x_coord + 18, 116, 0.99f, "%c", hof_chars[hof_char_indexes[1]]);		
	jo_font_printf(game_black_font, center_x_coord+36-1, 117, 0.99f, "%c", hof_chars[hof_char_indexes[2]]);
	jo_font_printf(game_white_font, center_x_coord + 36, 116, 0.99f, "%c", hof_chars[hof_char_indexes[2]]);		
	jo_sprite_change_sprite_scale_xy(0.50f, 0.50f);
	jo_sprite_draw3D2(hof_selected_index == 3 ? endh_sprite_id : end_sprite_id, center_x_coord + 42, 110, TEXT_ZINDEX);
	jo_sprite_restore_sprite_scale();
	jo_sprite_draw3D2(hof_selected_index == 0 ? tgldnh_sprite_id : tgldn_sprite_id, center_x_coord, 140, TEXT_ZINDEX);
	jo_sprite_draw3D2(hof_selected_index == 1 ? tgldnh_sprite_id : tgldn_sprite_id, center_x_coord + 18, 140, TEXT_ZINDEX);
	jo_sprite_draw3D2(hof_selected_index == 2 ? tgldnh_sprite_id : tgldn_sprite_id, center_x_coord + 36, 140, TEXT_ZINDEX);
}

void end_hof()
{
	Thisc post;
	post.score = hof_score;
	post.name[0] = hof_chars[hof_char_indexes[0]];
	post.name[1] = hof_chars[hof_char_indexes[1]];
	post.name[2] = hof_chars[hof_char_indexes[2]];
	post.name[3] = '\0';
	enter_table(hisc,post);
	sort_table(hisc);
	ply[hof_p].score = 0; // reset player score so we dont register it again
	action_counter = 0;
	if (does_any_score_hof_qualify())
	{
		current_game_mode = MODE_HOF;
	}
	else
	{
		current_game_mode = MODE_HIGH_SCORES;
	}
}

void hof()
{
	// check player 1 and player 2 to figure out who needs to enter their score
	// we might need to enter one and then the other
	// when we are done we need to switch to high score mode
	if (action_counter <= 1)
	{
		Thisc tmp;
		hof_score = 0;
		hof_p = 1;
		hof_selected_index = 0;
		hof_char_indexes[0] = 0;
		hof_char_indexes[1] = 0;
		hof_char_indexes[2] = 0;
		if (ply[1].score>=ply[2].score) 
		{
			tmp.score = ply[1].score;
			if (qualify_table(hisc, tmp) && cpu!=1) 
			{
				hof_p = 1;
				hof_score = tmp.score;
			}
			else
			{
				tmp.score = ply[2].score;
				if (qualify_table(hisc, tmp) && cpu!=2) 
				{
					hof_p = 2;
					hof_score = tmp.score;
				}
			}
		}
		else 
		{
			tmp.score = ply[2].score;
			if (qualify_table(hisc, tmp) && cpu!=2)
			{
				hof_p = 2;
				hof_score = tmp.score;
			}
			else
			{
				tmp.score = ply[1].score;
				if (qualify_table(hisc, tmp) && cpu!=1) 
				{
					hof_p = 1;
					hof_score = tmp.score;
				}
			}
		}

		jo_clear_screen();
		int background_color = (hof_p==1 ? JO_COLOR_RGB(73,97,40) : JO_COLOR_RGB(40,81,97));
		jo_set_default_background_color(background_color);
		reset_fade();
	}

	process_fade(draw_hof, end_hof);

	if (current_fade_state == FADE_STATE_NONE)
	{
		input_type current_pad_input = get_pad_input_type(current_game_mode, current_game_type == GAME_TYPE_HVH && hof_p == 2 ? 2 : 1);

		if (current_pad_input == INPUT_TYPE_START ||
			current_pad_input == INPUT_TYPE_A || 
			current_pad_input == INPUT_TYPE_C)
		{
			if (hof_selected_index == HOF_MAX_INDEX)
			{
				current_fade_state = FADE_STATE_OUT;
				pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
			}
		}
		else if (current_pad_input == INPUT_TYPE_LEFT)
		{
			if (hof_selected_index > 0)
				hof_selected_index--;
		}
		else if (current_pad_input == INPUT_TYPE_RIGHT)
		{
			if (hof_selected_index < HOF_MAX_INDEX)
				hof_selected_index++;
		}
		else if (current_pad_input == INPUT_TYPE_DOWN && hof_selected_index < HOF_MAX_INDEX)
		{
			if (hof_char_indexes[hof_selected_index] > 0)
				hof_char_indexes[hof_selected_index]--;
			else
				hof_char_indexes[hof_selected_index] = HOF_CHARS_COUNT - 1;
		}
		else if (current_pad_input == INPUT_TYPE_UP && hof_selected_index < HOF_MAX_INDEX)
		{
			if (hof_char_indexes[hof_selected_index] == (HOF_CHARS_COUNT - 1))
				hof_char_indexes[hof_selected_index] = 0;
			else
				hof_char_indexes[hof_selected_index]++;
		}
	}
}

int loser_warning(int player, int pos_score) {
	int p = player;
	int o = (player==1?2:1);
	int e = check_end();

	if (ply[p].score+pos_score>ply[o].score && e) return 100000;  // I have more score, I will win
	if (ply[p].score+pos_score>ply[o].score) return 0;  // I have more score, all is well
	if (!e) return 0; // The game is not done, a'right
	return -10000; // he will win!!! danger!
}

int get_hint(int player, int recurse) 
{
	int x,y,i,j;
	int best_score = 0;
	int crisis = 0;
	int cx=-1,cy=-1;
	int tmp_score;
	int s_array[5] = {1,0,0,10,-2};
	int move = 0; // 1 = slide, 2 = place, 3 = random
	
	s_array[player] = 8;
	s_array[(player==1?2:1)] = -1;

	hint_x = get_random(8)-1; // gives us 0 to 7 
	hint_y = get_random(8)-1; // gives us 0 to 7

	hint = 60;

// 	// check for good slide moves
// 	for(x=0;x<8;x++) {
// 		if (x!=locked_row) {
// 			rotate_row(x,0);  // right slide
// 			tmp_score = check_board_score(); 	// check possible score
// 			tmp_score += loser_warning(player,tmp_score);   // add win warnings
// 			rotate_row(x,1);	// reset board
// 			if (tmp_score<0) { crisis=2; cx=x; cy=2; }
// 			if (tmp_score > best_score) {
// 				hint_x = 8;
// 				hint_y = x;
// 				best_score = tmp_score;
// 				move = 1;
// 				if (best_score>=100000) return 100000;
// 			}
// 			rotate_row(x,1); // left slide
// 			tmp_score = check_board_score(); 	// check possible score
// 			tmp_score += loser_warning(player,tmp_score);   // add win warnings
// 			rotate_row(x,0);	// reset board
// 			if (tmp_score<0) { crisis=2; cx=x; cy=4; }
// 			if (tmp_score > best_score) {
// 				hint_x = -1;
// 				hint_y = x;
// 				best_score = tmp_score;
// 				move = 1;
// 				if (best_score>=100000) return 100000;
// 			}
// 		}
// 		if (x!=locked_col) {
// 			rotate_column(x,0); // down slide
// 			tmp_score = check_board_score(); 	// check possible score
// 			tmp_score += loser_warning(player,tmp_score);   // add win warnings
// 			rotate_column(x,1);	// reset board
// 			if (tmp_score<0) { crisis=2; cx=x; cy=3; }
// 			if (tmp_score > best_score) {
// 				hint_x = x;
// 				hint_y = 8;
// 				best_score = tmp_score;
// 				move = 1;
// 				if (best_score>=100000) return 100000;
// 			}
// 			rotate_column(x,1);  // up slide
// 			tmp_score = check_board_score(); 	// check possible score
// 			tmp_score += loser_warning(player,tmp_score);   // add win warnings
// 			rotate_column(x,0);	// reset board
// 			if (tmp_score<0) { crisis=2; cx=x; cy=1; }
// 			if (tmp_score > best_score) {
// 				hint_x = x;
// 				hint_y = -1;
// 				best_score = tmp_score;
// 				move = 1;
// 				if (best_score>=100000) return 100000;
// 			}
// 		}
// 	}

	// check for good token positions
	for(x=0;x<8;x++)
		for(y=0;y<8;y++)
			if (!board[x][y].token) {
				place_token(x,y,player);
				tmp_score = check_board_score(); 	// check possible score
				tmp_score += loser_warning(player,tmp_score);   // add win warnings
				board[x][y] = empty_square;		// remove temporary token
				if (tmp_score<0) { crisis=1; cx=x; cy=y; }
				if (tmp_score > best_score) {
					hint_x = x;
					hint_y = y;
					best_score = tmp_score;
					move = 2;
					if (best_score>=100000) return 100000;
				}
			}

// 	if (best_score==0 && !crisis) { // no good positions where found -> conglomerate
// 		int numMoves=0;
// 		for(x=0;x<8;x++)
// 			for(y=0;y<8;y++)
// 				if (!board[x][y].token) {  // empty slot, search surroundings
// 					numMoves++;
// 					tmp_score = 0;
// 					for(i=MAX(x-1,0);i<MIN(8,x+2);i++)
// 						for(j=MAX(y-1,0);j<MIN(8,y+2);j++) 
// 							tmp_score += (x==i||y==j ? 2 : 1) * s_array[board[i][j].token];
// 					place_token(x,y,player);
// 					board[x][y] = empty_square;		// remove temporary token
// 					if (tmp_score > best_score) {
// 						hint_x = x;
// 						hint_y = y;
// 						best_score = tmp_score;
// 						move = 3;
// 						if (best_score>=100000) return 100000;
// 					}
// 				}
// 		if (numMoves==0) crisis=3;
// 	}

	
	// // check if opponent can win next turn
	// if (recurse && !crisis && move>1) {
	// 	int ox,oy;
	// 	ox = hint_x;	// backup own move
	// 	oy = hint_y;  // backup own move

	// 	// do move (place)
	// 	if (place_token(ox,oy,player)) {			// make move if available
	// 		i = get_hint((player==1?2:1),0);		// no recurse!!!
	// 		board[ox][oy] = empty_square;		// remove temporary token
	// 		if (i < 10000) {					// opponent can't win next time, use own move
	// 			hint_x = ox;	
	// 			hint_y = oy;  
	// 		}	
	// 		else {  // opponent can win, stop him!
	// 			crisis = 1;
	// 		}
	// 	}
	// 	else 
	// 		crisis = 3;   // can't find spot, make random slide
	// }

// 	if (crisis==1) { // opponent can win by placing a token
// 		get_hint((player==1?2:1),0); // find out where and put it there
// 	}
// 	if (crisis==2) {  // opponent can win by sliding -> must slide other way
// 		if (cy==1) { hint_x = cx; hint_y = 8; }
// 		if (cy==2) { hint_y = cx; hint_x = -1; }
// 		if (cy==3) { hint_x = cx; hint_y = -1; }
// 		if (cy==4) { hint_y = cx; hint_x = 8; }
// 	}
// 	if (crisis==3) {  // can't find good slide and board is full -> random slide
// 		int r = rand()%100;
// 		if (r>75) {	hint_x = rand()%8; hint_y = 8; }
// 		else if (r>50) { hint_x = rand()%8; hint_y = -1; }
// 		else if (r>25) { hint_y = rand()%8; hint_x = -1; }
// 		else { hint_y = rand()%8; hint_x = 8; }
// 	}

	// possible temporary fail safe to look for empty location
	// we were sometimes seeing a hint on top of a block but some code was commented
	if (board[hint_x][hint_y].token)
	{
		for(x=0;x<8;x++)
			for(y=0;y<8;y++)
				if (!board[x][y].token) 
				{
					hint_x = x;
					hint_y = y;
				}
	}

	return best_score;
}

void play() 
{
	int done = 0;
	int x,y;
	int mx,my;

	if (action_counter <= 1)
	{
		did_play_game = true;
		CDDA_Stop();
		jo_clear_screen();
		jo_set_default_background_color(JO_COLOR_INDEX_Black);

		return; // give the screen a chance to clear before we do sprite loading
	}
	else if (action_counter == 2)
	{
		thinking = 0;
		
		int song_choice = get_random(4);
		switch (song_choice)
		{
			case 1:
				CDDA_PlaySingle(SONG1_TRACKID, true);
				break;
			case 2:
				CDDA_PlaySingle(SONG2_TRACKID, true);
				break;
			case 3:
				CDDA_PlaySingle(SONG3_TRACKID, true);
				break;
			default:
				CDDA_PlaySingle(SONG4_TRACKID, true);
				break;
		}
	
		start_new_game();
	}

	make_bg();
	
	input_type current_pad1_input = get_pad_input_type(current_game_mode, 1);
	input_type current_pad2_input = get_pad_input_type(current_game_mode, 2);

	set_pointer_position(1, current_pad1_input);
	if (current_game_type == GAME_TYPE_HVH)
	{
		set_pointer_position(2, current_pad2_input);
	}

	draw_game(1);
// 	fade_in(data[GAMEPAL].dat,4);

// 	while (!done && !winner) {
	if (!done && !winner)
	{
		if (ply[1].anim) ply[1].anim--;
		if (ply[2].anim) ply[2].anim--;

		if (hint && !jo_is_pad1_key_pressed(JO_KEY_Z)) hint--; // hold hint steady if z is held

		if (scrolling) if (--scrolling==0) {
			playing=1;
			check_board(player);
			player = (player==1?2:1); // next player
		}

		if (placeing) 
		{
			placeing -= 2;
			if (placeing==0) 
			{
				playing=1;
				check_board(player);
				player = (player==1?2:1); // next player
				token_count++;
				if (token_count==10) {
					token_count=0;
					if (stone_count<10) {
						if (!anim_place_token(get_random(8)-1,get_random(8)-1,4)) {  // can't place stone, try next time
							token_count = 9;
						} 
						else  {// stone placed
							stone_count++;
							player = (player==1?2:1); // adjust player
						}
					}
				}
			}
		}

		if (cpu==player && playing && !thinking) thinking = 30;
		if (cpu==player && thinking) thinking--;
		if (thinking==1) 
		{
			thinking=0;
			get_hint(player,3);
// 			if (hint_x>7 || hint_y>7 || hint_x<0 || hint_y<0) { // slide
// 				if (hint_x<0) anim_rotate_row(hint_y, 1);
// 				if (hint_x>7) anim_rotate_row(hint_y, 0);
// 				if (hint_y<0) anim_rotate_column(hint_x, 1);
// 				if (hint_y>7) anim_rotate_column(hint_x, 0);
// 			}
// 			else { // place
				if (ply[player].multi) 
				{  // use multi if available
					ply[player].multi--;
					ply[player].carry++;
				}
				anim_place_token(hint_x,hint_y,(ply[player].carry?3:player));
				locked_col = locked_row = -1;
				ply[player].carry = 0;
// 			}
		}

		// check if the user simulated a mouse click
		if (player != cpu)
		{
			if ((player == 1 && current_pad1_input == INPUT_TYPE_A) || // non cpu player 1 is always pad1
				(player == 1 && current_pad1_input == INPUT_TYPE_C) || 
				(player == 2 && current_game_type == GAME_TYPE_CVH && current_pad1_input == INPUT_TYPE_A) || // player 2 is pad1 in cvh
				(player == 2 && current_game_type == GAME_TYPE_CVH && current_pad1_input == INPUT_TYPE_C) ||
				(player == 2 && current_game_type == GAME_TYPE_HVH && current_pad2_input == INPUT_TYPE_A) || // player 2 is pad2 in hvh
				(player == 2 && current_game_type == GAME_TYPE_HVH && current_pad2_input == INPUT_TYPE_C))
			{
				mx = ((player == 1 || current_game_type == GAME_TYPE_CVH) ? pointer1_x : pointer2_x);
				my = ((player == 1 || current_game_type == GAME_TYPE_CVH) ? pointer1_y : pointer2_y);
				if (playing)
				{
					// check board
					for(x=0;x<8;x++)
						for(y=0;y<8;y++)
							if (mx>21+x*24 && mx<44+x*24 && my>21+y*24 && my<44+y*24) 
							{
								if (anim_place_token(x,y,(ply[player].carry?3:player))) 
								{
									locked_col = locked_row = -1;
									ply[player].carry = 0;
								}
							}

					// check arrows
					if (!ply[player].carry) 
						for(x=0;x<8;x++) {
							int moved = 0;
							if (mx>27+x*24 && mx<37+x*24 && my>6 && my<16 && locked_col!=x) moved = anim_rotate_column(x, 1);
							if (mx>27+x*24 && mx<37+x*24 && my>216 && my<226 && locked_col!=x) moved = anim_rotate_column(x, 0);
							if (mx>6 && mx<16 && my>27+x*24 && my<37+x*24 && locked_row!=x) moved = anim_rotate_row(x, 1);
							if (mx>216 && mx<226 && my>27+x*24 && my<37+x*24 && locked_row!=x) moved = anim_rotate_row(x, 0);
						}

					// check other (multi)
					if (ply[player].multi && !ply[player].carry) {
						if (mx>245 && mx<268 && my>73+112*(player-1) && my<96+112*(player-1)) {
							ply[player].multi--;
							ply[player].carry = 1;
						}
					}
					else if (ply[player].carry) {
						if (mx>245 && mx<268 && my>73+112*(player-1) && my<96+112*(player-1)) {
							ply[player].multi ++;
							ply[player].carry = 0;
						}
					}
				}
			}
			else if ((player == 1 && current_pad1_input == INPUT_TYPE_Z) || 
				(player == 2 && current_game_type == GAME_TYPE_CVH && current_pad1_input == INPUT_TYPE_Z) || 
				(player == 2 && current_game_type == GAME_TYPE_HVH && current_pad2_input == INPUT_TYPE_Z))
			{
				get_hint(player,3);
			}
		}

// 		if (key[KEY_ESC]) done = confirm("Really quit? (Y/N)");
	}

	if (winner) 
	{
		char buf[128];

		winner = (ply[1].score>ply[2].score ? 1:2);
		done = 0;
		if (ply[1].score == ply[2].score) winner = 3;
		else ply[winner].anim = 100000;
		if (!cpu) sprintf(buf,"PLAYER %d WON THE GAME!",winner);
		else 
		{
			if (cpu==winner) 
			{
				if (winner==1) sprintf(buf,"%s WON THE GAME!","ALEX");
				if (winner==2) sprintf(buf,"%s WON THE GAME!","AARON");
			}
			else sprintf(buf,"%s WON THE GAME!","YOU");
		}

		if (!done)
		{
			if (winner_presses==0) 
			{
				jo_font_print_centered(game_black_font, -1, 1, 0.99f, "BOARD CLEARED!");
				jo_font_print_centered(game_white_font, 0, 0, 0.99f, "BOARD CLEARED!");
			}
			if (winner_presses==1) 
			{ 
				if (winner<3) 
				{
					jo_font_print_centered(game_black_font, -1, 1, 0.90f, buf);
					jo_font_print_centered(game_white_font, 0, 0, 0.90f, buf);
				}
				else 
				{
					jo_font_print_centered(game_black_font, -1, 1, 0.99f, "IT'S A DRAW!");
					jo_font_print_centered(game_white_font, 0, 0, 0.99f, "IT'S A DRAW!");
				}
			}
			if (winner_presses==2) done = 1;
			if (current_pad1_input == INPUT_TYPE_A || 
				current_pad1_input == INPUT_TYPE_C || 
				current_pad1_input == INPUT_TYPE_START || 
				(current_game_type == GAME_TYPE_HVH && current_pad2_input == INPUT_TYPE_A) || 
				(current_game_type == GAME_TYPE_HVH && current_pad2_input == INPUT_TYPE_C) || 
				(current_game_type == GAME_TYPE_HVH && current_pad2_input == INPUT_TYPE_START))
			{
				winner_presses++;
			}

		}
// 		fade_out(4);

		if (done)
		{
			// prepare to prompt for initials or display high scores
			action_counter = 0;
			
			if (does_any_score_hof_qualify())
			{
				current_game_mode = MODE_HOF;
			}
			else
			{
				current_game_mode = MODE_HIGH_SCORES;
			}
		}
	}

// 	fade_out(4);
}

// int fadeText(char *txt, int msecs) {
// 	int i;
// 	set_palette(black_palette);
// 	clear(screen);
// 	textout_centre(screen,data[MYFONT].dat,txt,160,100,-1);
// 	fade_in(data[GAMEPAL].dat,8);
// 	i = myRest(msecs);
// 	fade_out(8);
// 	clear(screen);
// 	return i;
// }

static void draw_tile(int x, int y, int sprite_id, int z, int angle)
{
    if (angle == 0)
        jo_sprite_draw3D2(sprite_id, x, y, z);
    else
        jo_sprite_draw3D_and_rotate2(sprite_id, x, y, z, angle);

#if JO_DEBUG
    jo_printf_with_color(0, 0, JO_COLOR_INDEX_White, "tile x: %d", x);
    jo_printf_with_color(0, 1, JO_COLOR_INDEX_White, "tile y: %d", y);
#endif
}

static void process_intro_text_display()
{
	if (action_counter < INTRO_FADE_TEXT_TIME)
	{
		if (action_counter <= 1)
		{
			fade_brightness = 0;
		}
		else if (action_counter % 4 == 0 && fade_brightness <= JO_DEFAULT_BRIGHTNESS)
			fade_brightness++;

		if (fade_brightness <= JO_DEFAULT_BRIGHTNESS)
		{
			jo_sprite_enable_gouraud_shading();
			jo_set_gouraud_shading_brightness(fade_brightness);
			jo_font_print_centered(game_white_font, 0, 0, 0.99f, intro_text[current_intro_text_index]);
			jo_set_gouraud_shading_brightness(JO_DEFAULT_BRIGHTNESS);
			jo_sprite_disable_gouraud_shading();
#if JO_DEBUG
			jo_printf_with_color(0, 2, JO_COLOR_INDEX_White, "fade %d", fade_counter);
#endif
		}
	}
	else if (action_counter < (INTRO_FADE_TEXT_TIME + INTRO_STILL_TEXT_TIME))
	{
		jo_font_print_centered(game_white_font, 0, 0, 0.99f, intro_text[current_intro_text_index]);
	}
	else if (action_counter < (INTRO_FADE_TEXT_TIME + INTRO_STILL_TEXT_TIME + INTRO_FADE_TEXT_TIME))
	{
		if (action_counter == (INTRO_FADE_TEXT_TIME + INTRO_STILL_TEXT_TIME))
		{
			fade_brightness = JO_DEFAULT_BRIGHTNESS;
		}
		else if (action_counter % 4 == 0 && fade_brightness >= 0)
			fade_brightness--;

		if (fade_brightness >= 0)
		{
			jo_sprite_enable_gouraud_shading();
			jo_set_gouraud_shading_brightness(fade_brightness);
			jo_font_print_centered(game_white_font, 0, 0, 0.99f, intro_text[current_intro_text_index]);
			jo_set_gouraud_shading_brightness(JO_DEFAULT_BRIGHTNESS);
			jo_sprite_disable_gouraud_shading();
#if JO_DEBUG
			jo_printf_with_color(0, 2, JO_COLOR_INDEX_White, "fade %d", fade_counter);
#endif
		}
	}
	else if (action_counter > (INTRO_FADE_TEXT_TIME + INTRO_STILL_TEXT_TIME + INTRO_FADE_TEXT_TIME + INTRO_BLANK_TEXT_TIME))
	{
		action_counter = 0;
		current_intro_text_index++;
		if (current_intro_text_index == INTRO_TEXT_COUNT)
		{
			intro_text_shown = true;
			current_game_mode = MODE_TITLE;
		}
	}
}

static void process_intro_graphic_fade()
{
	if (action_counter <= 1)
	{
		fade_brightness = JO_DEFAULT_BRIGHTNESS;
	}
	else if (action_counter % 4 == 0 && fade_brightness >= 0)
		fade_brightness--;

	if (fade_brightness >= 0)
	{
		jo_sprite_enable_gouraud_shading();
		jo_set_gouraud_shading_brightness(fade_brightness);
		jo_sprite_draw3D2(shlogo_sprite_id, 0, 0, BACKGROUND_ZINDEX);
		jo_set_gouraud_shading_brightness(JO_DEFAULT_BRIGHTNESS);
		jo_sprite_disable_gouraud_shading();
#if JO_DEBUG
		jo_printf_with_color(0, 2, JO_COLOR_INDEX_White, "fade %d", fade_counter);
#endif
	}

	if (action_counter == INTRO_FADE_GRAPHIC_TIME)
	{
		action_counter = 0;
		intro_graphic_faded = true;
	}
}

static void process_intro_graphic_scale()
{
	float scale_y = (float)(action_counter << 1)/240.0f;
	jo_sprite_change_sprite_scale_xy(1.0f, scale_y);
	// it seems that draw3d2 uses centered coords only when scaling is applied!
	jo_sprite_draw3D2(shlogo_sprite_id, 0, 0 - (action_counter/20), BACKGROUND_ZINDEX);
	jo_sprite_restore_sprite_scale();

	if (action_counter == INTRO_SCALE_GRAPHIC_TIME)
	{
		action_counter = 0;
		intro_graphic_scaled = true;
	}

#if JO_DEBUG		
	jo_printf_with_color(0, 0, JO_COLOR_INDEX_White, "Counter %d", action_counter);
	jo_printf_with_color(0, 1, JO_COLOR_INDEX_White, "scale %d%%", (int)(scale_y * 100.0f));
#endif
}

void draw_load()
{
	jo_font_print_centered(game_white_font, 0, 0, 0.99f, "LOADING...");
}

void end_load()
{
	current_game_mode = load_game_mode;
	action_counter = 0;
}

// show a loading screen while actions are performed
// we need to assign load_game_mode which is our target mode
// we also need to assign load_action which is the action to perform
void load()
{
	if (action_counter <= 1)
	{
		jo_clear_screen();
		jo_set_default_background_color(JO_COLOR_INDEX_Black);
		reset_fade();
	}

	process_fade(draw_load, end_load);

	if (current_fade_state == FADE_STATE_NONE)
	{
		load_action();
		current_fade_state = FADE_STATE_OUT;
	}
}

void intro() 
{
	if (!intro_mode_started)
	{
		CDDA_PlaySingle(TITLE_TRACKID, true);
		intro_mode_started = true;
	}

	input_type current_pad1_input = get_pad_input_type(current_game_mode, 1);

	if (current_pad1_input == INPUT_TYPE_START || 
		current_pad1_input == INPUT_TYPE_A || 
		current_pad1_input == INPUT_TYPE_C)
	{
		// user wants to skip to title
		action_counter = 0;
		intro_text_shown = true;
		current_game_mode = MODE_TITLE;
	}

	// vertically stretch speedhack logo over time
	if (!intro_graphic_scaled)
	{
		process_intro_graphic_scale();
	}
	else if (!intro_graphic_shown)
	{
		jo_sprite_draw3D2(shlogo_sprite_id, 0, 0, BACKGROUND_ZINDEX);

		if (action_counter == INTRO_STILL_GRAPHIC_TIME)
		{
			action_counter = 0;
			intro_graphic_shown = true;
		}
	}
	else if (!intro_graphic_faded)
	{
		process_intro_graphic_fade();
	}
	else if (!intro_text_shown)
	{
		process_intro_text_display();
	}
}

// void shutdown() {
// 	saveTable(hisc,"hiscores.sav");
// 	saveSoundCFG();
// 	allegro_exit();
// }

void draw_instructions()
{
	jo_sprite_draw3D2(title_sprite_id, 0, 16, TEXT_ZINDEX);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(aa2_sprite_id, AA2_FINAL_X, 8, AA2_ZINDEX);
	jo_sprite_disable_half_transparency();

	int title_y = 50;
	if (instructions_titles[current_instructions_page_index] != "")
	{
		jo_font_print(game_black_font, 5, title_y+1, 0.99f, instructions_titles[current_instructions_page_index]);
		jo_font_print(game_white_font, 6, title_y, 0.99f, instructions_titles[current_instructions_page_index]);		
	}

	for (int i = 0; i < INSTRUCTIONS_LINE_COUNT; i++)
	{
		jo_font_print(game_black_font, 3, title_y+3+(16*(i+1)), 0.50f, instructions_lines[current_instructions_page_index][i]);
		jo_font_print(game_white_font, 4, title_y+2+(16*(i+1)), 0.50f, instructions_lines[current_instructions_page_index][i]);		
	}

	draw_donkeys();
}

void end_instructions()
{
	action_counter = 0;
	current_game_mode = MODE_TITLE;
}

void instructions() 
{
	if (action_counter <= 1)
	{
		jo_clear_screen();
		jo_set_default_background_color(JO_COLOR_RGB(121,52,52)); // pink
		current_instructions_page_index = 0;
		reset_particles();
		reset_fade();
	}
	
	process_fade(draw_instructions, end_instructions);

	if (current_fade_state == FADE_STATE_NONE)
	{
		input_type current_pad1_input = get_pad_input_type(current_game_mode, 1);

		if (current_pad1_input == INPUT_TYPE_START ||
			current_pad1_input == INPUT_TYPE_A || 
			current_pad1_input == INPUT_TYPE_C)
		{
			// user wants to return to title
			current_fade_state = FADE_STATE_OUT;
		}
		else if (current_pad1_input == INPUT_TYPE_LEFT)
		{
			if (current_instructions_page_index > 0)
				current_instructions_page_index--;
		}
		else if (current_pad1_input == INPUT_TYPE_RIGHT)
		{
			if (current_instructions_page_index < (INSTRUCTIONS_PAGE_COUNT-1))
				current_instructions_page_index++;
		}

		if ((get_random(500)-1)<5) create_donkey(-40,(get_random(220)-1)+20,get_random(4)-1);
	}
}

void draw_credits()
{

	jo_sprite_draw3D2(title_sprite_id, 0, 16, TEXT_ZINDEX);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(aa2_sprite_id, AA2_FINAL_X, 8, AA2_ZINDEX);
	jo_sprite_disable_half_transparency();

	int title_y = -60;
	jo_font_print_centered(game_black_font, -1, title_y+1, 0.99f, "CREDITS");
	jo_font_print_centered(game_white_font, 0, title_y, 0.99f, "CREDITS");

	int credit_start_y = 65;

	for (int i = 0; i < CREDITS_TEXT_COUNT; i++)
	{
		jo_font_print(game_black_font, 3, credit_start_y+1+i*16, 0.50f, credits_text[i]);
		jo_font_print(game_white_font, 4, credit_start_y+i*16, 0.50f, credits_text[i]);		
	}

	draw_donkeys();
}

void end_credits()
{
	action_counter = 0;
	current_game_mode = MODE_TITLE;
}

void credits() 
{
	if (action_counter <= 1)
	{
		jo_clear_screen();
		jo_set_default_background_color(JO_COLOR_RGB(140,110,75)); // brown
		reset_particles();
		reset_fade();
	}
	
	process_fade(draw_credits, end_credits);

	if (current_fade_state == FADE_STATE_NONE)
	{
		if ((get_random(500)-1)<5) create_donkey(-40,(get_random(220)-1)+20,get_random(4)-1);

		input_type current_pad1_input = get_pad_input_type(current_game_mode, 1);

		if (current_pad1_input == INPUT_TYPE_START ||
			current_pad1_input == INPUT_TYPE_A || 
			current_pad1_input == INPUT_TYPE_C)
		{
			// user wants to return to title so start fading out
			current_fade_state = FADE_STATE_OUT;
		}
	}
}

void update_game()
{
	// shutdown();
	if (action_counter == 0)
		jo_disable_all_screen_color_filter(); // need to clear at start of game loop for smoother transitions
	
	if (action_counter < MAX_ACTION_CYCLES)
		action_counter++;

	switch (current_game_mode)
	{
		case MODE_INTRO:
			intro();
			break;

		case MODE_TITLE:
			title();
			break;

		case MODE_INSTRUCTIONS:
			instructions();
			break;

		case MODE_CREDITS:
			credits();
			break;

		case MODE_HIGH_SCORES:
			high_scores();
			break;

		case MODE_HOF:
			hof();
			break;

		case MODE_GAME:
			play();
			break;

		case MODE_LOAD:
			load();
			break;

		default:
			jo_printf_with_color(0, 0, JO_COLOR_INDEX_White, "Hello world!");
			action_counter = 0; // always reset as we are not timing anything
			break;
	}
}

void jo_main(void) 
{
	int playGame = 1;

	init();
	jo_core_add_vblank_callback(sdrv_vblank_rq);
	jo_core_add_callback(update_game);
	jo_core_run();
}

/*
** END OF FILE
*/