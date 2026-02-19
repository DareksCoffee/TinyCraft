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
#include <player/player.h>
#include <core/window.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdlib.h>
#include <math.h>

static Chunk* chunks[9];
static int chunk_count = 0;

int world_init(Player* player)
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

  vec3 spawn_pos = {8.0f, 65.0f, 8.0f};
  if(player_init(player, spawn_pos) != PLAYER_OK)
    return MESH_FAIL;

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

void world_mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  Window* win = (Window*)glfwGetWindowUserPointer(window);
  if(win && win->user_data && win->mouse_captured)
  {
    Player* plyr = (Player*)win->user_data;
    camera_mouse_callback(&plyr->camera, xpos, ypos);
  }
}

int world_get_block_at(int x, int y, int z)
{
  WorldBlock* block = world_grid_get_block_at(x, y, z);
  return block ? block->type : BLOCK_TYPE_AIR;
}

int world_check_collision(AABB* aabb)
{
  int min_x = (int)floorf(aabb->min[0]);
  int max_x = (int)ceilf(aabb->max[0]);
  int min_y = (int)floorf(aabb->min[1]);
  int max_y = (int)ceilf(aabb->max[1]);
  int min_z = (int)floorf(aabb->min[2]);
  int max_z = (int)ceilf(aabb->max[2]);

  for(int x = min_x; x < max_x; x++) {
    for(int y = min_y; y < max_y; y++) {
      for(int z = min_z; z < max_z; z++) {
        if(world_get_block_at(x, y, z) != BLOCK_TYPE_AIR) {
          AABB block_aabb = aabb_create((vec3){x + 0.5f, y + 0.5f, z + 0.5f}, 1.0f, 1.0f, 1.0f);
          if(aabb_intersects(aabb, &block_aabb))
            return 1;
        }
      }
    }
  }
  return 0;
}
