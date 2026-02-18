#ifndef BLOCK_H
#define BLOCK_H

#include <world/block_type.h>
#include <stdint.h>

typedef struct
{
  BlockType type;
  uint32_t x;
  uint32_t y;
  uint32_t z;
} Block;

Block block_create(BlockType type, uint32_t x, uint32_t y, uint32_t z);
int block_is_air(Block* block);

#endif
