#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#define MAX_ENTRIES       512

#include <graphics/texture.h>
#include <world/block_type.h>
#include <world/registry.h>

typedef struct
{
  float u;
  float v;
  float u_size;
  float v_size;
} AtlasCoord;

typedef struct
{
  AtlasCoord front;
  AtlasCoord back;
  AtlasCoord left;
  AtlasCoord right;
  AtlasCoord top;
  AtlasCoord bottom;
} BlockTextures;

typedef struct {
  char* name;
  BlockType type;
} BlockNameMapping;

typedef struct
{
  Texture texture;
  AtlasCoord coords[MAX_BLOCK_TYPES];
  BlockTextures block_textures[MAX_BLOCK_TYPES];
} TextureAtlas;

void texture_atlas_init(TextureAtlas* atlas);
AtlasCoord texture_atlas_get_coord(TextureAtlas* atlas, BlockType type);
BlockTextures texture_atlas_get_block_textures(TextureAtlas* atlas, BlockType type);
void texture_atlas_bind(TextureAtlas* atlas);
void texture_atlas_cleanup(TextureAtlas* atlas);

#endif
