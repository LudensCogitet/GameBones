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

#define EDITOR_MAX_STATIC_COLLIDERS 50

typedef enum {
    SERIALIZE_STATIC_COLLIDERS,
    SERIALIZE_DYNAMIC_ENTITIES,
    SERIALIZE_BACKGROUND_NAME,
    SERIALIZE_END
} SERIALIZE_SIGNAL;

typedef enum {
    NEW_ROOM,
    LOAD_ROOM,
    SAVE_ROOM,
    LOAD_BG,
    NUM_MENU_ITEMS
} MENU_ITEM;

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

static Room currentRoom;
static DynamicEntity *player = 0;

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

static char roomFilepath[50] = {'\0'};

// Forward Declarations
void handleTextInput();
void addHitboxToButton(Position *pos, Sprite *sprite, CollisionStaticRect **colRect, uint8_t active);
void serializeRoom(char *filepath);
void deserializeRoom(char *filepath);

void editorInit() {
    modeButtonPosition = (Position) {GB_LOGICAL_SCREEN_WIDTH - (GB_LOGICAL_SCREEN_WIDTH / 7.27), 0};
    buttonX[PLACE_STATIC_GEOMETRY] = 0;
    buttonX[PLACE_PLAYER] = buttonSrcWidth;

    uiColliderCursor = 0;
    currentRoom.numColliders = 0;
    for (unsigned int i = 0; i < COLLISION_MAX_STATIC_COLLIDERS; i++) {
        currentRoom.staticColliders[i] = 0;
    }

    currentRoom.numEntities = 0;
    for (unsigned int i = 0; i < DYNAMIC_ENTITY_MAX_ENTITIES; i++) {
        currentRoom.entities[i] = 0;
    }

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
                  1, 1, SDL_FLIP_NONE);
        menuPositions[i] = (Position *)malloc(sizeof(Position));
        menuPositions[i]->x = 0;
        menuPositions[i]->y = i * GB_LOGICAL_SCREEN_HEIGHT / 11.25;
        spriteRegister(menuSprites[i], menuPositions[i], SPRITE_LAYER_FOREGROUND);
        addHitboxToButton(menuPositions[i], menuSprites[i], menuHitboxes + i, 1);
    }

    modeButtonSprite = (Sprite *)malloc(sizeof(Sprite));
    spriteSet(
              modeButtonSprite,
              uiTexture,
              buttonX[mode], buttonSrcHeight,
              buttonSrcWidth, buttonSrcHeight,
              buttonSrcWidth, buttonSrcHeight,
              1, 1, SDL_FLIP_NONE);
    spriteRegister(modeButtonSprite, &modeButtonPosition, SPRITE_LAYER_FOREGROUND);

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
              0, 1, SDL_FLIP_NONE);
    spriteRegister(inputFieldBackground, &inputFieldPosition, SPRITE_LAYER_MIDGROUND);

    inputFieldText = (Text*)malloc(sizeof(Text));
    textNew(inputFieldText, " ", GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, GB_SPRITE_LAYER_FOREGROUND, inputFieldPosition.x + 10, inputFieldPosition.y + 15, 1, 1);

    addHitboxToButton(
                      &inputFieldPosition,
                      inputFieldBackground,
                      &inputFieldHitbox,
                      0);
}

void editorTeardown() {
    spriteUnregister(modeButtonSprite);
    spriteUnregister(inputFieldBackground);

    free(modeButtonSprite);
    free(inputFieldBackground);

    textDelete(inputFieldText);
    free(inputFieldText);
    inputFieldText = 0;

    clearRoom();

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
            return;
        }

        for (int i = 0; i < NUM_MENU_ITEMS; i++) {
            if (menuHitboxes[i] == collider) {
                int clearTextBuffer = selectedMenuItem != i;
                switch (i) {
                    case NEW_ROOM:
                        clearRoom();
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
                currentRoom.staticColliders[currentRoom.numColliders++] = rect;

                collisionStaticRectSet(rect, x1 < x2 ? x1 : x2, y1 < y2 ? y1 : y2, x2 > x1 ? x2 : x1, y2 > y1 ? y2 : y1, 1);
                collisionStaticRectRegister(rect);

                break;
            case PLACE_PLAYER: ;
                int x, y;
                SDL_GetMouseState(&x, &y);
                gbGfxScreenCoordsToGridSquare(x, y, &x, &y);
                gbGfxGridSquareToWorldCoords(x, y, &x, &y, 0);

                if (!player) {
                    guyInit();
                    player = guyNew(x, y);
                    dynamicEntityRegister(player);
                } else {
                    player->pos.x = x;
                    player->pos.y = y;
                }
        }
    }

    if(gbInputCheckState(GB_INPUT_MOUSE_ALT, GB_INPUT_RELEASED)) {
        switch (mode) {
            int x, y;
            case PLACE_STATIC_GEOMETRY:
                SDL_GetMouseState(&x, &y);
                CollisionStaticRect *collider = collisionDetectPointCollision(x, y);
                if (!collider) break;

                for (unsigned int i = 0; i < currentRoom.numColliders; i++) {
                    if (currentRoom.staticColliders[i] != collider) continue;

                    collisionStaticRectUnregister(currentRoom.staticColliders[i]);
                    free(currentRoom.staticColliders[i]);

                    currentRoom.staticColliders[i] = 0;

                    if (--currentRoom.numColliders > 0)
                        currentRoom.staticColliders[i] = currentRoom.staticColliders[currentRoom.numColliders];

                    currentRoom.staticColliders[currentRoom.numColliders] = 0;
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
            inputFieldHitbox->active = 0;
            inputFieldBackground->active = 0;
            inputFieldText->sprite.active = 0;

            if (buffer[0] == '\0') return;

            switch (selectedMenuItem) {
                case SAVE_ROOM:
                    serializeRoom(&buffer);
                break;
                case LOAD_ROOM:
                    deserializeRoom(&buffer);
                break;
                case LOAD_BG:
                    if (!loadBackground(buffer)) {
                        sprintf(buffer, "Failed to load background image.");
                        textChange(inputFieldText, GB_FONT_MID_FREE_MONO, GB_COLOR_WHITE, buffer);
                    } else {
                        sprintf(&currentRoom.backgroundFilename, buffer);
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

void clearRoom() {
    unloadBackground();

    for (int i = 0; i < currentRoom.numEntities; i++) {
        dynamicEntityUnregister(currentRoom.entities[i]->id);
        free(currentRoom.entities[i]);
        currentRoom.entities[i] = 0;
    }
    currentRoom.numEntities = 0;
    player = 0;

    for (int i = 0; i < currentRoom.numColliders; i++) {
        collisionStaticRectUnregister(&currentRoom.staticColliders[i]);
        free(currentRoom.staticColliders[i]);
        currentRoom.staticColliders[i] = 0;
    }
}

void serializeRoom(char *filepath) {
    SDL_RWops *file = SDL_RWFromFile(filepath, "wb");

    // SERIALIZE BACKGROUND FILEPATH
    // Write signal
    SDL_WriteBE16(file, SERIALIZE_BACKGROUND_NAME);

    // Count filepath length
    int charCount = 0;
    while (currentRoom.backgroundFilename[charCount++] != '\0');

    // Write filepath length
    SDL_WriteBE16(file, charCount);

    // Write filepath
    for (int i = 0; i < charCount; i++)
        SDL_WriteU8(file, (uint8_t)currentRoom.backgroundFilename[i]);

    // SERIALIZE STATIC COLLIDERS
    // Write signal
    SDL_WriteBE16(file, SERIALIZE_STATIC_COLLIDERS);

    // Write static colliders
    serializeStaticCollisionRects(file);

    // SERIALIZE DYNAMIC ENTITIES
    // Write signal
    SDL_WriteBE16(file, SERIALIZE_DYNAMIC_ENTITIES);

    // Write dynamic entities
    dynamicEntitySerializeAll(file);

    // Write end signal
    SDL_WriteBE16(file, SERIALIZE_END);

    SDL_RWclose(file);
}

void deserializeRoom(char *filepath) {
    SDL_RWops *file = SDL_RWFromFile(filepath, "r");
    uint16_t signal;

    clearRoom();

    do {
        signal = SDL_ReadBE16(file);

        switch (signal) {
            case SERIALIZE_BACKGROUND_NAME:
                {
                    uint16_t filenameLength = SDL_ReadBE16(file);
                    int i = 0;
                    for (; i < filenameLength; i++)
                        currentRoom.backgroundFilename[i] = SDL_ReadU8(file);
                    currentRoom.backgroundFilename[i] = '\0';

                    loadBackground(currentRoom.backgroundFilename);
                }
            break;
            case SERIALIZE_STATIC_COLLIDERS:
                {
                    uint16_t numColliders = SDL_ReadBE16(file);
                    CollisionStaticRect *rect;
                    for (int i = 0; i < numColliders; i++) {
                        rect = deserializeStaticCollisionRect(file);
                        currentRoom.staticColliders[currentRoom.numColliders++] = rect;
                        collisionStaticRectRegister(rect);
                    }
                }
            break;
            case SERIALIZE_DYNAMIC_ENTITIES:
                {
                    uint16_t numEntities = SDL_ReadBE16(file);
                    currentRoom.numEntities = numEntities;
                    for (int i = 0; i < numEntities; i++) {
                        DynamicEntity *entity = dynamicEntityDeserialize(file);
                        currentRoom.entities[i] = entity;
                        dynamicEntityRegister(entity);
                        if (entity->type == DYNAMIC_ENTITY_TYPE_GUY)
                            player = entity;
                    }
                }
            break;
        }
    } while (signal != SERIALIZE_END);

    SDL_RWclose(file);
}
