#include <graphics/voxel.h>
#include <graphics/vertex.h>
#include <graphics/mesh.h>
#include <graphics/texture_registry.h>
#include <world/block_type.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

static Mesh shared_cube_mesh;
static Mesh culled_meshes[6]; 
static int mesh_initialized = 0;

/* Helper function to get vertices for a specific face */
static void get_face_vertices(VoxelFace face, Vertex** out_vertices, int* out_count)
{
  static Vertex front_face[] = {
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } }
  };

  static Vertex back_face[] = {
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } }
  };

  static Vertex left_face[] = {
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } }
  };

  static Vertex right_face[] = {
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } }
  };

  static Vertex bottom_face[] = {
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f } }
  };

  static Vertex top_face[] = {
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } }
  };

  switch(face) {
    case FACE_FRONT:
      *out_vertices = front_face;
      *out_count = 6;
      break;
    case FACE_BACK:
      *out_vertices = back_face;
      *out_count = 6;
      break;
    case FACE_LEFT:
      *out_vertices = left_face;
      *out_count = 6;
      break;
    case FACE_RIGHT:
      *out_vertices = right_face;
      *out_count = 6;
      break;
    case FACE_BOTTOM:
      *out_vertices = bottom_face;
      *out_count = 6;
      break;
    case FACE_TOP:
      *out_vertices = top_face;
      *out_count = 6;
      break;
    default:
      *out_vertices = NULL;
      *out_count = 0;
  }
}

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

  for(int i = 0; i < 6; i++) {
    Vertex* face_verts;
    int vert_count;
    get_face_vertices((VoxelFace)i, &face_verts, &vert_count);
    if(face_verts && vert_count > 0) {
      mesh_init(&culled_meshes[i], face_verts, vert_count);
    }
  }

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

void voxel_render_culled(Voxel* voxel, VoxelNeighborCheck neighbor_check, void* context)
{
  if(!voxel || !mesh_initialized || !neighbor_check)
    return;

  /* Check each face direction and render if neighbor is air/empty */
  /* Front face: -Z direction */
  if(!neighbor_check(0, 0, -1, context))
    mesh_render(&culled_meshes[FACE_FRONT]);

  /* Back face: +Z direction */
  if(!neighbor_check(0, 0, 1, context))
    mesh_render(&culled_meshes[FACE_BACK]);

  /* Left face: -X direction */
  if(!neighbor_check(-1, 0, 0, context))
    mesh_render(&culled_meshes[FACE_LEFT]);

  /* Right face: +X direction */
  if(!neighbor_check(1, 0, 0, context))
    mesh_render(&culled_meshes[FACE_RIGHT]);

  /* Bottom face: -Y direction */
  if(!neighbor_check(0, -1, 0, context))
    mesh_render(&culled_meshes[FACE_BOTTOM]);

  /* Top face: +Y direction */
  if(!neighbor_check(0, 1, 0, context))
    mesh_render(&culled_meshes[FACE_TOP]);
}

void voxel_system_cleanup(void)
{
  if(!mesh_initialized)
    return;
  glDeleteBuffers(1, &shared_cube_mesh.vbo);
  glDeleteVertexArrays(1, &shared_cube_mesh.vao);
  
  for(int i = 0; i < 6; i++) {
    glDeleteBuffers(1, &culled_meshes[i].vbo);
    glDeleteVertexArrays(1, &culled_meshes[i].vao);
  }
  
  mesh_initialized = 0;
}
