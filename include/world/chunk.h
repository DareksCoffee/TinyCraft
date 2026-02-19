#ifndef CHUNK_H
#define CHUNK_H

#define CHUNK_XZ  16
#define CHUNK_Y   64

#include <world/world_grid.h>
#include <world/world_neighbors.h>
#include <graphics/mesh.h>
#include <graphics/texture_atlas.h>
#include <cglm/cglm.h>
#include <string.h>

typedef struct {
  int x, z;
  Mesh mesh;
  int is_dirty;
} Chunk;

void chunk_gen(int chunk_x, int chunk_z);
Chunk* chunk_create(int chunk_x, int chunk_z);
void chunk_rebuild_mesh(Chunk* chunk);
void chunk_render(Chunk* chunk);
void chunk_destroy(Chunk* chunk);

#endif
