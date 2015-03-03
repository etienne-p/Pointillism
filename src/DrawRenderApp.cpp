#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ObjLoader.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/params/Params.h"
#include "cinder/gl/Fbo.h"

#include "Resources.h"

// Custom
#include "Ink.h"
#include "Particles.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DrawRenderApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void mouseMove( MouseEvent event );
	void update();
	void draw();
    void resize(); //TODO: remove, is AppBAsic the best choice?
    void renderToFBO();
    
    // helpers
    void drawModel(const Matrix44f& modelMatrix);
    
    params::InterfaceGlRef mParams;
    gl::GlslProgRef mShader; // shader used to render to fbo
    Vec2f mMousePos;
    
    gl::VboMeshRef mTeapotVBO;
    TriMesh mTeapotMesh;
    
    unique_ptr<gl::Fbo> sceneFbo;
    
    unique_ptr<Ink> ink;
    
    unique_ptr<Particles> particles;
    
    static const int FBO_WIDTH = 1024, FBO_HEIGHT = 1024;
};

void DrawRenderApp::setup()
{
    ObjLoader loader( (DataSourceRef)loadResource( RES_TEAPOT_OBJ ) );
    loader.load( &mTeapotMesh );
    mTeapotVBO = gl::VboMesh::create( mTeapotMesh);
    
    mShader =  gl::GlslProg::create( loadResource( RES_BASIC_VERT ), loadResource( RES_BASIC_FRAG) );
    
    sceneFbo = unique_ptr<gl::Fbo>(new gl::Fbo( FBO_WIDTH, FBO_HEIGHT ));
    
    ink = unique_ptr<Ink>(new Ink());
    ink->setup(FBO_WIDTH, FBO_HEIGHT);
    
    particles = unique_ptr<Particles>(new Particles());
    particles->setup(4000, FBO_WIDTH, FBO_HEIGHT);
    
    mParams = params::InterfaceGl::create( "Params", Vec2i( 225, 200 ) );
    
    mParams->addParam( "Particle Size", &(particles->pointSizeMul)).min(.0f).max(20.0f);
    mParams->addParam( "Particle Size Variation", &(particles->pointSizeVariation)).min(.0f).max(1.0f);
    
	std::function<void( int )> setterNumParticles = std::bind( &Particles::setNumParticles, particles.get(), std::placeholders::_1 );
	std::function<int ()> getterNumParticles	  = std::bind( &Particles::getNumParticles, particles.get() );
	mParams->addParam( "Particles Number", setterNumParticles, getterNumParticles );
    
    mParams->addParam( "Particle Min Velocity", &(particles->minVelocity)).updateFn( [this] { particles->syncVelocity(); } );
    mParams->addParam( "Particle Max Velocity", &(particles->maxVelocity)).updateFn( [this] { particles->syncVelocity(); } );
    
    mParams->addParam( "Particle Tone 0", &(particles->tone0)).updateFn( [this] { particles->syncColor(); } );
    mParams->addParam(  "Particle Tone 1", &(particles->tone1)).updateFn( [this] { particles->syncColor(); } );
    mParams->addParam("Particle Tone 2", &(particles->tone2)).updateFn( [this] { particles->syncColor(); } );
    
    mParams->addParam( "Ink Persistence", &(ink->persistence)).min(.9f).max(1.0f);
    mParams->addParam( "Ink Threshold", &(ink->threshold)).min(.0f).max(1.0f);
    mParams->addParam( "Ink Max Rate", &(ink->maxRate)).min(.0f).max(.25f);
}


void DrawRenderApp::drawModel(const Matrix44f& modelMatrix)
{
    glPushMatrix();
    gl::multModelView( modelMatrix );
    gl::draw( mTeapotVBO );
    glPopMatrix();
}

void DrawRenderApp::renderToFBO()
{
    gl::enableDepthWrite();
	gl::enableDepthRead();
    
    // camera & viewport setup once, both FBOs are same size
    gl::setViewport( sceneFbo->getBounds() );
    
    CameraPersp mCam( sceneFbo->getWidth(), sceneFbo->getHeight(), 60.0f );
    mCam.lookAt( Vec3f(0, 0, -10 ), Vec3f::zero() );
    mCam.setPerspective( 60, sceneFbo->getAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
    
    Vec3f modelPosition(.0f, .0f, 20.0f);
    
    // Make model matrix
    Matrix44f modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(modelPosition);
    modelMatrix.scale(20.0f);
    float angle = ci::app::getElapsedSeconds() * .4f;
    modelMatrix.rotate(Vec3f(1.0f, 1.0f, .0f), angle);
    
    // Draw Lightened Scene
    sceneFbo->bindFramebuffer();
    gl::clear(Color::black());
    gl::color(Color::white());
	mShader->bind();
    Vec3f lightPosition = Vec3f( mMousePos.x, mMousePos.y, -100.0f);
    Vec3f lightLookAt = modelPosition - lightPosition;
    lightLookAt.normalize();
    mShader->uniform( "lightLookAt", lightLookAt);
    drawModel(modelMatrix);
    mShader->unbind();
    
    gl::disableDepthWrite();
	gl::disableDepthRead();
}

void DrawRenderApp::mouseDown( MouseEvent event )
{
}

void DrawRenderApp::mouseMove( MouseEvent event )
{
    mMousePos.x = event.getX() - getWindowWidth() * 0.5f;
	mMousePos.y = getWindowHeight() * 0.5f - event.getY();
}

void DrawRenderApp::resize()
{
}

void DrawRenderApp::update()
{
    renderToFBO();
    particles->update(sceneFbo.get());
    ink->update(sceneFbo.get(), particles->getFbo());
}

void DrawRenderApp::draw()
{
    
    gl::setMatricesWindow( getWindowSize() );
    gl::setViewport( getWindowBounds() );
    gl::clear();
    gl::draw( ink->getFbo()->getTexture(0), getWindowBounds() );
	   
    mParams->draw();
    
}

CINDER_APP_NATIVE( DrawRenderApp, RendererGl )
