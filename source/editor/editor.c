#include <stdio.h>

#include "SDL2/SDL.h"
#include "../gbRenderer/gbRenderer_sys.h"

#include "./editor.h"
#include "../gbEntity/gbPosition_type.h"
#include "../gbInput/gbInput_sys.h"
#include "../gbGfx/gbGfx_sys.h"
#include "../gbEntity/gbEntity_sys.h"
#include "../gbCollision/gbCollision_sys.h"
#include "../entities/Guy/entityGuy.h"

typedef enum {
    PLACE_STATIC_GEOMETRY,
    PLACE_PLAYER,
    NUM_MODES
} MODE;

uint8_t editorEditModeOn = 0;

static MODE mode = PLACE_STATIC_GEOMETRY;

static uint8_t clickInGrid = 0;

static int clickX = -1;
static int clickY = -1;

static int dragX = -1;
static int dragY = -1;

static int releaseX = -1;
static int releaseY = -1;

// **UI**
// Mode Button
static gbText *modeButtonText[NUM_MODES];
static unsigned int modeButtonHitboxId;
static SDL_Rect modeButtonRect;

// Input Field
static gbText *inputFieldText;
static unsigned int inputFieldHitBoxId;
static SDL_Rect textInputFieldRect;
static uint8_t textMode = 0;

// Forward Declarations
void handleTextInput();
void addRectAndHitboxToText(gbText *text, unsigned int padding, SDL_Rect *rect, unsigned int *colRect);

void editorInit() {
    modeButtonText[PLACE_STATIC_GEOMETRY] = gbGfxTextNew("Geometry", GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, GB_GFX_LAYER_FOREGROUND, 20, 20, 1, 1);
    modeButtonText[PLACE_PLAYER] = gbGfxTextNew("Player", GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, GB_GFX_LAYER_FOREGROUND, 20, 20, 0, 1);
    addRectAndHitboxToText(
                           modeButtonText[PLACE_STATIC_GEOMETRY],
                           15,
                           &modeButtonRect,
                           &modeButtonHitboxId);

    inputFieldText = gbGfxTextNew("Click to type", GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, GB_GFX_LAYER_FOREGROUND, 20, 70, 1, 1);
    addRectAndHitboxToText(
                           inputFieldText,
                           15,
                           &textInputFieldRect,
                           &inputFieldHitBoxId);
}

void editorTeardown() {
    for (unsigned int i = 0; i < NUM_MODES; i++)
        gbGfxTextDelete(modeButtonText[i]);
}

void editorUpdate() {
    if (textMode) handleTextInput();

    if (gbInputCheckState(GB_INPUT_MOUSE_SELECT, GB_INPUT_JUST_PRESSED)) {
        SDL_GetMouseState(&clickX, &clickY);
        clickInGrid = gbGfxScreenCoordsToGridSquare(clickX, clickY, &clickX, &clickY);
    }

    if (gbInputCheckState(GB_INPUT_MOUSE_SELECT, GB_INPUT_PRESSED)) {
        switch (mode) {
            case PLACE_STATIC_GEOMETRY:
                SDL_GetMouseState(&dragX, &dragY);
                gbGfxScreenCoordsToGridSquare(dragX, dragY, &dragX, &dragY);
        }
    }

    if(gbInputCheckState(GB_INPUT_MOUSE_SELECT, GB_INPUT_RELEASED)) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        gbGfxScreenToWorldCoords(&x, &y);
        unsigned int colliderId = gbCollisionDetectPointCollisionStatic(x, y);
        if (colliderId == modeButtonHitboxId) {
            modeButtonText[mode]->sprite->active = 0;
            if (++mode >= NUM_MODES) mode = 0;
            modeButtonText[mode]->sprite->active = 1;
            return;
        }

        if (colliderId == inputFieldHitBoxId) {
            textMode = 1;
            gbInputClearTextInput();
            gbInputStartTextInput(SDLK_RETURN);
            handleTextInput();
        }

        if (!clickInGrid) return;

        switch (mode) {
            case PLACE_STATIC_GEOMETRY:
                dragX = -1;
                dragY = -1;
                SDL_GetMouseState(&releaseX, &releaseY);
                gbGfxScreenCoordsToGridSquare(releaseX, releaseY, &releaseX, &releaseY);

                int x1 = clickX < releaseX ? clickX : releaseX;
                int y1 = clickY < releaseY ? clickY : releaseY;
                int x2 = releaseX > clickX ? releaseX : clickX;
                int y2 = releaseY > clickY ? releaseY : clickY;

                gbGfxGridSquareToWorldCoords(x1, y1, &x1, &y1, 0);
                gbGfxGridSquareToWorldCoords(x2, y2, &x2, &y2, 1);

                gbCollisionStaticColliderNew(x1 < x2 ? x1 : x2, y1 < y2 ? y1 : y2, x2 > x1 ? x2 : x1, y2 > y1 ? y2 : y1);
                break;
            case PLACE_PLAYER: ;
                gbEntity *player = gbEntityFindOfType(GB_ENTITY_TYPE_GUY);
                if (!player) break;

                int x, y;
                SDL_GetMouseState(&x, &y);
                gbGfxScreenCoordsToGridSquare(x, y, &x, &y);
                gbGfxGridSquareToWorldCoords(x, y, &x, &y, 0);

                Guy *guy = player->entity;
                guy->pos.x = x;
                guy->pos.y = y;
        }
    }

    if(gbInputCheckState(GB_INPUT_MOUSE_ALT, GB_INPUT_RELEASED)) {
        switch (mode) {
            int x, y;
            case PLACE_STATIC_GEOMETRY:
                SDL_GetMouseState(&x, &y);
                gbCollisionRemoveStaticCollider(gbCollisionDetectPointCollisionStatic(x, y));
        }
    }
}

void editorRender() {
    SDL_SetRenderDrawColor(gbMainRenderer, 0, 0, 0, 255);
    SDL_RenderFillRect(gbMainRenderer, &textInputFieldRect);
    SDL_RenderFillRect(gbMainRenderer, &modeButtonRect);
    gbRendererResetDrawColor();
    switch(mode) {
        case PLACE_STATIC_GEOMETRY:
            if (clickX < 0 || clickY < 0 || dragX < 0 || dragY < 0 || !clickInGrid) break;

            SDL_SetRenderDrawColor(gbMainRenderer, 0xFF, 0x00, 0x00, 0xFF);

            int x1 = clickX < dragX ? clickX : dragX;
            int y1 = clickY < dragY ? clickY : dragY;
            int x2 = dragX > clickX ? dragX : clickX;
            int y2 = dragY > clickY ? dragY : clickY;

            gbGfxGridSquareToWorldCoords(x1, y1, &x1, &y1, 0);
            gbGfxGridSquareToWorldCoords(x2, y2, &x2, &y2, 1);

            SDL_Rect rect;
            rect.x = x1;
            rect.y = y1;
            rect.w = x2 - x1;
            rect.h = y2 - y1;

            SDL_RenderDrawRect(gbMainRenderer, &rect);
            gbRendererResetDrawColor();
    }
}

void handleTextInput() {
    char buffer[GB_INPUT_BUFFER_MAX_LEN];
    uint8_t readStatus = gbInputReadTextInput(buffer);
    if (readStatus != GB_INPUT_BUFFER_READ) {
        gbGfxTextChange(inputFieldText, GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, buffer);
        if (readStatus == GB_INPUT_BUFFER_STOP) {
            textMode = 0;
            printf("%s", buffer);
        }
    }
}
void addRectAndHitboxToText(gbText *text, unsigned int padding, SDL_Rect *rect, unsigned int *colRect) {
    SDL_Rect newRect = (SDL_Rect){
        text->pos.x - padding,
        text->pos.y - padding,
        text->sprite->width + (padding * 2),
        text->sprite->height + (padding * 2)
        };

    *rect = newRect;
    *colRect = gbCollisionStaticColliderNew(
                                            newRect.x,
                                            newRect.y,
                                            newRect.x + newRect.w,
                                            newRect.y + newRect.h);
}
