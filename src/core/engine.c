#include <core/engine.h>


static Window window;
int engine_init_components()
{
  if(!glfwInit())
  {
    printf("Failed to initialize GLFW...\n");
    return ENGINE_FAIL;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  #ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  if(win_init(&window, 800, 600, "TinyCraft") != WIN_OK)
    return ENGINE_FAIL;
  
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if(err != GLEW_OK && err != GLEW_ERROR_NO_GLX_DISPLAY)
  {
    printf("GLEW error: %s\n", glewGetErrorString(err));
    return ENGINE_FAIL;
  }
  glfwSwapInterval(1);
  return ENGINE_OK;
}

int engine_init()
{
  if(engine_init_components() != ENGINE_OK)
    return ENGINE_FAIL;
  
  engine_run();
  return ENGINE_OK;
}


void engine_run()
{
  int frame = 0;
  while(!window.should_close)
  {
    frame++;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    win_update(&window);
  }
}

void engine_stop()
{
  // Nothing to do for now
}
