#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "../headers/renderer.h"

SDL_Window* gb_main_window = NULL;
SDL_Renderer* gb_main_renderer = NULL;

uint32_t gb_screen_width = 1920;
uint32_t gb_screen_height = 1080;

float gb_scale_factor_x;
float gb_scale_factor_y;

void gb_init_main_renderer(char * caption) {
    gb_scale_factor_x = (float)gb_screen_width / (float)LOGICAL_SCREEN_WIDTH;
    gb_scale_factor_y = (float)gb_screen_height / (float)LOGICAL_SCREEN_HEIGHT;

    gb_main_window = SDL_CreateWindow(
        caption,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        gb_screen_width,
        gb_screen_height,
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
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    SDL_SetRenderDrawColor(gb_main_renderer, 0X0, 0X0, 0X0, 0x00);
    //SDL_RenderSetLogicalSize(gb_main_renderer, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT);
}

void gb_destroy_main_renderer() {
    SDL_DestroyRenderer(gb_main_renderer);
    SDL_DestroyWindow(gb_main_window);
}
