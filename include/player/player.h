#ifndef PLAYER_H
#define PLAYER_H

#include <player/aabb.h>
#include <graphics/camera.h>
#include <graphics/shader.h>
#include <cglm/cglm.h>

#define PLAYER_WIDTH  0.8f
#define PLAYER_HEIGHT 1.8f
#define PLAYER_DEPTH  1.2f
#define PLAYER_EYE_HEIGHT 1.62f  /* Distance from feet to eye level */
#define PLAYER_OK     1
#define PLAYER_FAIL  -1

#define GRAVITY       -25.0f
#define JUMP_FORCE    9.0f
#define MOVE_SPEED    12.0f

typedef struct {
    vec3 position;
    vec3 velocity;
    Camera camera;
    AABB aabb;
    int on_ground;
    float head_bob_time;
    float last_move_length;
    float bob_decay;
    float bob_amplitude;
} Player;

int player_init(Player* player, vec3 spawn_pos);
void player_update(Player* player, void* window_ptr, float delta_time);
void player_get_view_matrix(Player* player, mat4 view);
void player_render_arm(Player* player, Shader* shader, mat4 view, mat4 projection);

#endif
