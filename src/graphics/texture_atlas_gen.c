#include <graphics/texture_atlas_gen.h>
#include <graphics/texture.h>
#include <graphics/shader.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stb_image.h>
#include <stb_image_write.h>

static unsigned char* load_image(const char* path, int* width, int* height, int* channels)
{
  unsigned char* data = stbi_load(path, width, height, channels, 4);
  if (!data) {
    printf("Failed to load image: %s\n", path);
    return NULL;
  }
  return data;
}

static void copy_image_to_atlas(unsigned char* atlas_data, int atlas_width,
                                unsigned char* image_data, int img_width, int img_height,
                                int offset_x, int offset_y)
{
  for (int y = 0; y < img_height; y++) {
    for (int x = 0; x < img_width; x++) {
      int src_idx = ((img_height - 1 - y) * img_width + x) * 4;
      int dst_idx = ((offset_y + y) * atlas_width + (offset_x + x)) * 4;
      
      atlas_data[dst_idx + 0] = image_data[src_idx + 0];
      atlas_data[dst_idx + 1] = image_data[src_idx + 1];
      atlas_data[dst_idx + 2] = image_data[src_idx + 2];
      atlas_data[dst_idx + 3] = image_data[src_idx + 3];
    }
  }
}

Texture texture_atlas_gen_create(TextureEntry* entries, int count, TextureAtlas* atlas)
{
  if (!entries || count == 0 || !atlas) {
    Texture empty = {0, 0, 0, 0};
    return empty;
  }

  int atlas_size = 512;
  int tile_size = 16;
  int tiles_per_row = atlas_size / tile_size;

  unsigned char* atlas_data = malloc(atlas_size * atlas_size * 4);
  if (!atlas_data) {
    printf("Failed to allocate atlas memory\n");
    Texture empty = {0, 0, 0, 0};
    return empty;
  }

  memset(atlas_data, 0, atlas_size * atlas_size * 4);

  int block_positions[MAX_BLOCK_TYPES][6];
  memset(block_positions, -1, sizeof(block_positions));

  int current_tile = 0;

  for (int i = 0; i < count; i++) {
    if (current_tile >= tiles_per_row * tiles_per_row) {
      printf("Atlas is full, cannot add more textures\n");
      break;
    }

    int img_width, img_height, img_channels;
    unsigned char* img_data = load_image(entries[i].path, &img_width, &img_height, &img_channels);
    
    if (!img_data) {
      continue;
    }

    int tile_x = current_tile % tiles_per_row;
    int tile_y = current_tile / tiles_per_row;
    int offset_x = tile_x * tile_size;
    int offset_y = tile_y * tile_size;

    copy_image_to_atlas(atlas_data, atlas_size, img_data, img_width, img_height, offset_x, offset_y);

    float u = (float)offset_x / atlas_size;
    float v = (float)offset_y / atlas_size;
    float u_size = (float)tile_size / atlas_size;
    float v_size = (float)tile_size / atlas_size;

    AtlasCoord coord = {u, v, u_size, v_size};

    BlockType block_type = entries[i].block_type;
    TextureGenFace face = entries[i].face;

    switch (face) {
      case FACE_FRONT:
        atlas->block_textures[block_type].front = coord;
        break;
      case FACE_BACK:
        atlas->block_textures[block_type].back = coord;
        break;
      case FACE_LEFT:
        atlas->block_textures[block_type].left = coord;
        break;
      case FACE_RIGHT:
        atlas->block_textures[block_type].right = coord;
        break;
      case FACE_TOP:
        atlas->block_textures[block_type].top = coord;
        break;
      case FACE_BOTTOM:
        atlas->block_textures[block_type].bottom = coord;
        break;
    }

    stbi_image_free(img_data);
    current_tile++;
  }

  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_size, atlas_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  free(atlas_data);

  Texture result = {texture_id, atlas_size, atlas_size, 4};
  return result;
}
