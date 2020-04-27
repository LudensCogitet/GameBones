#ifndef GB_GFX_SYSTEM_H_INCLUDED
#define GB_GFX_SYSTEM_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdint.h>

#include "./gbSprite_type.h"
#include "./gbGfxLayer_type.h"

void gbGfxInit();
void gbGfxTeardown();

gbSprite *gbGfxSpriteNew(
    GB_GFX_LAYER layer,
    unsigned int texture,

    int srcX,
    int srcY,
    int srcW,
    int srcH,

    gbPosition *pos,
    int w,
    int h,

    uint8_t fixed
);

void gbGfxMoveSprite(int32_t x, int32_t y, gbSprite *sprite);
void gbGfxDraw();
//
//void gb_gfx_font_load(char *ttfFile, GB_GFX_FONT font, uint16_t pt);
//void gb_gfx_font_unload(GB_GFX_FONT font);
//
//void gb_gfx_font_set(GB_GFX_FONT font);
//void gb_gfx_font_color_set(GB_GFX_COLOR color);
//void gb_gfx_font_layer_set(GB_GFX_LAYER layer);
//
//gbSprite *gb_gfx_new_text(char *text, uint32_t wrapW, double x, double y, uint8_t anchor, uint8_t fixed);
//void gb_gfx_text_change(gbSprite *sprite, char *text, uint32_t wrapW);

void gb_gfx_camera_set(int32_t x, int32_t y);
void gb_gfx_camera_move(int32_t dx, int32_t dy);
void gb_gfx_camera_get_pos(int32_t *x, int32_t *y);
void gb_gfx_get_camera_offset(int32_t *x, int32_t *y);
void gb_gfx_camera_follow(double *x, double *y, uint32_t distance_x, uint32_t distance_y);
void gb_gfx_camera_unfollow();

void gb_gfx_screen_to_world_coords(int *x, int *y);

#endif // GB_GFX_SYSTEM_H_INCLUDED
