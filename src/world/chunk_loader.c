#include <world/chunk_loader.h>
#include <world/world.h>
#include <stdlib.h>

#define MAX_PRELOAD_QUEUE 512

typedef struct {
  int cx, cz, distance_sq;
} ChunkLoadOrder;

static ChunkLoadOrder preload_queue_internal[MAX_PRELOAD_QUEUE];
static ChunkCoord preload_queue_output[MAX_PRELOAD_QUEUE];
static int preload_count = 0;

static int compare_chunk_distance(const void* a, const void* b)
{
  const ChunkLoadOrder* ca = (const ChunkLoadOrder*)a;
  const ChunkLoadOrder* cb = (const ChunkLoadOrder*)b;
  return ca->distance_sq - cb->distance_sq;
}

int chunk_loader_queue_preload(int player_chunk_x, int player_chunk_z)
{
  preload_count = 0;
  
  for(int cx = player_chunk_x - PRELOAD_DISTANCE; cx <= player_chunk_x + PRELOAD_DISTANCE; cx++) {
    for(int cz = player_chunk_z - PRELOAD_DISTANCE; cz <= player_chunk_z + PRELOAD_DISTANCE; cz++) {
      int dx = cx - player_chunk_x;
      int dz = cz - player_chunk_z;
      int distance_sq = dx * dx + dz * dz;
      
      if(distance_sq > PRELOAD_DISTANCE * PRELOAD_DISTANCE)
        continue;
      
      if(preload_count >= MAX_PRELOAD_QUEUE)
        break;
      
      preload_queue_internal[preload_count].cx = cx;
      preload_queue_internal[preload_count].cz = cz;
      preload_queue_internal[preload_count].distance_sq = distance_sq;
      preload_count++;
    }
  }
  
  qsort(preload_queue_internal, preload_count, sizeof(ChunkLoadOrder), compare_chunk_distance);
  
  for(int i = 0; i < preload_count; i++) {
    preload_queue_output[i].cx = preload_queue_internal[i].cx;
    preload_queue_output[i].cz = preload_queue_internal[i].cz;
  }
  
  return preload_count;
}

ChunkCoord* chunk_loader_get_preload_queue(void)
{
  return preload_queue_output;
}
