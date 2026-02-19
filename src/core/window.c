#include <core/window.h>

static void window_size_callback(GLFWwindow* glfw_window, int width, int height)
{
  Window* win = (Window*)glfwGetWindowUserPointer(glfw_window);
  if (win)
  {
    win->width = width;
    win->height = height;
    glViewport(0, 0, width, height);
    if (win->on_resize)
      win->on_resize(width, height);
  }
}

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
  win->on_resize = NULL;
  win->user_data = NULL;
  win->mouse_captured = false;
  
  glfwMakeContextCurrent(win->window);
  
  if(!glfwGetCurrentContext())
    return WIN_FAIL;
  
  glfwSetWindowUserPointer(win->window, win);
  glfwSetFramebufferSizeCallback(win->window, window_size_callback);
  glViewport(0, 0, width, height);
  
  return WIN_OK;
}

void win_update(Window* win)
{
  glfwSwapBuffers(win->window);
  glfwPollEvents();

  win->should_close = glfwWindowShouldClose(win->window);
}

void win_set_resize_callback(Window* win, WindowSizeCallback callback)
{
  if (win)
    win->on_resize = callback;
}

void win_set_user_data(Window* win, void* data)
{
  if (win)
    win->user_data = data;
}

void win_capture_mouse(Window* win)
{
  if (win && !win->mouse_captured)
  {
    win->mouse_captured = true;
    glfwSetInputMode(win->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
      glfwSetInputMode(win->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }
}

void win_release_mouse(Window* win)
{
  if (win && win->mouse_captured)
  {
    win->mouse_captured = false;
    glfwSetInputMode(win->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwRawMouseMotionSupported())
      glfwSetInputMode(win->window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
  }
}

void win_toggle_mouse_capture(Window* win)
{
  if (win)
  {
    if (win->mouse_captured)
      win_release_mouse(win);
    else
      win_capture_mouse(win);
  }
}

void win_close(Window* win)
{
  if(win && win->window)
  {
    glfwDestroyWindow(win->window);
    glfwTerminate();
  }
}
