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

#define TITLE_TRACKID 2
#define BACKGROUND_ZINDEX 500
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

 // BITMAP *swap_screen;
// BITMAP *bg_screen;
// BITMAP *scroller = NULL;
// DATAFILE *data;
Thisc *hisc;						// a hiscore table
Ttoken board[8][8];					// the board
Tparticle dust[MAX_PARTICLES];		// particles for the particle engine
// Tplayer ply[3];						// 2 players, ignore ply[0]

// // RGB -> color mapping table. Not needed, but speeds things up 
// RGB_MAP rgb_table;

// // lighting and translucency color mapping tables
// COLOR_MAP dark_table;
// COLOR_MAP light_table;
// COLOR_MAP trans_table;

// // global stuff
// int playing;			// can the player interact?
// int scrolling;			// are we scrolling the board?
// int scrollDir;			// 1 up, 2 right, 3 down, 4 left
// int scrollX, scrollY;	// scrolling offset
// int placeing, placeType, placeX, placeY;	// placeToken stuff
// byte player;			// current player
// int winner;				// who won? 3=draw
byte sound_vol, music_vol;
// int playingMidi = 0;
// int lockedRow, lockedCol; // current locked row/col
// int hint, hintX, hintY;   // hint stuff
// int cpu;				  // 0 = none, 1 = ply1, 2= ply2

// // timer variables
// volatile int frame_count;
// volatile int fps;
// volatile int game_count;

static mode game_mode;
static mode previous_game_mode;
static game_type current_game_type;
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
static int action_counter;
static int fade_counter;
static jo_font *game_white_font;
static jo_font *game_black_font;
static int current_intro_text_index = 0;
static int current_instructions_page_index = 0;
static bool intro_graphic_scaled = false;
static bool intro_graphic_shown = false;
static bool intro_graphic_faded = false;
static bool intro_text_shown = false;
static int aa2_x;
static int pointer_x;
static int pointer_y;
bool is_showing_main_menu_options = false;
bool is_showing_start_game_options = false;
static short select_sound_id;
static int title_menu_x, title_menu_y;

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
    "",
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

// track button changes for better title menu input handling
static bool current_input_states[INPUT_TYPE_COUNT];
static bool previous_input_states[INPUT_TYPE_COUNT];

// void fps_counter(void) {
// 	fps=frame_count;
// 	frame_count=0;
// }
// END_OF_FUNCTION(fps_counter);

// void game_counter(void) {
// 	game_count++;
// }
// END_OF_FUNCTION(game_counter);

// void myAlert(char *txt) {
// 	alert("A L E X   I I", NULL, txt, "Cool", "Yeah", 'y', 27);
// }

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

// int loadData() {
// 	int ok=1;

// 	data = load_datafile("../assets/alex2.dat");
// 	if (!data) ok=0;

// 	return ok;
// }

void load_sound_config() 
{
// 	PACKFILE *fp;

// 	fp = pack_fopen("sound.sav", "rp");
// 	if (!fp) {
 		
	// start sound at full volume and music at half volume
	// sound and music are on a different scale with ponesound
	// sound is 0-7
	// music is 0-7
	sound_vol=6;
 	music_vol=4;
// 	} 
// 	else {
// 		pack_fread(&soundvol, 1, fp);
// 		pack_fread(&musicvol, 1, fp);
// 		pack_fclose(fp);
// 	}	
}

// void saveSoundCFG() {
// 	PACKFILE *fp;

// 	fp = pack_fopen("sound.sav", "wp");
// 	pack_fwrite(&soundvol, 1, fp);
// 	pack_fwrite(&musicvol, 1, fp);
// 	pack_fclose(fp);
// }

void reset_game()
{
	game_mode = MODE_INTRO;
	current_game_type = GAME_TYPE_HVC;
	action_counter = 0;

	title_menu_x=-200, title_menu_y=144;
	is_showing_start_game_options = false;
}

void init() 
{
	jo_core_init(JO_COLOR_Black);
	previous_game_mode = MODE_NONE;

	// initialize sound
	load_drv(ADX_MASTER_2304);
	CDDA_SetVolume(4);
	select_sound_id = load_8bit_pcm((Sint8 *)"SELECT.PCM", 15360); // using ponesound due to issues with jo engine audio

	// initialize graphics
	shlogo_sprite_id = jo_sprite_add_tga("TEX", "SHLOGO.TGA", JO_COLOR_Transparent);
	title_sprite_id = jo_sprite_add_tga("TEX", "TITLE.TGA", JO_COLOR_RGB(255,0,255));
	aa2_sprite_id = jo_sprite_add_tga("TEX", "AA2.TGA", JO_COLOR_Black);
	aalogo_sprite_id = jo_sprite_add_tga("TEX", "AALOGO.TGA", JO_COLOR_RGB(255,0,255));
	vol1_sprite_id = jo_sprite_add_tga("TEX", "VOL1.TGA", JO_COLOR_Black);
	vol2_sprite_id = jo_sprite_add_tga("TEX", "VOL2.TGA", JO_COLOR_Black);
	vol3_sprite_id = jo_sprite_add_tga("TEX", "VOL3.TGA", JO_COLOR_Black);
	pointer_sprite_id = jo_sprite_add_tga("TEX", "POINTER.TGA", JO_COLOR_Black);
	emptytkn_sprite_id = jo_sprite_add_tga("TEX", "EMPTYTKN.TGA", JO_COLOR_Black);
	greentkn_sprite_id = jo_sprite_add_tga("TEX", "GREENTKN.TGA", JO_COLOR_Black);
	bluetkn_sprite_id = jo_sprite_add_tga("TEX", "BLUETKN.TGA", JO_COLOR_Black);
	multitkn_sprite_id = jo_sprite_add_tga("TEX", "MULTITKN.TGA", JO_COLOR_Black);
	deadtkn_sprite_id = jo_sprite_add_tga("TEX", "DEADTKN.TGA", JO_COLOR_Black);

	token_sprite_ids[0] = emptytkn_sprite_id;
	token_sprite_ids[1] = greentkn_sprite_id;
	token_sprite_ids[2] = bluetkn_sprite_id;
	token_sprite_ids[3] = multitkn_sprite_id;
	token_sprite_ids[4] = deadtkn_sprite_id; 

	// initialize fonts
	game_white_font = jo_font_load("FNT", "GAMEWHT.TGA", JO_COLOR_RGB(255,0,255),GAME_FONT_WIDTH, GAME_FONT_HEIGHT, 0, GAME_FONT_MAPPING);
	game_white_font->z_index = BACKGROUND_ZINDEX;
	game_black_font = jo_font_load("FNT", "GAMEBLK.TGA", JO_COLOR_RGB(255,0,255),GAME_FONT_WIDTH, GAME_FONT_HEIGHT, 0, GAME_FONT_MAPPING);
	game_black_font->z_index = BACKGROUND_ZINDEX;
// 	allegro_init();

// 	set_gfx_mode(GFX_GDI, 320, 240, 0, 0);

// 	packfile_password("speedhack");

// 	install_keyboard();
// 	install_mouse();
// 	install_timer();
// 	LOCK_VARIABLE(game_count);
// 	LOCK_VARIABLE(fps);
// 	LOCK_VARIABLE(frame_count);
// 	LOCK_FUNCTION(fps_counter);
// 	install_int(fps_counter,1000);
// 	fps=0;
// 	frame_count=0;
// 	LOCK_FUNCTION(game_counter);
// 	install_int(game_counter,15);
// 	game_count=0;

// 	swap_screen  = create_bitmap(320,240);
// 	bg_screen  = create_bitmap(320,240);

// 	if (!loadData()) { allegro_message("Failed to load data."); exit(1); }

// 	install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL);

// 	text_mode(-1);

// 	create_rgb_table(&rgb_table, data[GAMEPAL].dat, NULL);
// 	rgb_map = &rgb_table;
// 	create_light_table(&dark_table, data[GAMEPAL].dat, 0, 0, 0, NULL);
// 	create_light_table(&light_table, data[GAMEPAL].dat, 63, 63, 63, NULL);
// 	create_trans_table(&trans_table, data[GAMEPAL].dat, 0, 64, 0, NULL);

// 	soundvol=200;
// 	musicvol=200;

	hisc = make_table();
// 	if (!loadTable(hisc,"hiscores.sav")) resetScores(hisc);
	reset_scores(hisc);
	load_sound_config();
// 	set_volume(soundvol,musicvol);

	reset_game();
}

// check if input was newly pressed
bool input_pressed(input_type candidate_input)
{
    return current_input_states[candidate_input] && !previous_input_states[candidate_input];
}

void reset_input_states()
{
    for (int i = 0; i < INPUT_TYPE_COUNT; i++)
    {
        previous_input_states[i] = false;
        current_input_states[i] = false;
    }

}

void save_previous_inputstates()
{
    // save previous state
    for (int i = 0; i < INPUT_TYPE_COUNT; i++)
        previous_input_states[i] = current_input_states[i];
}

// track all current and previous input states so we can check on input presses
static void update_input_states(bool current_input_states[INPUT_TYPE_COUNT])
{
    save_previous_inputstates();

    // read current state
    current_input_states[INPUT_TYPE_UP] = jo_is_pad1_key_pressed(JO_KEY_UP);
    current_input_states[INPUT_TYPE_DOWN] = jo_is_pad1_key_pressed(JO_KEY_DOWN);
    current_input_states[INPUT_TYPE_LEFT] = jo_is_pad1_key_pressed(JO_KEY_LEFT);
    current_input_states[INPUT_TYPE_RIGHT] = jo_is_pad1_key_pressed(JO_KEY_RIGHT);
    current_input_states[INPUT_TYPE_START] = jo_is_pad1_key_pressed(JO_KEY_START);    
	current_input_states[INPUT_TYPE_A] = jo_is_pad1_key_pressed(JO_KEY_A);
	current_input_states[INPUT_TYPE_C] = jo_is_pad1_key_pressed(JO_KEY_C);
}

// retrieve the input type from the user
input_type get_input_types(mode game_mode, bool current_input_states[INPUT_TYPE_COUNT])
{
    input_type current_input = INPUT_TYPE_NOTHING;
    if (jo_is_pad1_available())
    {
        update_input_states(current_input_states);
        switch(game_mode)
        {
            case MODE_INTRO:

                if (input_pressed(INPUT_TYPE_START)) current_input = INPUT_TYPE_START;
				else if (input_pressed(INPUT_TYPE_A)) current_input = INPUT_TYPE_A;
				else if (input_pressed(INPUT_TYPE_C)) current_input = INPUT_TYPE_C;

                break;

            case MODE_INSTRUCTIONS:
                if (input_pressed(INPUT_TYPE_START)) current_input = INPUT_TYPE_START;  
				else if (input_pressed(INPUT_TYPE_A)) current_input = INPUT_TYPE_A;
				else if (input_pressed(INPUT_TYPE_C)) current_input = INPUT_TYPE_C;
				else if (input_pressed(INPUT_TYPE_LEFT)) current_input = INPUT_TYPE_LEFT;
				else if (input_pressed(INPUT_TYPE_RIGHT)) current_input = INPUT_TYPE_RIGHT;

				break;

			default:

                if (input_pressed(INPUT_TYPE_START)) current_input = INPUT_TYPE_START;  
				else if (input_pressed(INPUT_TYPE_A)) current_input = INPUT_TYPE_A;
				else if (input_pressed(INPUT_TYPE_C)) current_input = INPUT_TYPE_C;
                else if (jo_is_pad1_key_pressed(JO_KEY_UP) && jo_is_pad1_key_pressed(JO_KEY_LEFT)) current_input = INPUT_TYPE_UP_LEFT;
                else if (jo_is_pad1_key_pressed(JO_KEY_UP) && jo_is_pad1_key_pressed(JO_KEY_RIGHT)) current_input = INPUT_TYPE_UP_RIGHT;
                else if (jo_is_pad1_key_pressed(JO_KEY_DOWN) && jo_is_pad1_key_pressed(JO_KEY_LEFT)) current_input = INPUT_TYPE_DOWN_LEFT;
                else if (jo_is_pad1_key_pressed(JO_KEY_DOWN) && jo_is_pad1_key_pressed(JO_KEY_RIGHT)) current_input = INPUT_TYPE_DOWN_RIGHT;
				else if (jo_is_pad1_key_pressed(JO_KEY_DOWN)) current_input = INPUT_TYPE_DOWN;
                else if (jo_is_pad1_key_pressed(JO_KEY_UP)) current_input = INPUT_TYPE_UP;
                else if (jo_is_pad1_key_pressed(JO_KEY_LEFT)) current_input = INPUT_TYPE_LEFT;
                else if (jo_is_pad1_key_pressed(JO_KEY_RIGHT)) current_input = INPUT_TYPE_RIGHT;

                break;
        }
    }
    else
    {
        reset_input_states();
    }

    return current_input;
}

// void etchedBox(int x1, int y1, int x2, int y2, int up) {
// 	line(bg_screen, x1-2, y1-1, x2, y1-1, (up?38:40));
// 	line(bg_screen, x1-2, y1-2, x2+1, y1-2, (up?38:40));
// 	line(bg_screen, x1-1, y1-1, x1-1, y2, (up?38:40));
// 	line(bg_screen, x1-2, y1-2, x1-2, y2+1, (up?38:40));
// 	line(bg_screen, x1, y2+1, x2+1, y2+1, (up?40:38));
// 	line(bg_screen, x1-1, y2+2, x2+2, y2+2, (up?40:38)); 
// 	line(bg_screen, x2+1, y2+1, x2+1, y1, (up?40:38));
// 	line(bg_screen, x2+2, y2+2, x2+2, y1-1, (up?40:38));
// }

// void makeBg() {
// 	int x;

// 	// draw game board
// 	clear_to_color(bg_screen,39);
// 	rect(bg_screen, 19, 19, 213, 213, 0);
// 	rectfill(bg_screen, 20, 20, 212, 212, 12);
// 	etchedBox(19,19,213,213,0);
// 	etchedBox(240,19,310,100,0);
// 	etchedBox(240,121,310,212,0);

// 	// draw arrows
// 	color_map = &dark_table;
// 	for(x=0;x<8;x++) {
// 		draw_lit_sprite(bg_screen, data[ARROW1].dat, 27+x*24, 6, 150);
// 		draw_lit_sprite(bg_screen, data[ARROW3].dat, 27+x*24, 216, 150);
// 		draw_lit_sprite(bg_screen, data[ARROW2].dat, 216, 27+x*24, 150);
// 		draw_lit_sprite(bg_screen, data[ARROW4].dat, 6, 27+x*24, 150);
// 	}

// 	// draw squares
// 	for(x=0;x<9;x++) {
// 		line(bg_screen, 20+x*24, 20, 20+x*24, 212, 10);
// 		line(bg_screen, 20, 20+x*24, 212, 20+x*24, 10);
// 	}
// }

// void createParticle(int x,int y,int im) {
//    int i=0;

//    while(dust[i].exist && i<MAX_PARTICLES-1) i++;  // find available i

//    dust[i].x = itofix(x); 
//    dust[i].y = itofix(y);
//    dust[i].dx = fcos(itofix(rand()%256));
//    dust[i].dy = fsin(itofix(rand()%256));
//    dust[i].image = im;
//    dust[i].exist = 1;
// }

// void drawParticles() {
// 	int i,y;

// 	for(i=0;i<MAX_PARTICLES;i++)
// 		if (dust[i].exist) {
// 			draw_sprite(swap_screen, data[DUST000+dust[i].image].dat, fixtoi(dust[i].x)-2, fixtoi(dust[i].y)-2);
// 			dust[i].x += dust[i].dx;
// 			dust[i].y += dust[i].dy;
// 			dust[i].dy += fsin(itofix(2));
// 			y = fixtoi(dust[i].y);
// 			if (y > 240) dust[i].exist = 0;
// 		}
// }

void draw_game(int showMouse) {
	int x,y;

// 	// fps...
// 	frame_count++;

// 	blit(bg_screen, swap_screen, 0, 0, 0, 0, 320, 240);
	
	for(x=0;x<8;x++)
		for(y=0;y<8;y++)
			if (board[x][y].token)
			{
				jo_sprite_draw3D2(token_sprite_ids[board[x][y].token], 21+x*24, 21+y*24, BACKGROUND_ZINDEX);
			}

// 	if (placeing) {
// 		rectfill(swap_screen, 21+24*placeX, 21+24*placeY, 21+24*placeX+22, 21+24*placeY+22, 12);
// 		stretch_sprite(swap_screen, data[TOKEN000+placeType].dat, 21+24*placeX+(placeing>>1), 21+24*placeY+(placeing>>1), 23-placeing, 23-placeing);
// 	}

// 	if (scrolling) {
// 		if (scrollDir==1) {
// 			blit(scroller, swap_screen, 0, 24-scrolling, scrollX, scrollY, 23, 194-24+scrolling);
// 			blit(scroller, swap_screen, 0, 0, scrollX, 212-24+scrolling, 23, 25-scrolling);
// 		}
// 		if (scrollDir==2) {
// 			blit(scroller, swap_screen, 0, 0, scrollX+24-scrolling, scrollY, 194-25+scrolling, 23);
// 			blit(scroller, swap_screen, 194-26+scrolling, 0, 20, scrollY, 24-scrolling, 23);
// 		}
// 		if (scrollDir==3) {
// 			blit(scroller, swap_screen, 0, 0, scrollX, scrollY+24-scrolling, 23, 194-25+scrolling);
// 			blit(scroller, swap_screen, 0, 194-26+scrolling, scrollX, 20, 23, 24-scrolling);
// 		}
// 		if (scrollDir==4) {
// 			blit(scroller, swap_screen, 24-scrolling, 0, scrollX, scrollY, 194-24+scrolling, 23);
// 			blit(scroller, swap_screen, 0, 0, 212-24+scrolling, scrollY, 25-scrolling, 23);
// 		}
// 	}


// 	// draw arrows
// 	for(x=0;x<8;x++) {
// 		if (x != lockedCol) {
// 			draw_sprite(swap_screen, data[ARROW1].dat, 27+x*24, 6);
// 			draw_sprite(swap_screen, data[ARROW3].dat, 27+x*24, 216);
// 		}
// 		if (x != lockedRow) {
// 			draw_sprite(swap_screen, data[ARROW2].dat, 216, 27+x*24);
// 			draw_sprite(swap_screen, data[ARROW4].dat, 6, 27+x*24);
// 		}
// 	}

// 	// draw hint
// 	if (hint & 8) {
// 		draw_sprite(swap_screen, data[TOKEN006].dat, 21+hintX*24, 21+hintY*24);
// 	}

// 	// draw scores
// 	textprintf(swap_screen,data[MYFONT].dat,246,20,1,"%5d",ply[1].score);
// 	textprintf(swap_screen,data[MYFONT].dat,246,122,1,"%5d",ply[2].score);
// 	textprintf(swap_screen,data[MYFONT].dat,245,19,-1,"%5d",ply[1].score);
// 	textprintf(swap_screen,data[MYFONT].dat,245,121,-1,"%5d",ply[2].score);


// 	// draw characters
// 	if (ply[1].anim) draw_sprite(swap_screen,data[PLAYER11+ply[1].animOffset+(ply[1].anim&8?1:0)].dat,280,68);
// 	else draw_sprite(swap_screen,data[PLAYER11].dat,280,68);
// 	if (ply[2].anim) draw_sprite(swap_screen,data[PLAYER21+ply[2].animOffset+(ply[2].anim&8?0:1)].dat,280,180);
// 	else draw_sprite(swap_screen,data[PLAYER21].dat,280,180);

// 	// draw avail. multi tokens
// 	draw_sprite(swap_screen,data[(ply[1].multi?TOKEN003:TOKEN005)].dat,245,73);
// 	if (ply[1].multi>1) textprintf(swap_screen,data[MYFONT2].dat,245,76,-1,"%d",ply[1].multi);
// 	draw_sprite(swap_screen,data[(ply[2].multi?TOKEN003:TOKEN005)].dat,245,185);
// 	if (ply[2].multi>1) textprintf(swap_screen,data[MYFONT2].dat,245,188,-1,"%d",ply[2].multi);

// 	if (showMouse) drawParticles();

// 	if (showMouse) {
// 		int mx = mouse_x;
// 		int my = mouse_y;
// 		if (ply[1].carry || ply[2].carry)
// 			draw_sprite(swap_screen, data[TOKEN003].dat, mx-11, my-11);
// 		draw_sprite(swap_screen, data[POINTER].dat, mx-1, my-1);
// 		draw_sprite(swap_screen, data[DUST000+player].dat, mx+9, my+11);
// 	}
}

// void blitScreen() {
// 	vsync();
// 	blit(swap_screen,screen,0,0,0,0,320,240);
// }

void reset_particles() {
	int i;
	for(i=0;i<MAX_PARTICLES;i++)
		dust[i].exist = 0;
}

void draw_title(int x, int y, int m, int menu_x, int menu_y) 
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
	jo_sprite_draw3D2(title_sprite_id, 0, 16, BACKGROUND_ZINDEX);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(aa2_sprite_id, aa2_x, 8, 450);
	jo_sprite_disable_half_transparency();
	jo_sprite_draw3D2(aalogo_sprite_id, 64, 48, 400);

	// title menu options
	if (is_showing_main_menu_options || menu_y < JO_TV_HEIGHT)
	{
		jo_font_print(game_black_font, 39, menu_y + 1, 0.99f, "START GAME");
		jo_font_print(game_white_font, 40, menu_y, 0.99f, "START GAME");
		jo_font_print(game_black_font, 39, menu_y + 21, 0.99f, "HIGH SCORES");
		jo_font_print(game_white_font, 40, menu_y + 20, 0.99f, "HIGH SCORES");
		jo_font_print(game_black_font, 39, menu_y + 41, 0.99f, "INSTRUCTIONS");
		jo_font_print(game_white_font, 40, menu_y + 40, 0.99f, "INSTRUCTIONS");
		jo_font_print(game_black_font, 39, menu_y + 61, 0.99f, "CREDITS");
		jo_font_print(game_white_font, 40, menu_y + 60, 0.99f, "CREDITS");
	}
	
	if (is_showing_start_game_options)
	{
		jo_font_print(game_black_font, menu_x-1, 144 + 1, 0.99f, "HUMAN VS AARON");
		jo_font_print(game_black_font, menu_x-1, 164 + 1, 0.99f, "ALEX VS HUMAN");
		jo_font_print(game_black_font, menu_x-1, 184 + 1, 0.99f, "HUMAN VS HUMAN");
		jo_font_print(game_white_font, menu_x, 144, 0.99f, "HUMAN VS AARON");
		jo_font_print(game_white_font, menu_x, 164, 0.99f, "ALEX VS HUMAN");
		jo_font_print(game_white_font, menu_x, 184, 0.99f, "HUMAN VS HUMAN");
	}

	// volume controls
	jo_sprite_draw3D2(vol3_sprite_id, 280, 152, 500);
	jo_sprite_draw3D2(vol3_sprite_id, 304, 152, 500);
	jo_sprite_draw3D2(vol1_sprite_id, 276, 197-(sound_vol*7), 500);
	jo_sprite_draw3D2(vol2_sprite_id, 300, 197-(music_vol*7), 500);
}

// void draw_donkeys() {
// 	int i,x;

// 	for(i=0;i<MAX_PARTICLES;i++)
// 		if (dust[i].exist) {
// 			draw_sprite(swap_screen, data[DONKEY1+dust[i].image].dat, fixtoi(dust[i].x), fixtoi(dust[i].y));
// 			dust[i].x += dust[i].dx;
// 			dust[i].y += dust[i].dy;
// 			dust[i].dy += fsin(itofix(2));
// 			if (dust[i].dy > itofix(2)) dust[i].dy = -dust[i].dy;
// 			x = fixtoi(dust[i].x);
// 			if (x > 320) dust[i].exist = 0;
// 		}
// }

// void createDonkey(int x,int y,int im) {
//    int i=0;

//    while(dust[i].exist && i<MAX_PARTICLES-1) i++;  // find available i

//    dust[i].x = itofix(x); 
//    dust[i].y = itofix(y);
//    dust[i].dx = itofix(rand()%2+2);
//    dust[i].dy = itofix(rand()%2+1);
//    dust[i].image = im;
//    dust[i].exist = 1;
// }

bool pointer_on_hvc_game_option()
{
	return (pointer_x >= title_menu_x && 
		pointer_x <= (title_menu_x + GAME_FONT_WIDTH * 14)) && // 14 chars
		(pointer_y >= (144 + 8) && 
		pointer_y <= (144 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_cvh_game_option()
{
	return (pointer_x >= title_menu_x && 
		pointer_x <= (title_menu_x + GAME_FONT_WIDTH * 14)) && // 14 chars
		(pointer_y >= (164 + 8) && 
		pointer_y <= (164 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_hvh_game_option()
{
	return (pointer_x >= title_menu_x && 
		pointer_x <= (title_menu_x + GAME_FONT_WIDTH * 14)) && // 14 chars
		(pointer_y >= (184 + 8) && 
		pointer_y <= (184 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_start_game_option(int menu_y)
{
	return (pointer_x >= 40 && 
		pointer_x <= (40 + GAME_FONT_WIDTH * 10)) && // 10 chars
		(pointer_y >= (menu_y + 8) && 
		pointer_y <= (menu_y + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_high_scores_option(int menu_y)
{
	return (pointer_x >= 40 && 
		pointer_x <= (40 + GAME_FONT_WIDTH * 11)) && // 11 chars
		(pointer_y >= (menu_y + 28) && 
		pointer_y <= (menu_y + 20 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_instructions_option(int menu_y)
{
	return (pointer_x >= 40 && 
		pointer_x <= (40 + GAME_FONT_WIDTH * 11)) && // 12 chars but subtract one for going too far
		(pointer_y >= (menu_y + 48) && 
		pointer_y <= (menu_y + 40 + GAME_FONT_HEIGHT - 8));
}

bool pointer_on_credits_option(int menu_y)
{
	return (pointer_x >= 40 && 
		pointer_x <= (40 + GAME_FONT_WIDTH * 7)) && // 7 chars
		(pointer_y >= (menu_y + 68) && 
		pointer_y <= (menu_y + 60 + GAME_FONT_HEIGHT - 8));
}

void title() {
	int x=320, y=10;
// 	int done=0;
	int mode=0;  // 0=menu, 1=player-menu

	if (action_counter <= 1)
	{
	 	pointer_x=JO_TV_WIDTH_2;
		pointer_y=JO_TV_HEIGHT_2; 
		//clicked;
	}

	input_type current_input = get_input_types(game_mode, current_input_states);

	if (current_input == INPUT_TYPE_LEFT || current_input == INPUT_TYPE_UP_LEFT || current_input == INPUT_TYPE_DOWN_LEFT)
	{
		pointer_x -= 2;
		if (pointer_x < 0)
			pointer_x = 0;
	}
	
	if (current_input == INPUT_TYPE_RIGHT || current_input == INPUT_TYPE_UP_RIGHT || current_input == INPUT_TYPE_DOWN_RIGHT)
	{
		pointer_x += 2;
		if (pointer_x > (JO_TV_WIDTH - 1 - POINTER_WIDTH))
			pointer_x = JO_TV_WIDTH - 1 - POINTER_WIDTH;
	}
	
	if (current_input == INPUT_TYPE_UP || current_input == INPUT_TYPE_UP_LEFT || current_input == INPUT_TYPE_UP_RIGHT)
	{
		pointer_y -= 2;
		if (pointer_y < 0)
			pointer_y = 0;
	}

	if (current_input == INPUT_TYPE_DOWN || current_input == INPUT_TYPE_DOWN_LEFT || current_input == INPUT_TYPE_DOWN_RIGHT)
	{
		pointer_y += 2;
		if (pointer_y > (JO_TV_HEIGHT - 1 - POINTER_HEIGHT))
			pointer_y = JO_TV_HEIGHT - 1 - POINTER_HEIGHT;
	}

// 	if (!playingMidi) {
// 		play_midi(data[TITLESONG].dat,1);
// 		playingMidi=1;
// 	}

	reset_particles();
	if (is_showing_start_game_options && title_menu_y<JO_TV_HEIGHT) title_menu_y+=4;
	if (is_showing_start_game_options && title_menu_x < 40) title_menu_x+= 4;

	draw_title(x,y,mode,title_menu_x,title_menu_y);

// 	while(!done) {
// 		mx = mouse_x;
// 		my = mouse_y;
// 		if (x != 250) x-=2;
// 		clear_to_color(swap_screen,34);
//	draw_donkeys();
// 		draw_title(swap_screen,x,y,mode,menuX,menuY);
	jo_sprite_draw3D2(pointer_sprite_id, pointer_x, pointer_y, POINTER_ZINDEX);
//	draw_sprite(swap_screen,data[POINTER].dat, mx-1,my-1);
// 		blitScreen();

// 		if (rand()%500<5) createDonkey(-40,rand()%220+20,rand()%4);
// 		if (mode && menuX<40) menuX+=4;
// 		if (mode && menuY<400) menuY+=4;

// 		if (!mouse_b) clicked=0;
// 		// check sound setup
// 		if (mouse_b) {
// 			int sv=soundvol, mv=musicvol;
// 			if (mx>275 && mx<290 && my>149 && my<201) soundvol=250 - (my-150)*5;	// fx
// 			if (mx>295 && mx<310 && my>149 && my<201) musicvol=250 - (my-150)*5;	// music
// 			if (mv!=musicvol || soundvol!=sv) {
// 				set_volume(-1,musicvol);
// 				if (sv!=soundvol) play_sample(data[PLACE1].dat,soundvol,128,800+rand()%400,0);
// 			}
// 		}
// 		if (mouse_b==1 && !mode && !clicked) {   // check menu
// 			if (mx>40 && mx<166 && my>149 && my<165) { 		// start game
// 				mode = 1;	// switch to game selection
// 				menuX=-200; 
// 				play_sample(data[PLACE1].dat,soundvol,128,800+rand()%400,0);
// 			}
// 			if (mx>40 && mx<166 && my>169 && my<185) done = 3;						// high scores
// 			if (mx>40 && mx<166 && my>189 && my<205) done = 2;						// instructions
// 			if (mx>40 && mx<89 && my>209 && my<225) done = 1;						// quit
// 			clicked=1;
// 		}
// 		if (mouse_b==1 && mode && !clicked) {   // check game config
// 			if (mx>40 && mx<166 && my>169 && my<185) { done = 4; cpu = 2; }		// hvc
// 			if (mx>40 && mx<166 && my>189 && my<205) { done = 4; cpu = 1; }		// cvh
// 			if (mx>40 && mx<166 && my>209 && my<225) { done = 4; cpu = 0; }		// hvh
// 		}
// 	}
// 	play_sample(data[PLACE1].dat,soundvol,128,800+rand()%400,0);
// 	fade_out(4);
// 	while(mouse_b);		// wait out mouse

// 	return done-1;

	// check if instructions was selected
	// check if a, c, or start was pressed and the pointer is on instructions
	if ((current_input == INPUT_TYPE_START || 
		current_input == INPUT_TYPE_A || 
		current_input == INPUT_TYPE_C))
	{
		if (is_showing_main_menu_options)
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
				action_counter = 0;
				game_mode = MODE_INSTRUCTIONS;
			}
			else if (pointer_on_credits_option(title_menu_y))
			{
				pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
				action_counter = 0;
				game_mode = MODE_CREDITS;
			}
			else if (pointer_on_high_scores_option(title_menu_y))
			{
				pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
				action_counter = 0;
				game_mode = MODE_HIGH_SCORES;
			}
		}
		else if (is_showing_start_game_options)
		{
			if (pointer_on_hvc_game_option(title_menu_y))
			{
				pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
				action_counter = 0;
				game_mode = MODE_GAME;
				current_game_type = GAME_TYPE_HVC;
			}
			else if (pointer_on_cvh_game_option(title_menu_y))
			{
				pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
				action_counter = 0;
				game_mode = MODE_GAME;
				current_game_type = GAME_TYPE_CVH;
			}
			else if (pointer_on_hvh_game_option(title_menu_y))
			{
				pcm_play(select_sound_id, PCM_PROTECTED, sound_vol);
				action_counter = 0;
				game_mode = MODE_GAME;
				current_game_type = GAME_TYPE_HVH;
			}
		}
	}
}

void start_new_game() {
	int x,y;

	for(x=0;x<8;x++)
		for(y=0;y<8;y++)
			board[x][y] = emptySquare;

	board[3][3] = greenToken;
	board[4][4] = greenToken;
	board[3][4] = blueToken;
	board[4][3] = blueToken;

	// makeBg();

	// playing = 1;
	// player = 1;
	// scrolling = 0;
	// winner = 0;
	// lockedCol = lockedRow = -1;
	// hint = 0;

	// ply[1] = ply[2] = resetPlayer;

	reset_particles();
}

// int rotateRow(int row, int goLeft) {
// 	int i;
// 	Ttoken tmp;

// 	// move tokens
// 	if (goLeft) {
// 		tmp = board[0][row];
// 		for(i=0;i<7;i++) board[i][row] = board[i+1][row];
// 		board[7][row] = tmp;
// 	}
// 	else {
// 		tmp = board[7][row];
// 		for(i=7;i>0;i--) board[i][row] = board[i-1][row];
// 		board[0][row] = tmp;
// 	}

// 	return 1;
// }

// int animRotateRow(int row, int goLeft) {
// 	// setup scrolling area
// 	if (scroller != NULL) destroy_bitmap(scroller);
// 	scroller = create_bitmap(194,23);
// 	scrollDir = (goLeft?4:2);

// 	draw_game(0);
// 	blit(swap_screen, scroller, 20, 21+row*24, 0, 0, 194, 23);
// 	scrollX = 20;
// 	scrollY = 21+row*24;
// 	scrolling = 24;

// 	rotateRow(row,goLeft);
		
// 	playing = 0;
// 	play_sample(data[ROTATE].dat,soundvol,128,800+rand()%400,0);
// 	lockedRow = row;
// 	lockedCol = -1;

// 	return 1;
// }

// int rotateColumn(int col, int goUp) {
// 	int i;
// 	Ttoken tmp;

// 	// move tokens
// 	if (goUp) {
// 		tmp = board[col][0];
// 		for(i=0;i<7;i++) board[col][i] = board[col][i+1];
// 		board[col][7] = tmp;
// 	}
// 	else {
// 		tmp = board[col][7];
// 		for(i=7;i>0;i--) board[col][i] = board[col][i-1];
// 		board[col][0] = tmp;
// 	}

// 	return 1;
// }

// int animRotateColumn(int col, int goUp) {
// 	// setup scrolling area
// 	if (scroller != NULL) destroy_bitmap(scroller);
// 	scroller = create_bitmap(23,194);
// 	scrollDir = (goUp?1:3);

// 	draw_game(0);
// 	blit(swap_screen, scroller, 21+col*24, 20, 0, 0, 23, 194);
// 	scrollX = 21+col*24;
// 	scrollY = 20;
// 	scrolling = 24;

// 	rotateColumn(col,goUp);

// 	playing=0;
// 	lockedCol = col;
// 	lockedRow = -1;
// 	play_sample(data[ROTATE].dat,soundvol,128,800+rand()%400,0);

// 	return 1;
// }

// int placeToken(int x, int y, int type) {
// 	if (board[x][y].token != 0) return 0;

// 	if (type == BLUETOKEN) board[x][y] = blueToken;
// 	else if (type == GREENTOKEN) board[x][y] = greenToken;
// 	else if (type == MULTITOKEN) board[x][y] = multiToken;
// 	else if (type == DEADTOKEN) board[x][y] = deadToken;

// 	return 1;
// }

// int animPlaceToken(int x, int y, int type) {
// 	if (!placeToken(x,y,type)) return 0;

// 	play_sample(data[(type<3 ? PLACE1 : (type==3 ? PLACE2 : PLACE3))].dat,soundvol,128,800+rand()%400,0);

// 	placeX = x;
// 	placeY = y;
// 	placeing = 24;
// 	placeType = type;
// 	playing = 0;

// 	return 1;
// }

// int getToken(int x, int y, int p) {
// 	int t = board[x][y].token;
// 	if (t==DEADTOKEN) t = 0;
// 	if (t==MULTITOKEN) t = p;
// 	return t;
// }

// void flagSquare(int x, int y) {
// 	board[x][y].flag = 
// 	board[x+1][y].flag = 
// 	board[x+1][y+1].flag = 
// 	board[x][y+1].flag = 1;
// }

// int checkBoardScore() {
// 	int x,y;
// 	int me;
// 	int accScore = 0;

// 	// unflag tokens
// 	for(x=0;x<8;x++)
// 		for(y=0;y<8;y++) 
// 			board[x][y].flag = 0;

// 	// flag all tokens that are in a four
// 	for(x=0;x<8;x++)
// 		for(y=0;y<8;y++) {
// 			me = board[x][y].token;
// 			if (me) {
// 				if (x<7 && y<7)
// 					if (getToken(x+1,y,me) == me && getToken(x+1,y+1,me) == me && getToken(x,y+1,me) == me) flagSquare(x,y);
// 				if (x>0 && y<7)
// 					if (getToken(x-1,y,me) == me && getToken(x-1,y+1,me) == me && getToken(x,y+1,me) == me) flagSquare(x-1,y);
// 				if (x<7 && y>0)
// 					if (getToken(x+1,y,me) == me && getToken(x+1,y-1,me) == me && getToken(x,y-1,me) == me) flagSquare(x,y-1);
// 				if (x>0 && y>0)
// 					if (getToken(x-1,y,me) == me && getToken(x-1,y-1,me) == me && getToken(x,y-1,me) == me) flagSquare(x-1,y-1);
// 			}
// 		}

// 	// score flagged tokens
// 	me = 0;
// 	for(x=0;x<8;x++)
// 		for(y=0;y<8;y++) 
// 			if (board[x][y].flag) {
// 				accScore += board[x][y].score;
// 				me++;
// 			}

// 	// get plenty-bonus
// 	if (me) accScore += (me-1)*(me+1);

// 	return accScore;
// }

// int checkEnd() {
// 	int x,y;
// 	int p1=0,p2=0;

// 	checkBoardScore(); // force flagging of tokens are to be removed
// 	for(x=0;x<8;x++)
// 		for(y=0;y<8;y++) {
// 			if (board[x][y].token==1 && !board[x][y].flag) p1++;
// 			if (board[x][y].token==2 && !board[x][y].flag) p2++;
// 		}

// 	if (!p1 && !p2) return 3;
// 	if (!p2) return 2;
// 	if (!p1) return 1;
// 	return 0;
// }

// void checkBoard(byte player) {
// 	int x,y;
// 	int me,i;
// 	int accScore;
// 	int p1=0,p2=0;

// 	// unflag tokens
// 	for(x=0;x<8;x++)
// 		for(y=0;y<8;y++) 
// 			board[x][y].flag = 0;

// 	accScore = checkBoardScore();			// flags tokens
	
// 	// remove flagged tokens
// 	me = 0;
// 	for(x=0;x<8;x++)
// 		for(y=0;y<8;y++) 
// 			if (board[x][y].flag) {
// 				for(i=0;i<10;i++) createParticle(20+x*24+rand()%24,20+y*24+rand()%24,board[x][y].token);
// 				board[x][y] = emptySquare;
// 				me++;
// 			}

// 	if (me) {
// 		play_sample(data[REMOVE].dat,soundvol,128,800+rand()%400,0);
// 		ply[player].anim=100;
// 		ply[player].animOffset=0;
// 		if (me>4) {
// 			ply[player].multi++;
// 			ply[player].animOffset=2;
// 			ply[player].anim=200;
// 		}
// 	}

// 	ply[player].score += accScore;

// 	for(x=0;x<8;x++)
// 		for(y=0;y<8;y++) {
// 			if (board[x][y].token==1 && !board[x][y].flag) p1++;
// 			if (board[x][y].token==2 && !board[x][y].flag) p2++;
// 		}

// 	if (!p1 || !p2) winner=1;
// }

int get_right_aligned_x_coord(const jo_font *font, int right_x, float scale, char *text)
{
	return right_x - (strlen(text) * font->spacing * scale);
}

void draw_high_scores() {
	int i;

// 	// draw header
// 	draw_character(swap_screen, data[TITLE].dat, 8, 21, 1);
// 	draw_sprite(swap_screen,data[TITLE].dat,7,20);
// 	draw_rle_sprite(swap_screen,data[AA2].dat,250,10);
// 	color_map = &trans_table;
// 	draw_trans_sprite(swap_screen,data[TITLE].dat,7,20);
// 	textout_centre(swap_screen,data[MYFONT].dat,"ALL TIME HIGH",161,51,1);
// 	textout_centre(swap_screen,data[MYFONT].dat,"ALL TIME HIGH",160,50,-1);

// 	for(i=0;i<MAX_SCORES;i++) {
// 		textout(swap_screen,data[MYFONT2].dat,hisc[i].name,41,81+i*12,1);
// 		textout(swap_screen,data[MYFONT2].dat,hisc[i].name,40,80+i*12,-1);
// 		textprintf_right(swap_screen,data[MYFONT2].dat,281,81+i*12,1,"%5d",hisc[i].score);
// 		textprintf_right(swap_screen,data[MYFONT2].dat,280,80+i*12,-1,"%5d",hisc[i].score);
// 	}	

	jo_sprite_draw3D2(title_sprite_id, 0, 16, BACKGROUND_ZINDEX);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(aa2_sprite_id, AA2_FINAL_X, 8, 450);
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
}

void high_scores() {
	if (action_counter <= 1)
	{
		reset_particles();
		jo_clear_screen();
		jo_set_default_background_color(JO_COLOR_RGB(40,81,97)); // blue
	}
	
	input_type current_input = get_input_types(game_mode, current_input_states);

	if (current_input == INPUT_TYPE_START ||
		current_input == INPUT_TYPE_A || 
		current_input == INPUT_TYPE_C)
	{
		// user wants to return to title
		action_counter = 0;
		game_mode = MODE_TITLE;
	}

	draw_high_scores();

// 	clear_to_color(swap_screen,37);
// 	drawHiScores();
// 	blitScreen();
// 	fade_in(data[GAMEPAL].dat,4);

// 	while(!key[KEY_ESC] && !key[KEY_ENTER] && !key[KEY_SPACE] && !mouse_b) {
// 		clear_to_color(swap_screen,37);
// 		draw_donkeys();
// 		drawHiScores();
// 		blitScreen();
// 		if (rand()%500<5) createDonkey(-40,rand()%220+20,rand()%4);
// 	}

// 	fade_out(4);
// 	clear(screen);
}

// void enterHof(Thisc post, int p) {
// 	int i,kp;

// 	fade_out(4);
// 	clear_to_color(screen,(p==1?34:37));
// 	draw_character(screen, data[TITLE].dat, 8, 21, 1);
// 	draw_sprite(screen,data[TITLE].dat,7,20);
// 	draw_rle_sprite(screen,data[AA2].dat,250,10);
// 	color_map = &trans_table;
// 	draw_trans_sprite(screen,data[TITLE].dat,7,20);
// 	textout_centre(screen,data[MYFONT].dat,"ALL TIME HIGH",161,51,1);
// 	textout_centre(screen,data[MYFONT].dat,"ALL TIME HIGH",160,50,-1);

// 	draw_sprite(screen, data[TOKEN000+p].dat, 0, 80);
// 	draw_sprite(screen, data[TOKEN000+p].dat, 319-23, 80);
// 	textprintf_centre(screen,data[MYFONT2].dat,161,81,1,"Player %d, you got a high score!",p);
// 	textprintf_centre(screen,data[MYFONT2].dat,160,80,-1,"Player %d, you got a high score!",p);

// 	rectfill(screen,0,130,320,160,(p==1?37:34));
// 	strcpy(post.name,"*");
// 	textprintf_centre(screen,data[MYFONT].dat,161,131,1,"%s",post.name);
// 	textprintf_centre(screen,data[MYFONT].dat,160,130,-1,"%s",post.name);

// 	fade_in(data[GAMEPAL].dat,4);

// 	i=0; clear_keybuf();
// 	do {
// 		if (keypressed()) {
// 			play_sample(data[PLACE1].dat,soundvol,128,1000+rand()%800,0);
// 			kp=readkey();
// 			post.name[i]=(kp & 0xff);
// 			if (kp>>8==KEY_BACKSPACE) { post.name[i]='\0'; i--; }
// 			else i++;
// 			i=MAX(0,MIN(18,i));
// 			post.name[i]='*';
// 			post.name[i+1]='\0';
// 			if ((kp>>8)==KEY_ENTER) post.name[(i==18?i:i-1)]='\0';
// 		}
	
// 		vsync();
// 		rectfill(screen,0,130,320,160,(p==1?37:34));
// 		textprintf_centre(screen,data[MYFONT].dat,161,131,1,"%s",post.name);
// 		textprintf_centre(screen,data[MYFONT].dat,160,130,-1,"%s",post.name);
// 	} while((kp>>8)!=KEY_ENTER);
// 	play_sample(data[REMOVE].dat,soundvol,128,1000,0);
// 	post.name[i]='\0';

// 	enterTable(hisc,post);
// 	sortTable(hisc);
// 	fade_out(4);
// }

// int confirm(char *txt) {
// 	int ok;

// 	textout_centre(screen,data[MYFONT].dat,txt,161,101,1);
// 	textout_centre(screen,data[MYFONT].dat,txt,160,100,-1);
// 	while (key[KEY_Y] || key[KEY_ENTER] || key[KEY_N] || key[KEY_ESC]);
// 	ok=0;
// 	while (!ok) {
// 		if (key[KEY_Y] || key[KEY_ENTER]) ok=1;
// 		if (key[KEY_N] || key[KEY_ESC]) ok=2;
// 	}
// 	while (key[KEY_Y] || key[KEY_ENTER] || key[KEY_N] || key[KEY_ESC]);
// 	if (ok==2) return 0;
// 	return 1;
// }

// int loserWarning(int player, int posScore) {
// 	int p = player;
// 	int o = (player==1?2:1);
// 	int e = checkEnd();

// 	if (ply[p].score+posScore>ply[o].score && e) return 100000;  // I have more score, I will win
// 	if (ply[p].score+posScore>ply[o].score) return 0;  // I have more score, all is well
// 	if (!e) return 0; // The game is not done, a'right
// 	return -10000; // he will win!!! danger!
// }

// int getHint(int player, int recurse) {
// 	int x,y,i,j;
// 	int bestScore = 0;
// 	int crisis = 0;
// 	int cx=-1,cy=-1;
// 	int tmpScore;
// 	int sArray[5] = {1,0,0,10,-2};
// 	int move = 0; // 1 = slide, 2 = place, 3 = random
	
// 	sArray[player] = 8;
// 	sArray[(player==1?2:1)] = -1;

// 	hintX = rand()%8; 
// 	hintY = rand()%8; 

// 	hint = 60;

// 	// check for good slide moves
// 	for(x=0;x<8;x++) {
// 		if (x!=lockedRow) {
// 			rotateRow(x,0);  // right slide
// 			tmpScore = checkBoardScore(); 	// check possible score
// 			tmpScore += loserWarning(player,tmpScore);   // add win warnings
// 			rotateRow(x,1);	// reset board
// 			if (tmpScore<0) { crisis=2; cx=x; cy=2; }
// 			if (tmpScore > bestScore) {
// 				hintX = 8;
// 				hintY = x;
// 				bestScore = tmpScore;
// 				move = 1;
// 				if (bestScore>=100000) return 100000;
// 			}
// 			rotateRow(x,1); // left slide
// 			tmpScore = checkBoardScore(); 	// check possible score
// 			tmpScore += loserWarning(player,tmpScore);   // add win warnings
// 			rotateRow(x,0);	// reset board
// 			if (tmpScore<0) { crisis=2; cx=x; cy=4; }
// 			if (tmpScore > bestScore) {
// 				hintX = -1;
// 				hintY = x;
// 				bestScore = tmpScore;
// 				move = 1;
// 				if (bestScore>=100000) return 100000;
// 			}
// 		}
// 		if (x!=lockedCol) {
// 			rotateColumn(x,0); // down slide
// 			tmpScore = checkBoardScore(); 	// check possible score
// 			tmpScore += loserWarning(player,tmpScore);   // add win warnings
// 			rotateColumn(x,1);	// reset board
// 			if (tmpScore<0) { crisis=2; cx=x; cy=3; }
// 			if (tmpScore > bestScore) {
// 				hintX = x;
// 				hintY = 8;
// 				bestScore = tmpScore;
// 				move = 1;
// 				if (bestScore>=100000) return 100000;
// 			}
// 			rotateColumn(x,1);  // up slide
// 			tmpScore = checkBoardScore(); 	// check possible score
// 			tmpScore += loserWarning(player,tmpScore);   // add win warnings
// 			rotateColumn(x,0);	// reset board
// 			if (tmpScore<0) { crisis=2; cx=x; cy=1; }
// 			if (tmpScore > bestScore) {
// 				hintX = x;
// 				hintY = -1;
// 				bestScore = tmpScore;
// 				move = 1;
// 				if (bestScore>=100000) return 100000;
// 			}
// 		}
// 	}

// 	// check for good token positions
// 	for(x=0;x<8;x++)
// 		for(y=0;y<8;y++)
// 			if (!board[x][y].token) {
// 				placeToken(x,y,player);
// 				tmpScore = checkBoardScore(); 	// check possible score
// 				tmpScore += loserWarning(player,tmpScore);   // add win warnings
// 				board[x][y] = emptySquare;		// remove temporary token
// 				if (tmpScore<0) { crisis=1; cx=x; cy=y; }
// 				if (tmpScore > bestScore) {
// 					hintX = x;
// 					hintY = y;
// 					bestScore = tmpScore;
// 					move = 2;
// 					if (bestScore>=100000) return 100000;
// 				}
// 			}

// 	if (bestScore==0 && !crisis) { // no good positions where found -> conglomerate
// 		int numMoves=0;
// 		for(x=0;x<8;x++)
// 			for(y=0;y<8;y++)
// 				if (!board[x][y].token) {  // empty slot, search surroundings
// 					numMoves++;
// 					tmpScore = 0;
// 					for(i=MAX(x-1,0);i<MIN(8,x+2);i++)
// 						for(j=MAX(y-1,0);j<MIN(8,y+2);j++) 
// 							tmpScore += (x==i||y==j ? 2 : 1) * sArray[board[i][j].token];
// 					placeToken(x,y,player);
// 					board[x][y] = emptySquare;		// remove temporary token
// 					if (tmpScore > bestScore) {
// 						hintX = x;
// 						hintY = y;
// 						bestScore = tmpScore;
// 						move = 3;
// 						if (bestScore>=100000) return 100000;
// 					}
// 				}
// 		if (numMoves==0) crisis=3;
// 	}

	
// 	// check if opponent can win next turn
// 	if (recurse && !crisis && move>1) {
// 		int ox,oy;
// 		ox = hintX;	// backup own move
// 		oy = hintY;  // backup own move

// 		// do move (place)
// 		if (placeToken(ox,oy,player)) {			// make move if available
// 			i = getHint((player==1?2:1),0);		// no recurse!!!
// 			board[ox][oy] = emptySquare;		// remove temporary token
// 			if (i < 10000) {					// opponent can't win next time, use own move
// 				hintX = ox;	
// 				hintY = oy;  
// 			}	
// 			else {  // opponent can win, stop him!
// 				crisis = 1;
// 			}
// 		}
// 		else 
// 			crisis = 3;   // can't find spot, make random slide
// 	}

// 	if (crisis==1) { // opponent can win by placing a token
// 		getHint((player==1?2:1),0); // find out where and put it there
// 	}
// 	if (crisis==2) {  // opponent can win by sliding -> must slide other way
// 		if (cy==1) { hintX = cx; hintY = 8; }
// 		if (cy==2) { hintY = cx; hintX = -1; }
// 		if (cy==3) { hintX = cx; hintY = -1; }
// 		if (cy==4) { hintY = cx; hintX = 8; }
// 	}
// 	if (crisis==3) {  // can't find good slide and board is full -> random slide
// 		int r = rand()%100;
// 		if (r>75) {	hintX = rand()%8; hintY = 8; }
// 		else if (r>50) { hintX = rand()%8; hintY = -1; }
// 		else if (r>25) { hintY = rand()%8; hintX = -1; }
// 		else { hintY = rand()%8; hintX = 8; }
// 	}

// 	return bestScore;
// }

int play() {
// 	int done = 0;
// 	int x,y;
// 	int mx,my;
// 	int clicked = 0;
// 	int tokenCount = 0;
// 	int stoneCount = 0;
// 	int thinking = 0;

	if (action_counter <= 1)
	{
		jo_clear_screen();
		jo_set_default_background_color(JO_COLOR_INDEX_Black);
		CDDA_Stop();
	}
	
	// 	play_midi(data[SONG1+rand()%4].dat,1);
// 	playingMidi=1;

	start_new_game();
	draw_game(1);
// 	blitScreen();
// 	fade_in(data[GAMEPAL].dat,4);

// 	while (!done && !winner) {
// 		if (key[KEY_W]) set_gfx_mode(GFX_GDI, 320, 240, 0, 0);
// 		game_count=0;
// 		draw_game(1);
// 		blitScreen();

// 		if (ply[1].anim) ply[1].anim--;
// 		if (ply[2].anim) ply[2].anim--;

// 		if (hint) hint--;
// 		if (key[KEY_H]) getHint(player,3);

// 		if (scrolling) if (--scrolling==0) {
// 			playing=1;
// 			checkBoard(player);
// 			player = (player==1?2:1); // next player
// 		}

// 		if (placeing) {
// 			placeing -= 2;
// 			if (placeing==0) {
// 				playing=1;
// 				checkBoard(player);
// 				player = (player==1?2:1); // next player
// 				tokenCount++;
// 				if (tokenCount==10) {
// 					tokenCount=0;
// 					if (stoneCount<10) {
// 						if (!animPlaceToken(rand()%8,rand()%8,4)) {  // can't place stone, try next time
// 							tokenCount = 9;
// 						} 
// 						else  {// stone placed
// 							stoneCount++;
// 							player = (player==1?2:1); // adjust player
// 						}
// 					}
// 				}
// 			}
// 		}

// 		if (cpu==player && playing && !thinking) thinking = 30;
// 		if (cpu==player && thinking) thinking--;
// 		if (thinking==1) {
// 			thinking=0;
// 			getHint(player,3);
// 			if (hintX>7 || hintY>7 || hintX<0 || hintY<0) { // slide
// 				if (hintX<0) animRotateRow(hintY, 1);
// 				if (hintX>7) animRotateRow(hintY, 0);
// 				if (hintY<0) animRotateColumn(hintX, 1);
// 				if (hintY>7) animRotateColumn(hintX, 0);
// 			}
// 			else { // place
// 				if (ply[player].multi) {  // use multi if available
// 					ply[player].multi--;
// 					ply[player].carry++;
// 				}
// 				animPlaceToken(hintX,hintY,(ply[player].carry?3:player));
// 				lockedCol = lockedRow = -1;
// 				ply[player].carry = 0;
// 			}
// 		}

// 		if (mouse_b!=1) clicked = 0;
// 		if (mouse_b==1 && !clicked && cpu!=player) {   // user has clicked mouse
// 			clicked++;
// 			mx = mouse_x;
// 			my = mouse_y;
			
// 			if (playing) {
// 				// check board
// 				for(x=0;x<8;x++)
// 					for(y=0;y<8;y++)
// 						if (mx>21+x*24 && mx<44+x*24 && my>21+y*24 && my<44+y*24) {
// 							if (animPlaceToken(x,y,(ply[player].carry?3:player))) {
// 								lockedCol = lockedRow = -1;
// 								ply[player].carry = 0;
// 							}
// 						}

// 				// check arrows
// 				if (!ply[player].carry) 
// 					for(x=0;x<8;x++) {
// 						int moved = 0;
// 						if (mx>27+x*24 && mx<37+x*24 && my>6 && my<16 && lockedCol!=x) moved = animRotateColumn(x, 1);
// 						if (mx>27+x*24 && mx<37+x*24 && my>216 && my<226 && lockedCol!=x) moved = animRotateColumn(x, 0);
// 						if (mx>6 && mx<16 && my>27+x*24 && my<37+x*24 && lockedRow!=x) moved = animRotateRow(x, 1);
// 						if (mx>216 && mx<226 && my>27+x*24 && my<37+x*24 && lockedRow!=x) moved = animRotateRow(x, 0);
// 					}
				
// 				// check other (multi)
// 				if (ply[player].multi && !ply[player].carry) {
// 					if (mx>245 && mx<268 && my>73+112*(player-1) && my<96+112*(player-1)) {
// 						ply[player].multi--;
// 						ply[player].carry = 1;
// 					}
// 				}
// 				else if (ply[player].carry) {
// 					if (mx>245 && mx<268 && my>73+112*(player-1) && my<96+112*(player-1)) {
// 						ply[player].multi ++;
// 						ply[player].carry = 0;
// 					}
// 				}
// 			}

// 			while(!game_count);
// 		}

// 		if (key[KEY_ESC]) done = confirm("Really quit? (Y/N)");
// 		while(!game_count);
// 	}

// 	if (winner) {
// 		char buf[128];
// 		Thisc tmp;

// 		winner = (ply[1].score>ply[2].score ? 1:2);
// 		done = 0;
// 		x=0;
// 		if (ply[1].score == ply[2].score) winner = 3;
// 		else ply[winner].anim = 100000;
// 		if (!cpu) sprintf(buf,"Player %d won the game!",winner);
// 		else {
// 			if (cpu==winner) {
// 				if (winner==1) sprintf(buf,"%s won the game!","ALEX");
// 				if (winner==2) sprintf(buf,"%s won the game!","AARON");
// 			}
// 			else sprintf(buf,"%s won the game!","YOU");
// 		}

// 		while(!done) {
// 			game_count=0;
// 			draw_game(1);
// 			if (x==0) {
// 				textout_centre(swap_screen, data[MYFONT].dat, "Board cleared!", 161, 91, 1);
// 				textout_centre(swap_screen, data[MYFONT].dat, "Board cleared!", 160, 90, -1);
// 			}
// 			if (x==1) { 
// 				if (winner<3) {
// 					textprintf_centre(swap_screen, data[MYFONT].dat, 161, 91, 1, buf, winner);
// 					textprintf_centre(swap_screen, data[MYFONT].dat, 160, 90, -1, buf, winner);
// 				}
// 				else {
// 					textout_centre(swap_screen, data[MYFONT].dat, "It's a draw!", 161, 91, 1);
// 					textout_centre(swap_screen, data[MYFONT].dat, "It's a draw!", 160, 90, -1);
// 				}
// 			}
// 			if (x==2) done = 1;
// 			blitScreen();
// 			if (mouse_b!=1) clicked = 0;
// 			if (!clicked && mouse_b==1) { x++; clicked = 1; }
// 			while(!game_count);
// 		}
// 		fade_out(4);
// 		if (ply[1].score>=ply[2].score) {
// 			tmp.score = ply[1].score;
// 			if (qualifyTable(hisc, tmp) && cpu!=1) enterHof(tmp,1);
// 			tmp.score = ply[2].score;
// 			if (qualifyTable(hisc, tmp) && cpu!=2) enterHof(tmp,2);
// 		}
// 		else {
// 			tmp.score = ply[2].score;
// 			if (qualifyTable(hisc, tmp) && cpu!=2) enterHof(tmp,2);
// 			tmp.score = ply[1].score;
// 			if (qualifyTable(hisc, tmp) && cpu!=1) enterHof(tmp,1);
// 		}
// 		showHighscores();
// 	}

// 	fade_out(4);
// 	play_midi(NULL,0);
// 	playingMidi=0;

// 	return 0;
}

// int myRest(int msecs) {
// 	int gclick = msecs/15;
// 	game_count=0;
// 	while(game_count<gclick && !key[KEY_ESC]);
// 	if (key[KEY_ESC]) return 1;
// 	return 0;
// }

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
			fade_counter = 0;
		}
		else if (action_counter % 4 == 0 && fade_counter <= JO_DEFAULT_BRIGHTNESS)
			fade_counter++;

		if (fade_counter <= JO_DEFAULT_BRIGHTNESS)
		{
			jo_sprite_enable_gouraud_shading();
			jo_set_gouraud_shading_brightness(fade_counter);
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
			fade_counter = JO_DEFAULT_BRIGHTNESS;
		}
		else if (action_counter % 4 == 0 && fade_counter >= 0)
			fade_counter--;

		if (fade_counter >= 0)
		{
			jo_sprite_enable_gouraud_shading();
			jo_set_gouraud_shading_brightness(fade_counter);
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
			game_mode = MODE_TITLE;
		}
	}
}

static void process_intro_graphic_fade()
{
	if (action_counter <= 1)
	{
		fade_counter = JO_DEFAULT_BRIGHTNESS;
	}
	else if (action_counter % 4 == 0 && fade_counter >= 0)
		fade_counter--;

	if (fade_counter >= 0)
	{
		jo_sprite_enable_gouraud_shading();
		jo_set_gouraud_shading_brightness(fade_counter);
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

void intro() 
{
	if (game_mode != previous_game_mode)
	{
		previous_game_mode = MODE_INTRO;
		CDDA_PlaySingle(TITLE_TRACKID, true);
	}

	input_type current_input = get_input_types(game_mode, current_input_states);

	if (current_input == INPUT_TYPE_START || 
		current_input == INPUT_TYPE_A || 
		current_input == INPUT_TYPE_C)
	{
		// user wants to skip to title
		action_counter = 0;
		intro_text_shown = true;
		game_mode = MODE_TITLE;
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

// void outro() {
// 	fade_out(4);
// 	play_midi(NULL,0);
// 	if (fadeText("Thanks for playing.",1000)) return;
// 	if (fadeText("See you in next SpeedHack.",1500)) return;
// 	myRest(1000);
// }

// void shutdown() {
// 	saveTable(hisc,"hiscores.sav");
// 	saveSoundCFG();
// 	allegro_exit();
// }

// void printPage(char *txt, int page) {
// 	int i,p,x,y;
// 	char buf[2] = {' ','\0'};

// 	// draw header
// 	draw_character(swap_screen, data[TITLE].dat, 8, 21, 1);
// 	draw_sprite(swap_screen,data[TITLE].dat,7,20);
// 	draw_rle_sprite(swap_screen,data[AA2].dat,250,10);
// 	color_map = &trans_table;
// 	draw_trans_sprite(swap_screen,data[TITLE].dat,7,20);

// 	// find page
// 	i=0; p=0;
// 	while(p!=page) {
// 		if (txt[i] == '@') p++;
// 		i++;
// 	}
// 	if (i) i+=2;

// 	// print text
// 	x=10; y=70;
// 	while(txt[i] != '@') {
// 		if (txt[i+1] == '\n') {
// 			y += 12;
// 			x = 10;
// 			i += 2;
// 		}
// 		else {
// 			buf[0] = txt[i];
// 			textout(swap_screen,data[MYFONT2].dat, buf, x+1, y+1, 1);
// 			textout(swap_screen,data[MYFONT2].dat, buf, x, y, -1);
// 			x += text_length(data[MYFONT2].dat,buf);
// 			i++;
// 		}
// 	}
// }

void instructions() 
{
	if (action_counter <= 1)
	{
		jo_clear_screen();
		jo_set_default_background_color(JO_COLOR_RGB(121,52,52)); // pink
		current_instructions_page_index = 0;
	}
	
	input_type current_input = get_input_types(game_mode, current_input_states);

	if (current_input == INPUT_TYPE_START ||
		current_input == INPUT_TYPE_A || 
		current_input == INPUT_TYPE_C)
	{
		// user wants to return to title
		action_counter = 0;
		game_mode = MODE_TITLE;
	}
	else if (current_input == INPUT_TYPE_LEFT)
	{
		if (current_instructions_page_index > 0)
			current_instructions_page_index--;
	}
	else if (current_input == INPUT_TYPE_RIGHT)
	{
		if (current_instructions_page_index < (INSTRUCTIONS_PAGE_COUNT-1))
			current_instructions_page_index++;
	}

	jo_sprite_draw3D2(title_sprite_id, 0, 16, BACKGROUND_ZINDEX);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(aa2_sprite_id, AA2_FINAL_X, 8, 450);
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


	// 	int currPage=0;
// 	char *txt = data[INSTRUCTIONS].dat;
// 	int pressed=0;

// 	reset_particles();

// 	clear_to_color(swap_screen,40);
// 	printPage(txt, currPage);
// 	blitScreen();

// 	fade_in(data[GAMEPAL].dat,4);
// 	while(!key[KEY_ESC]) {
// 		clear_to_color(swap_screen,40);
// 		draw_donkeys();
// 		printPage(txt, currPage);
// 		blitScreen();
// 		if (rand()%500<5) createDonkey(-40,rand()%220+20,rand()%4);
// 		if (!pressed && key[KEY_LEFT] && currPage>0) { currPage--; pressed = 1; }
// 		if (!pressed && key[KEY_RIGHT] && currPage<8) { currPage++; pressed = 1; }
// 		if (!key[KEY_LEFT] && !key[KEY_RIGHT]) pressed = 0;
// 	}

// 	fade_out(4);
// 	clear(screen);
}

void credits() 
{
	if (action_counter <= 1)
	{
		jo_clear_screen();
		jo_set_default_background_color(JO_COLOR_RGB(140,110,75)); // brown
	}
	
	input_type current_input = get_input_types(game_mode, current_input_states);

	if (current_input == INPUT_TYPE_START ||
		current_input == INPUT_TYPE_A || 
		current_input == INPUT_TYPE_C)
	{
		// user wants to return to title
		action_counter = 0;
		game_mode = MODE_TITLE;
	}

	jo_sprite_draw3D2(title_sprite_id, 0, 16, BACKGROUND_ZINDEX);
	jo_sprite_enable_half_transparency();
	jo_sprite_draw3D2(aa2_sprite_id, AA2_FINAL_X, 8, 450);
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

}

void update_game()
{
	// while (playGame) {
	// 	playGame = title();
	// 	if (playGame==3) play();
	// 	if (playGame==2) showHighscores();
	// 	if (playGame==1) instructions();
	// }
	// outro();
	// shutdown();
	if (action_counter < MAX_ACTION_CYCLES)
		action_counter++;

	switch (game_mode)
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

		case MODE_GAME:
			play();
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