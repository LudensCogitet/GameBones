#ifndef COLLISION_SYS_H_INCLUDED
#define COLLISION_SYS_H_INCLUDED

#include "../DynamicEntity/DynamicEntity_type.h"

#include "CollisionDynamicRect_type.h"
#include "CollisionStaticRect_type.h"

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

void collisionDynamicRectSet(
                             CollisionDynamicRect *rect,
                             Position *pos,
                             unsigned int entityId,
                             int offsetX,
                             int offsetY,
                             unsigned int width,
                             unsigned int height
                             );
void collisionDynamicRectRegister(CollisionDynamicRect *rect);
void collisionDynamicRectUnregister(CollisionDynamicRect *rect);

void collisionStaticRectSet(CollisionStaticRect * rect, int x1, int y1, int x2, int y2);
void collisionStaticRectRegister(CollisionStaticRect *rect);
void collisionStaticRectUnregister(CollisionStaticRect *rect);

unsigned int collisionResolveStaticCollisions(unsigned int index, CollisionDynamicRect *dynamicCollider, double dx, double dy, uint8_t *collData);
CollisionStaticRect *collisionDetectPointCollisionStatic(int x, int y);

void collisionDebugDraw();

#endif // COLLISION_SYS_H_INCLUDED
