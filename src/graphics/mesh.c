#include <graphics/mesh.h>

int mesh_init(Mesh* mesh, Vertex* vertex_data, uint32_t vertex_count)
{
  mesh->vertex_count = vertex_count; 
  
  glGenVertexArrays(1, &mesh->vao);
  glGenBuffers(1, &mesh->vbo);

  glBindVertexArray(mesh->vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
  
  glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertex_data, GL_DYNAMIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return MESH_OK;
}

void mesh_render(Mesh* mesh)
{
  if(!mesh)
    return;
  
  glBindVertexArray(mesh->vao);
  glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
  glBindVertexArray(0);
}
