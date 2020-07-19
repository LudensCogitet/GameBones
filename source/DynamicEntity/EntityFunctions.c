#include "DynamicEntityType_enum.h"
#include "DynamicEntity_sys.h"

#include "../entities/Guy/entityGuy.h"

void initEntityFunctions() {
    // Guy
    dynamicEntityInits[DYNAMIC_ENTITY_TYPE_GUY]     = &guyInit;
    dynamicEntityTeardowns[DYNAMIC_ENTITY_TYPE_GUY] = &guyTeardown;
    dynamicEntitySetups[DYNAMIC_ENTITY_TYPE_GUY]    = &guyNew;
    dynamicEntityThinks[DYNAMIC_ENTITY_TYPE_GUY]    = &guyThink;
    dynamicEntityResponds[DYNAMIC_ENTITY_TYPE_GUY]  = &guyRespond;
}
