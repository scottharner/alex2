/*
 *   Alex the Allegator 2
 */

typedef unsigned char byte;

typedef struct {
	int score;
	int anim;
	int multi;
	int carry;
	int animOffset;
} Tplayer;

Tplayer resetPlayer = {0,0,0,0,0};

typedef struct {
	fixed x;
	fixed y;
	fixed dx;
	fixed dy;
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

Ttoken emptySquare = { 0,		  	  0, 0 };
Ttoken greenToken  = { GREENTOKEN,   10, 0 };
Ttoken blueToken   = { BLUETOKEN,	 10, 0 };
Ttoken multiToken  = { MULTITOKEN,   50, 0 };
Ttoken deadToken  =  { DEADTOKEN,   100, 0 };


