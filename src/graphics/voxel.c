#include <graphics/voxel.h>
#include <graphics/vertex.h>
#include <graphics/mesh.h>
#include <graphics/texture_registry.h>
#include <world/block_type.h>
#include <GL/glew.h>
#include <stdio.h>

static Mesh shared_cube_mesh;
static int mesh_initialized = 0;

void voxel_system_init(void)
{
  if(mesh_initialized)
    return;

  Vertex cube_vertices[] = {
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } },

    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },

    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } },

    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } },

    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f } },

    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } }
  };

  mesh_init(&shared_cube_mesh, cube_vertices, 36);
  mesh_initialized = 1;
}

int voxel_init(Voxel* voxel, BlockType block_type)
{
  if(block_type == BLOCK_TYPE_AIR)
  {
    printf("Warning: Attempted to create voxel with air block type\n");
    return MESH_FAIL;
  }

  voxel->block_type = block_type;
  voxel->rotation[0] = 0.0f;
  voxel->rotation[1] = 0.0f;
  voxel->rotation[2] = 0.0f;

  voxel->atlas_coord = texture_registry_get_coord(block_type);

  return MESH_OK;
}

void voxel_render(Voxel* voxel)
{
  if(!voxel || !mesh_initialized)
    return;
  mesh_render(&shared_cube_mesh);
}

void voxel_system_cleanup(void)
{
  if(!mesh_initialized)
    return;
  glDeleteBuffers(1, &shared_cube_mesh.vbo);
  glDeleteVertexArrays(1, &shared_cube_mesh.vao);
  mesh_initialized = 0;
}
