#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

/* Forward declaration to avoid GL header include issues */
typedef struct GLFWwindow GLFWwindow;

#define INPUT_OK        1
#define INPUT_FAIL     -1

int input_is_key_pressed(GLFWwindow* window, int key);

#endif
