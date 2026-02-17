#ifndef SHADER_H
#define SHADER_H

#define SHADER_OK      0
#define SHADER_FAIL   -1

#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
  GLuint program;
} Shader;

/**
 * Load and compile a shader program from vertex and fragment shader files
 * @param shader Pointer to shader structure to initialize
 * @param vertex_path Path to vertex shader file
 * @param fragment_path Path to fragment shader file
 * @return SHADER_OK on success, SHADER_FAIL on failure
 */
int shader_load(Shader* shader, const char* vertex_path, const char* fragment_path);

/**
 * Use the shader program
 * @param shader Pointer to shader structure
 */
void shader_use(Shader* shader);

/**
 * Delete the shader program
 * @param shader Pointer to shader structure
 */
void shader_delete(Shader* shader);

#endif
