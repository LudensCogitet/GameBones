#include <math.h>

// Only used for debug mode
#include "../gbRenderer/gbRenderer_sys.h"

#include "./gbCollision_sys.h"
#include "./gbCollisionDynamicRect_type.h"
#include "./gbCollisionStaticRect_type.h"

#define GB_COLLISION_MAX_STATIC_COLLIDERS 100
#define GB_COLLISION_MAX_DYNAMIC_COLLIDERS 100

static gbCollisionStaticRect *staticColliders[GB_COLLISION_MAX_STATIC_COLLIDERS];
static unsigned int staticColliderCursor = 0;

static gbCollisionDynamicRect *dynamicColliders[GB_COLLISION_MAX_DYNAMIC_COLLIDERS];
static unsigned int dynamicColliderCursor = 0;

void gbCollisionInit() {
    for (unsigned int i = 0; i < GB_COLLISION_MAX_STATIC_COLLIDERS; i++) {
        staticColliders[i] = 0;
    }

    for (unsigned int i = 0; i < GB_COLLISION_MAX_DYNAMIC_COLLIDERS; i++) {
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
    if (dynamicColliderCursor + 1 > GB_COLLISION_MAX_DYNAMIC_COLLIDERS) {
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

gbCollisionStaticRect *gbCollisionStaticColliderNew(int x1, int y1, int x2, int y2) {
    if (staticColliderCursor + 1 > GB_COLLISION_MAX_STATIC_COLLIDERS) {
        return 0;
    }

    gbCollisionStaticRect *rect = (gbCollisionStaticRect *)malloc(sizeof(gbCollisionStaticRect));

    rect->x1 = x1;
    rect->y1 = y1;
    rect->x2 = x2;
    rect->y2 = y2;

    staticColliders[staticColliderCursor++] = rect;

    return rect;
}

// Check collisions between A and B and return flags
// Collision detection is performed from A's perspective
uint8_t detectCollision(
                        int x1A,
                        int y1A,
                        int x2A,
                        int y2A,

                        int x1B,
                        int y1B,
                        int x2B,
                        int y2B,

                        double dx,
                        double dy
                        ) {
    uint8_t collData = 0;
    //printf("%d ", collData);
    // Check x axis
    if ((x1A > x1B && x2A < x2B)    ||
        (x1A < x1B && x2A > x2B)    ||
        (x1A < x2B && x2A > x2B)  ||
        (x1A < x1B && x2A > x1B)) {
            collData |= dx > 0 ? GB_COLLISION_X | GB_COLLISION_LEFT : dx < 0 ? GB_COLLISION_X | GB_COLLISION_RIGHT : GB_COLLISION_X;
        }

    // If there is no overlap on the x axis, there is no collision.
    if ((y1A > y1B && y2A < y2B)    ||
        (y1A < y1B && y2A > y2B)    ||
        (y1A < y2B && y2A > y2B)  ||
        (y1A < y1B && y2A > y1B)) {
        collData |= dy > 0 ? GB_COLLISION_Y | GB_COLLISION_TOP : dy < 0 ? GB_COLLISION_Y | GB_COLLISION_BOTTOM : GB_COLLISION_Y;
    }

    // If there is no overlap on the y axis, there is also no collision
    return collData;
}

unsigned int gbCollisionResolveStaticCollisions(unsigned int index, gbCollisionDynamicRect *dynamicCollider, double dx, double dy, uint8_t *collData) {
    for (;index < staticColliderCursor; index++) {
        int x1A = dynamicCollider->pos->x + dynamicCollider->offsetX;
        int y1A = dynamicCollider->pos->y + dynamicCollider->offsetY;
        int x2A = x1A + dynamicCollider->width;
        int y2A = y1A + dynamicCollider->height;
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
                                    dy
                                    );

        if ((data & GB_COLLISION_X) && (data & GB_COLLISION_Y)) {
            int xOverlap = 0;
            int yOverlap = 0;

            if (data & GB_COLLISION_RIGHT) {
                xOverlap = rect->x2 - x1A;
            } else if (data & GB_COLLISION_LEFT) {
                xOverlap = rect->x1- x2A;
            }

            if (data & GB_COLLISION_TOP) {
                yOverlap = rect->y1 - y2A;
            } else if (data & GB_COLLISION_BOTTOM) {
                yOverlap = rect->y2 - y1A;
            }

            if (dx && dy) {
                if (abs(xOverlap) <= abs(yOverlap))
                    dynamicCollider->pos->x += xOverlap;

                if (abs(yOverlap) <= abs(xOverlap))
                    dynamicCollider->pos->y += yOverlap;
            } else if (dx) {
                dynamicCollider->pos->x += xOverlap;
            } else if (dy) {
                dynamicCollider->pos->y += yOverlap;
            }

            *collData = data;

            return index + 1;
        }
    }

    *collData = 0;
    return 0;
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
