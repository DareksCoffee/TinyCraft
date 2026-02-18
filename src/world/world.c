#include <world/world.h>
#include <world/block_registry.h>
#include <graphics/voxel.h>
#include <graphics/mesh.h>
#include <graphics/texture.h>
#include <graphics/texture_registry.h>
#include <cglm/cglm.h>

static Voxel world_voxel;

int world_init(void)
{
  block_registry_init();
  texture_registry_init();
  voxel_system_init();

  if(voxel_init(&world_voxel, BLOCK_TYPE_COBBLESTONE) != MESH_OK)
    return MESH_FAIL;

  return MESH_OK;
}

void world_update(float delta_time)
{
  world_voxel.rotation[0] += 0.5f * delta_time;
  world_voxel.rotation[1] += 0.7f * delta_time;
  world_voxel.rotation[2] += 0.3f * delta_time;
}

void world_render(Shader* shader, mat4 view, mat4 projection)
{
  (void)shader;
  (void)view;
  (void)projection;
  Texture* texture = texture_registry_get(world_voxel.block_type);
  if(texture)
    texture_bind(texture);
  voxel_render(&world_voxel);
}
