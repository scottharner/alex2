# alex2
Alex the Allegator 2 port to Sega Saturn

                           ************************
                           * ALEX THE ALLEGATOR 2 *
                           * (c) Johan Peitz 2001 *
                           ************************

GENERAL

Alex the Allegator 2 is a puzzle game for one or two players. Place your 
tokens in turn on the board or slide a row or column. All in order to 
wipe the board from your own tokens. But beware, you must also have a 
higher score to beat your opponent.


WHY?

Alex the Allegator 2 was written for the Allegro Speedhack 2001. During 
72 hrs you had to create a game under the following circumstances:

        1) The genre was to be puzzle game.
        2) It must contain a particle engine.
        3) Implement a high score table.
        4) Have a donkey somewhere in the production.
        5) Advertise for an imaginary product.
        6) ZIP-file including source and datafiles less than 250KB.

The game reaches all constraints: It is a puzzle game, particles are used
for the removal of blocks as well as the jumping donkeys in the background.
The high score table is fully implemented and donkeys (as said above) jump
around every here and there. The intialized high score table holds an
advertisment for spam and the ZIP-file is less than 250 KB.

All code was produced during the weekend except for the hiscore stuff, I had
written it for another game earlier. The same goes for the graphics, except 
for 2 frames of allegator action, everything is new.


COMPILING

If you only have the source to this game, you need to compile it before you
can play it. I haven't bothered to create a makefile, instead run the batch
file COMPILE.BAT. You can then start the game by entering ALEX2.

Compiled and ready to play versions for both windows and dos can be found
at http://www.dtek.chalmer.se/~d98peitz/alex
or http://www.dtek.chalmer.se/~d98peitz/fld


RULES

Each player has an unlimited set of tokens that can be played on the board.
When of 4 tokens of the same color connect in a square, they are all removed.
When there is no more tokens of one coler on the board, the player with
the highest score wins the game.


MOVES

Every turn you can either place a token, or slide a row or column on the
board one slot. A slided row or column may not be moved the next turn by
the other player.


SPECIAL TOKENS

Except for the normal tokens each players have there are also some special
ones. Yellow tokens act as multicolored and can be used anywhere. Gray tokens
can only be removed if surrounded by 3 yellow ones.

About every tenth time a token is placed on the board, a gray token
appeares at a random place. If a player manages to remove more then
4 tokens in one go, he is rewareded with a yellow token, that he can
play at any time.


SCORING

When tokens are removed from the board, the player who removed them recieves
some score. This means that if green player slides a row and 4 blue tokens
are removed, green player gets the score.

        Normal token           10 point
        Multi token            50 points
        Stone token           100 points

Also, the more tokens removed in one go, the higher score you get. The
proportion is exponential menaing that removing 4 tokens and then 4 more will
score less then removing all 8 in one go.


HELP

If you would like to have a clue on where to put your tokens, press 'H' on 
your keyboard. A cross will give you a suggestion on how to play.


AUTHORS & CONTACT

    Alex the Allegator 2 was written and designed by Johan Peitz. Johan Peitz
    also draw the graphics.

    If you have comments, bug reports, or just want to ask something
    you can contact me by e-mail.

      E-Mail: d98peitz@dtek.chalmers.se

    You can also visit me on the web.

      http://www.dtek.chalmers.se/~d98peitz/fld


DISCLAIMER

I do not accept responsibility for any effects, adverse or otherwise,
that this software may have on you, your computer, your sanity, your
dog, or anything else that you can think of. Use it at your own risk.
I have, however, never experienced any trouble with it.
                           



