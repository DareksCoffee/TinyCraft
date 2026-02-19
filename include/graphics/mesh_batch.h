#ifndef MESH_BATCH_H
#define MESH_BATCH_H

#include <graphics/vertex.h>
#include <graphics/mesh.h>
#include <stdint.h>

#define BATCH_MAX_MESHES 256
#define BATCH_VERTEX_POOL_SIZE 1000000

typedef struct {
  Vertex* vertices;
  uint32_t vertex_count;
  uint32_t vertex_capacity;
  Mesh batch_mesh;
  int dirty;
} MeshBatch;

MeshBatch* mesh_batch_create(void);
void mesh_batch_destroy(MeshBatch* batch);
void mesh_batch_add_vertices(MeshBatch* batch, Vertex* vertices, uint32_t vertex_count);
void mesh_batch_rebuild(MeshBatch* batch);
void mesh_batch_render(MeshBatch* batch);
void mesh_batch_clear(MeshBatch* batch);

#endif
