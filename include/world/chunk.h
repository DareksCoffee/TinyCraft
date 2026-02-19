#ifndef CHUNK_H
#define CHUNK_H

#define CHUNK_XZ  16
#define CHUNK_Y   64

#include <world/chunk_spatial.h>
#include <world/neighbor_query.h>
#include <graphics/mesh.h>
#include <graphics/vertex.h>
#include <graphics/texture_atlas.h>
#include <cglm/cglm.h>
#include <string.h>

typedef struct {
  int x, z;
  Mesh mesh;
  Vertex* vertices;
  int vertex_count;
  int vertex_capacity;
  int is_dirty;
  int is_loaded;
} Chunk;

Chunk* chunk_create(int chunk_x, int chunk_z);
void chunk_load(Chunk* chunk);
void chunk_unload(Chunk* chunk);
void chunk_rebuild_mesh(Chunk* chunk);
void chunk_render(Chunk* chunk);
void chunk_destroy(Chunk* chunk);

#endif
