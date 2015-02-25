//
//  Particles.cpp
//  DrawRender
//
//  Created by Etienne on 2015-02-23.
//
//

#include "Particles.h"

Particles::Particles(){}

void Particles::setup(int particlesCount_, float fboWidth, float fboHeight)
{
    particlesCount = particlesCount_;
    
    mFbo = unique_ptr<gl::Fbo>(new gl::Fbo(fboWidth, fboHeight));
    
    mShader =  gl::GlslProg::create( loadResource( RES_PARTICLES_VERT ), loadResource( RES_PARTICLES_FRAG ) );
    
    mTexture = gl::Texture::create(loadImage(loadResource(RES_TEX_DOT)));
    
    gl::VboMesh::Layout layout;
    layout.setDynamicPositions();
    layout.setDynamicNormals();
    
    mVbo = gl::VboMesh::create(particlesCount, 0, layout, GL_POINTS);
    //mVbo->bufferPositions( &(vertices.front()), vertices.size() );
}

void Particles::update()
{
    updatePhysics();
    renderFbo();
}

void Particles::updatePhysics()
{
    gl::VboMesh::VertexIter iter = mVbo->mapVertexBuffer();
    const float w = (float)mFbo->getWidth();
    const float h = (float)mFbo->getHeight();
    for( int i = 0; i < particlesCount; ++i ) {
        iter.setPosition(Vec3f(
                               Rand::randFloat(w * .1f, w * .9f),
                               Rand::randFloat(h * .1f, h * .9f),
                               .0f));
        iter.setNormal(Vec3f(Rand::randFloat(1.0f, 20.0f), .0f, .0f));
        ++iter;
    }
}

void Particles::renderFbo()
{
    mFbo->bindFramebuffer();
    
    mTexture->bind(0);
    
    gl::setMatricesWindow( mFbo->getSize() );
    gl::setViewport( mFbo->getBounds() );
    gl::clear(Color::black());
    
    gl::enableAdditiveBlending();
    enablePointSprites();
    
    gl::color(Color::white());
    gl::draw(mVbo);
    
    
    disablePointSprites();
    gl::disableAlphaBlending();
    
    mTexture->unbind();
    
    mFbo->unbindFramebuffer();
}

void Particles::enablePointSprites()
{
    // store current OpenGL state
    glPushAttrib( GL_POINT_BIT | GL_ENABLE_BIT );
    
    // enable point sprites and initialize it
    gl::enable( GL_POINT_SPRITE_ARB );
    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 0.1f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, 200.0f );
    
    // allow vertex mShader to change point size
    gl::enable( GL_VERTEX_PROGRAM_POINT_SIZE );
    
    // bind mShader
    mShader->bind();
    mShader->uniform("particleTex", 0);
}

void Particles::disablePointSprites()
{
    // unbind mShader
    mShader->unbind();
    
    // restore OpenGL state
    glPopAttrib();
}

gl::Fbo * Particles::getFbo()
{
    return mFbo.get();
}
