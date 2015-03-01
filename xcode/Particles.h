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
#include "cinder/Perlin.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Particle
{
    Vec2f velocity;
    Vec2f position;
};

class Particles {

public:
    
    Particles();
    void setup(int count, float fboWidth, float fboHeight);
    void update(gl::Fbo * sceneFbo);
    void syncVelocity();
    void setNumParticles(int count);
    int getNumParticles();
    void draw();
    gl::Fbo * getFbo();
    float pointSizeMul;
    float pointSizeVariation;
    float minVelocity;
    float maxVelocity;
    
private:
    
    void enablePointSprites();
    void disablePointSprites();
    void updatePhysics();
    void renderFbo(gl::Fbo * sceneFbo);
    void reset(Particle& p, const Area&  bounds);
    void resetFromOuterArea(Particle& p, const Area& bounds);
    
    vector<Particle> particles;
    vector<Color> colors;
    gl::VboMeshRef mVbo;
    unique_ptr<gl::Fbo> mFbo;
    gl::GlslProgRef mShader;
    gl::TextureRef mTexture;
    gl::TextureRef noiseTexture;
    Perlin mPerlin;
};

#endif /* defined(__DrawRender__Particles__) */
