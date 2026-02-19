#include <graphics/texture_registry.h>
#include <graphics/texture_atlas.h>
#include <world/block_type.h>
#include <world/registry.h>
#include <stdlib.h>

static TextureAtlas atlas;

void texture_registry_init(void)
{
  texture_atlas_init(&atlas);
}

Texture* texture_registry_get(BlockType type)
{
  return &atlas.texture;
}

AtlasCoord texture_registry_get_coord(BlockType type)
{
  return texture_atlas_get_coord(&atlas, type);
}

BlockTextures texture_registry_get_block_textures(BlockType type)
{
  return texture_atlas_get_block_textures(&atlas, type);
}

void texture_registry_cleanup(void)
{
  texture_atlas_cleanup(&atlas);
}
