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

typedef enum {
  FACE_FRONT  = 0,
  FACE_BACK   = 1,
  FACE_LEFT   = 2,
  FACE_RIGHT  = 3,
  FACE_BOTTOM = 4,
  FACE_TOP    = 5
} VoxelFace;

typedef int (*VoxelNeighborCheck)(int dx, int dy, int dz, void* context);

void voxel_system_init(void);
int voxel_init(Voxel* voxel, BlockType block_type);
void voxel_render(Voxel* voxel);
void voxel_render_culled(Voxel* voxel, VoxelNeighborCheck neighbor_check, void* context);
void voxel_system_cleanup(void);

#endif
