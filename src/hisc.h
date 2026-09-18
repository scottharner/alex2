/*
 * Hiscore-lib
 * 2001 (c) Johan Peitz 
 */

#define MAX_SCORES      10

typedef struct {
   char name[20];
   unsigned int score;
} Thisc;

Thisc* make_table();
int qualify_table(Thisc *table, Thisc post);
void sort_table(Thisc *table);
void enter_table(Thisc *table, Thisc post);
void reset_table(Thisc *table, char *name, int hi, int lo);
int loadTable(Thisc *table, char *fname);
void saveTable(Thisc *table, char *fname);
