#ifndef VOXEL_H
#define VOXEL_H

#include <graphics/texture_atlas.h>
#include <world/block_type.h>
#include <cglm/cglm.h>

typedef struct 
{
  AtlasCoord atlas_coord;
  float rotation[3];
  BlockType block_type;
} Voxel;

void voxel_system_init(void);
int voxel_init(Voxel* voxel, BlockType block_type);
void voxel_render(Voxel* voxel);
void voxel_system_cleanup(void);

#endif
