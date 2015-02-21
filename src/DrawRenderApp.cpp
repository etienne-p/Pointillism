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
    void resize();
    void renderToFBO();
    
    // helpers
    void drawModel(const Matrix44f& modelMatrix);
    gl::GlslProgRef loadShader(string vert, string frag);
    
    params::InterfaceGlRef mParams;
    gl::Texture mTexture;
    gl::GlslProgRef mShader; // shader used to render to fbo
    gl::GlslProgRef mNormalShader; // shader used to render normals to fbo
    gl::GlslProgRef mPostShader; // shader used process fbo texture
    Vec2f mMousePos;
    
    gl::VboMesh mTeapotVBO;
    TriMesh mTeapotMesh;
    
    gl::Fbo mFbo;
    gl::Fbo mNormalFbo;
    gl::Fbo pingFbo;
    gl::Fbo pongFbo;
    
    vector<int> rnd;
    
    bool pingPongFlag;
    
    static const int FBO_WIDTH = 1024, FBO_HEIGHT = 1024;
};

void DrawRenderApp::setup()
{
    pingPongFlag = true;
    
    for (int i = 0; i < 120; i++) rnd.push_back(randFloat(-1.0f, 1.0f));
    
	ObjLoader loader( (DataSourceRef)loadResource( RES_TEAPOT_OBJ ) );
    loader.load( &mTeapotMesh );
    mTeapotVBO = gl::VboMesh( mTeapotMesh);
    
    mShader = loadShader(RES_VERT, RES_FRAG);
    mNormalShader = loadShader(RES_NORMAL_VERT, RES_NORMAL_FRAG);
    mPostShader = loadShader(RES_POST_VERT, RES_POST_FRAG);
    
    try {
        gl::Texture::Format fmt;
        fmt.setWrap(GL_REPEAT, GL_REPEAT);
        mTexture = gl::Texture(loadImage(loadResource(RES_TEX_HATCH)), fmt);
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Texture loading error: " << std::endl;
		std::cout << exc.what();
        return;
	}
    
    gl::Fbo::Format format;
    //format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	mFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
    mNormalFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
    pingFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
    pongFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
    
    pingFbo.bindFramebuffer();
	gl::clear();
    pingFbo.unbindFramebuffer();
    
    pongFbo.bindFramebuffer();
	gl::clear();
    pongFbo.unbindFramebuffer();
    
    // mParams = params::InterfaceGl::create( "Projection", Vec2i( 225, 200 ) );
    // mParams->addParam( "FOV", &FOV).min(.0f).max(2.0f * M_PI);
    
    
}

gl::GlslProgRef DrawRenderApp::loadShader(string vert, string frag)
{
    try {
		return gl::GlslProg::create( loadResource( vert ), loadResource( frag ) );
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
        return NULL;
	}
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
    gl::setViewport( mFbo.getBounds() );
    
    CameraPersp mCam( mFbo.getWidth(), mFbo.getHeight(), 60.0f );
    mCam.lookAt( Vec3f(0, 0, -10 ), Vec3f::zero() );
    mCam.setPerspective( 60, mFbo.getAspectRatio(), 1, 1000 );
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
    mFbo.bindFramebuffer();
    gl::clear(Color::black());
    gl::color(Color::white());
	mShader->bind();
    Vec3f lightPosition = Vec3f( mMousePos.x, mMousePos.y, -100.0f);
    Vec3f lightLookAt = modelPosition - lightPosition;
    lightLookAt.normalize();
    mShader->uniform( "lightLookAt", lightLookAt);
    drawModel(modelMatrix);
    mShader->unbind();
    
    // Draw Normal Map
    mFbo.unbindFramebuffer();
    mNormalFbo.bindFramebuffer();
    gl::clear(Color::black());
    gl::color(Color::white());
    mNormalShader->bind();
    drawModel(modelMatrix);
    mNormalShader->unbind();
    mNormalFbo.unbindFramebuffer();
    
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
    renderToFBO(); // TODO: Render if needed only
}

void DrawRenderApp::draw()
{
    
    gl::Fbo * prevFrame = pingPongFlag ? &pingFbo : &pongFbo;
    gl::Fbo * currentFrame = pingPongFlag ? &pongFbo : &pingFbo;
    
    currentFrame->bindFramebuffer();
    
	gl::setMatricesWindow( mFbo.getSize() );
	gl::setViewport( mFbo.getBounds() );
    gl::clear();
    
    mPostShader->bind();
    prevFrame->getTexture(0).bind(1);
    mPostShader->uniform( "previousFrame", 1 );
    
    gl::draw( mFbo.getTexture(0), mFbo.getBounds() );
    
	mPostShader->unbind();
    
    currentFrame->unbindFramebuffer();
    
    gl::setMatricesWindow( getWindowSize() );
    gl::setViewport( getWindowBounds() );
    gl::clear();
    gl::draw( currentFrame->getTexture(0), getWindowBounds() );
	   
    //mParams->draw();
    
    pingPongFlag = !pingPongFlag; // toggle
}

CINDER_APP_NATIVE( DrawRenderApp, RendererGl )
