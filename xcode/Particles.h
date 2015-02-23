//
//  Particles.h
//  DrawRender
//
//  Created by Etienne on 2015-02-23.
//
//

#ifndef __DrawRender__Particles__
#define __DrawRender__Particles__

#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/gl/Fbo.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Particles {

public:
    
    void setup(int pCount, float fboWidth, float fboHeight);
    void update();
    void draw();
    
    void enablePointSprites();
    void disablePointSprites();
    
    gl::VboMeshRef mVbo;
    gl::Fbo mFbo;
    gl::GlslProgRef mShader;
    gl::Texture mTexture;
    
    
};

#endif /* defined(__DrawRender__Particles__) */
