#include "Text_sys.h"
#include "../gbRenderer/gbRenderer_sys.h"
#include "../Sprite/Sprite_sys.h"

void textNew(Text *textStruct,
               const char *text,
               GB_FONT font,
               GB_COLOR color,
               SPRITE_LAYER layer,
               double x,
               double y,
               uint8_t active,
               uint8_t fixed) {
    SDL_Surface *temp = TTF_RenderText_Solid(fonts[font], text, colors[color]);

    int textureIndex = gbTextureLoadFromSurface(temp);
    SDL_FreeSurface(temp);

    int w, h;
    SDL_QueryTexture(gbTextures[textureIndex], 0, 0, &w, &h);

    textStruct->pos = (Position){ x, y };
    spriteSet(&textStruct->sprite,
              textureIndex,
              0, 0, w, h,
              w / gbScaleFactorX,
              h / gbScaleFactorY,
              active,
              fixed,
              SDL_FLIP_NONE);
    spriteRegister(&textStruct->sprite, &textStruct->pos, layer);
}

void textDelete(Text *textGraphic) {
    spriteUnregister(&textGraphic->sprite);
    gbTextureUnload(textGraphic->sprite.texture);
}

void textChange(Text *textGraphic, GB_FONT font, GB_COLOR color, const char *text) {
    SDL_Surface *temp = TTF_RenderText_Solid(fonts[font], text[0] != '\0' ? text : " ", colors[color]);

    gbTextureLoadToIndexFromSurface(textGraphic->sprite.texture, temp);

    SDL_FreeSurface(temp);

    SDL_QueryTexture(gbTextures[textGraphic->sprite.texture], 0, 0, &textGraphic->sprite.src.w, &textGraphic->sprite.src.h);

    textGraphic->sprite.width = textGraphic->sprite.src.w;
    textGraphic->sprite.height = textGraphic->sprite.src.h;
}
