/*
 * Hiscore-lib
 * 2001 (c) Johan Peitz 
 */

#include "hisc.h"
#include "../platform/platform.h"

/*
 *  Creates a table to work with
 */
Thisc* makeTable() {
	Thisc *tmp;

	tmp = platform_memory_allocate(MAX_SCORES*sizeof(Thisc));
	if (!tmp) return NULL;

	return tmp;
}

/*
 *  Check if provided score is allowd to enter the table
 */
int qualifyTable(Thisc *table, Thisc post) {
	int i;
	for (i=0;i<MAX_SCORES;i++)
		if (post.score >= table[i].score) return 1;
	return 0;
}

/* 
 * Sorts the table
 */
void sortTable(Thisc *table) {
	int i,j;
	Thisc tmp;

	for (i=1;i<MAX_SCORES;i++) {
		j=i;
		tmp = table[i];
		while (j!=0 && tmp.score > table[j-1].score) {
			table[j]= table[j-1];
			j--;
		}
		table[j] = tmp;
	}
}

/* 
 * Replaces the lowest score with the specified
 */
void enterTable(Thisc *table, Thisc post) { 
	unsigned int lo=10000000;
	int loID=-1;
	int i;

	// find lowest score
	for (i=0;i<MAX_SCORES;i++) {
		if (table[i].score < lo) {
			loID = i;
			lo = table[i].score;
		}
	}

	if (loID>=0) table[loID] = post;
}

/* 
 * Resets the table to the values specified
 */
void resetTable(Thisc *table, char *name, int hi, int lo) {
	int i;
	int d = (hi-lo)/MAX_SCORES;
	int acc = hi;

	for (i=0;i<MAX_SCORES;i++) {
		platform_copy_string(table[i].name, name);
		table[i].score = acc;
		acc-=d;
	}
}

/* 
 * Loads table from disk, returns 1 on success
 */
int loadTable(Thisc *table, char *fname) {
	return platform_load_score_table(table, fname);
}

/* 
 * Saves table to disk
 */
void saveTable(Thisc *table, char *fname) {
	platform_save_score_table(table, fname);
}

