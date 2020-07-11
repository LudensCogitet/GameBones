#include "Room_sys.h"
#include "Room_type.h"

#include "../gbGfx/gbGfx_sys.h"
#include "../Sprite/Sprite_sys.h"

static int backgroundTextureId;
static Sprite *backgroundSprite = 0;
static Position backgroundPos;

// Forward Declarations
void unloadBackground();
int loadBackground(char *filename);

int loadBackground(char *filename) {
    int oldTextureId = backgroundTextureId;
    backgroundTextureId = gbTextureLoadFromFile(filename);

    if (backgroundTextureId < 0) {
        backgroundTextureId = oldTextureId;
        return 0;
    }

    unloadBackground(oldTextureId);

    if (!backgroundSprite) {
        backgroundSprite = (Sprite *)malloc(sizeof(Sprite));
    }

    spriteSet(
              backgroundSprite,
              backgroundTextureId,
              0, 0, 640, 640,
              GB_GFX_GRID_SIZE * GB_GFX_GRID_WIDTH, GB_GFX_GRID_SIZE * GB_GFX_GRID_HEIGHT,
              1, 1, SDL_FLIP_NONE
              );
    int x, y;
    gbGfxGridSquareToWorldCoords(0, 0, &x, &y, 0);
    backgroundPos.x = x;
    backgroundPos.y = y;

    spriteRegister(backgroundSprite, &backgroundPos, SPRITE_LAYER_BACKGROUND);

    return 1;
}

void unloadBackground(int textureId) {
    if (backgroundSprite) {
        spriteUnregister(backgroundSprite);
        gbTextureUnload(textureId);
    }
}
