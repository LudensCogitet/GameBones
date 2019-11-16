#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <SDL2/SDL.h>

#define LOGICAL_SCREEN_WIDTH 1777
#define LOGICAL_SCREEN_HEIGHT 1000

uint32_t gb_screen_width;
uint32_t gb_screen_height;
float gb_scale_factor_x;
float gb_scale_factor_y;

SDL_Window* gb_main_window;
SDL_Renderer* gb_main_renderer;

void gb_init_main_renderer(char * caption);
void gb_destroy_main_renderer();

#endif // RENDERER_H_INCLUDED
