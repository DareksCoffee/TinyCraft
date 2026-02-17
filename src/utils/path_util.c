#include <utils/path_util.h>
#include <stdio.h>
#include <limits.h>

#ifdef __linux__
  #include <unistd.h>
#elif __APPLE__
  #include <mach-o/dyld.h>
#elif _WIN32
  #include <windows.h>
#endif

char* path_get_executable_dir(char* buffer, size_t buffer_size)
{
  if(!buffer || buffer_size == 0)
    return NULL;

  char exe_path[PATH_MAX];
  ssize_t len = 0;

#ifdef __linux__
  len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
  if(len == -1)
    return NULL;
  exe_path[len] = '\0';

#elif __APPLE__
  uint32_t path_size = sizeof(exe_path);
  if(_NSGetExecutablePath(exe_path, &path_size) != 0)
    return NULL;

#elif _WIN32
  if(GetModuleFileNameA(NULL, exe_path, sizeof(exe_path)) == 0)
    return NULL;

#else
  return NULL;
#endif

  char* last_slash = strrchr(exe_path, '/');
#ifdef _WIN32
  char* last_backslash = strrchr(exe_path, '\\');
  if(last_backslash && (!last_slash || last_backslash > last_slash))
    last_slash = last_backslash;
#endif

  if(!last_slash)
    return NULL;

  size_t dir_len = last_slash - exe_path + 1;
  if(dir_len >= buffer_size)
    return NULL;

  strncpy(buffer, exe_path, dir_len);
  buffer[dir_len] = '\0';

  return buffer;
}

char* path_get_resource_path(char* buffer, size_t buffer_size, const char* relative_path)
{
  if(!buffer || buffer_size == 0 || !relative_path)
    return NULL;

  char exe_dir[PATH_MAX];
  if(!path_get_executable_dir(exe_dir, sizeof(exe_dir)))
    return NULL;

  size_t exe_dir_len = strlen(exe_dir);
  size_t relative_len = strlen(relative_path);

  // +1 for potential separator, +1 for null terminator
  if(exe_dir_len + relative_len + 1 >= buffer_size)
    return NULL;

  strcpy(buffer, exe_dir);

  // Add separator if needed
  if(exe_dir[exe_dir_len - 1] != '/' && exe_dir[exe_dir_len - 1] != '\\')
  {
    strcat(buffer, "/");
  }

  strcat(buffer, relative_path);

  return buffer;
}
