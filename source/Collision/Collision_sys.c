#include <string.h>
#include <stdlib.h>
#include <math.h>

// Only used for debug mode
#include "../gbRenderer/gbRenderer_sys.h"
#include "../gbUtils/gbUtils.h"

#include "Collision_sys.h"
#include "CollisionDynamicRect_type.h"
#include "CollisionStaticRect_type.h"

static Position *dynamicColliderPositions[COLLISION_MAX_DYNAMIC_COLLIDERS];
static CollisionDynamicRect *dynamicColliders[COLLISION_MAX_DYNAMIC_COLLIDERS];
static unsigned int dynamicColliderCursor;

static CollisionStaticRect *staticColliders[COLLISION_MAX_STATIC_COLLIDERS];
static unsigned int staticColliderCursor;

static CollisionStaticRect *passiveColliders[COLLISION_MAX_PASSIVE_COLLIDERS];
static unsigned int passiveColliderCursor;

void collisionInit() {
    dynamicColliderCursor = 0;
    staticColliderCursor = 0;
    passiveColliderCursor = 0;

    for (unsigned int i = 0; i < COLLISION_MAX_DYNAMIC_COLLIDERS; i++) {
        dynamicColliders[i] = 0;
        dynamicColliderPositions[i] = 0;
    }

    for (unsigned int i = 0; i < COLLISION_MAX_STATIC_COLLIDERS; i++) {
        staticColliders[i] = 0;
    }

    for (unsigned int i = 0; i < COLLISION_MAX_PASSIVE_COLLIDERS; i++) {
        passiveColliders[i] = 0;
    }
}

void collisionDynamicTeardown() {
    for (unsigned i = 0; i < dynamicColliderCursor; i++) {
        dynamicColliders[i] = 0;
    }
    dynamicColliderCursor = 0;
}

void collisionStaticTeardown() {
    for (unsigned i = 0; i < staticColliderCursor; i++) {
        staticColliders[i] = 0;
    }
    staticColliderCursor = 0;
}

void collisionTeardown() {
    collisionDynamicTeardown();
    collisionStaticTeardown();

    for (unsigned i = 0; i < passiveColliderCursor; i++) {
        passiveColliders[i] = 0;
    }
    passiveColliderCursor = 0;
}

void collisionDynamicRectSet(
                             CollisionDynamicRect *rect,
                             unsigned int entityId,
                             int offsetX,
                             int offsetY,
                             unsigned int width,
                             unsigned int height,
                             uint8_t solid
                             ) {
    rect->entityId = entityId;

    rect->active = 1;
    rect->solid = solid;
    rect->offsetX = offsetX;
    rect->offsetY = offsetY;
    rect->width = width;
    rect->height = height;
}

void collisionDynamicRectRegister(CollisionDynamicRect *rect, Position *pos) {
    if (dynamicColliderCursor >= COLLISION_MAX_DYNAMIC_COLLIDERS)
        return;

    rect->index = dynamicColliderCursor;
    dynamicColliders[dynamicColliderCursor] = rect;
    dynamicColliderPositions[dynamicColliderCursor] = pos;

    dynamicColliderCursor++;
}

void collisionDynamicRectUnregister(CollisionDynamicRect *rect) {
    unsigned int index = rect->index;

    dynamicColliderPositions[index] = 0;
    dynamicColliders[index] = 0;

    if (--dynamicColliderCursor > 0 && dynamicColliderCursor != index) {
        dynamicColliders[index] = dynamicColliders[dynamicColliderCursor];
        dynamicColliderPositions[index] = dynamicColliderPositions[dynamicColliderCursor];
        dynamicColliders[index]->index = index;
    }

    dynamicColliders[dynamicColliderCursor] = 0;
    dynamicColliderPositions[dynamicColliderCursor] = 0;
}

void collisionStaticRectSet(CollisionStaticRect * rect, int x1, int y1, int x2, int y2, uint8_t active) {
    rect->active = active;
    rect->x1 = x1;
    rect->y1 = y1;
    rect->x2 = x2;
    rect->y2 = y2;
}

void collisionStaticRectRegister(CollisionStaticRect *rect) {
    if (staticColliderCursor >= COLLISION_MAX_STATIC_COLLIDERS)
        return;

    staticColliders[staticColliderCursor++] = rect;
}

void collisionStaticRectUnregister(CollisionStaticRect *rect) {
    unsigned int index = 0;
    for (; index < staticColliderCursor; index++) {
        if (staticColliders[index] == rect) break;
    }

    if (index == staticColliderCursor)
        return;

    staticColliders[index] = 0;

    if (--staticColliderCursor > 0 && staticColliderCursor != index) {
        staticColliders[index] = staticColliders[staticColliderCursor];
    }

    staticColliders[staticColliderCursor] = 0;
}

void collisionStaticRectPassiveRegister(CollisionStaticRect *rect) {
    if (passiveColliderCursor >= COLLISION_MAX_PASSIVE_COLLIDERS)
        return;

    passiveColliders[passiveColliderCursor++] = rect;
}

void collisionStaticRectPassiveUnregister(CollisionStaticRect *rect) {
    unsigned int index = 0;
    for (; index < passiveColliderCursor; index++) {
        if (passiveColliders[index] == rect) break;
    }

    if (index == passiveColliderCursor)
        return;

    passiveColliders[index] = 0;

    if (--passiveColliderCursor > 0 && passiveColliderCursor != index) {
        passiveColliders[index] = passiveColliders[passiveColliderCursor];
    }

    passiveColliders[passiveColliderCursor] = 0;
}

// Check collisions between A and B and return flags
// Collision detection is performed from A's perspective
uint8_t detectCollision(
                        double x1A,
                        double y1A,
                        double x2A,
                        double y2A,

                        double x1B,
                        double y1B,
                        double x2B,
                        double y2B,

                        double dx,
                        double dy,

                        double *xOverlap,
                        double *yOverlap
                        ) {
    uint8_t collData = 0;

    // Check x axis
    if ((x1A >= x1B && x2A <= x2B)    ||
        (x1A <= x1B && x2A >= x2B)    ||
        (x1A < x2B && x2A > x2B)    ||
        (x1A < x1B && x2A > x1B)) {
            collData |= COLLISION_X;
            if (x1A < x2B && x2A > x2B)
                collData |= COLLISION_RIGHT;
            else if (x1A < x1B && x2A > x1B)
                collData |= COLLISION_LEFT;
        }

    // If there is no overlap on the x axis, there is no collision.
    if ((y1A >= y1B && y2A <= y2B)    ||
        (y1A <= y1B && y2A >= y2B)    ||
        (y1A < y2B && y2A > y2B)    ||
        (y1A < y1B && y2A > y1B)) {
        collData |= COLLISION_Y;
        if ((y1A < y2B && y2A > y2B))
            collData |= COLLISION_BOTTOM;
        else if ((y1A < y1B && y2A > y1B))
            collData |= COLLISION_TOP;
    }


    if ((collData & (COLLISION_X | COLLISION_Y)) == (COLLISION_X | COLLISION_Y)) {
        int xOverlap1 = x2B - x1A;
        int xOverlap2 = x1B - x2A;

        *xOverlap = abs(xOverlap1) < abs(xOverlap2) ? xOverlap1 : xOverlap2;

        int yOverlap1 = y1B - y2A;
        int yOverlap2 = y2B - y1A;

        *yOverlap = abs(yOverlap1) < abs(yOverlap2) ? yOverlap1 : yOverlap2;

        if ((abs(*xOverlap) <= abs(*yOverlap)))
            collData |= COLLISION_X_MARKED;

        if ((abs(*yOverlap) <= abs(*xOverlap)))
            collData |= COLLISION_Y_MARKED;

    }

    return collData;
}

unsigned int collisionResolveStaticCollisions(unsigned int index, CollisionDynamicRect *dynamicCollider, double dx, double dy, uint8_t *collData) {
    Position *dynamicPosition = dynamicColliderPositions[dynamicCollider->index];

    for (;index < staticColliderCursor; index++) {
        double x1A = dynamicPosition->x + dynamicCollider->offsetX;
        double y1A = dynamicPosition->y + dynamicCollider->offsetY;
        double x2A = x1A + dynamicCollider->width;
        double y2A = y1A + dynamicCollider->height;
        double xOverlap = 0;
        double yOverlap = 0;

        CollisionStaticRect *rect = staticColliders[index];

        if (!rect->active) continue;

        uint8_t data = detectCollision(
                                    x1A, y1A, x2A, y2A,
                                    rect->x1, rect->y1, rect->x2, rect->y2,
                                    dx, dy,
                                    &xOverlap, &yOverlap
                                    );

        if ((data & COLLISION_X) && (data & COLLISION_Y)) {

            if (data & COLLISION_X_MARKED) {
                dynamicPosition->x += xOverlap;
            }
            if (data & COLLISION_Y_MARKED) {
                dynamicPosition->y += yOverlap;
            }

            *collData = data;
            return index + 1;
        }
    }

    *collData = 0;
    return 0;
}

unsigned int collisionResolveDynamicCollisions(unsigned int index, CollisionDynamicRect *dynamicCollider, double dx, double dy, unsigned int *entityId, uint8_t *collData) {
    Position *dynamicPosition = dynamicColliderPositions[dynamicCollider->index];

    for (;index < dynamicColliderCursor; index++) {
        double x1A = dynamicPosition->x + dynamicCollider->offsetX;
        double y1A = dynamicPosition->y + dynamicCollider->offsetY;
        double x2A = x1A + dynamicCollider->width;
        double y2A = y1A + dynamicCollider->height;

        CollisionDynamicRect *rect = dynamicColliders[index];
        if (rect->entityId == dynamicCollider->entityId) continue;

        Position *rectPos = dynamicColliderPositions[index];
        if (!rect->active) continue;

        double x1B = rectPos->x + rect->offsetX;
        double y1B = rectPos->y + rect->offsetY;
        double x2B = x1B + rect->width;
        double y2B = y1B + rect->height;

        double xOverlap = 0;
        double yOverlap = 0;

        uint8_t data = detectCollision(
                                    x1A, y1A, x2A, y2A,
                                    x1B, y1B, x2B, y2B,
                                    dx, dy,
                                    &xOverlap, &yOverlap
                                    );

        if ((data & COLLISION_X) && (data & COLLISION_Y)) {

            if (((xOverlap && dx) || (yOverlap && dy)) && dynamicCollider->solid && rect->solid) {
                if (data & COLLISION_X_MARKED) {
                    dynamicPosition->x += xOverlap;
                }
                if (data & COLLISION_Y_MARKED) {
                    dynamicPosition->y += yOverlap;
                }
            }

            *collData = data;
            *entityId = rect->entityId;
            return index + 1;
        }
    }

    *collData = 0;
    return 0;
}

CollisionStaticRect *collisionDetectPointCollision(int x, int y) {
    for (unsigned int i = 0; i < staticColliderCursor; i++) {
        if (!staticColliders[i]->active) continue;

        if (x > staticColliders[i]->x1 && x < staticColliders[i]->x2 &&
            y > staticColliders[i]->y1 && y < staticColliders[i]->y2) {
                return staticColliders[i];
            }
    }

    return 0;
}

CollisionStaticRect *collisionDetectPointCollisionPassive(int x, int y) {
    for (unsigned int i = 0; i < passiveColliderCursor; i++) {
        if (!passiveColliders[i]->active) continue;

        if (x > passiveColliders[i]->x1 && x < passiveColliders[i]->x2 &&
            y > passiveColliders[i]->y1 && y < passiveColliders[i]->y2) {
                return passiveColliders[i];
            }
    }

    return 0;
}

void collisionDebugDraw() {
    SDL_SetRenderDrawColor(gbMainRenderer, 0x00, 0xff, 0xff, 0xff);

    SDL_Rect rect;
    for (unsigned int i = 0; i < staticColliderCursor; i++) {
        rect.x = staticColliders[i]->x1;
        rect.y = staticColliders[i]->y1;
        rect.w = staticColliders[i]->x2 - staticColliders[i]->x1;
        rect.h = staticColliders[i]->y2 - staticColliders[i]->y1;
        SDL_RenderDrawRect(gbMainRenderer, &rect);
    }

    gbRendererResetDrawColor();
}

void serializeStaticCollisionRects(CollisionStaticRect **rects, unsigned int numRects, SDL_RWops *file) {
    if (!numRects) {
        SDL_WriteBE16(file, 0);
        return;
    }

    // Write number of colliders
    SDL_WriteBE16(file, numRects);

    // Write each collider info
    CollisionStaticRect *rect;
    for (int i = 0; i < numRects; i++) {
        rect = rects[i];

        SDL_WriteU8(file, rect->active);
        SDL_WriteBE64(file, rect->x1);
        SDL_WriteBE64(file, rect->y1);
        SDL_WriteBE64(file, rect->x2);
        SDL_WriteBE64(file, rect->y2);
    }
}

CollisionStaticRect *deserializeStaticCollisionRect(SDL_RWops *file) {
    CollisionStaticRect *rect = (CollisionStaticRect *)malloc(sizeof(CollisionStaticRect));

    rect->active = SDL_ReadU8(file);
    rect->x1 = SDL_ReadBE64(file);
    rect->y1 = SDL_ReadBE64(file);
    rect->x2 = SDL_ReadBE64(file);
    rect->y2 = SDL_ReadBE64(file);

    return rect;
}
