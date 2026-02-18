#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include <graphics/texture.h>
#include <world/block_type.h>
#include <world/block_registry.h>

typedef struct
{
  float u;
  float v;
  float u_size;
  float v_size;
} AtlasCoord;

typedef struct
{
  Texture texture;
  AtlasCoord coords[MAX_BLOCK_TYPES];
} TextureAtlas;

void texture_atlas_init(TextureAtlas* atlas);
AtlasCoord texture_atlas_get_coord(TextureAtlas* atlas, BlockType type);
void texture_atlas_bind(TextureAtlas* atlas);
void texture_atlas_cleanup(TextureAtlas* atlas);

#endif
