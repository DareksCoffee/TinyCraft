#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <stdbool.h>

#define INPUT_OK        1
#define INPUT_FAIL     -1

int input_is_key_pressed(GLFWwindow* window, int key);

#endif
