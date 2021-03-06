#include <stdlib.h>
#include <math.h>

// Only used for debug mode
#include "../gbRenderer/gbRenderer_sys.h"
#include "../gbUtils/gbUtils.h"

#include "./gbCollision_sys.h"
#include "./gbCollisionDynamicRect_type.h"
#include "./gbCollisionStaticRect_type.h"

#define COLLISION_MAX_STATIC_COLLIDERS 100
#define COLLISION_MAX_DYNAMIC_COLLIDERS 100

#define COLLISION_NO_STATIC_COLLIDER (COLLISION_MAX_STATIC_COLLIDERS + 1)

static gbCollisionStaticRect *staticColliders[COLLISION_MAX_STATIC_COLLIDERS];
static unsigned int staticColliderCursor = 0;

static gbCollisionDynamicRect *dynamicColliders[COLLISION_MAX_DYNAMIC_COLLIDERS];
static unsigned int dynamicColliderCursor = 0;

void gbCollisionInit() {
    for (unsigned int i = 0; i < COLLISION_MAX_STATIC_COLLIDERS; i++) {
        staticColliders[i] = 0;
    }

    for (unsigned int i = 0; i < COLLISION_MAX_DYNAMIC_COLLIDERS; i++) {
        dynamicColliders[i] = 0;
    }
}
void gbCollisionTeardown() {
    for (unsigned i = 0; i < staticColliderCursor; i++) {
        free(staticColliders[i]);
        staticColliders[i] = 0;
    }

    for (unsigned i = 0; i < dynamicColliderCursor; i++) {
        free(dynamicColliders[i]);
        dynamicColliders[i] = 0;
    }
}

gbCollisionDynamicRect *gbCollisionDynamicColliderNew(
                                               gbPosition *pos,
                                               gbEntity *entity,
                                               int offsetX,
                                               int offsetY,
                                               unsigned int width,
                                               unsigned int height
                                               ) {
    if (dynamicColliderCursor + 1 > COLLISION_MAX_DYNAMIC_COLLIDERS) {
        return 0;
    }

    gbCollisionDynamicRect *rect = (gbCollisionDynamicRect *)malloc(sizeof(gbCollisionDynamicRect));

    rect->active = 1;
    rect->dispose = 0;

    rect->pos = pos;
    rect->entity = entity;

    rect->offsetX   = offsetX;
    rect->offsetY   = offsetY;
    rect->width     = width;
    rect->height    = height;

    dynamicColliders[dynamicColliderCursor++] = rect;

    return rect;
}

unsigned int gbCollisionStaticColliderNew(int x1, int y1, int x2, int y2) {
    if (staticColliderCursor + 1 > COLLISION_MAX_STATIC_COLLIDERS) {
        return 0;
    }

    gbCollisionStaticRect *rect = (gbCollisionStaticRect *)malloc(sizeof(gbCollisionStaticRect));

    rect->x1 = x1;
    rect->y1 = y1;
    rect->x2 = x2;
    rect->y2 = y2;

    staticColliders[staticColliderCursor] = rect;

    return staticColliderCursor++;
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

unsigned int gbCollisionResolveStaticCollisions(unsigned int index, gbCollisionDynamicRect *dynamicCollider, double dx, double dy, uint8_t *collData) {
    for (;index < staticColliderCursor; index++) {
        double x1A = dynamicCollider->pos->x + dynamicCollider->offsetX;
        double y1A = dynamicCollider->pos->y + dynamicCollider->offsetY;
        double x2A = x1A + dynamicCollider->width;
        double y2A = y1A + dynamicCollider->height;
        double xOverlap = 0;
        double yOverlap = 0;

        gbCollisionStaticRect *rect = staticColliders[index];
        uint8_t data = detectCollision(
                                    x1A,
                                    y1A,
                                    x2A,
                                    y2A,

                                    rect->x1,
                                    rect->y1,
                                    rect->x2,
                                    rect->y2,

                                    dx,
                                    dy,

                                    &xOverlap,
                                    &yOverlap
                                    );

        if ((data & COLLISION_X) && (data & COLLISION_Y)) {

            if (data & COLLISION_X_MARKED) {
                dynamicCollider->pos->x += xOverlap;
            }
            if (data & COLLISION_Y_MARKED) {
                dynamicCollider->pos->y += yOverlap;
            }

            *collData = data;
            return index + 1;
        }
    }

    *collData = 0;
    return 0;
}

unsigned int gbCollisionDetectPointCollisionStatic(int x, int y) {
    for (unsigned int i = 0; i < staticColliderCursor; i++) {
        if (x > staticColliders[i]->x1 && x < staticColliders[i]->x2 &&
            y > staticColliders[i]->y1 && y < staticColliders[i]->y2) {
                return i;
            }
    }

    return COLLISION_NO_STATIC_COLLIDER;
}

void gbCollisionRemoveStaticCollider(unsigned int index) {
    if (staticColliderCursor <= index || index >= COLLISION_NO_STATIC_COLLIDER) {
        return;
    }

    free(staticColliders[index]);

    if (--staticColliderCursor > index) {
        staticColliders[index] = staticColliders[staticColliderCursor];
        staticColliders[staticColliderCursor] = 0;
    } else {
        staticColliders[index] = 0;
    }
}

void gbCollisionDebugDraw() {
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
