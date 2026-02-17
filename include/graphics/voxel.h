#ifndef VOXEL_H
#define VOXEL_H

#include <graphics/mesh.h>
#include <cglm/cglm.h>

typedef struct 
{
  Mesh mesh;
  float rotation[3];
} Voxel;

int voxel_init(Voxel* voxel);
void voxel_delete(Voxel* voxel);

#endif
