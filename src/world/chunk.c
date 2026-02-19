#define FNL_IMPL
#include <fastnoise.h>
#include <world/chunk.h>
#include <world/chunk_spatial.h>
#include <world/neighbor_query.h>
#include <world/block_type.h>
#include <graphics/texture_registry.h>
#include <graphics/voxel.h>
#include <graphics/vertex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static fnl_state noise_state;
static int noise_initialized = 0;

static void init_noise(void)
{
  if(noise_initialized)
    return;
  noise_state = fnlCreateState();
  noise_state.noise_type = FNL_NOISE_PERLIN;
  noise_state.frequency = 0.005f;
  noise_state.octaves = 4;
  noise_initialized = 1;
}

void chunk_load(Chunk* chunk)
{
  if(!chunk || !chunk->blocks)
    return;

  int flat_height = 20;
  int* blocks = chunk->blocks;

  for(int x = 0; x < CHUNK_XZ; x++) {
    for(int z = 0; z < CHUNK_XZ; z++) {
      for(int y = 0; y < CHUNK_Y && y <= flat_height; y++) {
        BlockType block_type = BLOCK_TYPE_DIRT;
        if(y == flat_height)
          block_type = BLOCK_TYPE_GRASS;
        else if(y < 10)
          block_type = BLOCK_TYPE_COBBLESTONE;
        
        int idx = x * CHUNK_Y * CHUNK_XZ + y * CHUNK_XZ + z;
        blocks[idx] = block_type;
      }
    }
  }
  
  chunk->is_loaded = 1;
}

void chunk_unload(Chunk* chunk)
{
  if(!chunk)
    return;
  
  chunk->is_loaded = 0;
}

int chunk_get_block(Chunk* chunk, int local_x, int local_y, int local_z)
{
  if(!chunk || !chunk->blocks)
    return BLOCK_TYPE_AIR;
  if(local_x < 0 || local_x >= CHUNK_XZ || local_y < 0 || local_y >= CHUNK_Y || local_z < 0 || local_z >= CHUNK_XZ)
    return BLOCK_TYPE_AIR;
  int idx = local_x * CHUNK_Y * CHUNK_XZ + local_y * CHUNK_XZ + local_z;
  return chunk->blocks[idx];
}

static int is_block_air(int x, int y, int z, int base_x, int base_z, Chunk* chunk)
{
  int local_x = x - base_x;
  int local_z = z - base_z;
  
  if(local_x >= 0 && local_x < CHUNK_XZ && local_z >= 0 && local_z < CHUNK_XZ &&
     y >= 0 && y < CHUNK_Y) {
    int idx = local_x * CHUNK_Y * CHUNK_XZ + y * CHUNK_XZ + local_z;
    return chunk->blocks[idx] == BLOCK_TYPE_AIR;
  }
  
  WorldBlock* neighbor = chunk_spatial_get_block_at(x, y, z);
  if(!neighbor || neighbor->type == BLOCK_TYPE_AIR)
    return 1;
  return 0;
}

static inline void add_face_vertices_fast(Vertex* __restrict verts, int* __restrict vert_count, 
                              int bx, int by, int bz, const Vertex* __restrict base, 
                              float u, float v, float u_size, float v_size)
{
  int vc = *vert_count;
  Vertex* v_ptr = verts + vc;
  for(int i = 0; i < 6; i++) {
    v_ptr[i].position[0] = base[i].position[0] + bx;
    v_ptr[i].position[1] = base[i].position[1] + by;
    v_ptr[i].position[2] = base[i].position[2] + bz;
    v_ptr[i].texcoord[0] = base[i].texcoord[0] * u_size + u;
    v_ptr[i].texcoord[1] = base[i].texcoord[1] * v_size + v;
  }
  *vert_count = vc + 6;
}

Chunk* chunk_create(int chunk_x, int chunk_z)
{
  Chunk* chunk = (Chunk*)malloc(sizeof(Chunk));
  if(!chunk)
    return NULL;
  
  chunk->x = chunk_x;
  chunk->z = chunk_z;
  chunk->is_dirty = 1;
  chunk->is_loaded = 0;
  chunk->mesh.vao = 0;
  chunk->mesh.vbo = 0;
  chunk->mesh.vertex_count = 0;
  chunk->vertex_capacity = CHUNK_XZ * CHUNK_Y * CHUNK_XZ * 6 * 6;
  chunk->vertices = (Vertex*)malloc(chunk->vertex_capacity * sizeof(Vertex));
  if(!chunk->vertices) {
    free(chunk);
    return NULL;
  }
  chunk->vertex_count = 0;
  
  int block_count = CHUNK_XZ * CHUNK_Y * CHUNK_XZ;
  chunk->blocks = (int*)malloc(block_count * sizeof(int));
  if(!chunk->blocks) {
    free(chunk->vertices);
    free(chunk);
    return NULL;
  }
  memset(chunk->blocks, 0, block_count * sizeof(int));
  
  return chunk;
}

void chunk_rebuild_mesh(Chunk* chunk)
{
  if(!chunk || !chunk->vertices || !chunk->blocks)
    return;
  
  int vert_count = 0;
  int base_x = chunk->x * CHUNK_XZ;
  int base_z = chunk->z * CHUNK_XZ;
  
  Vertex* verts = chunk->vertices;
  int* blocks = chunk->blocks;
  
  const Vertex* face_front = voxel_get_face_base_vertices(FACE_FRONT);
  const Vertex* face_back = voxel_get_face_base_vertices(FACE_BACK);
  const Vertex* face_left = voxel_get_face_base_vertices(FACE_LEFT);
  const Vertex* face_right = voxel_get_face_base_vertices(FACE_RIGHT);
  const Vertex* face_bottom = voxel_get_face_base_vertices(FACE_BOTTOM);
  const Vertex* face_top = voxel_get_face_base_vertices(FACE_TOP);
  
  for(int y = 0; y < CHUNK_Y; y++) {
    for(int x = 0; x < CHUNK_XZ; x++) {
      for(int z = 0; z < CHUNK_XZ; z++) {
        int idx = x * CHUNK_Y * CHUNK_XZ + y * CHUNK_XZ + z;
        int block_type = blocks[idx];
        if(block_type == BLOCK_TYPE_AIR)
          continue;

        int world_x = base_x + x;
        int world_y = y;
        int world_z = base_z + z;

        BlockTextures block_tex = texture_registry_get_block_textures(block_type);

        if(is_block_air(world_x, world_y, world_z - 1, base_x, base_z, chunk))
          add_face_vertices_fast(verts, &vert_count, world_x, world_y, world_z, face_front, 
                                block_tex.front.u, block_tex.front.v, block_tex.front.u_size, block_tex.front.v_size);
        if(is_block_air(world_x, world_y, world_z + 1, base_x, base_z, chunk))
          add_face_vertices_fast(verts, &vert_count, world_x, world_y, world_z, face_back, 
                                block_tex.back.u, block_tex.back.v, block_tex.back.u_size, block_tex.back.v_size);
        if(is_block_air(world_x - 1, world_y, world_z, base_x, base_z, chunk))
          add_face_vertices_fast(verts, &vert_count, world_x, world_y, world_z, face_left, 
                                block_tex.left.u, block_tex.left.v, block_tex.left.u_size, block_tex.left.v_size);
        if(is_block_air(world_x + 1, world_y, world_z, base_x, base_z, chunk))
          add_face_vertices_fast(verts, &vert_count, world_x, world_y, world_z, face_right, 
                                block_tex.right.u, block_tex.right.v, block_tex.right.u_size, block_tex.right.v_size);
        if(is_block_air(world_x, world_y - 1, world_z, base_x, base_z, chunk))
          add_face_vertices_fast(verts, &vert_count, world_x, world_y, world_z, face_bottom, 
                                block_tex.bottom.u, block_tex.bottom.v, block_tex.bottom.u_size, block_tex.bottom.v_size);
        if(is_block_air(world_x, world_y + 1, world_z, base_x, base_z, chunk))
          add_face_vertices_fast(verts, &vert_count, world_x, world_y, world_z, face_top, 
                                block_tex.top.u, block_tex.top.v, block_tex.top.u_size, block_tex.top.v_size);
      }
    }
  }

  chunk->vertex_count = vert_count;

  if(chunk->mesh.vao != 0) {
    glDeleteBuffers(1, &chunk->mesh.vbo);
    glDeleteVertexArrays(1, &chunk->mesh.vao);
  }

  if(vert_count > 0) {
    mesh_init(&chunk->mesh, chunk->vertices, vert_count);
  } else {
    chunk->mesh.vao = 0;
    chunk->mesh.vbo = 0;
    chunk->mesh.vertex_count = 0;
  }

  chunk->is_dirty = 0;
}

void chunk_render(Chunk* chunk)
{
  if(!chunk || chunk->mesh.vertex_count == 0)
    return;
  
  mesh_render(&chunk->mesh);
}

void chunk_destroy(Chunk* chunk)
{
  if(!chunk)
    return;
  
  if(chunk->mesh.vao != 0) {
    glDeleteBuffers(1, &chunk->mesh.vbo);
    glDeleteVertexArrays(1, &chunk->mesh.vao);
  }
  
  if(chunk->vertices) {
    free(chunk->vertices);
  }
  
  if(chunk->blocks) {
    free(chunk->blocks);
  }
  
  free(chunk);
}

