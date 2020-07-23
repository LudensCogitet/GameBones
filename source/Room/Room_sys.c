#include "Room_sys.h"
#include "Room_type.h"

#include <stdlib.h>

#include "../gbGfx/gbGfx_sys.h"
#include "../Sprite/Sprite_sys.h"

static int backgroundTextureId;
static Sprite *backgroundSprite = 0;
static Position backgroundPos;

// Forward Declarations
void unloadBackground();
int loadBackground(char *filename);

int loadBackground(char *filename) {
    int newTextureId = gbTextureLoadFromFile(filename);

    if (newTextureId < 0) {
        return 0;
    }

    unloadBackground();
    backgroundTextureId = newTextureId;

    if (!backgroundSprite) {
        backgroundSprite = (Sprite *)malloc(sizeof(Sprite));
    }

    spriteSet(
              backgroundSprite,
              backgroundTextureId,
              0, 0, 640, 640,
              GB_GFX_GRID_SIZE * GB_GFX_GRID_WIDTH, GB_GFX_GRID_SIZE * GB_GFX_GRID_HEIGHT,
              SPRITE_LAYER_BACKGROUND, 1, 1, SDL_FLIP_NONE
              );
    int x, y;
    gbGfxGridSquareToWorldCoords(0, 0, &x, &y, 0);
    backgroundPos.x = x;
    backgroundPos.y = y;

    spriteRegister(backgroundSprite, &backgroundPos);

    return 1;
}

void unloadBackground() {
    if (backgroundSprite) {
        spriteUnregister(backgroundSprite);
        gbTextureUnload(backgroundTextureId);
        backgroundTextureId = -1;
    }
}
