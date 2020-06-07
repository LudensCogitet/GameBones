#ifndef GB_RENDERER_SYSTEM_H_INCLUDED
#define GB_RENDERER_SYSTEM_H_INCLUDED

#include <SDL2/SDL.h>

#define GB_LOGICAL_SCREEN_WIDTH 1280
#define GB_LOGICAL_SCREEN_HEIGHT 720

uint32_t gbScreenWidth;
uint32_t gbScreenHeight;
float gbScaleFactorX;
float gbScaleFactorY;
float gbSizeScaleFactor;

SDL_Window* gbMainWindow;
SDL_Renderer* gbMainRenderer;

void gbRendererInit(char * caption, uint8_t filtered, uint8_t uncapFps);
void gbRendererResetDrawColor();
void gbRendererTeardown();

#endif // GB_RENDERER_SYSTEM_H_INCLUDED
