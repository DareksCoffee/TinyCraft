#include <core/engine.h>
#include <stdio.h>

int main()
{
  if(engine_init() == ENGINE_FAIL)
  {
    printf("Failed to initialize engine...\n");
    return -1;
  }
  return 0;
}
