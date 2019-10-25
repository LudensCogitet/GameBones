#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "../headers/renderer.h"

SDL_Window* gb_main_window = NULL;
SDL_Renderer* gb_main_renderer = NULL;

void gb_init_main_renderer(char * caption) {
    gb_main_window = SDL_CreateWindow(
        caption,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    if (gb_main_window == NULL) {
        fprintf(stderr, "SDL_CreateWindow failed: %s", SDL_GetError());
        exit(1);
    }

    gb_main_renderer = SDL_CreateRenderer(
        gb_main_window,
        -1,
        SDL_RENDERER_ACCELERATED    |
        SDL_RENDERER_TARGETTEXTURE  |
        SDL_RENDERER_PRESENTVSYNC
    );

    if (gb_main_renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s", SDL_GetError());
        exit(1);
    }

    SDL_SetRenderDrawColor(gb_main_renderer, 0X3b, 0X3b, 0X3b, 0x00);
}

void gb_destroy_main_renderer() {
    SDL_DestroyRenderer(gb_main_renderer);
    SDL_DestroyWindow(gb_main_window);
}
