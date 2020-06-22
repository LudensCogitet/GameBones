#ifndef GB_GFX_SYSTEM_H_INCLUDED
#define GB_GFX_SYSTEM_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdint.h>

#include "./gbSprite_type.h"
#include "./gbGfxLayer_type.h"
#include "./gbText_type.h"
#include "./gbFont_type.h"
#include "./gbColor_type.h"

#define GB_GFX_GRID_WIDTH       20
#define GB_GFX_GRID_HEIGHT      20

#define GB_GFX_GRID_SIZE        (GB_LOGICAL_SCREEN_WIDTH / 40)

#define GB_GFX_GRID_OFFSET_X    (GB_GFX_GRID_SIZE * 10)
#define GB_GFX_GRID_OFFSET_Y    (GB_GFX_GRID_SIZE * 1.25)

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
    uint8_t active,
    uint8_t fixed,
    SDL_RendererFlip flip
);

void gbGfxMoveSprite(int32_t x, int32_t y, gbSprite *sprite);
void gbGfxDraw();

gbText *gbGfxTextNew(const char *text,
                       GB_FONT font,
                       GB_COLOR color,
                       GB_GFX_LAYER layer,
                       double x,
                       double y,
                       uint8_t active,
                       uint8_t fixed);
void gbGfxTextDelete(gbText *text);
void gbGfxTextChange(gbText *text, GB_FONT font, GB_COLOR color, const char *newText);

//void gb_gfx_camera_set(int32_t x, int32_t y);
//void gb_gfx_camera_move(int32_t dx, int32_t dy);
//void gb_gfx_camera_get_pos(int32_t *x, int32_t *y);
//void gb_gfx_get_camera_offset(int32_t *x, int32_t *y);
//void gb_gfx_camera_follow(double *x, double *y, uint32_t distance_x, uint32_t distance_y);
//void gb_gfx_camera_unfollow();

void gbGfxScreenToWorldCoords(int *x, int *y);
uint8_t gbGfxScreenCoordsToGridSquare(int x, int y, int *gridX, int *gridY);
void gbGfxGridSquareToWorldCoords(int x, int y, int *worldX, int *worldY, uint8_t bottomRight);

#endif // GB_GFX_SYSTEM_H_INCLUDED
