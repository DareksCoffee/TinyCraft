#include <world/world.h>
#include <world/registry.h>
#include <world/chunk_spatial.h>
#include <world/neighbor_query.h>
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

static Chunk* chunks[MAX_CHUNKS];
static int chunk_count = 0;
static int last_player_chunk_x = 0;
static int last_player_chunk_z = 0;
static Player* world_player = NULL;
static int chunk_rebuild_queue[MAX_CHUNKS];
static int queue_count = 0;
static int queue_index = 0;

static int world_find_chunk(int chunk_x, int chunk_z)
{
  for(int i = 0; i < MAX_CHUNKS; i++) {
    if(chunks[i] && chunks[i]->x == chunk_x && chunks[i]->z == chunk_z) {
      return i;
    }
  }
  return -1;
}

static int world_find_empty_slot(void)
{
  for(int i = 0; i < MAX_CHUNKS; i++) {
    if(!chunks[i]) {
      return i;
    }
  }
  return -1;
}

static int world_is_chunk_in_range(int chunk_x, int chunk_z, int player_chunk_x, int player_chunk_z)
{
  int dx = chunk_x - player_chunk_x;
  int dz = chunk_z - player_chunk_z;
  int distance = (dx * dx) + (dz * dz);
  return distance <= (RENDER_DISTANCE * RENDER_DISTANCE);
}

void world_unload_far_chunks(int player_chunk_x, int player_chunk_z)
{
  for(int i = 0; i < MAX_CHUNKS; i++) {
    if(!chunks[i] || !chunks[i]->is_loaded)
      continue;
    
    if(!world_is_chunk_in_range(chunks[i]->x, chunks[i]->z, player_chunk_x, player_chunk_z)) {
      chunk_unload(chunks[i]);
      chunk_destroy(chunks[i]);
      chunks[i] = NULL;
    }
  }
}

void world_load_new_chunks(int player_chunk_x, int player_chunk_z)
{
  for(int cx = player_chunk_x - RENDER_DISTANCE; cx <= player_chunk_x + RENDER_DISTANCE; cx++) {
    for(int cz = player_chunk_z - RENDER_DISTANCE; cz <= player_chunk_z + RENDER_DISTANCE; cz++) {
      if(!world_is_chunk_in_range(cx, cz, player_chunk_x, player_chunk_z))
        continue;
      
      int idx = world_find_chunk(cx, cz);
      if(idx == -1) {
        int empty_slot = world_find_empty_slot();
        if(empty_slot == -1) {
          continue;
        }
        
        chunks[empty_slot] = chunk_create(cx, cz);
        if(chunks[empty_slot]) {
          chunk_load(chunks[empty_slot]);
          if(queue_count < MAX_CHUNKS) {
            chunk_rebuild_queue[queue_count++] = empty_slot;
          }
          if(empty_slot >= chunk_count) {
            chunk_count = empty_slot + 1;
          }
        }
      } else if(!chunks[idx]->is_loaded) {
        chunk_load(chunks[idx]);
        if(queue_count < MAX_CHUNKS) {
          chunk_rebuild_queue[queue_count++] = idx;
        }
      }
    }
  }
}

void world_update_chunks(Player* player)
{
  if(!player)
    return;
  
  int player_chunk_x = (int)floorf(player->position[0] / CHUNK_XZ);
  int player_chunk_z = (int)floorf(player->position[2] / CHUNK_XZ);
  
  if(player_chunk_x != last_player_chunk_x || player_chunk_z != last_player_chunk_z) {
    world_unload_far_chunks(player_chunk_x, player_chunk_z);
    world_load_new_chunks(player_chunk_x, player_chunk_z);
    
    last_player_chunk_x = player_chunk_x;
    last_player_chunk_z = player_chunk_z;
  }
}

int world_init(Player* player)
{
  registry_init();
  texture_registry_init();
  voxel_system_init();
  chunk_spatial_clear();
  
  world_player = player;
  chunk_count = 0;
  
  int player_chunk_x = 0;
  int player_chunk_z = 0;
  
  world_load_new_chunks(player_chunk_x, player_chunk_z);
  
  last_player_chunk_x = player_chunk_x;
  last_player_chunk_z = player_chunk_z;

  vec3 spawn_pos = {8.0f, 70.0f, 8.0f};
  if(player_init(player, spawn_pos) != PLAYER_OK)
    return MESH_FAIL;

  return MESH_OK;
}

void world_update(float delta_time)
{
  (void)delta_time;
  
  if(world_player) {
    world_update_chunks(world_player);
  }
  
  int chunks_processed = 0;
  while(queue_index < queue_count && chunks_processed < CHUNKS_PER_FRAME) {
    int chunk_idx = chunk_rebuild_queue[queue_index];
    if(chunks[chunk_idx]) {
      chunk_rebuild_mesh(chunks[chunk_idx]);
    }
    queue_index++;
    chunks_processed++;
  }
  
  if(queue_index >= queue_count) {
    queue_index = 0;
    queue_count = 0;
  }
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

  for(int i = 0; i < MAX_CHUNKS; i++) {
    Chunk* chunk = chunks[i];
    if(!chunk || !chunk->is_loaded || chunk->mesh.vertex_count == 0)
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
  for(int i = 0; i < MAX_CHUNKS; i++) {
    if(chunks[i]) {
      chunk_unload(chunks[i]);
      chunk_destroy(chunks[i]);
      chunks[i] = NULL;
    }
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
  WorldBlock* block = chunk_spatial_get_block_at(x, y, z);
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
