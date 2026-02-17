#include <graphics/shader.h>
#include <string.h>
#include <utils/path_util.h>
#include <limits.h>

/**
 * Read entire file into memory
 * @param path File path to read
 * @return Allocated string with file contents or NULL on failure
 */
static char* shader_read_file(const char* path)
{
  FILE* file = fopen(path, "rb");
  if(!file)
  {
    printf("ERROR: Could not open shader file: %s\n", path);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* buffer = (char*)malloc(file_size + 1);
  if(!buffer)
  {
    printf("ERROR: Could not allocate memory for shader file\n");
    fclose(file);
    return NULL;
  }

  size_t read_size = fread(buffer, 1, file_size, file);
  fclose(file);

  if(read_size != (size_t)file_size)
  {
    printf("ERROR: Failed to read entire shader file\n");
    free(buffer);
    return NULL;
  }

  buffer[file_size] = '\0';
  return buffer;
}

/**
 * Compile a single shader
 * @param shader_type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
 * @param source Shader source code
 * @return Compiled shader handle or 0 on failure
 */
static GLuint shader_compile(GLenum shader_type, const char* source)
{
  GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  int success;
  char info_log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if(!success)
  {
    glGetShaderInfoLog(shader, 512, NULL, info_log);
    const char* shader_name = (shader_type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
    printf("ERROR: %s shader compilation failed:\n%s\n", shader_name, info_log);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

int shader_load(Shader* shader, const char* vertex_path, const char* fragment_path)
{
  if(!shader || !vertex_path || !fragment_path)
  {
    printf("ERROR: Invalid shader parameters\n");
    return SHADER_FAIL;
  }

  // Resolve full paths relative to executable directory
  char full_vertex_path[PATH_MAX];
  char full_fragment_path[PATH_MAX];

  if(!path_get_resource_path(full_vertex_path, sizeof(full_vertex_path), vertex_path))
  {
    printf("ERROR: Failed to resolve vertex shader path\n");
    return SHADER_FAIL;
  }

  if(!path_get_resource_path(full_fragment_path, sizeof(full_fragment_path), fragment_path))
  {
    printf("ERROR: Failed to resolve fragment shader path\n");
    return SHADER_FAIL;
  }

  char* vertex_source = shader_read_file(full_vertex_path);
  if(!vertex_source)
    return SHADER_FAIL;

  char* fragment_source = shader_read_file(full_fragment_path);
  if(!fragment_source)
  {
    free(vertex_source);
    return SHADER_FAIL;
  }

  GLuint vertex_shader = shader_compile(GL_VERTEX_SHADER, vertex_source);
  if(!vertex_shader)
  {
    free(vertex_source);
    free(fragment_source);
    return SHADER_FAIL;
  }

  GLuint fragment_shader = shader_compile(GL_FRAGMENT_SHADER, fragment_source);
  if(!fragment_shader)
  {
    glDeleteShader(vertex_shader);
    free(vertex_source);
    free(fragment_source);
    return SHADER_FAIL;
  }

  shader->program = glCreateProgram();
  glAttachShader(shader->program, vertex_shader);
  glAttachShader(shader->program, fragment_shader);
  glLinkProgram(shader->program);

  int success;
  char info_log[512];
  glGetProgramiv(shader->program, GL_LINK_STATUS, &success);

  if(!success)
  {
    glGetProgramInfoLog(shader->program, 512, NULL, info_log);
    printf("ERROR: Shader program linking failed:\n%s\n", info_log);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(shader->program);
    free(vertex_source);
    free(fragment_source);
    return SHADER_FAIL;
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  free(vertex_source);
  free(fragment_source);

  return SHADER_OK;
}

void shader_use(Shader* shader)
{
  if(shader)
    glUseProgram(shader->program);
}

void shader_delete(Shader* shader)
{
  if(shader && shader->program)
  {
    glDeleteProgram(shader->program);
    shader->program = 0;
  }
}
