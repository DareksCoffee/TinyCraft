#include <player/player.h>
#include <player/arm.h>
#include <world/world.h>
#include <core/input.h>
#include <core/window.h>
#include <graphics/texture_registry.h>
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
    player->head_bob_time = 0.0f;
    player->bob_amplitude = 0.0f;
    
    if (camera_init(&player->camera, player->position[0], player->position[1] + PLAYER_EYE_HEIGHT, player->position[2]) != CAMERA_OK)
        return PLAYER_FAIL;
    
    player->aabb = aabb_create(player->position, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_DEPTH);

    arm_init();
    
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
    
    int is_moving = 0;
    if (move[0] != 0.0f || move[2] != 0.0f) {
        glm_normalize(move);
        is_moving = 1;
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

    float target_amplitude = 0.0f;
    float bob_speed = 0.0f;
    if (is_moving) {
        if (player->on_ground) {
            target_amplitude = 1.0f;
            bob_speed = 1.8f;
        } else {
            target_amplitude = 0.35f;
            bob_speed = 0.9f;
        }
    }

    float lerp_t = delta_time * 6.0f;
    if (lerp_t > 1.0f) lerp_t = 1.0f;
    player->bob_amplitude += (target_amplitude - player->bob_amplitude) * lerp_t;

    if (is_moving || player->bob_amplitude > 0.01f) {
        player->head_bob_time += delta_time * (player->on_ground ? 1.8f : 0.9f);
    }

    float bob_vert = -fabsf(sinf(player->head_bob_time * GLM_PI)) * 0.06f * player->bob_amplitude;
    float bob_side =  sinf(player->head_bob_time * GLM_PI * 0.5f) * 0.015f * player->bob_amplitude;

    player->camera.position[0] = player->position[0] + bob_side;
    player->camera.position[1] = player->position[1] + PLAYER_EYE_HEIGHT + bob_vert;
    player->camera.position[2] = player->position[2];
}

void player_get_view_matrix(Player* player, mat4 view)
{
    if (!player)
        return;
    
    camera_get_view_matrix(&player->camera, view);
}

void player_render_arm(Player* player, Shader* shader, mat4 view, mat4 projection)
{
    if (!player || !shader)
        return;

    const float default_forward = 0.8f;
    const float min_forward = 0.5f;
    const float max_reach = 1.5f;
    const float step = 0.05f;

    float forward = default_forward;
    for (float t = step; t <= max_reach; t += step) {
        vec3 sample;
        vec3 tmp;
        glm_vec3_scale(player->camera.front, t, tmp);
        glm_vec3_add(player->camera.position, tmp, sample);
        int bx = (int)floorf(sample[0]);
        int by = (int)floorf(sample[1]);
        int bz = (int)floorf(sample[2]);
        if (world_get_block_at(bx, by, bz) != BLOCK_TYPE_AIR) {
            forward = t - 0.18f;
            if (forward < min_forward)
                forward = min_forward;
            break;
        }
    }
    (void)forward;

    float t = player->head_bob_time;
    float w = player->bob_amplitude;

    float arm_bob_y  = -fabsf(sinf(t * GLM_PI))  * 0.06f * w;
    float arm_bob_x  =  sinf(t * GLM_PI * 0.5f)  * 0.04f * w;
    float arm_tilt_z =  sinf(t * GLM_PI * 0.5f)  * 2.5f  * w;
    float arm_tilt_x = -fabsf(sinf(t * GLM_PI))  * 3.0f  * w;

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){0.5f + arm_bob_x, -0.6f + arm_bob_y, -1.3f});
    glm_rotate(model, glm_rad(-15.0f), (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(model, glm_rad(20.0f + arm_tilt_x), (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(model, glm_rad(-12.0f + arm_tilt_z), (vec3){0.0f, 0.0f, 1.0f});
    glm_scale(model, (vec3){0.35f, 0.35f, 1.0f});
    
    shader_use(shader);
    shader_set_mat4(shader, "model", model);
    mat4 identity;
    glm_mat4_identity(identity);
    shader_set_mat4(shader, "view", identity);
    shader_set_mat4(shader, "projection", projection);
    shader_set_int(shader, "texture_sampler", 0);

    Texture* tex = arm_get_texture();
    if (!tex || !tex->id)
        tex = texture_registry_get(BLOCK_TYPE_GRASS);
    if (tex)
        texture_bind(tex);

    arm_render();
}