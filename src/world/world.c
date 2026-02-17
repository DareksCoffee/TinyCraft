#include <world/world.h>
#include <graphics/voxel.h>
#include <cglm/cglm.h>

static Voxel world_voxel;

int world_init(void)
{
  if(voxel_init(&world_voxel) != MESH_OK)
    return MESH_FAIL;

  return MESH_OK;
}

void world_update(float delta_time)
{
  world_voxel.rotation[0] += 0.5f * delta_time;
  world_voxel.rotation[1] += 0.7f * delta_time;
  world_voxel.rotation[2] += 0.3f * delta_time;
}

void world_render(Voxel* v)
{
  mesh_render(&world_voxel.mesh);
}
