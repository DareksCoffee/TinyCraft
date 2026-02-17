#ifndef MESH_H
#define MESH_H 

#define MESH_OK      0
#define MESH_FAIL   -1

#include <graphics/vertex.h>
#include <stdlib.h>
#include <stdint.h>
#include <GL/glew.h>

typedef struct
{
  uint32_t vao;
  uint32_t vbo;
  uint32_t vertex_count;
} Mesh;

int mesh_init(Mesh* mesh, Vertex* vertex_data, uint32_t vertex_count);
void mesh_render(Mesh* mesh);

#endif
