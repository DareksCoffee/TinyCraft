#include <world/world_grid.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GRID_WIDTH 32
#define GRID_HEIGHT 32
#define GRID_DEPTH 32

static WorldBlock *blocks = NULL;
static int block_count = 0;
static int block_capacity = 0;
static WorldBlock* grid[GRID_WIDTH][GRID_HEIGHT][GRID_DEPTH];
static int grid_initialized = 0;

static void init_grid(void)
{
  if(grid_initialized)
    return;
  memset(grid, 0, sizeof(grid));
  grid_initialized = 1;
}

int world_grid_add_block(BlockType type, int x, int y, int z)
{
    if(!grid_initialized)
      init_grid();
    
    if (block_count >= block_capacity) {
        int new_capacity = (block_capacity == 0) ? 1024 : block_capacity * 2;
        WorldBlock *new_ptr = realloc(blocks, new_capacity * sizeof(WorldBlock));
        
        if (!new_ptr) {
            fprintf(stderr, "Error: Failed to allocate memory for world grid.\n");
            return 0; 
        }
        
        blocks = new_ptr;
        block_capacity = new_capacity;
    }

    blocks[block_count].type = type;
    blocks[block_count].x = x;
    blocks[block_count].y = y;
    blocks[block_count].z = z;
    
    if(x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT && z >= 0 && z < GRID_DEPTH)
      grid[x][y][z] = &blocks[block_count];
    
    block_count++;

    return 1;
}

WorldBlock* world_grid_get_block_at(int x, int y, int z)
{
  if(x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT || z < 0 || z >= GRID_DEPTH)
    return NULL;
  
  return grid[x][y][z];
}

int world_grid_get_block_count(void)
{
  return block_count;
}

WorldBlock* world_grid_get_blocks(void)
{
  return blocks;
}

void world_grid_clear(void)
{
  block_count = 0;
  if(grid_initialized)
    memset(grid, 0, sizeof(grid));
}
