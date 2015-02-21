#pragma once
#include "cinder/CinderResources.h"

#define RES_TEAPOT_OBJ  CINDER_RESOURCE( ../resources/, teapot.obj, 128, DATA )

#define RES_VERT        CINDER_RESOURCE( ../resources/, vert.glsl, 129, GLSL )
#define RES_FRAG        CINDER_RESOURCE( ../resources/, frag.glsl, 130, GLSL )

#define RES_POST_VERT        CINDER_RESOURCE( ../resources/, post_vert.glsl, 131, GLSL )
#define RES_POST_FRAG        CINDER_RESOURCE( ../resources/, post_frag.glsl, 132, GLSL )

#define RES_NORMAL_VERT        CINDER_RESOURCE( ../resources/, vert_normal.glsl, 133, GLSL )

#define RES_NORMAL_FRAG        CINDER_RESOURCE( ../resources/, frag_normal.glsl, 134, GLSL )

#define RES_TEX_HATCH        CINDER_RESOURCE( ../resources/, hatch_256x256.png, 135, PNG )
