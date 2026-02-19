#include <world/world.h>
#include <world/block_registry.h>
#include <world/world_grid.h>
#include <world/world_neighbors.h>
#include <world/chunk.h>
#include <graphics/voxel.h>
#include <graphics/mesh.h>
#include <graphics/texture.h>
#include <graphics/texture_registry.h>
#include <graphics/shader.h>
#include <graphics/frustum.h>
#include <cglm/cglm.h>
#include <stdlib.h>

static Chunk* chunks[9];
static int chunk_count = 0;

int world_init(void)
{
  block_registry_init();
  texture_registry_init();
  voxel_system_init();
  
  chunk_count = 0;
  for(int cx = -1; cx <= 1; cx++) {
    for(int cz = -1; cz <= 1; cz++) {
      chunk_gen(cx, cz);
      chunks[chunk_count] = chunk_create(cx, cz);
      if(chunks[chunk_count]) {
        chunk_rebuild_mesh(chunks[chunk_count]);
        chunk_count++;
      }
    }
  }

  return MESH_OK;
}

void world_update(float delta_time)
{
  (void)delta_time;
}

void world_render(Shader* shader, mat4 view, mat4 projection)
{
  Frustum frustum;
  frustum_from_matrices(&frustum, view, projection);

  Texture* grass_texture = texture_registry_get(BLOCK_TYPE_GRASS);
  if(grass_texture)
    texture_bind(grass_texture);

  mat4 model;
  glm_mat4_identity(model);
  shader_set_mat4(shader, "model", model);

  for(int i = 0; i < chunk_count; i++) {
    Chunk* chunk = chunks[i];
    if(!chunk)
      continue;

    vec3 chunk_min = {(float)(chunk->x * CHUNK_XZ) - 0.5f, -0.5f, (float)(chunk->z * CHUNK_XZ) - 0.5f};
    vec3 chunk_max = {(float)((chunk->x + 1) * CHUNK_XZ) + 0.5f, CHUNK_Y + 0.5f, (float)((chunk->z + 1) * CHUNK_XZ) + 0.5f};

    if(frustum_aabb_inside(&frustum, chunk_min, chunk_max)) {
      chunk_render(chunk);
    }
  }
}

void world_cleanup(void)
{
  for(int i = 0; i < chunk_count; i++) {
    if(chunks[i])
      chunk_destroy(chunks[i]);
  }
  chunk_count = 0;
}

