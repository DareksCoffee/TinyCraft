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

static const Vertex FACE_BASE_VERTICES[6][6] = {
  {
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } }
  },
  {
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } }
  },
  {
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } }
  },
  {
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } }
  },
  {
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f } }
  },
  {
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } }
  }
};

const Vertex* voxel_get_face_base_vertices(VoxelFace face)
{
  if(face >= 0 && face <= 5)
    return FACE_BASE_VERTICES[face];
  return NULL;
}

static Vertex apply_texture_coord(Vertex v, AtlasCoord coord)
{
  Vertex result = v;
  result.texcoord[0] = v.texcoord[0] * coord.u_size + coord.u;
  result.texcoord[1] = v.texcoord[1] * coord.v_size + coord.v;
  return result;
}

void voxel_get_face_vertices(VoxelFace face, Vertex* out_vertices[6], AtlasCoord atlas_coord)
{
  static Vertex result_verts[6];
  const Vertex* base = voxel_get_face_base_vertices(face);
  
  if(base) {
    for(int i = 0; i < 6; i++) {
      result_verts[i] = apply_texture_coord(base[i], atlas_coord);
    }
    *out_vertices = result_verts;
  }
}

static void get_face_vertices_with_texture(VoxelFace face, Vertex** out_vertices, int* out_count, AtlasCoord coord)
{
  const Vertex* base = voxel_get_face_base_vertices(face);
  static Vertex result_verts[6];
  
  if(base) {
    for(int i = 0; i < 6; i++) {
      result_verts[i] = apply_texture_coord(base[i], coord);
    }
    *out_vertices = result_verts;
    *out_count = 6;
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
    AtlasCoord default_coord = {0.0f, 0.0f, 1.0f, 1.0f};
    get_face_vertices_with_texture((VoxelFace)i, &face_verts, &vert_count, default_coord);
    if(face_verts && vert_count > 0) {
      Vertex adjusted_verts[6];
      for(int j = 0; j < vert_count; j++) {
        adjusted_verts[j] = face_verts[j];
      }
      mesh_init(&culled_meshes[i], adjusted_verts, vert_count);
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
  voxel->block_textures = texture_registry_get_block_textures(block_type);

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

  Vertex* face_verts;
  int vert_count;

  // Front face
  if(!neighbor_check(0, 0, -1, context)) {
    get_face_vertices_with_texture(FACE_FRONT, &face_verts, &vert_count, voxel->block_textures.front);
    if(face_verts && vert_count > 0) {
      glBindBuffer(GL_COPY_WRITE_BUFFER, culled_meshes[FACE_FRONT].vbo);
      glBufferSubData(GL_COPY_WRITE_BUFFER, 0, vert_count * sizeof(Vertex), face_verts);
      culled_meshes[FACE_FRONT].vertex_count = vert_count;
      mesh_render(&culled_meshes[FACE_FRONT]);
    }
  }

  // Back face
  if(!neighbor_check(0, 0, 1, context)) {
    get_face_vertices_with_texture(FACE_BACK, &face_verts, &vert_count, voxel->block_textures.back);
    if(face_verts && vert_count > 0) {
      glBindBuffer(GL_COPY_WRITE_BUFFER, culled_meshes[FACE_BACK].vbo);
      glBufferSubData(GL_COPY_WRITE_BUFFER, 0, vert_count * sizeof(Vertex), face_verts);
      culled_meshes[FACE_BACK].vertex_count = vert_count;
      mesh_render(&culled_meshes[FACE_BACK]);
    }
  }

  // Left face
  if(!neighbor_check(-1, 0, 0, context)) {
    get_face_vertices_with_texture(FACE_LEFT, &face_verts, &vert_count, voxel->block_textures.left);
    if(face_verts && vert_count > 0) {
      glBindBuffer(GL_COPY_WRITE_BUFFER, culled_meshes[FACE_LEFT].vbo);
      glBufferSubData(GL_COPY_WRITE_BUFFER, 0, vert_count * sizeof(Vertex), face_verts);
      culled_meshes[FACE_LEFT].vertex_count = vert_count;
      mesh_render(&culled_meshes[FACE_LEFT]);
    }
  }

  // Right face
  if(!neighbor_check(1, 0, 0, context)) {
    get_face_vertices_with_texture(FACE_RIGHT, &face_verts, &vert_count, voxel->block_textures.right);
    if(face_verts && vert_count > 0) {
      glBindBuffer(GL_COPY_WRITE_BUFFER, culled_meshes[FACE_RIGHT].vbo);
      glBufferSubData(GL_COPY_WRITE_BUFFER, 0, vert_count * sizeof(Vertex), face_verts);
      culled_meshes[FACE_RIGHT].vertex_count = vert_count;
      mesh_render(&culled_meshes[FACE_RIGHT]);
    }
  }

  // Bottom face
  if(!neighbor_check(0, -1, 0, context)) {
    get_face_vertices_with_texture(FACE_BOTTOM, &face_verts, &vert_count, voxel->block_textures.bottom);
    if(face_verts && vert_count > 0) {
      glBindBuffer(GL_COPY_WRITE_BUFFER, culled_meshes[FACE_BOTTOM].vbo);
      glBufferSubData(GL_COPY_WRITE_BUFFER, 0, vert_count * sizeof(Vertex), face_verts);
      culled_meshes[FACE_BOTTOM].vertex_count = vert_count;
      mesh_render(&culled_meshes[FACE_BOTTOM]);
    }
  }

  // Top face
  if(!neighbor_check(0, 1, 0, context)) {
    get_face_vertices_with_texture(FACE_TOP, &face_verts, &vert_count, voxel->block_textures.top);
    if(face_verts && vert_count > 0) {
      glBindBuffer(GL_COPY_WRITE_BUFFER, culled_meshes[FACE_TOP].vbo);
      glBufferSubData(GL_COPY_WRITE_BUFFER, 0, vert_count * sizeof(Vertex), face_verts);
      culled_meshes[FACE_TOP].vertex_count = vert_count;
      mesh_render(&culled_meshes[FACE_TOP]);
    }
  }

  glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
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