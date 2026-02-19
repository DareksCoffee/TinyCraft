#include <world/neighbor_query.h>
#include <world/block_type.h>

int neighbor_query_is_solid(int dx, int dy, int dz, void* context)
{
  WorldBlock* current = (WorldBlock*)context;
  int neighbor_x = current->x + dx;
  int neighbor_y = current->y + dy;
  int neighbor_z = current->z + dz;

  WorldBlock* neighbor = chunk_spatial_get_block_at(neighbor_x, neighbor_y, neighbor_z);
  return neighbor && neighbor->type != BLOCK_TYPE_AIR;
}
