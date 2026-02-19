#include <graphics/camera.h>
#include <core/input.h>
#include <GLFW/glfw3.h>
#include <math.h>

int camera_init(Camera* camera, float x, float y, float z)
{
  if (!camera)
    return CAMERA_FAIL;

  glm_vec3_copy((vec3){x, y, z}, camera->position);
  glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera->front);
  glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->up);

  camera->yaw = -90.0f;
  camera->pitch = 0.0f;
  camera->speed = 20.0f;
  camera->sensitivity = 0.1f;
  camera->last_x = 400.0f;
  camera->last_y = 300.0f;
  camera->first_mouse = 1;

  return CAMERA_OK;
}

void camera_update_direction(Camera* camera)
{
  vec3 front;

  front[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
  front[1] = sinf(glm_rad(camera->pitch));
  front[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));

  glm_normalize(front);
  glm_vec3_copy(front, camera->front);
}

void camera_update(Camera* camera, GLFWwindow* window, float delta_time)
{
  if (!camera || !window)
    return;

  float velocity = camera->speed * delta_time;

  vec3 move = {0.0f, 0.0f, 0.0f};

  /* W key - move forward */
  if (input_is_key_pressed(window, GLFW_KEY_W))
  {
    move[0] += camera->front[0];
    move[1] += camera->front[1];
    move[2] += camera->front[2];
  }

  /* S key - move backward */
  if (input_is_key_pressed(window, GLFW_KEY_S))
  {
    move[0] -= camera->front[0];
    move[1] -= camera->front[1];
    move[2] -= camera->front[2];
  }

  /* A key - move left */
  if (input_is_key_pressed(window, GLFW_KEY_A))
  {
    vec3 right;
    glm_cross(camera->front, camera->up, right);
    glm_normalize(right);
    move[0] -= right[0];
    move[1] -= right[1];
    move[2] -= right[2];
  }

  /* D key - move right */
  if (input_is_key_pressed(window, GLFW_KEY_D))
  {
    vec3 right;
    glm_cross(camera->front, camera->up, right);
    glm_normalize(right);
    move[0] += right[0];
    move[1] += right[1];
    move[2] += right[2];
  }

  /* Space - move up */
  if (input_is_key_pressed(window, GLFW_KEY_SPACE))
  {
    move[1] += 1.0f;
  }

  /* Shift - move down */
  if (input_is_key_pressed(window, GLFW_KEY_LEFT_SHIFT))
  {
    move[1] -= 1.0f;
  }

  camera->position[0] += move[0] * velocity;
  camera->position[1] += move[1] * velocity;
  camera->position[2] += move[2] * velocity;
}

void camera_get_view_matrix(Camera* camera, mat4 view)
{
  if (!camera)
    return;

  vec3 target;
  glm_vec3_add(camera->position, camera->front, target);
  glm_lookat(camera->position, target, camera->up, view);
}

void camera_mouse_callback(Camera* camera, double xpos, double ypos)
{
  if (!camera)
    return;

  if (camera->first_mouse)
  {
    camera->last_x = xpos;
    camera->last_y = ypos;
    camera->first_mouse = 0;
    return;
  }

  float xoffset = xpos - camera->last_x;
  float yoffset = camera->last_y - ypos;

  camera->last_x = xpos;
  camera->last_y = ypos;

  xoffset *= camera->sensitivity;
  yoffset *= camera->sensitivity;

  camera->yaw += xoffset;
  camera->pitch += yoffset;

  if (camera->pitch > 89.0f)
    camera->pitch = 89.0f;
  if (camera->pitch < -89.0f)
    camera->pitch = -89.0f;

  camera_update_direction(camera);
}
