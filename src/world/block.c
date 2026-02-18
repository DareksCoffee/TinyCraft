#include <world/block.h>

Block block_create(BlockType type, uint32_t x, uint32_t y, uint32_t z)
{
  Block block;
  block.type = type;
  block.x = x;
  block.y = y;
  block.z = z;
  return block;
}

int block_is_air(Block* block)
{
  return block->type == BLOCK_TYPE_AIR;
}
