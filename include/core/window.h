#ifndef WINDOW_H
#define WINDOW_H

#define WIN_OK        1
#define WIN_FAIL     -1

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct 
{
    GLFWwindow* window;
    int height;
    int width;
    bool should_close;
} Window;

int win_init(Window* win, int width, int height, const char* title);
void win_update(Window* win);
void win_close(Window* win);

#endif
