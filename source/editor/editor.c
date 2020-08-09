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

#include "../Room/Room_type.h"
#include "../Room/Room_sys.h"

#include "../entities/Guy/entityGuy.h"
#include "../entities/MoveRoomPanel/entityMoveRoomPanel.h"

#define EDITOR_MAX_STATIC_COLLIDERS 50

typedef enum {
    NEW_ROOM,
    LOAD_ROOM,
    SAVE_ROOM,
    LOAD_BG,
    NUM_MENU_ITEMS
} MENU_ITEM;

typedef enum {
    PLACE_STATIC_GEOMETRY,
    PLACE_DYNAMIC,
    PLACE_POWER,
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

Room *currentEditorRoom;

static unsigned int uiColliderCursor = 0;
static CollisionStaticRect *uiColliders[EDITOR_MAX_STATIC_COLLIDERS];

// **UI**
static int uiTexture;
static const unsigned int buttonSrcWidth = 176;
static const unsigned int buttonSrcHeight = 64;

// Menu
static Sprite *menuSprites[NUM_MENU_ITEMS];
static CollisionStaticRect *menuHitboxes[NUM_MENU_ITEMS];
static Position *menuPositions[NUM_MENU_ITEMS];
static MENU_ITEM selectedMenuItem = NEW_ROOM;

// Mode Button
static Position modeButtonPosition;
static unsigned int buttonX[NUM_MODES];
static Sprite *modeButtonSprite;
static CollisionStaticRect *modeButtonHitbox;

// Input Field
static const int inputFieldSrcWidth = 640;
static const int inputFieldSrcHeight = 64;
static Position inputFieldSrcPosition = (Position) {0, 128};
static Position inputFieldPosition = (Position) {GB_LOGICAL_SCREEN_WIDTH * 0.25, (GB_LOGICAL_SCREEN_HEIGHT * 0.5) - 32};
static Text *inputFieldText;
static CollisionStaticRect *inputFieldHitbox;
static Sprite *inputFieldBackground;
static uint8_t textMode = 0;

// Entity Palette
static Position entityPalettePos;
static Sprite entityPalette;
static CollisionStaticRect entityPaletteRects[DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES];
static DYNAMIC_ENTITY_TYPE selectedType = DYNAMIC_ENTITY_TYPE_GUY;

// Power Grid Palette
static Position powerGridPalettePos;
static Sprite powerGridPalette;
static CollisionStaticRect powerGridRects[POWER_GRID_WIRING_NUM];
static POWER_GRID_WIRING selectedWiring;

static char roomFilepath[50] = {'\0'};

extern void setPlayerPosition(double x, double y);

// Forward Declarations
void handleTextInput();
void addHitboxToButton(Position *pos, Sprite *sprite, CollisionStaticRect **colRect, uint8_t active);

void editorInit() {
    modeButtonPosition = (Position) {GB_LOGICAL_SCREEN_WIDTH - (GB_LOGICAL_SCREEN_WIDTH / 7.27), 0};
    buttonX[PLACE_STATIC_GEOMETRY] = 0;
    buttonX[PLACE_DYNAMIC] = buttonSrcWidth;
    buttonX[PLACE_POWER] = buttonSrcWidth * 2;

    currentEditorRoom = roomNew();
    spriteRegister(&currentEditorRoom->backgroundSprite, &currentEditorRoom->backgroundPos);

    uiColliderCursor = 0;
    for (int i = 0; i < EDITOR_MAX_STATIC_COLLIDERS; i++) {
        uiColliders[i] = 0;
    }

    uiTexture = gbTextureLoadFromFile("./assets/UIButtons-sheet.png");

    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
        menuSprites[i] = (Sprite *)malloc(sizeof(Sprite));
        spriteSet(menuSprites[i],
                  uiTexture,
                  i * buttonSrcWidth, 0,
                  buttonSrcWidth, buttonSrcHeight,
                  (GB_LOGICAL_SCREEN_WIDTH / 7.27), (GB_LOGICAL_SCREEN_HEIGHT / 11.25),
                  SPRITE_LAYER_FOREGROUND, 1, 1, SDL_FLIP_NONE);
        menuPositions[i] = (Position *)malloc(sizeof(Position));
        menuPositions[i]->x = 0;
        menuPositions[i]->y = i * GB_LOGICAL_SCREEN_HEIGHT / 11.25;
        spriteRegister(menuSprites[i], menuPositions[i]);
        addHitboxToButton(menuPositions[i], menuSprites[i], menuHitboxes + i, 1);
    }

    modeButtonSprite = (Sprite *)malloc(sizeof(Sprite));
    spriteSet(
              modeButtonSprite,
              uiTexture,
              buttonX[mode], buttonSrcHeight,
              buttonSrcWidth, buttonSrcHeight,
              buttonSrcWidth, buttonSrcHeight,
              SPRITE_LAYER_FOREGROUND, 1, 1, SDL_FLIP_NONE);
    spriteRegister(modeButtonSprite, &modeButtonPosition);

    addHitboxToButton(
                      &modeButtonPosition,
                      modeButtonSprite,
                      &modeButtonHitbox,
                      1);

    inputFieldBackground = (Sprite *)malloc(sizeof(Sprite));
    spriteSet(
              inputFieldBackground,
              uiTexture,
              inputFieldSrcPosition.x, inputFieldSrcPosition.y,
              inputFieldSrcWidth, inputFieldSrcHeight,
              GB_LOGICAL_SCREEN_WIDTH / 2 , GB_LOGICAL_SCREEN_HEIGHT / 11.25,
              SPRITE_LAYER_MIDGROUND, 0, 1, SDL_FLIP_NONE);
    spriteRegister(inputFieldBackground, &inputFieldPosition);

    inputFieldText = (Text*)malloc(sizeof(Text));
    textNew(inputFieldText, " ", GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, GB_SPRITE_LAYER_FOREGROUND, inputFieldPosition.x + 10, inputFieldPosition.y + 15, 1, 1);

    addHitboxToButton(
                      &inputFieldPosition,
                      inputFieldBackground,
                      &inputFieldHitbox,
                      0);

    // START entity palette init
    entityPalettePos = (Position) {GB_LOGICAL_SCREEN_WIDTH - 160, 200};
    spriteSet(&entityPalette,
              gbTextureLoadNamed(GB_TEXTURE_NAME_ENTITY_PALETTE),
              0, 0, 160, 160,
              160, 160, SPRITE_LAYER_MIDGROUND,
              0, 1, SDL_FLIP_NONE);
    spriteRegister(&entityPalette, &entityPalettePos);

    int x, y;
    x = entityPalettePos.x;
    y = entityPalettePos.y;

    collisionStaticRectSet(&entityPaletteRects[DYNAMIC_ENTITY_TYPE_GUY],
                           x, y,
                           x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&entityPaletteRects[DYNAMIC_ENTITY_TYPE_GUY]);

    x += 32;

    collisionStaticRectSet(&entityPaletteRects[DYNAMIC_ENTITY_TYPE_MOVE_ROOM_PANEL],
                           x, y,
                           x + 32, y + 64,
                           1);
    collisionStaticRectPassiveRegister(&entityPaletteRects[DYNAMIC_ENTITY_TYPE_MOVE_ROOM_PANEL]);
    // END entity palette init

    // START power grid palette init
    powerGridPalettePos = (Position) {GB_LOGICAL_SCREEN_WIDTH - 128, 200};
    spriteSet(&powerGridPalette,
              gbTextureLoadNamed(GB_TEXTURE_NAME_POWER_GRID_PALETTE),
              0, 0, 128, 128,
              128, 128, SPRITE_LAYER_MIDGROUND,
              0, 1, SDL_FLIP_NONE);
    spriteRegister(&powerGridPalette, &powerGridPalettePos);

    x = powerGridPalettePos.x;
    y = powerGridPalettePos.y;

    collisionStaticRectSet(&powerGridRects[POWER_GRID_EMPTY],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_EMPTY]);

    x += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_VERTICAL],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_VERTICAL]);

    x += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_HORIZONTAL],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_HORIZONTAL]);

    x += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_GENERATOR],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_GENERATOR]);

    x = powerGridPalettePos.x;
    y += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_CORNER_TOP_LEFT],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_CORNER_TOP_LEFT]);

    x += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_CORNER_TOP_RIGHT],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_CORNER_TOP_RIGHT]);

    x += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_CORNER_BOTTOM_RIGHT],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_CORNER_BOTTOM_RIGHT]);

    x = powerGridPalettePos.x;
    y += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_CORNER_BOTTOM_LEFT],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_CORNER_BOTTOM_LEFT]);

    x += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_T_NO_BOTTOM],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_T_NO_BOTTOM]);

    x += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_T_NO_LEFT],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_T_NO_LEFT]);

    x = powerGridPalettePos.x;
    y += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_T_NO_TOP],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_T_NO_TOP]);

    x += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_T_NO_RIGHT],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_T_NO_RIGHT]);

    x += 32;
    collisionStaticRectSet(&powerGridRects[POWER_GRID_CROSS],
                           x, y, x + 32, y + 32,
                           1);
    collisionStaticRectPassiveRegister(&powerGridRects[POWER_GRID_CROSS]);
    // END power grid palette init
}

void editorTeardown() {
    spriteUnregister(modeButtonSprite);
    spriteUnregister(inputFieldBackground);

    free(modeButtonSprite);
    free(inputFieldBackground);

    textDelete(inputFieldText);
    free(inputFieldText);
    inputFieldText = 0;

    for (unsigned int i = 0; i < uiColliderCursor; i++) {
        if (uiColliders[i]) {
            collisionStaticRectPassiveUnregister(uiColliders[i]);
            free(uiColliders[i]);
            uiColliders[i] = 0;
        }
    }

    uiColliderCursor = 0;

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
                break;
            case PLACE_POWER: ;
                {
                    int x = 0;
                    int y = 0;
                    SDL_GetMouseState(&x, &y);
                    if (gbGfxScreenCoordsToGridSquare(x, y, &x, &y)) {
                        if (selectedWiring)
                            currentEditorRoom->powerGrid[x][y] |= selectedWiring;
                        else
                            POWER_GRID_CLEAR_WIRING(currentEditorRoom->powerGrid[x][y]);
                    }
                }
                break;
        }
    }

    if(gbInputCheckState(GB_INPUT_MOUSE_SELECT, GB_INPUT_RELEASED)) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        gbGfxScreenToWorldCoords(&x, &y);
        CollisionStaticRect *collider = collisionDetectPointCollisionPassive(x, y);
        if (collider == modeButtonHitbox) {
            if (++mode >= NUM_MODES) mode = 0;
            modeButtonSprite->src.x = buttonX[mode];
            if (entityPalette.active = mode == PLACE_DYNAMIC) {
                for (int i = 0; i < POWER_GRID_WIRING_NUM; i++)
                    powerGridRects[i].active = 0;
                for (int i = 0; i < DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES; i++)
                    entityPaletteRects[i].active = 1;
            }

            if (powerGridPalette.active = mode == PLACE_POWER) {
                for (int i = 0; i < POWER_GRID_WIRING_NUM; i++)
                    powerGridRects[i].active = 1;
                for (int i = 0; i < DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES; i++)
                    entityPaletteRects[i].active = 0;
            }
            return;
        }

        for (int i = 0; i < NUM_MENU_ITEMS; i++) {
            if (menuHitboxes[i] == collider) {
                int clearTextBuffer = selectedMenuItem != i;
                switch (i) {
                    case NEW_ROOM:
                        roomStartDeactivation(currentEditorRoom);
                        roomFinishDeactivation(currentEditorRoom);
                        roomDestroy(currentEditorRoom);
                        currentEditorRoom = roomNew();
                    break;
                    case SAVE_ROOM:
                        if (selectedMenuItem == LOAD_ROOM)
                            clearTextBuffer = 0;
                    case LOAD_ROOM:
                        if (selectedMenuItem == SAVE_ROOM)
                            clearTextBuffer = 0;
                    case LOAD_BG:
                        if (clearTextBuffer) {
                            gbInputClearTextInput();
                            textChange(inputFieldText, GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, "");
                        }
                        selectedMenuItem = i;
                        inputFieldHitbox->active = 1;
                        inputFieldBackground->active = 1;
                        inputFieldText->sprite.active = 1;
                        textMode = 1;
                        gbInputStartTextInput(SDLK_RETURN);
                        handleTextInput();
                    break;
                }
            }
        }

        if (mode == PLACE_DYNAMIC) {
            for (int i = 0; i < DYNAMIC_ENTITY_TYPE_NUM_ENTITY_TYPES; i++) {
                if (collider == entityPaletteRects + i) {
                    selectedType = i;
                }
            }
        } else if (mode == PLACE_POWER) {
            for (int i = 0; i < POWER_GRID_WIRING_NUM; i++) {
                if (collider == powerGridRects + i) {
                    selectedWiring = i;
                }
            }
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
                roomAddStaticCollider(currentEditorRoom, rect);

                collisionStaticRectSet(rect, x1 < x2 ? x1 : x2, y1 < y2 ? y1 : y2, x2 > x1 ? x2 : x1, y2 > y1 ? y2 : y1, 1);
                collisionStaticRectRegister(rect);
                clickX = clickY = dragX = dragY = -1;
                break;
            case PLACE_DYNAMIC: ;
                {
                    SDL_GetMouseState(&x, &y);
                    gbGfxScreenCoordsToGridSquare(x, y, &x, &y);
                    gbGfxGridSquareToWorldCoords(x, y, &x, &y, 0);

                    switch (selectedType) {
                        case DYNAMIC_ENTITY_TYPE_GUY:
                            setPlayerPosition(x, y);

                            if (!currentEditorRoom->playerStart) {
                                currentEditorRoom->playerStart = (Position *)malloc(sizeof(Position));
                            }

                            currentEditorRoom->playerStart->x = x;
                            currentEditorRoom->playerStart->y = y;
                            break;
                        case DYNAMIC_ENTITY_TYPE_MOVE_ROOM_PANEL:
                            moveRoomPanelInit();
                            DynamicEntity * panel = moveRoomPanelNew(x, y);
                            roomAddDynamicEntity(currentEditorRoom, panel);
                            dynamicEntityRegister(panel);
                            break;
                    }
                }
                break;
        }
    }

    if(gbInputCheckState(GB_INPUT_MOUSE_ALT, GB_INPUT_RELEASED)) {
        switch (mode) {
            int x, y;
            case PLACE_STATIC_GEOMETRY:
                SDL_GetMouseState(&x, &y);
                CollisionStaticRect *collider = collisionDetectPointCollisionStatic(x, y);
                if (!collider) break;

                for (unsigned int i = 0; i < currentEditorRoom->numColliders; i++) {
                    if (currentEditorRoom->staticColliders[i] != collider) continue;

                    collisionStaticRectUnregister(currentEditorRoom->staticColliders[i]);
                    free(currentEditorRoom->staticColliders[i]);

                    currentEditorRoom->staticColliders[i] = 0;

                    if (--currentEditorRoom->numColliders > 0 && currentEditorRoom->numColliders != i)
                        currentEditorRoom->staticColliders[i] = currentEditorRoom->staticColliders[currentEditorRoom->numColliders];

                    currentEditorRoom->staticColliders[currentEditorRoom->numColliders] = 0;
                    break;
                }
        }
    }
}

void drawPaletteBackground(CollisionStaticRect paletteRect) {
    SDL_SetRenderDrawColor(gbMainRenderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_Rect rect;

    rect.x = paletteRect.x1;
    rect.y = paletteRect.y1;
    rect.w = paletteRect.x2 - paletteRect.x1;
    rect.h = paletteRect.y2 - paletteRect.y1;

    SDL_RenderDrawRect(gbMainRenderer, &rect);
    gbRendererResetDrawColor();
}

void editorRender() {

    switch(mode) {
        case PLACE_STATIC_GEOMETRY:
            {
                if (clickX < 0 || clickY < 0 || dragX < 0 || dragY < 0 || !clickInGrid) break;

                int x1 = clickX < dragX ? clickX : dragX;
                int y1 = clickY < dragY ? clickY : dragY;
                int x2 = dragX > clickX ? dragX : clickX;
                int y2 = dragY > clickY ? dragY : clickY;

                gbGfxGridSquareToWorldCoords(x1, y1, &x1, &y1, 0);
                gbGfxGridSquareToWorldCoords(x2, y2, &x2, &y2, 1);

                SDL_SetRenderDrawColor(gbMainRenderer, 0xFF, 0x00, 0x00, 0xFF);
                SDL_Rect rect;

                rect.x = x1;
                rect.y = y1;
                rect.w = x2 - x1;
                rect.h = y2 - y1;

                SDL_RenderDrawRect(gbMainRenderer, &rect);
                gbRendererResetDrawColor();
                break;
            }
        case PLACE_DYNAMIC:
            drawPaletteBackground(entityPaletteRects[selectedType]);
            break;
        case PLACE_POWER:
            drawPaletteBackground(powerGridRects[selectedWiring]);
    }
}

void handleTextInput() {
    char buffer[GB_INPUT_BUFFER_MAX_LEN];
    uint8_t readStatus = gbInputReadTextInput(buffer);
    if (readStatus != GB_INPUT_BUFFER_READ) {
        textChange(inputFieldText, GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, buffer);
        if (readStatus == GB_INPUT_BUFFER_STOP) {
            textMode = 0;
            inputFieldHitbox->active = 0;
            inputFieldBackground->active = 0;
            inputFieldText->sprite.active = 0;

            if (buffer[0] == '\0') return;

            switch (selectedMenuItem) {
                case SAVE_ROOM:
                    roomSerialize(currentEditorRoom, buffer);
                break;
                case LOAD_ROOM:
                    roomStartDeactivation(currentEditorRoom);
                    roomFinishDeactivation(currentEditorRoom);
                    roomDestroy(currentEditorRoom);
                    currentEditorRoom = roomNew();
                    roomDeserialize(currentEditorRoom, buffer);
                    roomStartActivation(currentEditorRoom);
                    roomFinishActivation(currentEditorRoom);
                break;
                case LOAD_BG:
                    if (!roomLoadBackground(currentEditorRoom, buffer)) {
                        sprintf(buffer, "Failed to load background image.");
                        textChange(inputFieldText, GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, buffer);
                    } else {
                        sprintf(currentEditorRoom->backgroundFilename, buffer);
                    }
                break;
            }
        }
    }
}

void addHitboxToButton(Position *pos, Sprite *button, CollisionStaticRect **colRect, uint8_t active) {
    *colRect = (CollisionStaticRect *)malloc(sizeof(CollisionStaticRect));

    collisionStaticRectSet(*colRect, pos->x,
                                     pos->y,
                                     pos->x + button->width,
                                     pos->y + button->height,
                                     active);
    collisionStaticRectPassiveRegister(*colRect);

    uiColliders[uiColliderCursor++] = *colRect;
}
