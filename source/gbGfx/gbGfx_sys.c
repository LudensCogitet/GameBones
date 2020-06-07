#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#include <SDL2/SDL_ttf.h>

#include "../gbRenderer/gbRenderer_sys.h"
#include "./gbGfx_sys.h"

#include "../gbTexture/gbTexture_sys.h"

#include "./gbGfxLayer_type.h"
#include "./gbSprite_type.h"

#define GB_GFX_MAX_SPRITES_PER_LAYER 1000

//static unsigned int gb_gfx_dynamic_texture_cursor = GFX_TEXTURE_DYNAMIC_FIRST;

//static TTF_Font *gb_gfx_fonts[GFX_FONT_NUM_FONTS];
//
//static SDL_Color gb_gfx_colors[GFX_COLOR_NUM_COLORS];
//
//static GB_GFX_FONT gb_gfx_font;
//static GB_GFX_COLOR gb_gfx_font_color;
//static GB_GFX_LAYER gb_gfx_font_layer;

uint8_t GB_GFX_DEBUG_FLAG = 1;

static unsigned int gbGfxSpriteCursors[GB_GFX_LAYER_NUM_LAYERS];
static gbSprite     *gbGfxSprites[GB_GFX_LAYER_NUM_LAYERS][GB_GFX_MAX_SPRITES_PER_LAYER];

static int32_t gbGfxCameraOffsetX = 0;
static int32_t gbGfxCameraOffsetY = 0;

void gbGfxInit() {
    for (unsigned int l = 0; l < GB_GFX_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < GB_GFX_MAX_SPRITES_PER_LAYER; s++) {
            gbGfxSprites[l][s] = 0;
        }
    }

    for (unsigned int i = 0; i < GB_GFX_LAYER_NUM_LAYERS; i++) {
        gbGfxSpriteCursors[i] = 0;
    }

//    gb_gfx_colors[GFX_COLOR_WHITE].r = 255;
//    gb_gfx_colors[GFX_COLOR_WHITE].g = 255;
//    gb_gfx_colors[GFX_COLOR_WHITE].b = 255;
//    gb_gfx_colors[GFX_COLOR_WHITE].a = 255;
//
//    gb_gfx_colors[GFX_COLOR_BLACK].r = 0;
//    gb_gfx_colors[GFX_COLOR_BLACK].g = 0;
//    gb_gfx_colors[GFX_COLOR_BLACK].b = 0;
//    gb_gfx_colors[GFX_COLOR_BLACK].a = 255;
//
//    TTF_Init();
}


void gbGfxTeardown() {
    for (unsigned int l = 0; l < GB_GFX_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < gbGfxSpriteCursors[l]; s++) {
            free(gbGfxSprites[l][s]);
            gbGfxSprites[l][s] = 0;
        }
        gbGfxSpriteCursors[l] = 0;
    }

//    for (unsigned int i = 0; i < GFX_FONT_NUM_FONTS; i++) {
//        gb_gfx_font_unload(i);
//    }
//
//    TTF_Quit();
}

/* TEXTURES */



//GB_GFX_TEXTURE gb_gfx_texture_dynamic_get_free() {
//    GB_GFX_TEXTURE current = gb_gfx_dynamic_texture_cursor;
//
//    while (++gb_gfx_dynamic_texture_cursor != current) {
//        if (gb_gfx_dynamic_texture_cursor > GFX_TEXTURE_DYNAMIC_LAST)
//            gb_gfx_dynamic_texture_cursor = GFX_TEXTURE_DYNAMIC_FIRST;
//
//        if (gb_gfx_textures[gb_gfx_dynamic_texture_cursor] == 0) {
//            return current;
//        }
//    }
//
//    gb_gfx_dynamic_texture_cursor = GFX_TEXTURE_DYNAMIC_LAST + 1;
//
//    return current;
//}
//
//void gb_gfx_texture_dynamic_unload(GB_GFX_TEXTURE texture) {
//    gb_gfx_texture_unload(texture);
//    gb_gfx_dynamic_texture_cursor = texture;
//}

/* SPRITES */

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

    uint8_t fixed,
    SDL_RendererFlip flip
) {
    if (gbGfxSpriteCursors[layer] + 1 >= GB_GFX_MAX_SPRITES_PER_LAYER) {
        return 0;
    }

    gbSprite *newSprite = (gbSprite *)malloc(sizeof(gbSprite));

    newSprite->dispose = 0;

    newSprite->width    = w;
    newSprite->height   = h;

    newSprite->pos = pos;

    newSprite->src.x = srcX;
    newSprite->src.y = srcY;
    newSprite->src.w = srcW;
    newSprite->src.h = srcH;

    newSprite->texture = texture;
    newSprite->flip = flip;

    gbGfxSprites[layer][gbGfxSpriteCursors[layer]++] = newSprite;

    return newSprite;
}

//void gbGfxSpriteMove(int32_t x, int32_t y, gbSprite *sprite) {
//    if (sprite->anchor == GFX_ANCHOR_DEFAULT) {
//        sprite->pos->x = x - sprite->width * 0.5;
//        sprite->pos->y = y - sprite->height * 0.5;
//        return;
//    }
//
//    if (sprite->anchor & GFX_ANCHOR_TOP) {
//        sprite->height = y;
//    } else if (sprite->anchor & GFX_ANCHOR_BOTTOM) {
//        sprite-> = y - sprite->height;
//    }
//
//    if (sprite->anchor & GFX_ANCHOR_LEFT) {
//        sprite->dst.x = x;
//    } else if(sprite->anchor & GFX_ANCHOR_RIGHT) {
//        sprite->dst.x = x - sprite->dst.w;
//    }
//}

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

    static SDL_Rect dst;
    for (unsigned int l = 0; l < GB_GFX_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < gbGfxSpriteCursors[l]; s++) {
            if (gbGfxSprites[l][s]->dispose) {
//                if (gbGfxSprites[l][s]->texture >= GFX_TEXTURE_DYNAMIC_FIRST) {
//                    gb_gfx_texture_dynamic_unload(gbGfxSprites[l][s]->texture);
//                }

                free(gbGfxSprites[l][s]);

                if (--gbGfxSpriteCursors[l] > 0) {
                    gbGfxSprites[l][s] = gbGfxSprites[l][gbGfxSpriteCursors[l]];
                }

                gbGfxSprites[l][gbGfxSpriteCursors[l]] = 0;

                if (gbGfxSprites[l][s] == 0) break;
            }

            dst.x = gbGfxSprites[l][s]->pos->x * gbScaleFactorX;
            dst.y = gbGfxSprites[l][s]->pos->y * gbScaleFactorY;
            dst.w = gbGfxSprites[l][s]->width * gbScaleFactorX;
            dst.h = gbGfxSprites[l][s]->height * gbScaleFactorY;

            if (!gbGfxSprites[l][s]->fixed) {
                dst.x -= gbGfxCameraOffsetX * gbScaleFactorX;
                dst.y -= gbGfxCameraOffsetY * gbScaleFactorY;
            }

            SDL_RenderCopyEx(
                gbMainRenderer,
                gbTextures[gbGfxSprites[l][s]->texture],
                &gbGfxSprites[l][s]->src,
                &dst,
                0,
                0,
                gbGfxSprites[l][s]->flip
            );
        }
    }

    SDL_RenderPresent(gbMainRenderer);
}

void gbGfxScreenToWorldCoords(int *x, int *y) {
    *x /= gbScaleFactorX;
    *y /= gbScaleFactorY;
}

void gbGfxScreenCoordsToGridSquare(int x, int y, int *gridX, int *gridY) {
    gbGfxScreenToWorldCoords(&x, &y);

    if (x < GB_GFX_GRID_OFFSET_X || x > GB_GFX_GRID_OFFSET_X + (GB_GFX_GRID_WIDTH * GB_GFX_GRID_SIZE) ||
        y < GB_GFX_GRID_OFFSET_Y || y > GB_GFX_GRID_OFFSET_Y + (GB_GFX_GRID_HEIGHT * GB_GFX_GRID_SIZE)) {
            *gridX = -1;
            *gridY = -1;
            return;
        }
    *gridX = x - GB_GFX_GRID_OFFSET_X;
    *gridY = y - GB_GFX_GRID_OFFSET_Y;

    *gridX /= GB_GFX_GRID_SIZE;
    *gridY /= GB_GFX_GRID_SIZE;

}

void gbGfxGridSquareToWorldCoords(int x, int y, int *worldX, int *worldY) {
    *worldX = GB_GFX_GRID_OFFSET_X + (x * GB_GFX_GRID_SIZE);
    *worldY = GB_GFX_GRID_OFFSET_Y + (y * GB_GFX_GRID_SIZE);
}

/* FONTS */
//
//void gb_gfx_font_unload(GB_GFX_FONT font) {
//    if (gb_gfx_fonts[font] != 0) {
//        TTF_CloseFont(gb_gfx_fonts[font]);
//        gb_gfx_fonts[font] = 0;
//    }
//}
//
//void gb_gfx_font_load(char *ttfFile, GB_GFX_FONT font, uint16_t pt) {
//    gb_gfx_font_unload(font);
//    //pt = (pt * ((float)LOGICAL_SCREEN_HEIGHT) / (float)gb_screen_height );
//
//    gb_gfx_fonts[font] = TTF_OpenFont(ttfFile, pt * gbScaleFactorY);
//}
//
//void gb_gfx_font_set(GB_GFX_FONT font) {
//    gb_gfx_font = font;
//}
//
//void gb_gfx_font_color_set(GB_GFX_COLOR color) {
//    gb_gfx_font_color = color;
//}
//
//void gb_gfx_font_layer_set(GB_GFX_LAYER layer) {
//    gb_gfx_font_layer = layer;
//}
//
///* TEXT */
//
//gbSprite *gb_gfx_new_text(char *text, uint32_t wrapW, double x, double y, uint8_t anchor, uint8_t fixed) {
//    GB_GFX_TEXTURE textureIndex = gb_gfx_texture_dynamic_get_free();
//    if (textureIndex > GFX_TEXTURE_DYNAMIC_LAST) {
//        return 0;
//    }
//
//    SDL_Surface *temp = TTF_RenderText_Blended_Wrapped(gb_gfx_fonts[gb_gfx_font], text, gb_gfx_colors[gb_gfx_font_color], wrapW * gbScaleFactorX);
//    gb_gfx_texture_unload(textureIndex);
//    gb_gfx_textures[textureIndex] = SDL_CreateTextureFromSurface(gb_main_renderer, temp);
//    SDL_FreeSurface(temp);
//
//    int w, h;
//    SDL_QueryTexture(gb_gfx_textures[textureIndex], 0, 0, &w, &h);
//
//    gbSprite *textSprite = gb_gfx_new_sprite(
//    gb_gfx_font_layer,
//    textureIndex,
//    0,
//    0,
//    w,
//    h,
//    x,
//    y,
//    anchor,
//    w / gbScaleFactorX,
//    h / gbScaleFactorY,
//    fixed);
//
//    return textSprite;
//}
//
//void gb_gfx_text_change(gbSprite *sprite, char *text, uint32_t wrapW) {
//    SDL_Surface *temp = TTF_RenderText_Blended_Wrapped(gb_gfx_fonts[gb_gfx_font], text, gb_gfx_colors[gb_gfx_font_color], wrapW);
//    gb_gfx_texture_unload(sprite->texture);
//    gb_gfx_textures[sprite->texture] = SDL_CreateTextureFromSurface(gb_main_renderer, temp);
//    SDL_FreeSurface(temp);
//
//    SDL_QueryTexture(gb_gfx_textures[sprite->texture], 0, 0, &sprite->src.w, &sprite->src.h);
//
//    sprite->dst.w = sprite->src.w;
//    sprite->dst.h = sprite->src.h;
//}

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
