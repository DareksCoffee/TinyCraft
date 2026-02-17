#ifndef PATH_UTIL_H
#define PATH_UTIL_H

#include <stdlib.h>
#include <string.h>

char* path_get_executable_dir(char* buffer, size_t buffer_size);
char* path_get_resource_path(char* buffer, size_t buffer_size, const char* relative_path);

#endif
