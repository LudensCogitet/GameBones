#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL_ttf.h>

#include "../gbRenderer/gbRenderer_sys.h"
#include "../gbCollision/gbCollision_sys.h"
#include "./gbGfx_sys.h"

#include "../gbTexture/gbTexture_sys.h"
#include "../editor/editor.h"

#include "../Sprite/Sprite_type.h"
#include "../Position_type.h"
#include "./gbColor.h"
#include "./gbFont.h"
#include "./gbText/gbText_type.h"

uint8_t GB_GFX_DEBUG_FLAG = 1;

static int32_t gbGfxCameraOffsetX = 0;
static int32_t gbGfxCameraOffsetY = 0;

// Forward Declarations
void gbGfxFontUnload(GB_FONT font);
void gbGfxFontLoad(char *ttfFile, GB_FONT font, uint16_t pt);

void gbGfxInit() {
    spriteInit();

    colors[GB_COLOR_WHITE] = (SDL_Color){ 255, 255, 255, 255 };
    colors[GB_COLOR_BLACK] = (SDL_Color){ 0, 0, 0, 255 };

    TTF_Init();
    gbGfxFontLoad("./assets/FreeMono.ttf", GB_FONT_MID_FREE_MONO, 30);
    //TTF_SetFontStyle(fonts[GB_FONT_MID_FREE_MONO], TTF_STYLE_BOLD);
}


void gbGfxTeardown() {
    spriteTeardown();

    for (unsigned int i = 0; i < GB_FONT_NUM_FONTS; i++) {
        gbGfxFontUnload(i);
    }

    TTF_Quit();
}

//void gbGfxCameraUpdate();
void gbGfxDraw() {
    //gbGfxCameraUpdate();

    SDL_RenderClear(gbMainRenderer);

    if (GB_GFX_DEBUG_FLAG) {
        int maxX = (GB_GFX_GRID_OFFSET_X + ((GB_GFX_GRID_WIDTH) * GB_GFX_GRID_SIZE)) * gbScaleFactorX;
        int maxY = (GB_GFX_GRID_OFFSET_Y + ((GB_GFX_GRID_HEIGHT) * GB_GFX_GRID_SIZE)) * gbScaleFactorY;

        SDL_SetRenderDrawColor(gbMainRenderer, 0x00, 0x00, 0x00, 0xFF);

        for (int x = GB_GFX_GRID_OFFSET_X * gbScaleFactorX; x <= maxX; x += GB_GFX_GRID_SIZE * gbScaleFactorX) {
            SDL_RenderDrawLine(gbMainRenderer, x, GB_GFX_GRID_OFFSET_Y * gbScaleFactorY, x, maxY);
        }

        for (int y = GB_GFX_GRID_OFFSET_Y * gbScaleFactorY; y <= maxY; y += GB_GFX_GRID_SIZE * gbScaleFactorY) {
            SDL_RenderDrawLine(gbMainRenderer, GB_GFX_GRID_OFFSET_X * gbScaleFactorX, y, maxX, y);
        }
        gbRendererResetDrawColor();
    }

    spriteDraw();

    if (GB_GFX_DEBUG_FLAG) {
        collisionDebugDraw();
        editorRender();
    }


    SDL_RenderPresent(gbMainRenderer);
}

void gbGfxScreenToWorldCoords(int *x, int *y) {
    *x /= gbScaleFactorX;
    *y /= gbScaleFactorY;
}

// Return value indicates whether screen coords lie inside grid
uint8_t gbGfxScreenCoordsToGridSquare(int x, int y, int *gridX, int *gridY) {
    uint8_t insideGrid = 1;
    gbGfxScreenToWorldCoords(&x, &y);

    if (x < GB_GFX_GRID_OFFSET_X) {
        insideGrid = 0;
        x = GB_GFX_GRID_OFFSET_X;
    } else if (x > GB_GFX_GRID_OFFSET_X + (GB_GFX_GRID_WIDTH * GB_GFX_GRID_SIZE)) {
        insideGrid = 0;
        x = GB_GFX_GRID_OFFSET_X + ((GB_GFX_GRID_WIDTH - 1) * GB_GFX_GRID_SIZE);
    }

    if (y < GB_GFX_GRID_OFFSET_Y) {
        insideGrid = 0;
        y = GB_GFX_GRID_OFFSET_Y;
    } else if (y > GB_GFX_GRID_OFFSET_Y + (GB_GFX_GRID_HEIGHT * GB_GFX_GRID_SIZE)) {
        insideGrid = 0;
        y = GB_GFX_GRID_OFFSET_Y + ((GB_GFX_GRID_HEIGHT - 1) * GB_GFX_GRID_SIZE);
    }

    *gridX = x - GB_GFX_GRID_OFFSET_X;
    *gridY = y - GB_GFX_GRID_OFFSET_Y;

    *gridX /= GB_GFX_GRID_SIZE;
    *gridY /= GB_GFX_GRID_SIZE;

    return insideGrid;
}

void gbGfxGridSquareToWorldCoords(int x, int y, int *worldX, int *worldY, uint8_t bottomRight) {
    *worldX = GB_GFX_GRID_OFFSET_X + (x * GB_GFX_GRID_SIZE);
    *worldY = GB_GFX_GRID_OFFSET_Y + (y * GB_GFX_GRID_SIZE);

    if (bottomRight) {
        *worldX += GB_GFX_GRID_SIZE;
        *worldY += GB_GFX_GRID_SIZE;
    }
}

/* FONTS */

void gbGfxFontUnload(GB_FONT font) {
    if (fonts[font] != 0) {
        TTF_CloseFont(fonts[font]);
        fonts[font] = 0;
    }
}

void gbGfxFontLoad(char *ttfFile, GB_FONT font, uint16_t pt) {
    gbGfxFontUnload(font);
    //pt = (pt * ((float)LOGICAL_SCREEN_HEIGHT) / (float)gb_screen_height );

    fonts[font] = TTF_OpenFont(ttfFile, pt * gbScaleFactorY);

    if (!fonts[font]) {
        printf("Error loading font: %s", TTF_GetError());
    }
}

/* CAMERA */

//static double *follow_x = 0;
//static double *follow_y = 0;
//static int32_t follow_distance_x = 0;
//static int32_t follow_distance_y = 0;
//
//void gb_gfx_camera_set(int32_t x, int32_t y) {
//    gbGfxCameraOffsetX = x;
//    gbGfxCameraOffsetY = y;
//}
//
//void gb_gfx_camera_move(int32_t dx, int32_t dy) {
//    gbGfxCameraOffsetX += dx;
//    gbGfxCameraOffsetY += dy;
//}
//
//void gb_gfx_camera_get_pos(int32_t *x, int32_t *y) {
//    *x = gbGfxCameraOffsetX * gbScaleFactorX;
//    *y = gbGfxCameraOffsetY * gbScaleFactorY;
//}
//
//void gb_gfx_get_camera_offset(int32_t *x, int32_t *y) {
//    *x = gbGfxCameraOffsetX;
//    *y = gbGfxCameraOffsetY;
//}
//
//void gb_gfx_camera_follow(double *x, double *y, uint32_t distance_x, uint32_t distance_y) {
//    follow_x = x;
//    follow_y = y;
//    follow_distance_x = distance_x;
//    follow_distance_y = distance_y;
//}
//
//void gb_gfx_camera_unfollow() {
//    follow_x = 0;
//    follow_y = 0;
//    follow_distance_x = 0;
//    follow_distance_y = 0;
//}
//
//void gbGfxCameraUpdate() {
//    if (!follow_x || !follow_y) return;
//
//    int32_t dx = 0;
//    int32_t dy = 0;
//
//    int16_t leftDiff = *follow_x - gbGfxCameraOffsetX;
//
//    if (abs(leftDiff) <= follow_distance_x) {
//        dx = leftDiff - follow_distance_x;
//    } else {
//        int16_t rightDiff = (gbGfxCameraOffsetX + LOGICAL_SCREEN_WIDTH) - *follow_x;
//        if (abs(rightDiff) <= follow_distance_x) {
//            dx = -(rightDiff - follow_distance_x);
//        }
//    }
//
//    int16_t topDiff = *follow_y - gbGfxCameraOffsetY;
//
//    if (abs(topDiff) <= follow_distance_y) {
//        dy = topDiff - follow_distance_y;
//    } else {
//        int16_t bottomDiff = (gbGfxCameraOffsetY + LOGICAL_SCREEN_HEIGHT) - *follow_y;
//        if (abs(bottomDiff) <= follow_distance_y) {
//            dy = -(bottomDiff - follow_distance_y);
//        }
//    }
//
//    if (dx || dy) {
//        gb_gfx_camera_move(dx, dy);
//    }
//}
