#ifndef INCLUDED_ENUM_H
#define INCLUDED_ENUM_H

typedef enum GB_GFX_FONT {
    GFX_FONT_LARGE_FREE_MONO,
    GFX_FONT_NUM_FONTS
} GB_GFX_FONT;

typedef enum GB_GFX_COLOR {
    GFX_COLOR_WHITE,
    GFX_COLOR_BLACK,
    GFX_COLOR_NUM_COLORS
} GB_GFX_COLOR;

typedef enum GB_PHYSICS_COLLIDER_TYPE {
    PHYSICS_COLLIDER_CIRCLE,
    PHYSICS_COLLIDER_NUM_COLLIDER_TYPES
} GB_PHYSICS_COLLIDER_TYPE;

typedef enum GB_GFX_TEXTURE {
    GFX_TEXTURE_SHIP,
    GFX_TEXTURE_ASTEROID,
    GFX_TEXTURE_BULLET,
    GFX_TEXTURE_BACKGROUND,
    GFX_TEXTURE_DYNAMIC_FIRST,
    GFX_TEXTURE_DYNAMIC_LAST,
    GFX_TEXTURE_NUM_TEXTURES
} GB_GFX_TEXTURE;

typedef enum GB_SFX_SOUND {
    SFX_SOUND_SHIP_BOOST,
    SFX_SOUND_SHIP_CRUISE,
    SFX_SOUND_SHIP_CRASH,
    SFX_SOUND_PHASER_BULLET,
    SFX_SOUND_ASTEROID_HIT,
    SFX_SOUND_NUM_SOUNDS
} GB_SFX_SOUND;

#endif
