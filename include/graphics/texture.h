#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct
{
  uint32_t id;
  int width;
  int height;
  int channels;
} Texture;

Texture texture_load(const char* path);
void texture_bind(Texture* texture);
void texture_delete(Texture* texture);

#endif
