#ifndef COLLISION_SYS_H_INCLUDED
#define COLLISION_SYS_H_INCLUDED

#include "../DynamicEntity/DynamicEntity_type.h"

#include "CollisionDynamicRect_type.h"
#include "CollisionStaticRect_type.h"

#define COLLISION_MAX_PASSIVE_COLLIDERS 50
#define COLLISION_MAX_STATIC_COLLIDERS 100
#define COLLISION_MAX_DYNAMIC_COLLIDERS 100

#define COLLISION_NO_STATIC_COLLIDER (COLLISION_MAX_STATIC_COLLIDERS + 1)

#define COLLISION_TOP        1
#define COLLISION_BOTTOM     2
#define COLLISION_LEFT       4
#define COLLISION_RIGHT      8
#define COLLISION_X          16
#define COLLISION_Y          32
#define COLLISION_X_MARKED   64
#define COLLISION_Y_MARKED  128

void collisionInit();
void collisionTeardown();

void collisionStaticTeardown();
void collisionDynamicTeardown();

void collisionDynamicRectSet(
                             CollisionDynamicRect *rect,
                             unsigned int entityId,
                             int offsetX,
                             int offsetY,
                             unsigned int width,
                             unsigned int height,
                             uint8_t solid
                             );
void collisionDynamicRectRegister(CollisionDynamicRect *rect, Position *pos);
void collisionDynamicRectUnregister(CollisionDynamicRect *rect);

void collisionStaticRectSet(CollisionStaticRect * rect, int x1, int y1, int x2, int y2, uint8_t active);
void collisionStaticRectRegister(CollisionStaticRect *rect);
void collisionStaticRectUnregister(CollisionStaticRect *rect);
void collisionStaticRectPassiveRegister(CollisionStaticRect *rect);
void collisionStaticRectPassiveUnregister(CollisionStaticRect *rect);

unsigned int collisionResolveStaticCollisions(unsigned int index, CollisionDynamicRect *dynamicCollider, double dx, double dy, uint8_t *collData);
unsigned int collisionResolveDynamicCollisions(unsigned int index, CollisionDynamicRect *dynamicCollider, double dx, double dy, unsigned int *entityId, uint8_t *collData);
CollisionStaticRect *collisionDetectPointCollisionStatic(int x, int y);
CollisionStaticRect *collisionDetectPointCollisionPassive(int x, int y);
uint8_t collisionCheckPlayer(DynamicEntity *other);

void serializeStaticCollisionRects(CollisionStaticRect **rects, unsigned int numRects, SDL_RWops *file);
CollisionStaticRect *deserializeStaticCollisionRect(SDL_RWops *file);
void collisionDebugDraw();

#endif // COLLISION_SYS_H_INCLUDED
