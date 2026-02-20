#ifndef ARM_H
#define ARM_H

#include <graphics/mesh.h>
#include <graphics/texture.h>

int arm_init(void);
void arm_render(void);
void arm_cleanup(void);
Texture* arm_get_texture(void);

#endif
