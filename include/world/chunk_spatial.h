#ifndef CHUNK_SPATIAL_H
#define CHUNK_SPATIAL_H

#include <world/block_type.h>

typedef struct {
  BlockType type;
  int x, y, z;
} WorldBlock;

int chunk_spatial_add_block(BlockType type, int x, int y, int z);
int chunk_spatial_get_block_count(void);
WorldBlock* chunk_spatial_get_blocks(void);
WorldBlock* chunk_spatial_get_block_at(int x, int y, int z);
void chunk_spatial_clear(void);

#endif
