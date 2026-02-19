#ifndef WINDOW_H
#define WINDOW_H

#define WIN_OK        1
#define WIN_FAIL     -1

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>

typedef void (*WindowSizeCallback)(int width, int height);

typedef struct 
{
    GLFWwindow* window;
    int height;
    int width;
    bool should_close;
    bool mouse_captured;
    WindowSizeCallback on_resize;
    void* user_data;
} Window;

int win_init(Window* win, int width, int height, const char* title);
void win_update(Window* win);
void win_close(Window* win);
void win_set_resize_callback(Window* win, WindowSizeCallback callback);
void win_set_user_data(Window* win, void* data);
void win_capture_mouse(Window* win);
void win_release_mouse(Window* win);
void win_toggle_mouse_capture(Window* win);

#endif
