#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <SDL2/SDL.h>

#define LOGICAL_SCREEN_WIDTH 960
#define LOGICAL_SCREEN_HEIGHT 540

uint32_t gb_screen_width;
uint32_t gb_screen_height;

SDL_Window* gb_main_window;
SDL_Renderer* gb_main_renderer;

void gb_init_main_renderer(char * caption);
void gb_destroy_main_renderer();

#endif // RENDERER_H_INCLUDED
