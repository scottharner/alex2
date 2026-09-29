/*
 * Saturn project utilities
 * 2026 (c) Scott Harner
 */

 #ifndef UTILS_H
 #define UTILS_H

 #include <jo/jo.h>
 
int get_random(int max);
void draw_tile(int x, int y, int sprite_id, int z, int angle);
jo_fixed get_radian_angle(jo_fixed binary_angle);

#endif