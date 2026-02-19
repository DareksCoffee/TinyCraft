#ifndef WORLD_H
#define WORLD_H

#include <graphics/voxel.h>
#include <graphics/shader.h>
#include <player/player.h>
#include <cglm/cglm.h>

typedef struct GLFWwindow GLFWwindow;

int world_init(Player* player);
void world_update(float delta_time);
void world_render(Shader* shader, mat4 view, mat4 projection);
void world_cleanup(void);
void world_mouse_callback(GLFWwindow* window, double xpos, double ypos);
int world_get_block_at(int x, int y, int z);
int world_check_collision(AABB* aabb);

#endif
