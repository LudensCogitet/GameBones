#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <SDL2/SDL.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

SDL_Window* g_mainWindow;
SDL_Renderer* g_mainRenderer;

void initMainRenderer(char * caption);
void destroyMainRenderer();

#endif // RENDERER_H_INCLUDED
