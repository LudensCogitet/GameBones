#include <stdio.h>

#include "SDL2/SDL.h"
#include "../gbRenderer/gbRenderer_sys.h"

#include "./editor.h"
#include "../gbEntity/gbPosition_type.h"
#include "../gbInput/gbInput_sys.h"
#include "../gbGfx/gbGfx_sys.h"
#include "../gbCollision/gbCollision_sys.h"

typedef enum {
    PLACE_STATIC_GEOMETRY,
} MODE;

uint8_t editorEditModeOn = 0;

static MODE mode = PLACE_STATIC_GEOMETRY;

static int clickX = -1;
static int clickY = -1;

static int dragX = -1;
static int dragY = -1;

static int releaseX = -1;
static int releaseY = -1;

void editorInit() {

}

void editorTeardown() {
}

void editorUpdate() {
    if (mode == PLACE_STATIC_GEOMETRY) {
        // Clicked select
        if (gbInputCheckState(GB_INPUT_MOUSE_SELECT, GB_INPUT_JUST_PRESSED)) {
            SDL_GetMouseState(&clickX, &clickY);
            gbGfxScreenCoordsToGridSquare(clickX, clickY, &clickX, &clickY);
        }

        // Holding select
        if (gbInputCheckState(GB_INPUT_MOUSE_SELECT, GB_INPUT_PRESSED)) {
            SDL_GetMouseState(&dragX, &dragY);
            gbGfxScreenCoordsToGridSquare(dragX, dragY, &dragX, &dragY);
        }

        // Release select
        if(gbInputCheckState(GB_INPUT_MOUSE_SELECT, GB_INPUT_RELEASED)) {
            dragX = -1;
            dragY = -1;
            SDL_GetMouseState(&releaseX, &releaseY);
            gbGfxScreenCoordsToGridSquare(releaseX, releaseY, &releaseX, &releaseY);

            if (clickX != releaseX && clickY != releaseY) {
                int x1, y1, x2, y2;
                gbGfxGridSquareToWorldCoords(clickX, clickY, &x1, &y1);
                gbGfxGridSquareToWorldCoords(releaseX, releaseY, &x2, &y2);

                gbCollisionStaticColliderNew(x1 < x2 ? x1 : x2, y1 < y2 ? y1 : y2, x2 > x1 ? x2 : x1, y2 > y1 ? y2 : y1);
            }
        }

        // Release alt
        if(gbInputCheckState(GB_INPUT_MOUSE_ALT, GB_INPUT_RELEASED)) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            gbCollisionRemoveStaticCollider(gbCollisionDetectPointCollisionStatic(x, y));
        }
    }
}

void editorDebugRender() {
    switch(mode) {
        case PLACE_STATIC_GEOMETRY:
            if (clickX < 0 || clickY < 0 || dragX < 0 || dragY < 0) break;

            SDL_SetRenderDrawColor(gbMainRenderer, 0xFF, 0x00, 0x00, 0xFF);

            int x1, y1, x2, y2;

            gbGfxGridSquareToWorldCoords(clickX, clickY, &x1, &y1);
            gbGfxGridSquareToWorldCoords(dragX, dragY, &x2, &y2);

            SDL_Rect rect;
            rect.x = x1;
            rect.y = y1;
            rect.w = x2 - x1;
            rect.h = y2 - y1;

            SDL_RenderDrawRect(gbMainRenderer, &rect);
            gbRendererResetDrawColor();
    }
}
