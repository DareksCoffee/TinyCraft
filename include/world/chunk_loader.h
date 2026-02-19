#ifndef CHUNK_LOADER_H
#define CHUNK_LOADER_H

typedef struct {
  int cx, cz;
} ChunkCoord;

int chunk_loader_queue_preload(int player_chunk_x, int player_chunk_z);
ChunkCoord* chunk_loader_get_preload_queue(void);

#endif
