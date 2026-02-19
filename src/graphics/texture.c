#include <graphics/texture.h>
#include <utils/path_util.h>
#include <GL/glew.h>
#include <stdio.h>
#include <limits.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture texture_load(const char* path)
{
  Texture texture = {0};

  char full_path[PATH_MAX];
  if (!path_get_resource_path(full_path, sizeof(full_path), path)) {
    printf("Failed to get resource path for: %s\n", path);
    return texture;
  }

  unsigned char* data = stbi_load(full_path, &texture.width, &texture.height, &texture.channels, 0);
  
  if(!data)
  {
    printf("Failed to load texture: %s (resolved to: %s)\n", path, full_path);
    return texture;
  }

  glGenTextures(1, &texture.id);
  glBindTexture(GL_TEXTURE_2D, texture.id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  GLenum format = GL_RGB;
  if(texture.channels == 4)
    format = GL_RGBA;
  else if(texture.channels == 1)
    format = GL_RED;

  glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, data);

  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);

  return texture;
}

void texture_bind(Texture* texture)
{
  if(!texture || !texture->id)
    return;
  glBindTexture(GL_TEXTURE_2D, texture->id);
}

void texture_delete(Texture* texture)
{
  if(texture && texture->id)
    glDeleteTextures(1, &texture->id);
}
