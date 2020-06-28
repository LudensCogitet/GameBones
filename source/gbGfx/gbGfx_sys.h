#ifndef GB_GFX_SYSTEM_H_INCLUDED
#define GB_GFX_SYSTEM_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdint.h>

#include "./gbSprite/gbSprite_type.h"
#include "./gbText/gbText_type.h"
#include "./gbFont.h"
#include "./gbColor.h"

#define GB_GFX_GRID_WIDTH       20
#define GB_GFX_GRID_HEIGHT      20

#define GB_GFX_GRID_SIZE        (GB_LOGICAL_SCREEN_WIDTH / 40)

#define GB_GFX_GRID_OFFSET_X    (GB_GFX_GRID_SIZE * 10)
#define GB_GFX_GRID_OFFSET_Y    (GB_GFX_GRID_SIZE * 1.25)

void gbGfxInit();
void gbGfxTeardown();

void gbGfxMoveSprite(int32_t x, int32_t y, gbSprite *sprite);
void gbGfxDraw();

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
