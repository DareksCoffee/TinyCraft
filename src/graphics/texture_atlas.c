#include <graphics/texture_atlas.h>
#include <stdio.h>

static TextureAtlas* current_atlas = NULL;

void texture_atlas_init(TextureAtlas* atlas)
{
  if(!atlas)
    return;

  atlas->texture = texture_load("rsc/textures/blocks/cobblestone.png");

  if(!atlas->texture.id)
  {
    printf("Failed to load texture atlas\n");
    return;
  }

  for(int i = 0; i < MAX_BLOCK_TYPES; i++)
  {
    atlas->coords[i].u = 0.0f;
    atlas->coords[i].v = 0.0f;
    atlas->coords[i].u_size = 1.0f;
    atlas->coords[i].v_size = 1.0f;
  }

  atlas->coords[BLOCK_TYPE_AIR].u = 0.0f;
  atlas->coords[BLOCK_TYPE_AIR].v = 0.0f;
  atlas->coords[BLOCK_TYPE_AIR].u_size = 0.0f;
  atlas->coords[BLOCK_TYPE_AIR].v_size = 0.0f;

  atlas->coords[BLOCK_TYPE_COBBLESTONE].u = 0.0f;
  atlas->coords[BLOCK_TYPE_COBBLESTONE].v = 0.0f;
  atlas->coords[BLOCK_TYPE_COBBLESTONE].u_size = 1.0f;
  atlas->coords[BLOCK_TYPE_COBBLESTONE].v_size = 1.0f;

  current_atlas = atlas;
}

AtlasCoord texture_atlas_get_coord(TextureAtlas* atlas, BlockType type)
{
  AtlasCoord zero = {0.0f, 0.0f, 0.0f, 0.0f};
  
  if(!atlas || type >= MAX_BLOCK_TYPES)
    return zero;

  return atlas->coords[type];
}

void texture_atlas_bind(TextureAtlas* atlas)
{
  if(!atlas)
    return;
  texture_bind(&atlas->texture);
}

void texture_atlas_cleanup(TextureAtlas* atlas)
{
  if(!atlas)
    return;
  texture_delete(&atlas->texture);
  current_atlas = NULL;
}
