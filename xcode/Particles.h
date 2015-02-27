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
    
    Particles();
    void setup(int particlesCount_, float fboWidth, float fboHeight);
    void update();
    void draw();
    gl::Fbo * getFbo();
    
private:
    
    struct Particle
    {
        Vec2f velocity;
        Vec2f position;
        float size;
    };
    
    void enablePointSprites();
    void disablePointSprites();
    void updatePhysics();
    void renderFbo();
    void reset(Particle& p, const Area&  bounds);
    void resetFromOuterArea(Particle& p, const Area& bounds);
    
    int particlesCount;
    vector<Particle> particles;
    gl::VboMeshRef mVbo;
    unique_ptr<gl::Fbo> mFbo;
    gl::GlslProgRef mShader;
    gl::TextureRef mTexture;
};

#endif /* defined(__DrawRender__Particles__) */
