#ifndef BLOCK_REGISTRY_H
#define BLOCK_REGISTRY_H

#include <world/block_type.h>
#include <stdint.h>

#define MAX_BLOCK_TYPES 256

typedef struct
{
  BlockType type;
  const char* name;
} BlockData;

void block_registry_init(void);
BlockData* block_registry_get(BlockType type);

#endif
