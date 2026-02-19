#include <graphics/chunk_renderer.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

ChunkRenderer* chunk_renderer_create(void)
{
  ChunkRenderer* renderer = (ChunkRenderer*)malloc(sizeof(ChunkRenderer));
  if (!renderer)
    return NULL;

  renderer->shared_mesh.vao = 0;
  renderer->shared_mesh.vbo = 0;
  renderer->shared_mesh.vertex_count = 0;
  renderer->instance_count = 0;
  renderer->dirty = 0;

  return renderer;
}

void chunk_renderer_destroy(ChunkRenderer* renderer)
{
  if (!renderer)
    return;

  if (renderer->shared_mesh.vao != 0) {
    glDeleteBuffers(1, &renderer->shared_mesh.vbo);
    glDeleteVertexArrays(1, &renderer->shared_mesh.vao);
  }

  free(renderer);
}

void chunk_renderer_add_transform(ChunkRenderer* renderer, mat4 transform)
{
  if (!renderer || renderer->instance_count >= MAX_INSTANCE_TRANSFORMS)
    return;

  glm_mat4_copy(transform, renderer->transforms[renderer->instance_count]);
  renderer->instance_count++;
  renderer->dirty = 1;
}

void chunk_renderer_render(ChunkRenderer* renderer)
{
  if (!renderer || renderer->instance_count == 0)
    return;

  if (renderer->shared_mesh.vao == 0)
    return;

  glBindVertexArray(renderer->shared_mesh.vao);
  glDrawArraysInstanced(GL_TRIANGLES, 0, renderer->shared_mesh.vertex_count, renderer->instance_count);
  glBindVertexArray(0);
}

void chunk_renderer_clear(ChunkRenderer* renderer)
{
  if (!renderer)
    return;

  renderer->instance_count = 0;
  renderer->dirty = 1;
}
