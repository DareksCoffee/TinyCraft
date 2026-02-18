#include <core/input.h>

int input_is_key_pressed(GLFWwindow* window, int key)
{
  if (!window)
    return INPUT_FAIL;
  
  return glfwGetKey(window, key) == GLFW_PRESS;
}
