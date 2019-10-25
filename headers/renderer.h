#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <SDL2/SDL.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

SDL_Window* gb_main_window;
SDL_Renderer* gb_main_renderer;

void gb_init_main_renderer(char * caption);
void gb_destroy_main_renderer();

#endif // RENDERER_H_INCLUDED
