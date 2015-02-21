#version 120

uniform sampler2D tex0; // scene rendered in a fbo
uniform sampler2D previousFrame; // previous ink cells state
//uniform sampler2D noise; // gives textures, used for propagation


void main(void)
{
    vec4 sc = texture2D(tex0, gl_TexCoord[0].xy);
    
    vec2 prevCoord = gl_TexCoord[0].xy;
    
    // https://www.opengl.org/wiki/Common_Mistakes
    // For textures, GL considers the y-axis to be bottom to top,
    // the bottom being 0.0 and the top being 1.0.
    // Some people load their bitmap to GL texture and wonder why it appears inverted on their model.
    // The solution is simple: invert your bitmap or invert your model's texcoord by doing 1.0 - v.
    prevCoord.y = 1.0 - prevCoord.y;
    
    float d = 1.0 / 1024.0;
    
    float dInkMax = 0.01;
    
    //
    
    float prev   = texture2D(previousFrame, prevCoord).r;
    // collect neighbors
    float top    = texture2D(previousFrame, prevCoord + vec2(0.0,  -d)).r;
    float left   = texture2D(previousFrame, prevCoord + vec2( -d, 0.0)).r;
    float right  = texture2D(previousFrame, prevCoord + vec2(  d, 0.0)).r;
    float bottom = texture2D(previousFrame, prevCoord + vec2(0.0,   d)).r;
              
    // ink exchange
    float exchange = 0.0;
    exchange += clamp(top - prev, -dInkMax, dInkMax);
    exchange += clamp(left - prev, -dInkMax, dInkMax);
    exchange += clamp(right - prev, -dInkMax, dInkMax);
    exchange += clamp(bottom - prev, -dInkMax, dInkMax);
    
    gl_FragColor = (vec4(1.0, 1.0, 1.0, 1.0) * (prev + exchange) * 0.99) + (sc * 0.2);
}