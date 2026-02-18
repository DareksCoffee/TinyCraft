#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

#define CAMERA_OK        1
#define CAMERA_FAIL     -1

typedef struct
{
    vec3 position;
    vec3 front;
    vec3 up;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float last_x;
    float last_y;
    int first_mouse;
} Camera;

int camera_init(Camera* camera, float x, float y, float z);
void camera_update(Camera* camera, GLFWwindow* window, float delta_time);
void camera_get_view_matrix(Camera* camera, mat4 view);
void camera_mouse_callback(Camera* camera, double xpos, double ypos);

#endif
