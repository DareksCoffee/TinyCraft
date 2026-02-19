#ifndef WORLD_H
#define WORLD_H

#include <graphics/voxel.h>
#include <graphics/shader.h>
#include <cglm/cglm.h>

int world_init(void);
void world_update(float delta_time);
void world_render(Shader* shader, mat4 view, mat4 projection);
void world_cleanup(void);

#endif
