#include <world/registry.h>
#include <stdlib.h>

static BlockData block_data[MAX_BLOCK_TYPES];

void registry_init(void)
{
  for(int i = 0; i < MAX_BLOCK_TYPES; i++)
  {
    block_data[i].type = 0;
    block_data[i].name = NULL;
  }

  block_data[BLOCK_TYPE_AIR].type = BLOCK_TYPE_AIR;
  block_data[BLOCK_TYPE_AIR].name = "air";

  block_data[BLOCK_TYPE_COBBLESTONE].type = BLOCK_TYPE_COBBLESTONE;
  block_data[BLOCK_TYPE_COBBLESTONE].name = "cobblestone";

  block_data[BLOCK_TYPE_GRASS].type = BLOCK_TYPE_GRASS;
  block_data[BLOCK_TYPE_GRASS].name = "grass";

  block_data[BLOCK_TYPE_DIRT].type = BLOCK_TYPE_DIRT;
  block_data[BLOCK_TYPE_DIRT].name = "dirt";
}

BlockData* registry_get(BlockType type)
{
  return &block_data[type];
}
