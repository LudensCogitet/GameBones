#ifndef GB_COLLISION_H_INCLUDED
#define GB_COLLISION_H_INCLUDED

#include "./gbCollisionDynamicRect_type.h"
#include "./gbCollisionStaticRect_type.h"

#define GB_COLLISION_TOP        1
#define GB_COLLISION_BOTTOM     2
#define GB_COLLISION_LEFT       4
#define GB_COLLISION_RIGHT      8
#define GB_COLLISION_X          16
#define GB_COLLISION_Y          32

void gbCollisionInit();
void gbCollisionTeardown();

gbCollisionDynamicRect *gbCollisionDynamicColliderNew(
                                               gbPosition *pos,
                                               gbEntity *entity,
                                               int offsetX,
                                               int offsetY,
                                               unsigned int width,
                                               unsigned int height
                                               );

gbCollisionStaticRect *gbCollisionStaticColliderNew(int x1, int y1, int x2, int y2);
unsigned int gbCollisionResolveStaticCollisions(unsigned int index, gbCollisionDynamicRect *dynamicCollider, double dx, double dy, uint8_t *collData);

void gbCollisionDebugDraw();

#endif // GB_COLLISION_H_INCLUDED
