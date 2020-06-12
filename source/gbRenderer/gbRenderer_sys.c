#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./gbRenderer_sys.h"

SDL_Window* gbMainWindow = 0;
SDL_Renderer* gbMainRenderer = 0;

uint32_t gbScreenWidth = 1280;
uint32_t gbScreenHeight = 720;

float gbScaleFactorX;
float gbScaleFactorY;

void gbRendererInit(char * caption, uint8_t filtered, uint8_t uncapFps) {
    gbScaleFactorX = (float)gbScreenWidth / (float)GB_LOGICAL_SCREEN_WIDTH;
    gbScaleFactorY = (float)gbScreenHeight / (float)GB_LOGICAL_SCREEN_HEIGHT;

    gbMainWindow = SDL_CreateWindow(
        caption,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        gbScreenWidth,
        gbScreenHeight,
        SDL_WINDOW_BORDERLESS
    );

    if (gbMainWindow == 0) {
        fprintf(stderr, "SDL_CreateWindow failed: %s", SDL_GetError());
        exit(1);
    }

    uint32_t renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;

    if (!uncapFps) {
        renderFlags |= SDL_RENDERER_PRESENTVSYNC;
    }

    gbMainRenderer = SDL_CreateRenderer(
        gbMainWindow,
        -1,
        renderFlags
    );

    if (gbMainRenderer == 0) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s", SDL_GetError());
        exit(1);
    }
    if (filtered) {
        SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1");
    }
    SDL_SetRenderDrawColor(gbMainRenderer, 0X3C, 0X56, 0X5C, 0x00);
    //SDL_RenderSetLogicalSize(gbMainRenderer, GB_LOGICAL_SCREEN_WIDTH, GB_LOGICAL_SCREEN_WIDTH);
}

void gbRendererResetDrawColor() {
    SDL_SetRenderDrawColor(gbMainRenderer, 0X3C, 0X56, 0X5C, 0x00);
}

void gbRendererTeardown() {
    SDL_DestroyRenderer(gbMainRenderer);
    SDL_DestroyWindow(gbMainWindow);
}
