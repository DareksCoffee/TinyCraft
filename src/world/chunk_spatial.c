#include <world/chunk_spatial.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASH_TABLE_SIZE 262144

typedef struct BlockNode {
  WorldBlock block;
  struct BlockNode* next;
} BlockNode;

static BlockNode* hash_table[HASH_TABLE_SIZE];
static int block_count = 0;

static inline unsigned int hash_coord(int x, int y, int z)
{
  unsigned int h = 2166136261u;
  h ^= (unsigned int)x;
  h *= 16777619u;
  h ^= (unsigned int)y;
  h *= 16777619u;
  h ^= (unsigned int)z;
  h *= 16777619u;
  return h & (HASH_TABLE_SIZE - 1);
}

int chunk_spatial_add_block(BlockType type, int x, int y, int z)
{
    BlockNode* node = (BlockNode*)malloc(sizeof(BlockNode));
    if (!node) {
        fprintf(stderr, "Error: Failed to allocate memory for block node.\n");
        return 0;
    }

    node->block.type = type;
    node->block.x = x;
    node->block.y = y;
    node->block.z = z;
    
    unsigned int hash = hash_coord(x, y, z);
    node->next = hash_table[hash];
    hash_table[hash] = node;
    
    block_count++;
    return 1;
}

static inline WorldBlock* chunk_spatial_get_block_at_internal(int x, int y, int z)
{
  unsigned int hash = hash_coord(x, y, z);
  register BlockNode* node = hash_table[hash];
  
  while(node) {
    if(__builtin_expect(node->block.x == x && node->block.y == y && node->block.z == z, 1))
      return &node->block;
    node = node->next;
  }
  
  return NULL;
}

WorldBlock* chunk_spatial_get_block_at(int x, int y, int z)
{
  return chunk_spatial_get_block_at_internal(x, y, z);
}

int chunk_spatial_get_block_count(void)
{
  return block_count;
}

WorldBlock* chunk_spatial_get_blocks(void)
{
  return NULL;
}

void chunk_spatial_clear(void)
{
  for(int i = 0; i < HASH_TABLE_SIZE; i++) {
    BlockNode* node = hash_table[i];
    while(node) {
      BlockNode* next = node->next;
      free(node);
      node = next;
    }
    hash_table[i] = NULL;
  }
  block_count = 0;
}
