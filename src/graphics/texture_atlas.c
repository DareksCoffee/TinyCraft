#include <graphics/texture_atlas.h>
#include <graphics/texture_atlas_gen.h>
#include <utils/path_util.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

static TextureAtlas* current_atlas = NULL;

#define MAX_ENTRIES 512

static int build_entries_from_directory(TextureEntry* entries, int max_count)
{
  char texture_dir[PATH_MAX];
  if (!path_get_resource_path(texture_dir, sizeof(texture_dir), "../.tinycraft/textures/block")) {
    printf("Failed to get resource path for .tinycraft/textures/block\n");
    return 0;
  }

  printf("Loading textures from: %s\n", texture_dir);

  DIR* dir = opendir(texture_dir);
  if (!dir) {
    printf("Failed to open texture directory: %s\n", texture_dir);
    return 0;
  }

  int count = 0;
  struct dirent* entry;

  while ((entry = readdir(dir)) && count < max_count) {
    if (entry->d_type != DT_REG) continue;

    int len = strlen(entry->d_name);
    if (len < 5 || strcmp(entry->d_name + len - 4, ".png") != 0) continue;

    char* filename = (char*)malloc(len + 1);
    strcpy(filename, entry->d_name);
    filename[len - 4] = '\0';

    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s.png", texture_dir, filename);

    BlockType block_type = BLOCK_TYPE_AIR;
    if (strcmp(filename, "cobblestone") == 0) {
      block_type = BLOCK_TYPE_COBBLESTONE;
    } else if (strcmp(filename, "dirt") == 0) {
      block_type = BLOCK_TYPE_DIRT;
    } else if (strcmp(filename, "grass_block_top") == 0) {
      block_type = BLOCK_TYPE_GRASS;
    } else if (strcmp(filename, "grass_block_side") == 0) {
      block_type = BLOCK_TYPE_GRASS;
    }

    if (block_type == BLOCK_TYPE_AIR) {
      free(filename);
      continue;
    }

    int faces_to_add[6];
    int face_count = 0;

    if (strstr(filename, "top")) {
      faces_to_add[face_count++] = FACE_TOP;
    } else if (strstr(filename, "bottom")) {
      faces_to_add[face_count++] = FACE_BOTTOM;
    } else if (strstr(filename, "side")) {
      faces_to_add[face_count++] = FACE_FRONT;
      faces_to_add[face_count++] = FACE_BACK;
      faces_to_add[face_count++] = FACE_LEFT;
      faces_to_add[face_count++] = FACE_RIGHT;
    } else {
      faces_to_add[face_count++] = FACE_FRONT;
      faces_to_add[face_count++] = FACE_BACK;
      faces_to_add[face_count++] = FACE_LEFT;
      faces_to_add[face_count++] = FACE_RIGHT;
      faces_to_add[face_count++] = FACE_TOP;
      faces_to_add[face_count++] = FACE_BOTTOM;
    }

    for (int i = 0; i < face_count; i++) {
      if (count >= max_count) break;
      entries[count].path = (char*)malloc(strlen(full_path) + 1);
      strcpy((char*)entries[count].path, full_path);
      entries[count].block_type = block_type;
      entries[count].face = faces_to_add[i];
      count++;
    }
  }

  closedir(dir);
  return count;
}

void texture_atlas_init(TextureAtlas* atlas)
{
  if(!atlas)
    return;

  for(int i = 0; i < MAX_BLOCK_TYPES; i++)
  {
    atlas->coords[i].u = 0.0f;
    atlas->coords[i].v = 0.0f;
    atlas->coords[i].u_size = 1.0f;
    atlas->coords[i].v_size = 1.0f;

    atlas->block_textures[i].front = atlas->coords[i];
    atlas->block_textures[i].back = atlas->coords[i];
    atlas->block_textures[i].left = atlas->coords[i];
    atlas->block_textures[i].right = atlas->coords[i];
    atlas->block_textures[i].top = atlas->coords[i];
    atlas->block_textures[i].bottom = atlas->coords[i];
  }

  TextureEntry entries[MAX_ENTRIES];
  int entry_count = build_entries_from_directory(entries, MAX_ENTRIES);

  if (entry_count == 0) {
    printf("No textures found in .tinycraft/textures/block\n");
    return;
  }

  atlas->texture = texture_atlas_gen_create(entries, entry_count, atlas);

  for (int i = 0; i < entry_count; i++) {
    free((void*)entries[i].path);
  }

  if(!atlas->texture.id)
  {
    printf("Failed to load texture atlas\n");
    return;
  }

  atlas->coords[BLOCK_TYPE_AIR].u = 0.0f;
  atlas->coords[BLOCK_TYPE_AIR].v = 0.0f;
  atlas->coords[BLOCK_TYPE_AIR].u_size = 0.0f;
  atlas->coords[BLOCK_TYPE_AIR].v_size = 0.0f;

  current_atlas = atlas;
}

AtlasCoord texture_atlas_get_coord(TextureAtlas* atlas, BlockType type)
{
  AtlasCoord zero = {0.0f, 0.0f, 0.0f, 0.0f};
  
  if(!atlas || type >= MAX_BLOCK_TYPES)
    return zero;

  return atlas->coords[type];
}

BlockTextures texture_atlas_get_block_textures(TextureAtlas* atlas, BlockType type)
{
  BlockTextures zero = {{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};
  
  if(!atlas || type >= MAX_BLOCK_TYPES)
    return zero;

  return atlas->block_textures[type];
}

void texture_atlas_bind(TextureAtlas* atlas)
{
  if(!atlas)
    return;
  texture_bind(&atlas->texture);
}

void texture_atlas_cleanup(TextureAtlas* atlas)
{
  if(!atlas)
    return;
  texture_delete(&atlas->texture);
  current_atlas = NULL;
}
