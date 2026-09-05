/*
 *   Alex the Allegator 2
 */

 #ifndef MAIN_H
 #define MAIN_H

 #include <jo/jo.h>
 
typedef unsigned char byte;

typedef struct {
	int score;
	int anim;
	int multi;
	int carry;
	int anim_offset;
} Tplayer;

Tplayer reset_player = {0,0,0,0,0};

typedef struct {
	jo_fixed x;
	jo_fixed y;
	jo_fixed dx;
	jo_fixed dy;
	byte image;
	byte exist;
} Tparticle;
#define	MAX_PARTICLES	1024

#define BLUETOKEN	1
#define GREENTOKEN	2
#define MULTITOKEN	3
#define DEADTOKEN	4

typedef struct {
	byte token;     // kind of token
	int score;		// value of token
	byte flag;		// used for lots of stuff
} Ttoken;

typedef enum
{
	MODE_NONE = 0, 
	MODE_INTRO, 
	MODE_TITLE,
	MODE_INSTRUCTIONS,
	MODE_CREDITS,
	MODE_HIGH_SCORES,
	MODE_GAME
}mode;

typedef enum
{
	GAME_TYPE_HVC = 0,
	GAME_TYPE_CVH,
	GAME_TYPE_HVH
}game_type;

typedef enum
{
	INPUT_TYPE_NOTHING = 0, 
	INPUT_TYPE_LEFT, 
	INPUT_TYPE_RIGHT, 
	INPUT_TYPE_UP, 
	INPUT_TYPE_DOWN, 
	INPUT_TYPE_START, 
	INPUT_TYPE_UP_LEFT, 
	INPUT_TYPE_UP_RIGHT, 
	INPUT_TYPE_DOWN_LEFT, 
	INPUT_TYPE_DOWN_RIGHT, 
	INPUT_TYPE_A,
	INPUT_TYPE_C,
	INPUT_TYPE_COUNT
}input_type;

Ttoken empty_square = { 0,		  	  0, 0 };
Ttoken green_token  = { GREENTOKEN,   10, 0 };
Ttoken blue_token   = { BLUETOKEN,	 10, 0 };
Ttoken multi_token  = { MULTITOKEN,   50, 0 };
Ttoken dead_token  =  { DEADTOKEN,   100, 0 };

#endif