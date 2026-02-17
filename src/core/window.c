#include <core/window.h>


int win_init(Window* win, int width, int height, const char* title)
{
  win->window = glfwCreateWindow(width, height, title, NULL, NULL);
  if(!win->window)
  {
    printf("Failed to initialize GLFW...\n");
    return WIN_FAIL;
  }

  win->height = height;
  win->width  = width;
  
  glfwMakeContextCurrent(win->window);
  
  if(!glfwGetCurrentContext())
    return WIN_FAIL;
  return WIN_OK;
}

void win_update(Window* win)
{
  glfwSwapBuffers(win->window);
  glfwPollEvents();

  win->should_close = glfwWindowShouldClose(win->window);
}

