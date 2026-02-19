#include <graphics/mesh_batch.h>
#include <stdlib.h>
#include <string.h>

MeshBatch* mesh_batch_create(void)
{
  MeshBatch* batch = (MeshBatch*)malloc(sizeof(MeshBatch));
  if (!batch)
    return NULL;

  batch->vertex_capacity = BATCH_VERTEX_POOL_SIZE;
  batch->vertices = (Vertex*)malloc(batch->vertex_capacity * sizeof(Vertex));
  
  if (!batch->vertices) {
    free(batch);
    return NULL;
  }

  batch->vertex_count = 0;
  batch->batch_mesh.vao = 0;
  batch->batch_mesh.vbo = 0;
  batch->batch_mesh.vertex_count = 0;
  batch->dirty = 0;

  return batch;
}

void mesh_batch_destroy(MeshBatch* batch)
{
  if (!batch)
    return;

  if (batch->batch_mesh.vao != 0) {
    glDeleteBuffers(1, &batch->batch_mesh.vbo);
    glDeleteVertexArrays(1, &batch->batch_mesh.vao);
  }

  free(batch->vertices);
  free(batch);
}

void mesh_batch_add_vertices(MeshBatch* batch, Vertex* vertices, uint32_t vertex_count)
{
  if (!batch || !vertices)
    return;

  if (batch->vertex_count + vertex_count > batch->vertex_capacity) {
    uint32_t new_capacity = batch->vertex_capacity * 2;
    Vertex* new_vertices = (Vertex*)realloc(batch->vertices, new_capacity * sizeof(Vertex));
    
    if (!new_vertices)
      return;

    batch->vertices = new_vertices;
    batch->vertex_capacity = new_capacity;
  }

  memcpy(batch->vertices + batch->vertex_count, vertices, vertex_count * sizeof(Vertex));
  batch->vertex_count += vertex_count;
  batch->dirty = 1;
}

void mesh_batch_rebuild(MeshBatch* batch)
{
  if (!batch || batch->vertex_count == 0)
    return;

  if (batch->batch_mesh.vao != 0) {
    glDeleteBuffers(1, &batch->batch_mesh.vbo);
    glDeleteVertexArrays(1, &batch->batch_mesh.vao);
  }

  mesh_init(&batch->batch_mesh, batch->vertices, batch->vertex_count);
  batch->dirty = 0;
}

void mesh_batch_render(MeshBatch* batch)
{
  if (!batch || batch->vertex_count == 0)
    return;

  if (batch->dirty)
    mesh_batch_rebuild(batch);

  mesh_render(&batch->batch_mesh);
}

void mesh_batch_clear(MeshBatch* batch)
{
  if (!batch)
    return;

  batch->vertex_count = 0;
  batch->dirty = 1;
}
