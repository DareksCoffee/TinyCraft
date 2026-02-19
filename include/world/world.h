#ifndef WORLD_H
#define WORLD_H

#include <graphics/voxel.h>
#include <graphics/shader.h>
#include <player/player.h>
#include <cglm/cglm.h>

typedef struct GLFWwindow GLFWwindow;

/* Render distance in chunks */
#define RENDER_DISTANCE 8
#define MAX_CHUNKS 128

int world_init(Player* player);
void world_update(float delta_time);
void world_render(Shader* shader, mat4 view, mat4 projection);
void world_cleanup(void);
void world_mouse_callback(GLFWwindow* window, double xpos, double ypos);
int world_get_block_at(int x, int y, int z);
int world_check_collision(AABB* aabb);

/* New functions for infinite world generation */
void world_update_chunks(Player* player);
void world_unload_far_chunks(int player_chunk_x, int player_chunk_z);
void world_load_new_chunks(int player_chunk_x, int player_chunk_z);

#endif
