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

#define P_COUNT 10000

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
    void updateParticles();
    void renderParticles();
    void enablePointSprites();
    void disablePointSprites();
    
    // helpers
    void drawModel(const Matrix44f& modelMatrix);
    gl::GlslProgRef loadShader(string vert, string frag);
    
    params::InterfaceGlRef mParams;
    gl::Texture mNoiseTexture;
    gl::Texture mDotTexture;
    gl::GlslProgRef mShader; // shader used to render to fbo
    gl::GlslProgRef mNormalShader; // shader used to render normals to fbo
    gl::GlslProgRef mPostShader; // shader used process fbo texture
    gl::GlslProgRef mParticlesShader;
    Vec2f mMousePos;
    
    gl::VboMesh mTeapotVBO;
    TriMesh mTeapotMesh;
    
    gl::VboMeshRef particlesVbo;
    
    gl::Fbo sceneFbo;
    gl::Fbo normalFbo;
    gl::Fbo pingFbo;
    gl::Fbo pongFbo;
    gl::Fbo particlesFbo;
    
    bool pingPongFlag;
    
    static const int FBO_WIDTH = 1024, FBO_HEIGHT = 1024;
};

void DrawRenderApp::setup()
{
    pingPongFlag = true;
    
	ObjLoader loader( (DataSourceRef)loadResource( RES_TEAPOT_OBJ ) );
    loader.load( &mTeapotMesh );
    mTeapotVBO = gl::VboMesh( mTeapotMesh);
    
    gl::VboMesh::Layout layout;
    //layout.setStaticColorsRGBA();
    layout.setStaticTexCoords2d();
    layout.setStaticPositions();
    
    vector<ColorA> colors;
    vector<Vec2f> texcoords;
    vector<Vec3f> vertices;
    const float w = (float)FBO_WIDTH;
    const float h = (float)FBO_HEIGHT;
    const ColorA white = ColorA(1.0f, 1.0f, 1.0f);
    int i=0;
    while(i < P_COUNT){
        colors.push_back(white);
        texcoords.push_back( Vec2f( 10.0f, 10.0f ));
        vertices.push_back(Vec3f(Rand::randFloat(w * .1f, w * .9f), Rand::randFloat(h * .1f, h * .9f), .0f));
        i++;
    }
    
    particlesVbo = gl::VboMesh::create(P_COUNT, 0, layout, GL_POINTS);
    particlesVbo->bufferPositions( &(vertices.front()), vertices.size() );
    particlesVbo->bufferTexCoords2d( 0, texcoords );
    //particlesVbo->bufferColorsRGBA(colors);
    
    
    
    /*
    gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	layout.setStaticTexCoords2d();
	layout.setStaticColorsRGB();
    
	mVboMesh = gl::VboMesh(mVertices.size(), 0, layout, GL_POINTS);
	mVboMesh.bufferPositions( &(mVertices.front()), mVertices.size() );
	mVboMesh.bufferTexCoords2d( 0, mTexcoords );
	mVboMesh.bufferColorsRGB( mColors );
     */
    
    mShader = loadShader(RES_VERT, RES_FRAG);
    mNormalShader = loadShader(RES_NORMAL_VERT, RES_NORMAL_FRAG);
    mPostShader = loadShader(RES_POST_VERT, RES_POST_FRAG);
    mParticlesShader = loadShader(RES_PARTICLES_VERT, RES_PARTICLES_FRAG);
    
    try {
        gl::Texture::Format fmt;
        fmt.setWrap(GL_REPEAT, GL_REPEAT);
        mNoiseTexture = gl::Texture(loadImage(loadResource(RES_TEX_NOISE)), fmt);
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Texture loading error: " << std::endl;
		std::cout << exc.what();
        return;
	}
    
    try {
        mDotTexture = gl::Texture(loadImage(loadResource(RES_TEX_DOT)));
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Texture loading error: " << std::endl;
		std::cout << exc.what();
        return;
	}
    
    gl::Fbo::Format format;
    //format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	sceneFbo     = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
    normalFbo    = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
    pingFbo      = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
    pongFbo      = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
    particlesFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
    
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
    gl::setViewport( sceneFbo.getBounds() );
    
    CameraPersp mCam( sceneFbo.getWidth(), sceneFbo.getHeight(), 60.0f );
    mCam.lookAt( Vec3f(0, 0, -10 ), Vec3f::zero() );
    mCam.setPerspective( 60, sceneFbo.getAspectRatio(), 1, 1000 );
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
    sceneFbo.bindFramebuffer();
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
    sceneFbo.unbindFramebuffer();
    normalFbo.bindFramebuffer();
    gl::clear(Color::black());
    gl::color(Color::white());
    mNormalShader->bind();
    drawModel(modelMatrix);
    mNormalShader->unbind();
    normalFbo.unbindFramebuffer();
    
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

void DrawRenderApp::updateParticles()
{
    gl::VboMesh::VertexIter iter = particlesVbo->mapVertexBuffer();
    const Vec3f one(1.0f, 1.0f, 1.0f);
    const Vec3f normal(.0f, .0f, -1.0f);
    const float w = (float)FBO_WIDTH;
    const float h = (float)FBO_HEIGHT;
    for( int i = 0; i < P_COUNT; ++i ) {
        iter.setPosition(Vec3f(
            Rand::randFloat(w * .1f, w * .9f),
            Rand::randFloat(h * .1f, h * .9f),
            .0f));
        ++iter;
    }
}

void DrawRenderApp::enablePointSprites()
{
	// store current OpenGL state
	glPushAttrib( GL_POINT_BIT | GL_ENABLE_BIT );
    
	// enable point sprites and initialize it
	gl::enable( GL_POINT_SPRITE_ARB );
	glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 1.0f );
	glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 0.1f );
	glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, 200.0f );
    
	// allow vertex shader to change point size
	gl::enable( GL_VERTEX_PROGRAM_POINT_SIZE );
    
	// bind shader
	mParticlesShader->bind();
	mParticlesShader->uniform("tex0", 0);
}

void DrawRenderApp::disablePointSprites()
{
	// unbind shader
	mParticlesShader->unbind();
	
	// restore OpenGL state
	glPopAttrib();
}

void DrawRenderApp::renderParticles()
{
    particlesFbo.bindFramebuffer();
    
    mDotTexture.bind(0);
    
    gl::setMatricesWindow( particlesFbo.getSize() );
	gl::setViewport( particlesFbo.getBounds() );
    gl::clear(Color::black());
    
	gl::enableAdditiveBlending();
    enablePointSprites();
    
    gl::color(Color::white());
    gl::draw(particlesVbo);
    
   
    disablePointSprites();
    gl::disableAlphaBlending();
    
    mDotTexture.unbind();
    
    particlesFbo.unbindFramebuffer();
}

void DrawRenderApp::resize()
{
}

void DrawRenderApp::update()
{
    renderToFBO();
    //updateParticles();
    renderParticles();
}

void DrawRenderApp::draw()
{
    /*
    gl::Fbo * prevFrame = pingPongFlag ? &pingFbo : &pongFbo;
    gl::Fbo * currentFrame = pingPongFlag ? &pongFbo : &pingFbo;
    
    currentFrame->bindFramebuffer();
    
	gl::setMatricesWindow( sceneFbo.getSize() );
	gl::setViewport( sceneFbo.getBounds() );
    gl::clear();
    
    mPostShader->bind();
    // bind textures
    prevFrame->getTexture(0).bind(1);
    mPostShader->uniform( "previousFrame", 1 );
    mNoiseTexture.bind(2);
    mPostShader->uniform( "noise", 2 );
    particlesFbo.getTexture().bind(3);
    mPostShader->uniform( "pen", 3 );
    mPostShader->uniform( "time", (float)getElapsedSeconds() * 1000.0f );
    
    gl::draw( sceneFbo.getTexture(0), sceneFbo.getBounds() );
    
	mPostShader->unbind();
    
    currentFrame->unbindFramebuffer();
     */
    
    gl::setMatricesWindow( getWindowSize() );
    gl::setViewport( getWindowBounds() );
    gl::clear();
    gl::draw( particlesFbo.getTexture(0), getWindowBounds() );
	   
    //mParams->draw();
    
    pingPongFlag = !pingPongFlag; // toggle
}

CINDER_APP_NATIVE( DrawRenderApp, RendererGl )
