//
//  Particles.cpp
//  DrawRender
//
//  Created by Etienne on 2015-02-23.
//
//

#include "Particles.h"

Particles::Particles(){}

void Particles::setup(int count, float fboWidth, float fboHeight)
{
    
    mFbo = unique_ptr<gl::Fbo>(new gl::Fbo(fboWidth, fboHeight));
    
    mShader =  gl::GlslProg::create( loadResource( RES_PARTICLES_VERT ), loadResource( RES_PARTICLES_FRAG ) );
    
    mTexture = gl::Texture::create(loadImage(loadResource(RES_TEX_DOT)));
    
    gl::Texture::Format fmt;
    fmt.setWrap(GL_REPEAT, GL_REPEAT);
    noiseTexture = gl::Texture::create(loadImage(loadResource(RES_TEX_NOISE)), fmt);
    
    setNumParticles(count);
}

void Particles::update(gl::Fbo * sceneFbo)
{
    updatePhysics();
    renderFbo(sceneFbo);
}

int Particles::getNumParticles()
{
    return particles.size();
}

void Particles::setNumParticles(int count)
{
    particles.clear();
    colors.clear();
    if (mVbo != nullptr) mVbo->reset();
    
    gl::VboMesh::Layout layout;
    layout.setDynamicPositions();
    layout.setStaticColorsRGBA();
    mVbo = gl::VboMesh::create(count, 0, layout, GL_POINTS);
    
    for (int i = 0; i < count; ++i)
    {
        Particle p;
        reset(p, mFbo->getBounds());
        particles.push_back(p);
    }
    
    syncColor();
}

void Particles::syncColor()
{
    if (mVbo == nullptr) return;
    
    colors.clear();
    
    vector<ColorA> tones;
    tones.push_back(tone0);
    tones.push_back(tone1);
    tones.push_back(tone2);
    
    for (int i = 0, count = particles.size(); i < count; ++i)
    {
        colors.push_back(tones[i % tones.size()]);
    }
    mVbo->bufferColorsRGBA(colors);
}

void Particles::syncVelocity()
{
    for(auto it = particles.begin(); it != particles.end(); ++it) {
        it->velocity *= Rand::randFloat(minVelocity, maxVelocity) / it->velocity.length();
    };
}

void Particles::updatePhysics()
{
    // update particles & sync vertices on particles
    gl::VboMesh::VertexIter iter = mVbo->mapVertexBuffer();
    const int particlesCount = particles.size();
    for( int i = 0; i < particlesCount; ++i ) {
        particles[i].position += particles[i].velocity;
        if (!mFbo->getBounds().contains(particles[i].position))
        {
            resetFromOuterArea(particles[i], mFbo->getBounds());
        }
        iter.setPosition(particles[i].position.x, particles[i].position.y, .0f);
        ++iter;
    }
}

void Particles::renderFbo(gl::Fbo * sceneFbo)
{
    mFbo->bindFramebuffer();
    
    mTexture->bind(0);
    
    gl::setMatricesWindow( mFbo->getSize() );
    gl::setViewport( mFbo->getBounds() );
    gl::clear(Color::black());
    
    gl::enableAdditiveBlending();
    enablePointSprites();
    
    noiseTexture->bind(1);
    mShader->uniform( "perlin", 1);
    mShader->uniform( "pointSizeMul", pointSizeMul);
    mShader->uniform( "pointSizeVariation", pointSizeVariation);
    sceneFbo->getTexture(0).bind(2);
    mShader->uniform( "scene", 2 );
    
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

void Particles::reset(Particle& p, const Area& bounds)
{
    p.position.set(Rand::randFloat(bounds.getX1(), bounds.getX2()), Rand::randFloat( bounds.getY1(), bounds.getY2()));
    const float angle = Rand::randFloat(.0f, M_PI_2);
    const float radius = Rand::randFloat(minVelocity, maxVelocity);
    p.velocity.set(radius * cos(angle), radius * sin(angle));
}

void Particles::resetFromOuterArea(Particle& p, const Area& bounds)
{
    // random point on a rectangle border
    int side = Rand::randInt(0, 4); // upper limit not included
    Vec2f a, b; // select a segment [a, b] clockwise
    float sideNormalAngle;
    switch (side) {
        case 0:
            a.set(bounds.getX1(), bounds.getY1());
            b.set(bounds.getX2(), bounds.getY1());
            sideNormalAngle = M_PI * .5f;
            break;
            
        case 1:
            a.set(bounds.getX2(), bounds.getY1());
            b.set(bounds.getX2(), bounds.getY2());
            sideNormalAngle = M_PI;
            break;
            
        case 2:
            a.set(bounds.getX1(), bounds.getY2());
            b.set(bounds.getX2(), bounds.getY2());
            sideNormalAngle = M_PI * -.5f;
            break;
            
        case 3:
            a.set(bounds.getX1(), bounds.getY1());
            b.set(bounds.getX1(), bounds.getY2());
            sideNormalAngle = .0f;
            break;
    }
    
    p.position.set(a + (b - a) * Rand::randFloat(.0f, 1.0f));
    const float angle = sideNormalAngle + Rand::randFloat(M_PI * -.05f, M_PI * .05f);
    const float radius = Rand::randFloat(minVelocity, maxVelocity);
    p.velocity.set(radius * cos(angle), radius * sin(angle));
}
