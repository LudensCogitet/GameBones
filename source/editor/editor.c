#include <stdlib.h>
#include <stdio.h>

#include "SDL2/SDL.h"
#include "../gbRenderer/gbRenderer_sys.h"

#include "./editor.h"
#include "../Position_type.h"
#include "../gbInput/gbInput_sys.h"
#include "../gbGfx/gbGfx_sys.h"
#include "../DynamicEntity/DynamicEntity_sys.h"
#include "../Collision/Collision_sys.h"
#include "../entities/Guy/entityGuy.h"

#include "../Collision/CollisionStaticRect_type.h"

#include "../Sprite/Sprite_type.h"
#include "../Sprite/Sprite_sys.h"
#include "../Sprite/SpriteLayer_enum.h"

#include "../Text/Text_type.h"
#include "../Text/Text_sys.h"

#include "../Room/Room_sys.h"

#define EDITOR_MAX_STATIC_COLLIDERS 100

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

static unsigned int staticColliderCursor;
static CollisionStaticRect *staticColliders[EDITOR_MAX_STATIC_COLLIDERS];

// **UI**
static int uiTexture;
static const unsigned int buttonSrcWidth = 176;
static const unsigned int buttonSrcHeight = 64;

// Mode Button
static Position modeButtonPosition = (Position) {20, 20};
static unsigned int buttonX[NUM_MODES] = { 0, buttonSrcWidth };
static Sprite *modeButtonSprite;
static CollisionStaticRect *modeButtonHitbox;

// Input Field
static const inputFieldSrcWidth = 640;
static const inputFieldSrcHeight = 64;
static Position inputFieldSrcPosition = (Position) {0, 128};
static Position inputFieldPosition = (Position) {GB_LOGICAL_SCREEN_WIDTH * 0.25, (GB_LOGICAL_SCREEN_HEIGHT * 0.5) - 32};
static Text *inputFieldText;
static CollisionStaticRect *inputFieldHitbox;
static Sprite *inputFieldBackground;
static uint8_t textMode = 0;

// Forward Declarations
void handleTextInput();
void addHitboxToButton(Position *pos, Sprite *sprite, CollisionStaticRect **colRect);

void editorInit() {
    staticColliderCursor = 0;
    for (unsigned int i = 0; i < EDITOR_MAX_STATIC_COLLIDERS; i++) {
        staticColliders[i] = 0;
    }

    uiTexture = gbTextureLoadFromFile("./assets/UIButtons-sheet.png");

    modeButtonSprite = (Sprite *)malloc(sizeof(Sprite));
    spriteSet(
              modeButtonSprite,
              uiTexture,
              buttonX[mode], 0,
              buttonSrcWidth, buttonSrcHeight,
              buttonSrcWidth, buttonSrcHeight,
              1, 1, SDL_FLIP_NONE);
    spriteRegister(modeButtonSprite, &modeButtonPosition, SPRITE_LAYER_FOREGROUND);

    addHitboxToButton(
                      &modeButtonPosition,
                      modeButtonSprite,
                      &modeButtonHitbox);

    inputFieldText = (Text*)malloc(sizeof(Text));
    textNew(inputFieldText, "Click to type", GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, GB_SPRITE_LAYER_FOREGROUND, 40, 110, 1, 1);

    inputFieldBackground = (Sprite *)malloc(sizeof(Sprite));
    spriteSet(
              inputFieldBackground,
              uiTexture,
              inputFieldSrcPosition.x, inputFieldSrcPosition.y,
              inputFieldSrcWidth, inputFieldSrcHeight,
              inputFieldSrcWidth, inputFieldSrcHeight,
              1, 1, SDL_FLIP_NONE);
    spriteRegister(inputFieldBackground, &inputFieldPosition, SPRITE_LAYER_MIDGROUND);
    addHitboxToButton(
                      &inputFieldPosition,
                      inputFieldBackground,
                      &inputFieldHitbox);
}

void editorTeardown() {
    spriteUnregister(modeButtonSprite);
    spriteUnregister(inputFieldBackground);

    free(modeButtonSprite);
    free(inputFieldBackground);

    textDelete(inputFieldText);
    free(inputFieldText);
    inputFieldText = 0;

    for (unsigned int i = 0; i < staticColliderCursor; i++) {
        free(staticColliders[i]);
        staticColliders[i] = 0;
    }

    modeButtonHitbox = 0;
    inputFieldHitbox = 0;
}

void editorUpdate() {
    if (textMode) {
        handleTextInput();
        return;
    }

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
        CollisionStaticRect *collider = collisionDetectPointCollisionStatic(x, y);
        if (collider == modeButtonHitbox) {
            if (++mode >= NUM_MODES) mode = 0;
            modeButtonSprite->src.x = buttonX[mode];
            return;
        }

        if (collider == inputFieldHitbox) {
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

                CollisionStaticRect *rect = (CollisionStaticRect *)malloc(sizeof(CollisionStaticRect));
                staticColliders[staticColliderCursor++] = rect;

                collisionStaticRectSet(rect, x1 < x2 ? x1 : x2, y1 < y2 ? y1 : y2, x2 > x1 ? x2 : x1, y2 > y1 ? y2 : y1);
                collisionStaticRectRegister(rect);

                break;
            case PLACE_PLAYER: ;
                DynamicEntity *player = dynamicEntityFindOfType(DYNAMIC_ENTITY_TYPE_GUY);
                if (!player) break;

                int x, y;
                SDL_GetMouseState(&x, &y);
                gbGfxScreenCoordsToGridSquare(x, y, &x, &y);
                gbGfxGridSquareToWorldCoords(x, y, &x, &y, 0);

                player->pos.x = x;
                player->pos.y = y;
        }
    }

    if(gbInputCheckState(GB_INPUT_MOUSE_ALT, GB_INPUT_RELEASED)) {
        switch (mode) {
            int x, y;
            case PLACE_STATIC_GEOMETRY:
                SDL_GetMouseState(&x, &y);
                unsigned int collider = collisionDetectPointCollisionStatic(x, y);
                if (!collider) break;

                for (unsigned int i = 0; i < staticColliderCursor; i++) {
                    if (staticColliders[i] != collider) continue;

                    collisionStaticRectUnregister(staticColliders[i]);
                    free(staticColliders[i]);

                    if (--staticColliderCursor > 0)
                        staticColliders[i] = staticColliders[staticColliderCursor];

                    staticColliders[staticColliderCursor] = 0;
                    break;
                }
        }
    }
}

void editorRender() {
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
        textChange(inputFieldText, GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, buffer);
        if (readStatus == GB_INPUT_BUFFER_STOP) {
            textMode = 0;
            if (!loadBackground(buffer)) {
                sprintf(buffer, "Failed to load background image.");
                textChange(inputFieldText, GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, buffer);
            }
        }
    }
}

void addHitboxToButton(Position *pos, Sprite *button, CollisionStaticRect **colRect) {
    *colRect = (CollisionStaticRect *)malloc(sizeof(CollisionStaticRect));

    collisionStaticRectSet(*colRect, pos->x,
                                     pos->y,
                                     pos->x + button->width,
                                     pos->y + button->height);
    collisionStaticRectRegister(*colRect);

    staticColliders[staticColliderCursor++] = *colRect;
}
