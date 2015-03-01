//
//  Ink.cpp
//  DrawRender
//
//  Created by Etienne on 2015-02-23.
//
//

#include "Ink.h"

Ink::Ink(){}

void Ink::setup(float fboWidth, float fboHeight)
{
    pingPongFlag = true;
    
    mShader =  gl::GlslProg::create( loadResource( RES_INK_VERT ), loadResource( RES_INK_FRAG) );
    
    gl::Texture::Format fmt;
    fmt.setWrap(GL_REPEAT, GL_REPEAT);
    noiseTexture = gl::Texture::create(loadImage(loadResource(RES_TEX_NOISE)), fmt);
    
    pingFbo = unique_ptr<gl::Fbo>(new gl::Fbo(fboWidth, fboHeight));
    pongFbo = unique_ptr<gl::Fbo>(new gl::Fbo(fboWidth, fboHeight));
    
    // make sure both fbos are cleared
    // TODO: necessary, best method?
    pingFbo->bindFramebuffer();
    gl::clear();
    pingFbo->unbindFramebuffer();
    
    pongFbo->bindFramebuffer();
    gl::clear();
    pongFbo->unbindFramebuffer();
}

void Ink::update(gl::Fbo * sceneFbo, gl::Fbo * particlesFbo)
{
    auto prevFrame = pingPongFlag ? pingFbo.get() : pongFbo.get();
    auto currentFrame = pingPongFlag ? pongFbo.get() : pingFbo.get();
    
    currentFrame->bindFramebuffer();
    
    gl::setMatricesWindow( sceneFbo->getSize() );
    gl::setViewport( sceneFbo->getBounds() );
    gl::clear();
    
    mShader->bind();
    // bind textures
    prevFrame->getTexture(0).bind(1);
    mShader->uniform( "previousFrame", 1 );
    noiseTexture->bind(2);
    mShader->uniform( "noise", 2 );
    particlesFbo->getTexture().bind(3);
    mShader->uniform( "pen", 3 );
    mShader->uniform( "time", (float)getElapsedSeconds() * 1000.0f );
    mShader->uniform( "persistence", persistence );
    mShader->uniform( "threshold", threshold );
    mShader->uniform( "maxRate", maxRate );
    
    
    gl::draw( sceneFbo->getTexture(0), sceneFbo->getBounds() );
    
    mShader->unbind();
    
    currentFrame->unbindFramebuffer();
    
    pingPongFlag = !pingPongFlag; // toggle

}

gl::Fbo * Ink::getFbo()
{
    return pingPongFlag ? pongFbo.get() : pingFbo.get();
}