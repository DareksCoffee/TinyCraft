#include <player/player.h>
#include <world/world.h>
#include <core/input.h>
#include <core/window.h>
#include <GLFW/glfw3.h>
#include <math.h>

int player_init(Player* player, vec3 spawn_pos)
{
    if (!player)
        return PLAYER_FAIL;
    
    glm_vec3_copy(spawn_pos, player->position);
    
    player->velocity[0] = 0.0f;
    player->velocity[1] = 0.0f;
    player->velocity[2] = 0.0f;
    player->on_ground = 0;
    
    if (camera_init(&player->camera, player->position[0], player->position[1] + PLAYER_EYE_HEIGHT, player->position[2]) != CAMERA_OK)
        return PLAYER_FAIL;
    
    player->aabb = aabb_create(player->position, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_DEPTH);
    
    return PLAYER_OK;
}

void player_update(Player* player, void* window_ptr, float delta_time)
{
    if (!player || !window_ptr)
        return;
    
    Window* window = (Window*)window_ptr;
    GLFWwindow* glfw_window = window->window;
    
    vec3 move = {0.0f, 0.0f, 0.0f};
    
    if (input_is_key_pressed(glfw_window, GLFW_KEY_W)) {
        move[0] += player->camera.front[0];
        move[2] += player->camera.front[2];
    }
    if (input_is_key_pressed(glfw_window, GLFW_KEY_S)) {
        move[0] -= player->camera.front[0];
        move[2] -= player->camera.front[2];
    }
    if (input_is_key_pressed(glfw_window, GLFW_KEY_A)) {
        vec3 right;
        glm_cross(player->camera.front, player->camera.up, right);
        glm_normalize(right);
        move[0] -= right[0];
        move[2] -= right[2];
    }
    if (input_is_key_pressed(glfw_window, GLFW_KEY_D)) {
        vec3 right;
        glm_cross(player->camera.front, player->camera.up, right);
        glm_normalize(right);
        move[0] += right[0];
        move[2] += right[2];
    }
    
    if (move[0] != 0.0f || move[2] != 0.0f) {
        glm_normalize(move);
    }
    float move_speed = MOVE_SPEED * delta_time;
    
    player->position[0] += move[0] * move_speed;
    aabb_update(&player->aabb, player->position);
    if (world_check_collision(&player->aabb)) {
        player->position[0] -= move[0] * move_speed;
        aabb_update(&player->aabb, player->position);
    }
    
    player->position[2] += move[2] * move_speed;
    aabb_update(&player->aabb, player->position);
    if (world_check_collision(&player->aabb)) {
        player->position[2] -= move[2] * move_speed;
        aabb_update(&player->aabb, player->position);
    }
    
    player->velocity[1] += GRAVITY * delta_time;
    
    if (input_is_key_pressed(glfw_window, GLFW_KEY_SPACE) && player->on_ground) {
        player->velocity[1] = JUMP_FORCE;
        player->on_ground = 0;
    }
    
    player->position[1] += player->velocity[1] * delta_time;
    
    aabb_update(&player->aabb, player->position);
    
    if (world_check_collision(&player->aabb)) {
        player->position[1] -= player->velocity[1] * delta_time;
        aabb_update(&player->aabb, player->position);
        
        if (player->velocity[1] < 0.0f) {
            player->on_ground = 1;
        }
        player->velocity[1] = 0.0f;
    } else {
        player->on_ground = 0;
    }
    
    player->camera.position[0] = player->position[0];
    player->camera.position[1] = player->position[1] + PLAYER_EYE_HEIGHT;
    player->camera.position[2] = player->position[2];
}

void player_get_view_matrix(Player* player, mat4 view)
{
    if (!player)
        return;
    
    camera_get_view_matrix(&player->camera, view);
}
