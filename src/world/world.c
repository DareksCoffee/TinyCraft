#include <world/world.h>
#include <world/block_registry.h>
#include <world/world_grid.h>
#include <world/world_neighbors.h>
#include <graphics/voxel.h>
#include <graphics/mesh.h>
#include <graphics/texture.h>
#include <graphics/texture_registry.h>
#include <graphics/shader.h>
#include <cglm/cglm.h>

int world_init(void)
{
  block_registry_init();
  texture_registry_init();
  voxel_system_init();

  for(int x = 0 ; x < 16 ; x++)
  {
    for(int y = 0 ; y < 16 ; y++)
    {
      for(int z = 0 ; z < 16 ; z++)
      {
      
      world_grid_add_block(BLOCK_TYPE_COBBLESTONE, x, y, z);
      }
    }
  }


  return MESH_OK;
}

void world_update(float delta_time)
{
  (void)delta_time;
}

static int world_is_fully_surrounded(WorldBlock* block)
{
  return world_neighbors_is_solid(0, 1, 0, block) &&
         world_neighbors_is_solid(0, -1, 0, block) &&
         world_neighbors_is_solid(1, 0, 0, block) &&
         world_neighbors_is_solid(-1, 0, 0, block) &&
         world_neighbors_is_solid(0, 0, 1, block) &&
         world_neighbors_is_solid(0, 0, -1, block);
}

void world_render(Shader* shader, mat4 view, mat4 projection)
{
  (void)view;
  (void)projection;

  WorldBlock* blocks = world_grid_get_blocks();
  int block_count = world_grid_get_block_count();

  for(int i = 0; i < block_count; i++) {
    if(blocks[i].type == BLOCK_TYPE_AIR)
      continue;

    if(world_is_fully_surrounded(&blocks[i]))
      continue;

    Texture* texture = texture_registry_get(blocks[i].type);
    if(texture)
      texture_bind(texture);

    mat4 block_model;
    glm_mat4_identity(block_model);
    glm_translate(block_model, (vec3){(float)blocks[i].x, (float)blocks[i].y, (float)blocks[i].z});

    shader_set_mat4(shader, "model", block_model);

    Voxel voxel;
    voxel_init(&voxel, blocks[i].type);
    voxel_render_culled(&voxel, world_neighbors_is_solid, &blocks[i]);
  }
}
