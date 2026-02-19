#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <cglm/cglm.h>

typedef struct {
    vec4 planes[6];
} Frustum;

void frustum_from_matrices(Frustum* frustum, mat4 view, mat4 projection);
int frustum_sphere_inside(Frustum* frustum, vec3 center, float radius);
int frustum_aabb_inside(Frustum* frustum, vec3 min, vec3 max);
int frustum_point_inside(Frustum* frustum, vec3 point);

#endif
