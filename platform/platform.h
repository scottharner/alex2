#ifndef PLATFORM_H
#define PLATFORM_H

#include "../common/types.h"
#include "../game/hisc.h"

int platform_load_score_table(Thisc *, char *);
void platform_save_score_table(Thisc *, char *);
void * platform_memory_allocate(unsigned int);
void platform_copy_string(char *, char *);

#endif