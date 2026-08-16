/*
 * Hiscore-lib
 * 2001 (c) Johan Peitz 
 */

#ifndef HISC_H
#define HISC_H

#define MAX_SCORES      10

typedef struct {
   char name[20];
   unsigned int score;
} Thisc;

Thisc* makeTable();
int qualifyTable(Thisc *table, Thisc post);
void sortTable(Thisc *table);
void enterTable(Thisc *table, Thisc post);
void resetTable(Thisc *table, char *name, int hi, int lo);
int loadTable(Thisc *table);
void saveTable(Thisc *table);
void resetScores(Thisc *table);

#endif