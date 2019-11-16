#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL_ttf.h>

#include "../headers/renderer.h"
#include "../headers/gfx.h"
#include "../headers/textures.h"

#define GB_GFX_MAX_SPRITES_PER_LAYER 100

static SDL_Texture *gb_gfx_textures[GFX_TEXTURE_NUM_TEXTURES];
static unsigned int gb_gfx_dynamic_texture_cursor = GFX_TEXTURE_DYNAMIC_FIRST;

static unsigned int gb_gfx_sprite_cursors[GFX_LAYER_NUM_LAYERS];

static GbSprite *gb_gfx_sprites[GFX_LAYER_NUM_LAYERS][GB_GFX_MAX_SPRITES_PER_LAYER];

static TTF_Font *gb_gfx_fonts[GFX_FONT_NUM_FONTS];

static SDL_Color gb_gfx_colors[GFX_COLOR_NUM_COLORS];

static GB_GFX_FONT gb_gfx_font;
static GB_GFX_COLOR gb_gfx_font_color;
static GB_GFX_LAYER gb_gfx_font_layer;

static int32_t gb_gfx_camera_offset_x = 0;
static int32_t gb_gfx_camera_offset_y = 0;

void gb_gfx_init() {
    for (unsigned int l = 0; l < GFX_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < GB_GFX_MAX_SPRITES_PER_LAYER; s++) {
            gb_gfx_sprites[l][s] = NULL;
        }
    }

    for (unsigned int i = 0; i < GFX_TEXTURE_NUM_TEXTURES; i++) {
        gb_gfx_textures[i] = NULL;
    }

    for (unsigned int i = 0; i < GFX_LAYER_NUM_LAYERS; i++) {
        gb_gfx_sprite_cursors[i] = 0;
    }

    gb_gfx_colors[GFX_COLOR_WHITE].r = 255;
    gb_gfx_colors[GFX_COLOR_WHITE].g = 255;
    gb_gfx_colors[GFX_COLOR_WHITE].b = 255;
    gb_gfx_colors[GFX_COLOR_WHITE].a = 255;

    gb_gfx_colors[GFX_COLOR_BLACK].r = 0;
    gb_gfx_colors[GFX_COLOR_BLACK].g = 0;
    gb_gfx_colors[GFX_COLOR_BLACK].b = 0;
    gb_gfx_colors[GFX_COLOR_BLACK].a = 255;

    TTF_Init();
}

void gb_gfx_texture_unload(GB_GFX_TEXTURE texture);
void gb_gfx_teardown() {
    for (unsigned int i = 0; i < GFX_TEXTURE_NUM_TEXTURES; i++) {
        gb_gfx_texture_unload(i);
    }

    for (unsigned int l = 0; l < GFX_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < gb_gfx_sprite_cursors[l]; s++) {
            free(gb_gfx_sprites[l][s]);
        }
        gb_gfx_sprite_cursors[l] = 0;
    }

    for (unsigned int i = 0; i < GFX_FONT_NUM_FONTS; i++) {
        gb_gfx_font_unload(i);
    }

    TTF_Quit();
}

/* TEXTURES */

void gb_gfx_texture_unload(GB_GFX_TEXTURE texture) {
    if (gb_gfx_textures[texture] != NULL) {
        SDL_DestroyTexture(gb_gfx_textures[texture]);
        gb_gfx_textures[texture] = NULL;
    }
}

int gb_gfx_texture_load(char* filename, GB_GFX_TEXTURE texture) {
    SDL_Surface* tempSurface = IMG_Load(filename);

    if (tempSurface == NULL) {
        fprintf(stderr, "Failed to load image from file: %s", IMG_GetError());
        return 0;
    }

    gb_gfx_texture_unload(texture);
    gb_gfx_textures[texture] = SDL_CreateTextureFromSurface(gb_main_renderer, tempSurface);

    if (gb_gfx_textures[texture] == NULL) {
        fprintf(stderr, "Failed to generate texture from surface: %s", IMG_GetError());
        SDL_FreeSurface(tempSurface);
        return 0;
    }

    SDL_FreeSurface(tempSurface);

    return 1;
}

GB_GFX_TEXTURE gb_gfx_texture_dynamic_get_free() {
    GB_GFX_TEXTURE current = gb_gfx_dynamic_texture_cursor;

    while (++gb_gfx_dynamic_texture_cursor != current) {
        if (gb_gfx_dynamic_texture_cursor > GFX_TEXTURE_DYNAMIC_LAST)
            gb_gfx_dynamic_texture_cursor = GFX_TEXTURE_DYNAMIC_FIRST;

        if (gb_gfx_textures[gb_gfx_dynamic_texture_cursor] == NULL) {
            return current;
        }
    }

    gb_gfx_dynamic_texture_cursor = GFX_TEXTURE_DYNAMIC_LAST + 1;

    return current;
}

void gb_gfx_texture_dynamic_unload(GB_GFX_TEXTURE texture) {
    gb_gfx_texture_unload(texture);
    gb_gfx_dynamic_texture_cursor = texture;
}

/* SPRITES */

void gb_gfx_draw() {
    SDL_RenderClear(gb_main_renderer);

    for (unsigned int l = 0; l < GFX_LAYER_NUM_LAYERS; l++) {
        for (unsigned int s = 0; s < gb_gfx_sprite_cursors[l]; s++) {
            if (gb_gfx_sprites[l][s]->dispose) {
                if (gb_gfx_sprites[l][s]->texture >= GFX_TEXTURE_DYNAMIC_FIRST) {
                    gb_gfx_texture_dynamic_unload(gb_gfx_sprites[l][s]->texture);
                }

                free(gb_gfx_sprites[l][s]);

                if (--gb_gfx_sprite_cursors[l] > 0) {
                    gb_gfx_sprites[l][s] = gb_gfx_sprites[l][gb_gfx_sprite_cursors[l]];
                }

                gb_gfx_sprites[l][gb_gfx_sprite_cursors[l]] = NULL;

                if (gb_gfx_sprites[l][s] == NULL) break;
            }

            if (!gb_gfx_sprites[l][s]->fixed) {
                SDL_Rect dst = gb_gfx_sprites[l][s]->dst;

                dst.x -= gb_gfx_camera_offset_x;
                dst.y -= gb_gfx_camera_offset_y;
                SDL_RenderCopyEx(
                    gb_main_renderer,
                    gb_gfx_textures[gb_gfx_sprites[l][s]->texture],
                    &gb_gfx_sprites[l][s]->src,
                    &dst,
                    0,
                    NULL,
                    SDL_FLIP_NONE
                );
            } else {
                SDL_RenderCopyEx(
                    gb_main_renderer,
                    gb_gfx_textures[gb_gfx_sprites[l][s]->texture],
                    &gb_gfx_sprites[l][s]->src,
                    &gb_gfx_sprites[l][s]->dst,
                    0,
                    NULL,
                    SDL_FLIP_NONE
                );
            }
        }
    }

    SDL_RenderPresent(gb_main_renderer);
}

GbSprite *gb_gfx_new_sprite(GB_GFX_LAYER layer, GB_GFX_TEXTURE texture, uint8_t fixed) {
    if (++gb_gfx_sprite_cursors[layer] >= GB_GFX_MAX_SPRITES_PER_LAYER) {
        --gb_gfx_sprite_cursors[layer];
        return NULL;
    }

    GbSprite *newSprite = (GbSprite *)malloc(sizeof(GbSprite));

    newSprite->dispose = 0;
    newSprite->fixed = fixed;

    newSprite->dst.x = 0;
    newSprite->dst.y = 0;
    newSprite->dst.w = 0;
    newSprite->dst.h = 0;

    newSprite->src.x = 0;
    newSprite->src.y = 0;
    newSprite->src.w = 0;
    newSprite->src.h = 0;

    newSprite->texture = texture;

    gb_gfx_sprites[layer][gb_gfx_sprite_cursors[layer] - 1] = newSprite;

    return newSprite;
}

/* FONTS */

void gb_gfx_font_unload(GB_GFX_FONT font) {
    if (gb_gfx_fonts[font] != NULL) {
        TTF_CloseFont(gb_gfx_fonts[font]);
        gb_gfx_fonts[font] = NULL;
    }
}

void gb_gfx_font_load(char *ttfFile, GB_GFX_FONT font, uint16_t pt) {
    gb_gfx_font_unload(font);
    pt = (pt * ((float)gb_screen_height / (float)LOGICAL_SCREEN_HEIGHT));

    gb_gfx_fonts[font] = TTF_OpenFont(ttfFile, pt);
}

void gb_gfx_font_set(GB_GFX_FONT font) {
    gb_gfx_font = font;
}

void gb_gfx_font_color_set(GB_GFX_COLOR color) {
    gb_gfx_font_color = color;
}

void gb_gfx_font_layer_set(GB_GFX_LAYER layer) {
    gb_gfx_font_layer = layer;
}

/* TEXT */

GbSprite *gb_gfx_new_text(char *text, uint32_t wrapW, uint8_t fixed) {
    GB_GFX_TEXTURE textureIndex = gb_gfx_texture_dynamic_get_free();
    if (textureIndex > GFX_TEXTURE_DYNAMIC_LAST) {
        return NULL;
    }

    SDL_Surface *temp = TTF_RenderText_Blended_Wrapped(gb_gfx_fonts[gb_gfx_font], text, gb_gfx_colors[gb_gfx_font_color], wrapW);
    gb_gfx_texture_unload(textureIndex);
    gb_gfx_textures[textureIndex] = SDL_CreateTextureFromSurface(gb_main_renderer, temp);
    SDL_FreeSurface(temp);

    GbSprite *textSprite = gb_gfx_new_sprite(gb_gfx_font_layer, textureIndex, fixed);
    SDL_QueryTexture(gb_gfx_textures[textureIndex], NULL, NULL, &textSprite->src.w, &textSprite->src.h);

    textSprite->dst.w = textSprite->src.w;
    textSprite->dst.h = textSprite->src.h;

    return textSprite;
}

void gb_gfx_text_change(GbSprite *sprite, char *text, uint32_t wrapW) {
    SDL_Surface *temp = TTF_RenderText_Blended_Wrapped(gb_gfx_fonts[gb_gfx_font], text, gb_gfx_colors[gb_gfx_font_color], wrapW);
    gb_gfx_texture_unload(sprite->texture);
    gb_gfx_textures[sprite->texture] = SDL_CreateTextureFromSurface(gb_main_renderer, temp);
    SDL_FreeSurface(temp);

    SDL_QueryTexture(gb_gfx_textures[sprite->texture], NULL, NULL, &sprite->src.w, &sprite->src.h);

    sprite->dst.w = sprite->src.w;
    sprite->dst.h = sprite->src.h;
}

/* CAMERA */

void gb_gfx_camera_set(int32_t x, int32_t y) {
    gb_gfx_camera_offset_x = x;
    gb_gfx_camera_offset_y = y;
}

void gb_gfx_camera_move(int32_t dx, int32_t dy) {
    gb_gfx_camera_offset_x += dx;
    gb_gfx_camera_offset_y += dy;
}

void gb_gfx_camera_get_pos(int32_t *x, int32_t *y) {
    *x = gb_gfx_camera_offset_x;
    *y = gb_gfx_camera_offset_y;
}
