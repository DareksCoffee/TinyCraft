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
  if(!chunk)
    return;
  
  if(!noise_initialized)
    init_noise();

  for(int x = 0; x < CHUNK_XZ; x++) {
    for(int z = 0; z < CHUNK_XZ; z++) {
      int world_x = chunk->x * CHUNK_XZ + x;
      int world_z = chunk->z * CHUNK_XZ + z;
      
      float noise_val = fnlGetNoise2D(&noise_state, (float)world_x, (float)world_z);
      int height = (int)((noise_val + 1.0f) * 32.0f); // Scale to reasonable height
      
      for(int y = 0; y < CHUNK_Y && y <= height; y++) {
        BlockType block_type = BLOCK_TYPE_DIRT;
        if(y == height)
          block_type = BLOCK_TYPE_GRASS;
        else if(y < 10)
          block_type = BLOCK_TYPE_COBBLESTONE;
        
        chunk_spatial_add_block(block_type, world_x, y, world_z);
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

static int is_face_visible(int bx, int by, int bz, int dx, int dy, int dz)
{
  WorldBlock* neighbor = chunk_spatial_get_block_at(bx + dx, by + dy, bz + dz);
  if(!neighbor || neighbor->type == BLOCK_TYPE_AIR)
    return 1;
  return 0;
}

static void add_face_vertices(Vertex** vertices, int* vert_count, 
                              int bx, int by, int bz, VoxelFace face, AtlasCoord atlas_coord)
{
  const Vertex* base = voxel_get_face_base_vertices(face);
  if(!base)
    return;
    
  for(int i = 0; i < 6; i++) {
    (*vertices)[*vert_count].position[0] = base[i].position[0] + bx;
    (*vertices)[*vert_count].position[1] = base[i].position[1] + by;
    (*vertices)[*vert_count].position[2] = base[i].position[2] + bz;
    (*vertices)[*vert_count].texcoord[0] = base[i].texcoord[0] * atlas_coord.u_size + atlas_coord.u;
    (*vertices)[*vert_count].texcoord[1] = base[i].texcoord[1] * atlas_coord.v_size + atlas_coord.v;
    (*vert_count)++;
  }
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
  chunk->vertex_count = 0;
  
  return chunk;
}

void chunk_rebuild_mesh(Chunk* chunk)
{
  if(!chunk)
    return;

  if(!chunk->vertices)
    return;
  
  int vert_count = 0;
  int base_x = chunk->x * CHUNK_XZ;
  int base_z = chunk->z * CHUNK_XZ;

  for(int x = 0; x < CHUNK_XZ; x++) {
    for(int y = 0; y < CHUNK_Y; y++) {
      for(int z = 0; z < CHUNK_XZ; z++) {
        int world_x = base_x + x;
        int world_y = y;
        int world_z = base_z + z;
        
        WorldBlock* block = chunk_spatial_get_block_at(world_x, world_y, world_z);
        if(!block || block->type == BLOCK_TYPE_AIR)
          continue;

        BlockTextures block_tex = texture_registry_get_block_textures(block->type);

        if(is_face_visible(world_x, world_y, world_z, 0, 0, -1))
          add_face_vertices(&chunk->vertices, &vert_count, world_x, world_y, world_z, FACE_FRONT, block_tex.front);
        
        if(is_face_visible(world_x, world_y, world_z, 0, 0, 1))
          add_face_vertices(&chunk->vertices, &vert_count, world_x, world_y, world_z, FACE_BACK, block_tex.back);
        
        if(is_face_visible(world_x, world_y, world_z, -1, 0, 0))
          add_face_vertices(&chunk->vertices, &vert_count, world_x, world_y, world_z, FACE_LEFT, block_tex.left);
        
        if(is_face_visible(world_x, world_y, world_z, 1, 0, 0))
          add_face_vertices(&chunk->vertices, &vert_count, world_x, world_y, world_z, FACE_RIGHT, block_tex.right);
        
        if(is_face_visible(world_x, world_y, world_z, 0, -1, 0))
          add_face_vertices(&chunk->vertices, &vert_count, world_x, world_y, world_z, FACE_BOTTOM, block_tex.bottom);
        
        if(is_face_visible(world_x, world_y, world_z, 0, 1, 0))
          add_face_vertices(&chunk->vertices, &vert_count, world_x, world_y, world_z, FACE_TOP, block_tex.top);
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
  
  free(chunk);
}

