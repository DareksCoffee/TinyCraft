#include <core/engine.h>
#include <graphics/shader.h>
#include <graphics/mesh.h>
#include <graphics/vertex.h>
#include <graphics/voxel.h>
#include <world/world.h>
#include <cglm/cglm.h>

static Window window;
static Shader basic_shader;
static double last_time;
static float total_time;

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
  
  if(shader_load(&basic_shader, 
                 ".tinycraft/shaders/basic.vsh", 
                 ".tinycraft/shaders/basic.fsh") != SHADER_OK)
  {
    printf("Failed to load shader\n");
    return ENGINE_FAIL;
  }

  if(world_init() != MESH_OK)
  {
    printf("Failed to initialize world\n");
    return ENGINE_FAIL;
  }

  glEnable(GL_DEPTH_TEST);
  last_time = glfwGetTime();
  total_time = 0.0f;
  engine_run();
  return ENGINE_OK;
}


void engine_run()
{
  mat4 projection, view, model;
  glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);
  glm_lookat((vec3){2.0f, 2.0f, 2.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, view);

  while(!window.should_close)
  {
    double current_time = glfwGetTime();
    float delta_time = (float)(current_time - last_time);
    last_time = current_time;
    total_time += delta_time;

    world_update(delta_time);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm_mat4_identity(model);
    glm_rotate(model, glm_rad(total_time * 30.0f), (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(model, glm_rad(total_time * 45.0f), (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(model, glm_rad(total_time * 60.0f), (vec3){0.0f, 0.0f, 1.0f});

    shader_use(&basic_shader);
    shader_set_mat4(&basic_shader, "model", model);
    shader_set_mat4(&basic_shader, "view", view);
    shader_set_mat4(&basic_shader, "projection", projection);

    world_render(NULL);

    win_update(&window);
  }

  shader_delete(&basic_shader);
  win_close(&window);
}

void engine_stop()
{
  // Nothing to do for now
}
