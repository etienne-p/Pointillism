//
//  Ink.h
//  DrawRender
//
//  Created by Etienne on 2015-02-23.
//
//

#ifndef __DrawRender__Ink__
#define __DrawRender__Ink__

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

class Ink {
    
public:
    Ink();
    void setup(float fboWidth, float fboHeight);
    void update(gl::Fbo * sceneFbo, gl::Fbo * particlesFbo);
    gl::Fbo * getFbo();
    
private:
    unique_ptr<gl::Fbo> pingFbo;
    unique_ptr<gl::Fbo> pongFbo;
    gl::GlslProgRef mShader;
    gl::TextureRef noiseTexture;
    bool pingPongFlag;
    
};
#endif /* defined(__DrawRender__Ink__) */
