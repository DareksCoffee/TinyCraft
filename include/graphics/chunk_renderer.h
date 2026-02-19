#ifndef CHUNK_RENDERER_H
#define CHUNK_RENDERER_H

#include <graphics/mesh.h>
#include <cglm/cglm.h>

#define MAX_INSTANCE_TRANSFORMS 256

typedef struct {
  Mesh shared_mesh;
  mat4 transforms[MAX_INSTANCE_TRANSFORMS];
  int instance_count;
  int dirty;
} ChunkRenderer;

ChunkRenderer* chunk_renderer_create(void);
void chunk_renderer_destroy(ChunkRenderer* renderer);
void chunk_renderer_add_transform(ChunkRenderer* renderer, mat4 transform);
void chunk_renderer_render(ChunkRenderer* renderer);
void chunk_renderer_clear(ChunkRenderer* renderer);

#endif
