#pragma once
#include "cinder/CinderResources.h"

#define RES_TEAPOT_OBJ     CINDER_RESOURCE( ../resources/Objects/, teapot.obj, 128, DATA )

#define RES_BASIC_VERT     CINDER_RESOURCE( ../resources/Shaders/, basic_light.vert, 129, GLSL )
#define RES_BASIC_FRAG     CINDER_RESOURCE( ../resources/Shaders/, basic_light.frag, 130, GLSL )

#define RES_INK_VERT       CINDER_RESOURCE( ../resources/Shaders/, ink.vert, 131, GLSL )
#define RES_INK_FRAG       CINDER_RESOURCE( ../resources/Shaders/, ink.frag, 132, GLSL )

#define RES_PARTICLES_VERT CINDER_RESOURCE( ../resources/Shaders/, particles.vert, 133, GLSL )
#define RES_PARTICLES_FRAG CINDER_RESOURCE( ../resources/Shaders/, particles.frag, 134, GLSL )

#define RES_TEX_NOISE      CINDER_RESOURCE( ../resources/Textures/, noise.jpg, 135, JPG )
#define RES_TEX_DOT        CINDER_RESOURCE( ../resources/Textures/, dot.png, 136, PNG )
