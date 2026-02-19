#ifndef PLAYER_H
#define PLAYER_H

#include <player/aabb.h>
#include <graphics/camera.h>
#include <cglm/cglm.h>

#define PLAYER_WIDTH  0.8f
#define PLAYER_HEIGHT 1.8f
#define PLAYER_DEPTH  0.8f
#define PLAYER_EYE_HEIGHT 1.62f  /* Distance from feet to eye level */
#define PLAYER_OK     1
#define PLAYER_FAIL  -1

#define GRAVITY       -25.0f
#define JUMP_FORCE    11.0f
#define MOVE_SPEED    5.0f

typedef struct {
    vec3 position;
    vec3 velocity;
    Camera camera;
    AABB aabb;
    int on_ground;
} Player;

int player_init(Player* player, vec3 spawn_pos);
void player_update(Player* player, void* window, float delta_time);
void player_get_view_matrix(Player* player, mat4 view);

#endif
