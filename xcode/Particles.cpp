//
//  Particles.cpp
//  DrawRender
//
//  Created by Etienne on 2015-02-23.
//
//

#include "Particles.h"

void Particles::setup(int pCount, float mFboWidth, float mFboHeight)
{
    mShader =  gl::GlslProg::create( loadResource( RES_PARTICLES_VERT ), loadResource( RES_PARTICLES_FRAG ) );
    
    mTexture = gl::Texture(loadImage(loadResource(RES_TEX_DOT)));
    
    gl::VboMesh::Layout layout;
    layout.setStaticPositions();
    
    vector<ColorA> colors;
    vector<Vec2f> texcoords;
    vector<Vec3f> vertices;
    const ColorA white = ColorA(1.0f, 1.0f, 1.0f);
    int i=0;
    while(i < pCount){
        colors.push_back(white);
        texcoords.push_back( Vec2f( 10.0f, 10.0f ));
        vertices.push_back(Vec3f(Rand::randFloat(mFboWidth * .1f, mFboWidth * .9f), Rand::randFloat(mFboHeight * .1f, mFboHeight * .9f), .0f));
        i++;
    }
    
    mVbo = gl::VboMesh::create(pCount, 0, layout, GL_POINTS);
    mVbo->bufferPositions( &(vertices.front()), vertices.size() );
}

void Particles::update()
{/*
    gl::mVboMesh::VertexIter iter = mVbo->mapVertexBuffer();
    const Vec3f one(1.0f, 1.0f, 1.0f);
    const Vec3f normal(.0f, .0f, -1.0f);
    for( int i = 0; i < P_COUNT; ++i ) {
        iter.setPosition(Vec3f(
                               Rand::randFloat(w * .1f, w * .9f),
                               Rand::randFloat(h * .1f, h * .9f),
                               .0f));
        ++iter;
    }*/
}

void Particles::draw()
{
    mFbo.bindFramebuffer();
    
    mTexture.bind(0);
    
    gl::setMatricesWindow( mFbo.getSize() );
    gl::setViewport( mFbo.getBounds() );
    gl::clear(Color::black());
    
    gl::enableAdditiveBlending();
    enablePointSprites();
    
    gl::color(Color::white());
    gl::draw(mVbo);
    
    
    disablePointSprites();
    gl::disableAlphaBlending();
    
    mTexture.unbind();
    
    mFbo.unbindFramebuffer();
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
