#include <player/aabb.h>

AABB aabb_create(vec3 center, float width, float height, float depth)
{
    AABB aabb;
    float half_w = width * 0.5f;
    float half_d = depth * 0.5f;
    
    aabb.min[0] = center[0] - half_w;
    aabb.min[1] = center[1];
    aabb.min[2] = center[2] - half_d;
    
    aabb.max[0] = center[0] + half_w;
    aabb.max[1] = center[1] + height;
    aabb.max[2] = center[2] + half_d;
    
    return aabb;
}

void aabb_update(AABB* aabb, vec3 center)
{
    float width = aabb->max[0] - aabb->min[0];
    float height = aabb->max[1] - aabb->min[1];
    float depth = aabb->max[2] - aabb->min[2];
    
    float half_w = width * 0.5f;
    float half_d = depth * 0.5f;
    
    aabb->min[0] = center[0] - half_w;
    aabb->min[1] = center[1];
    aabb->min[2] = center[2] - half_d;
    
    aabb->max[0] = center[0] + half_w;
    aabb->max[1] = center[1] + height;
    aabb->max[2] = center[2] + half_d;
}

int aabb_intersects(AABB* a, AABB* b)
{
    return (a->min[0] < b->max[0] && a->max[0] > b->min[0] &&
            a->min[1] < b->max[1] && a->max[1] > b->min[1] &&
            a->min[2] < b->max[2] && a->max[2] > b->min[2]);
}
