//
//  Ink.cpp
//  DrawRender
//
//  Created by Etienne on 2015-02-23.
//
//

#include "Ink.h"

void Ink::setup(float fboWidth, float fboHeight)
{
    pingPongFlag = true;
    
    mShader =  gl::GlslProg::create( loadResource( RES_POST_VERT ), loadResource( RES_POST_FRAG) );
    
    gl::Texture::Format fmt;
    fmt.setWrap(GL_REPEAT, GL_REPEAT);
    noiseTexture = gl::Texture(loadImage(loadResource(RES_TEX_NOISE)), fmt);
    
    pingFbo = gl::Fbo( fboWidth, fboHeight );
    pongFbo = gl::Fbo( fboWidth, fboHeight );
    
    // make sure both fbos are cleared
    // TODO: necessary, best method?
    pingFbo.bindFramebuffer();
    gl::clear();
    pingFbo.unbindFramebuffer();
    
    pongFbo.bindFramebuffer();
    gl::clear();
    pongFbo.unbindFramebuffer();
}

void Ink::update()
{

}

void Ink::draw(gl::Fbo * sceneFbo, gl::Fbo * particlesFbo)
{
    gl::Fbo * prevFrame = pingPongFlag ? &pingFbo : &pongFbo;
    gl::Fbo * currentFrame = pingPongFlag ? &pongFbo : &pingFbo;
    
    currentFrame->bindFramebuffer();
    
    gl::setMatricesWindow( sceneFbo->getSize() );
    gl::setViewport( sceneFbo->getBounds() );
    gl::clear();
    
    mShader->bind();
    // bind textures
    prevFrame->getTexture(0).bind(1);
    mShader->uniform( "previousFrame", 1 );
    noiseTexture.bind(2);
    mShader->uniform( "noise", 2 );
    particlesFbo->getTexture().bind(3);
    mShader->uniform( "pen", 3 );
    mShader->uniform( "time", (float)getElapsedSeconds() * 1000.0f );
    
    gl::draw( sceneFbo->getTexture(0), sceneFbo->getBounds() );
    
    mShader->unbind();
    
    currentFrame->unbindFramebuffer();
    
    pingPongFlag = !pingPongFlag; // toggle

}