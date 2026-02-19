#ifndef TEXTURE_ATLAS_GEN_H
#define TEXTURE_ATLAS_GEN_H

#include <graphics/texture.h>
#include <graphics/texture_atlas.h>
#include <world/block_type.h>

typedef enum {
  FACE_FRONT  = 0,
  FACE_BACK   = 1,
  FACE_LEFT   = 2,
  FACE_RIGHT  = 3,
  FACE_BOTTOM = 4,
  FACE_TOP    = 5
} TextureGenFace;

typedef struct
{
  const char* path;
  BlockType block_type;
  TextureGenFace face;
} TextureEntry;

Texture texture_atlas_gen_create(TextureEntry* entries, int count, TextureAtlas* atlas);

#endif
