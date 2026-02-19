#ifndef TEXTURE_REGISTRY_H
#define TEXTURE_REGISTRY_H

#include <graphics/texture.h>
#include <graphics/texture_atlas.h>
#include <world/block_type.h>

void texture_registry_init(void);
Texture* texture_registry_get(BlockType type);
AtlasCoord texture_registry_get_coord(BlockType type);
BlockTextures texture_registry_get_block_textures(BlockType type);
void texture_registry_cleanup(void);

#endif
