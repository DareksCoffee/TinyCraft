#ifndef NEIGHBOR_QUERY_H
#define NEIGHBOR_QUERY_H

#include <world/chunk_spatial.h>

int neighbor_query_is_solid(int dx, int dy, int dz, void* context);

#endif
