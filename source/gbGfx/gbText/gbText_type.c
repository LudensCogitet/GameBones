#include "./gbText_type.h"
#include "../../gbRenderer/gbRenderer_sys.h"

gbText *gbTextNew(const char *text,
                       GB_FONT font,
                       GB_COLOR color,
                       GB_SPRITE_LAYER layer,
                       double x,
                       double y,
                       uint8_t active,
                       uint8_t fixed) {

    SDL_Surface *temp = TTF_RenderText_Solid(fonts[font], text, colors[color]);

    unsigned int textureIndex = gbTextureLoadFromSurface(temp);
    SDL_FreeSurface(temp);

    int w, h;
    SDL_QueryTexture(gbTextures[textureIndex], 0, 0, &w, &h);

    gbText *textGraphic = (gbText *)malloc(sizeof(gbText));
    textGraphic->pos = (gbPosition){ x, y };

    textGraphic->sprite = gbSpriteNew(
                                     layer,
                                     textureIndex,
                                     0, 0, w, h,
                                     &textGraphic->pos,
                                     w / gbScaleFactorX,
                                     h / gbScaleFactorY,
                                     active,
                                     fixed,
                                     SDL_FLIP_NONE);
    return textGraphic;
}

void gbTextDelete(gbText *textGraphic) {
    textGraphic->sprite->dispose = 1;
    gbTextureUnload(textGraphic->sprite->texture);
    free(textGraphic);
}

void gbTextChange(gbText *textGraphic, GB_FONT font, GB_COLOR color, const char *text) {
    gbSprite *textSprite = textGraphic->sprite;
    SDL_Surface *temp = TTF_RenderText_Solid(fonts[font], text[0] != '\0' ? text : " ", colors[color]);

    gbTextureLoadToIndexFromSurface(textSprite->texture, temp);

    SDL_FreeSurface(temp);

    SDL_QueryTexture(gbTextures[textSprite->texture], 0, 0, &textSprite->src.w, &textSprite->src.h);

    textSprite->width = textSprite->src.w;
    textSprite->height = textSprite->src.h;
}
