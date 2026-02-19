#ifndef AABB_H
#define AABB_H

#include <cglm/cglm.h>

typedef struct {
    vec3 min;
    vec3 max;
} AABB;

AABB aabb_create(vec3 center, float width, float height, float depth);
void aabb_update(AABB* aabb, vec3 center);
int aabb_intersects(AABB* a, AABB* b);

#endif
