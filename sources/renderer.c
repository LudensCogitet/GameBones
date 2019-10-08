#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "../headers/renderer.h"

SDL_Window* g_mainWindow = NULL;
SDL_Renderer* g_mainRenderer = NULL;

void initMainRenderer(char * caption) {
    g_mainWindow = SDL_CreateWindow(
        caption,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    if (g_mainWindow == NULL) {
        fprintf(stderr, "SDL_CreateWindow failed: %s", SDL_GetError());
        exit(1);
    }

    g_mainRenderer = SDL_CreateRenderer(
        g_mainWindow,
        -1,
        SDL_RENDERER_ACCELERATED    |
        SDL_RENDERER_TARGETTEXTURE  |
        SDL_RENDERER_PRESENTVSYNC
    );

    if (g_mainRenderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s", SDL_GetError());
        exit(1);
    }

    SDL_SetRenderDrawColor(g_mainRenderer, 0X3b, 0X3b, 0X3b, 0x00);
}

void destroyMainRenderer() {
    SDL_DestroyRenderer(g_mainRenderer);
    SDL_DestroyWindow(g_mainWindow);
}
