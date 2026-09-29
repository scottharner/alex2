/*
 * Saturn project utilities
 * 2026 (c) Scott Harner
 */

#include "utils.h"
#include <jo/jo.h>

// generates a random number from 1 to max
int get_random(int max)
{
    if (max <= 0) return 0;
    return (jo_random(max)); // jo_random requires passing parm so we cant use modulo
}

// draws a sprite tile at the specified location
void draw_tile(int x, int y, int sprite_id, int z, int angle)
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

// some of the logic was written to work with allegro binary angles but we need radian angles for jo engine
jo_fixed get_radian_angle(jo_fixed binary_angle)
{
    return binary_angle * ((2 * JO_PI) / 256);
}