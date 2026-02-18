#ifndef WORLD_GRID_H
#define WORLD_GRID_H

#include <world/block_type.h>

typedef struct {
  BlockType type;
  int x, y, z;
} WorldBlock;

#define MAX_BLOCKS 256

int world_grid_add_block(BlockType type, int x, int y, int z);
int world_grid_get_block_count(void);
WorldBlock* world_grid_get_blocks(void);
WorldBlock* world_grid_get_block_at(int x, int y, int z);
void world_grid_clear(void);

#endif
