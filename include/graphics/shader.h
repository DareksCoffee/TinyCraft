#ifndef SHADER_H
#define SHADER_H

#define SHADER_OK      0
#define SHADER_FAIL   -1

#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <cglm/cglm.h>

typedef struct
{
  GLuint program;
} Shader;

int shader_load(Shader* shader, const char* vertex_path, const char* fragment_path);
void shader_use(Shader* shader);
void shader_delete(Shader* shader);
void shader_set_mat4(Shader* shader, const char* name, mat4 mat);

#endif
