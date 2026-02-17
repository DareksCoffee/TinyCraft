#ifndef WORLD_H
#define WORLD_H

#include <graphics/voxel.h>
#include <cglm/cglm.h>

int world_init(void);
void world_update(float delta_time);
void world_render(Voxel* v);

#endif
