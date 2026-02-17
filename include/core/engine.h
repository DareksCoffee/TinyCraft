#ifndef ENGINE_H
#define ENGINE_H

#define ENGINE_OK       0
#define ENGINE_FAIL    -1

#include <GL/glew.h>
#include <core/window.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int engine_init_components(void);
int engine_init(void); 
void engine_run(void);
void engine_stop(void);

#endif
