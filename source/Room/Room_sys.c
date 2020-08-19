#include "Room_sys.h"
#include "Room_type.h"

#include <stdlib.h>

#include "Room_sys.h"
#include "../gbGfx/gbGfx_sys.h"
#include "../Sprite/Sprite_sys.h"

#include "../global_state.h"

extern void setPlayerPosition(double x, double y);

// Forward Declarations
void roomUnloadBackground(Room *room);
int roomLoadBackground(Room *room, char *filename);

Room *roomNew(int gridX, int gridY) {
    Room *newRoom = (Room *)malloc(sizeof(Room));

    for (int i = 0; i < ROOM_FILENAME_SIZE_MAX; i++)
        newRoom->roomFilename[i] = '\0';

    for (int i = 0; i < ROOM_BACKGROUND_FILENAME_SIZE_MAX; i++)
        newRoom->backgroundFilename[i] = '\0';

    newRoom->backgroundSprite.active = 0;
    newRoom->backgroundTextureId = -1;
    newRoom->playerStart = 0;
    newRoom->numColliders = 0;
    newRoom->numEntities = 0;

    int x, y;
    gbGfxGridSquareToWorldCoords(0, 0, &x, &y, 0);
    newRoom->backgroundPos.x = x;
    newRoom->backgroundPos.y = y;
    newRoom->backgroundSprite.layer = SPRITE_LAYER_BACKGROUND;

    for (int x = 0; x < GB_GFX_GRID_WIDTH; x++) {
        for (int y = 0; y < GB_GFX_GRID_HEIGHT; y++) {
            newRoom->powerGrid[x][y] = 0;
        }
    }

    newRoom->gridX = gridX;
    newRoom->gridY = gridY;

    return newRoom;
}

void roomDestroy(Room *room) {
    if (room->playerStart) {
        free(room->playerStart);
        room->playerStart = 0;
    }

    for (unsigned int i = 0; i < room->numColliders; i++) {
        free(room->staticColliders[i]);
        room->staticColliders[i] = 0;
    }
    room->numColliders = 0;

    for (unsigned int i = 0; i < room->numEntities; i++) {
        free(room->entities[i]);
        room->entities[i] = 0;
    }
    room->numEntities = 0;

    roomUnloadBackground(room);
    free(room);
    room = 0;
}

int roomAddStaticCollider(Room *room, CollisionStaticRect *rect) {
    if (room->numColliders >= COLLISION_MAX_STATIC_COLLIDERS)
        return 0;

    room->staticColliders[room->numColliders++] = rect;
    return 1;
}

int roomAddDynamicEntity(Room *room, DynamicEntity *entity) {
    if (room->numEntities >= DYNAMIC_ENTITY_MAX_ENTITIES)
        return 0;

    room->entities[room->numEntities++] = entity;

    return 1;
}

int roomLoadBackground(Room * room, char *filename) {
    int newTextureId = gbTextureLoadFromFile(filename);

    if (newTextureId < 0) {
        return 0;
    }

    roomUnloadBackground(room);
    room->backgroundTextureId = newTextureId;

    spriteSet(
              &room->backgroundSprite,
              room->backgroundTextureId,
              0, 0, 640, 640,
              GB_GFX_GRID_SIZE * GB_GFX_GRID_WIDTH, GB_GFX_GRID_SIZE * GB_GFX_GRID_HEIGHT,
              SPRITE_LAYER_BACKGROUND, 1, 1, SDL_FLIP_NONE
              );
    return 1;
}

void roomUnloadBackground(Room *room) {
    if (room->backgroundSprite.active) {
        room->backgroundSprite.active = 0;
        gbTextureUnload(room->backgroundTextureId);
        room->backgroundTextureId = -1;
    }
}

void roomSerialize(Room *room, char *filepath) {
    SDL_RWops *file = SDL_RWFromFile(filepath, "wb");

    // SERIALIZE BACKGROUND FILEPATH
    // Write signal
    SDL_WriteBE16(file, SERIALIZE_BACKGROUND_NAME);

    // Count filepath length
    int charCount = 0;
    while (room->backgroundFilename[charCount++] != '\0');

    // Write filepath length
    SDL_WriteBE16(file, charCount);

    // Write filepath
    for (int i = 0; i < charCount; i++)
        SDL_WriteU8(file, (uint8_t)room->backgroundFilename[i]);

    // SERIALIZE PLAYER START POSITION
    // Check for player start in this room
    if (room->playerStart) {
        SDL_WriteBE16(file, SERIALIZE_PLAYER_START);
        // write x, y coords
        SDL_WriteBE64(file, room->playerStart->x);
        SDL_WriteBE64(file, room->playerStart->y);
    }

    // SERIALIZE STATIC COLLIDERS
    // Write signal
    SDL_WriteBE16(file, SERIALIZE_STATIC_COLLIDERS);

    // Write static colliders
    serializeStaticCollisionRects(room->staticColliders, room->numColliders, file);

    // SERIALIZE DYNAMIC ENTITIES
    // Write signal
    SDL_WriteBE16(file, SERIALIZE_DYNAMIC_ENTITIES);

    // Write dynamic entities
    dynamicEntitySerializeAll(room->entities, room->numEntities, file);

    // SERIALIZE POWER GRID
    // Write signal
    SDL_WriteBE16(file, SERIALIZE_POWER_GRID);

    // Write power grid data
    for (int x = 0; x < GB_GFX_GRID_WIDTH; x++) {
        for (int y = 0; y < GB_GFX_GRID_HEIGHT; y++) {
            SDL_WriteU8(file, room->powerGrid[x][y]);
        }
    }

    // Write end signal
    SDL_WriteBE16(file, SERIALIZE_END);

    SDL_RWclose(file);
}

void roomDeserialize(Room *room, char *filepath) {
    SDL_RWops *file = SDL_RWFromFile(filepath, "r");
    uint16_t signal;

    do {
        signal = SDL_ReadBE16(file);

        switch (signal) {
            case SERIALIZE_BACKGROUND_NAME:
                {
                    uint16_t filenameLength = SDL_ReadBE16(file);
                    int i = 0;
                    for (; i < filenameLength; i++)
                        room->backgroundFilename[i] = SDL_ReadU8(file);
                    room->backgroundFilename[i] = '\0';

                    if (filenameLength)
                        roomLoadBackground(room, room->backgroundFilename);
                }
            break;
            case SERIALIZE_PLAYER_START:
                {
                    double x = SDL_ReadBE64(file);
                    double y = SDL_ReadBE64(file);

                    if (!room->playerStart)
                        room->playerStart = (Position *)malloc(sizeof(Position));

                    room->playerStart->x = x;
                    room->playerStart->y = y;
                    setPlayerPosition(x, y);
                }
            break;
            case SERIALIZE_STATIC_COLLIDERS:
                {
                    uint16_t numColliders = SDL_ReadBE16(file);
                    CollisionStaticRect *rect;
                    for (int i = 0; i < numColliders; i++) {
                        rect = deserializeStaticCollisionRect(file);
                        room->staticColliders[i] = rect;
                    }
                    room->numColliders = numColliders;
                }
            break;
            case SERIALIZE_DYNAMIC_ENTITIES:
                {
                    uint16_t numEntities = SDL_ReadBE16(file);

                    for (int i = 0; i < numEntities; i++) {
                        DynamicEntity *entity = dynamicEntityDeserialize(file);
                        room->entities[i] = entity;
                    }
                    room->numEntities = numEntities;
                }
            break;
            case SERIALIZE_POWER_GRID:
                for (int x = 0; x < GB_GFX_GRID_WIDTH; x++) {
                    for (int y = 0; y < GB_GFX_GRID_HEIGHT; y++) {
                        room->powerGrid[x][y] = SDL_ReadU8(file);
                }
            }
            break;
        }
    } while (signal != SERIALIZE_END);

    SDL_RWclose(file);
}

void roomStartDeactivation(Room *room) {
    for (int i = 0; i < room->numEntities; i++)
        dynamicEntityUnregister(room->entities[i]->id);

    for (int i = 0; i < room->numColliders; i++)
        collisionStaticRectUnregister(room->staticColliders[i]);
}

void roomFinishDeactivation(Room *room) {
    if (room->backgroundSprite.active) {
        spriteUnregister(&room->backgroundSprite);
    }
}

void roomStartActivation(Room *room) {
    if (room->backgroundTextureId > -1) {
        room->backgroundSprite.active = 1;
        spriteRegister(&room->backgroundSprite, &room->backgroundPos);
    }

    currentRoom = 0;
}

void roomFinishActivation(Room *room) {
    for (int i = 0; i < room->numEntities; i++)
        dynamicEntityRegister(room->entities[i]);

    for (int i = 0; i < room->numColliders; i++)
        collisionStaticRectRegister(room->staticColliders[i]);

    if (currentRoom != room)
        currentRoom = room;
}

void roomTracePower(int x, int y) {
    int xCursor = 0;
    int yCursor = 0;
    int routesX[4] = { 0 };
    int routesY[4] = { 0 };

    if (rooms[PG_X(x)][PG_Y(y)]->powerGrid[PG_ROOM_X(x)][PG_ROOM_Y(y)] & POWER_GRID_BLOCKED)
        return;

    rooms[PG_X(x)][PG_Y(y)]->powerGrid[PG_ROOM_X(x)][PG_ROOM_Y(y)] |= POWER_GRID_ON;

    uint8_t wiring = POWER_GRID_GET_WIRING(rooms[PG_X(x)][PG_Y(y)]->powerGrid[PG_ROOM_X(x)][PG_ROOM_Y(y)]);

    if (wiring == POWER_GRID_HORIZONTAL ||
        wiring == POWER_GRID_GENERATOR ||
        wiring == POWER_GRID_CROSS ||
        wiring == POWER_GRID_CORNER_BOTTOM_RIGHT ||
        wiring == POWER_GRID_T_NO_LEFT ||
        wiring == POWER_GRID_T_NO_TOP ||
        wiring == POWER_GRID_T_NO_BOTTOM ||
        wiring == POWER_GRID_CORNER_TOP_RIGHT) {
            if (x + 1 < mainPowerGridWidth) {
                uint8_t cell = rooms[PG_X(x + 1)][PG_Y(y)]->powerGrid[PG_ROOM_X(x + 1)][PG_ROOM_Y(y)];

                if (POWER_GRID_GET_WIRING(cell) && !(cell & POWER_GRID_ON)) {
                routesX[xCursor++] = x + 1;
                routesY[yCursor++] = y;
            }
        }
    }
    if (wiring == POWER_GRID_HORIZONTAL ||
        wiring == POWER_GRID_GENERATOR ||
        wiring == POWER_GRID_CROSS ||
        wiring == POWER_GRID_CORNER_BOTTOM_LEFT ||
        wiring == POWER_GRID_CORNER_TOP_LEFT ||
        wiring == POWER_GRID_T_NO_RIGHT ||
        wiring == POWER_GRID_T_NO_TOP ||
        wiring == POWER_GRID_T_NO_BOTTOM) {
            if (x - 1 >= 0) {
                uint8_t cell = rooms[PG_X(x - 1)][PG_Y(y)]->powerGrid[PG_ROOM_X(x - 1)][PG_ROOM_Y(y)];

                if (POWER_GRID_GET_WIRING(cell) && !(cell & POWER_GRID_ON)) {
                    routesX[xCursor++] = x - 1;
                    routesY[yCursor++] = y;
                }
            }
        }

    if (wiring == POWER_GRID_VERTICAL ||
        wiring == POWER_GRID_GENERATOR ||
        wiring == POWER_GRID_CROSS ||
        wiring == POWER_GRID_CORNER_TOP_LEFT ||
        wiring == POWER_GRID_CORNER_TOP_RIGHT ||
        wiring == POWER_GRID_T_NO_RIGHT ||
        wiring == POWER_GRID_T_NO_LEFT ||
        wiring == POWER_GRID_T_NO_BOTTOM) {
            if (y - 1 >= 0) {
                uint8_t cell = rooms[PG_X(x)][PG_Y(y - 1)]->powerGrid[PG_ROOM_X(x)][PG_ROOM_Y(y - 1)];

                if (POWER_GRID_GET_WIRING(cell) && !(cell & POWER_GRID_ON)) {
                    routesX[xCursor++] = x;
                    routesY[yCursor++] = y - 1;
                }
            }
        }

    if (wiring == POWER_GRID_VERTICAL ||
        wiring == POWER_GRID_GENERATOR ||
        wiring == POWER_GRID_CROSS ||
        wiring == POWER_GRID_CORNER_BOTTOM_LEFT ||
        wiring == POWER_GRID_CORNER_BOTTOM_RIGHT ||
        wiring == POWER_GRID_T_NO_RIGHT ||
        wiring == POWER_GRID_T_NO_LEFT ||
        wiring == POWER_GRID_T_NO_TOP) {
            if (y + 1 < mainPowerGridHeight) {
                uint8_t cell = rooms[PG_X(x)][PG_Y(y + 1)]->powerGrid[PG_ROOM_X(x)][PG_ROOM_Y(y + 1)];

                if (POWER_GRID_GET_WIRING(cell) && !(cell & POWER_GRID_ON)) {
                    routesX[xCursor++] = x;
                    routesY[yCursor++] = y + 1;
                }
            }
        }

    for (int x = 0; x < xCursor; x++) {
        for (int y = 0; y < yCursor; y++) {
            roomTracePower(routesX[x], routesY[y]);
        }
    }
}

void roomRefreshPower() {
    // Clear power
    for (int x = 0; x < mainPowerGridWidth; x++) {
        for (int y = 0; y < mainPowerGridHeight; y++) {
            POWER_GRID_CLEAR_STATE(rooms[PG_X(x)][PG_Y(y)]->powerGrid[PG_ROOM_X(x)][PG_ROOM_Y(y)], POWER_GRID_ON);
        }
    }

    // Seek Generators
    for (int x = 0; x < mainPowerGridWidth; x++) {
        for (int y = 0; y < mainPowerGridHeight; y++) {
            if (POWER_GRID_GET_WIRING(rooms[PG_X(x)][PG_Y(y)]->powerGrid[PG_ROOM_X(x)][PG_ROOM_Y(y)]) == POWER_GRID_GENERATOR) {
                roomTracePower(x, y);
            }
        }
    }

//    if (room->gridY > 0) {
//        Room *above = rooms[room->gridX][room->gridY -1];
//        // Seek powered outlets from room above
//        for (int x = 0; x < GB_GFX_GRID_WIDTH; x++) {
//            if (!(above->powerGrid[x][GB_GFX_GRID_MAX_Y] & POWER_GRID_ON)) continue;
//            int wiring = POWER_GRID_GET_WIRING(above->powerGrid[x][GB_GFX_GRID_MAX_Y]);
//            if (wiring == POWER_GRID_VERTICAL ||
//                wiring == POWER_GRID_CORNER_BOTTOM_LEFT ||
//                wiring == POWER_GRID_CORNER_BOTTOM_RIGHT ||
//                wiring == POWER_GRID_T_NO_TOP ||
//                wiring == POWER_GRID_T_NO_LEFT ||
//                wiring == POWER_GRID_T_NO_RIGHT ||
//                wiring == POWER_GRID_CROSS ||
//                wiring == POWER_GRID_GENERATOR) {
//                roomTracePower(grid, x, 0);
//            }
//        }
//    }
//
//    if (room->gridY < MAIN_ROOM_GRID_MAX_Y) {
//        Room *below = rooms[room->gridX][room->gridY + 1];
//        // Seek powered outlets from room below
//        for (int x = 0; x < GB_GFX_GRID_WIDTH; x++) {
//            if (!(below->powerGrid[x][0] & POWER_GRID_ON)) continue;
//            int wiring = POWER_GRID_GET_WIRING(below->powerGrid[x][0]);
//            if (wiring == POWER_GRID_VERTICAL ||
//                wiring == POWER_GRID_CORNER_TOP_LEFT ||
//                wiring == POWER_GRID_CORNER_TOP_RIGHT ||
//                wiring == POWER_GRID_T_NO_BOTTOM ||
//                wiring == POWER_GRID_T_NO_LEFT ||
//                wiring == POWER_GRID_T_NO_RIGHT ||
//                wiring == POWER_GRID_CROSS ||
//                wiring == POWER_GRID_GENERATOR) {
//                roomTracePower(grid, x, GB_GFX_GRID_MAX_Y);
//            }
//        }
//    }
//
//    if (room->gridX > 0) {
//        Room *left = rooms[room->gridX - 1][room->gridY];
//        // Seek powered outlets from room to left
//        for (int y = 0; y < GB_GFX_GRID_HEIGHT; y++) {
//            if (!(left->powerGrid[GB_GFX_GRID_MAX_X][y] & POWER_GRID_ON)) continue;
//            int wiring = POWER_GRID_GET_WIRING(left->powerGrid[GB_GFX_GRID_MAX_X][y]);
//            if (wiring == POWER_GRID_HORIZONTAL ||
//                wiring == POWER_GRID_CORNER_BOTTOM_RIGHT ||
//                wiring == POWER_GRID_CORNER_TOP_RIGHT ||
//                wiring == POWER_GRID_T_NO_TOP ||
//                wiring == POWER_GRID_T_NO_LEFT ||
//                wiring == POWER_GRID_T_NO_BOTTOM ||
//                wiring == POWER_GRID_CROSS ||
//                wiring == POWER_GRID_GENERATOR) {
//                roomTracePower(grid, 0, y);
//            }
//        }
//    }
//
//    if (room->gridX < MAIN_ROOM_GRID_MAX_X) {
//        Room *right = rooms[room->gridX + 1][room->gridY];
//        // Seek powered outlets from room to right
//        for (int y = 0; y < GB_GFX_GRID_HEIGHT; y++) {
//            if (!(right->powerGrid[0][y] & POWER_GRID_ON)) continue;
//            int wiring = POWER_GRID_GET_WIRING(right->powerGrid[0][y]);
//            if (wiring == POWER_GRID_HORIZONTAL ||
//                wiring == POWER_GRID_CORNER_BOTTOM_LEFT ||
//                wiring == POWER_GRID_CORNER_TOP_LEFT ||
//                wiring == POWER_GRID_T_NO_TOP ||
//                wiring == POWER_GRID_T_NO_RIGHT ||
//                wiring == POWER_GRID_T_NO_BOTTOM ||
//                wiring == POWER_GRID_CROSS ||
//                wiring == POWER_GRID_GENERATOR) {
//                roomTracePower(grid, GB_GFX_GRID_MAX_X, y);
//            }
//        }
//    }
}
