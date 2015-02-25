#version 120

uniform sampler2D tex0; // scene rendered in a fbo
uniform sampler2D previousFrame; // previous ink cells state
uniform sampler2D noise;
uniform sampler2D pen;
uniform float time; // ms

void main(void)
{
    vec4 sc = texture2D(tex0, gl_TexCoord[0].xy) * texture2D(pen, (gl_TexCoord[0].xy * 3.0) - vec2(time * 0.001, 0.0));
    
    vec2 prevCoord = gl_TexCoord[0].xy;
    
    // https://www.opengl.org/wiki/Common_Mistakes
    // For textures, GL considers the y-axis to be bottom to top,
    // the bottom being 0.0 and the top being 1.0.
    // Some people load their bitmap to GL texture and wonder why it appears inverted on their model.
    // The solution is simple: invert your bitmap or invert your model's texcoord by doing 1.0 - v.
    prevCoord.y = 1.0 - prevCoord.y;
    
    float prev   = texture2D(previousFrame, prevCoord).r;
    float d = 1.0 / 1024.0;
    // collect neighbors
    float top    = texture2D(previousFrame, prevCoord + vec2(0.0,  -d)).r;
    float left   = texture2D(previousFrame, prevCoord + vec2( -d, 0.0)).r;
    float right  = texture2D(previousFrame, prevCoord + vec2(  d, 0.0)).r;
    float bottom = texture2D(previousFrame, prevCoord + vec2(0.0,   d)).r;
    
    float noise = texture2D(noise, gl_TexCoord[0].xy).r;
              
    // ink exchange
    float dInkMax = 0.16;
    float exchange = 0.0;
    float threshold = 0.2 + 0.8 * noise;
    exchange += clamp(max(top - threshold, 0.0) - max(prev - threshold, 0.0), -dInkMax, dInkMax);
    exchange += clamp(max(left - threshold, 0.0) - max(prev - threshold, 0.0), -dInkMax, dInkMax);
    exchange += clamp(max(right - threshold, 0.0) - max(prev - threshold, 0.0), -dInkMax, dInkMax);
    exchange += clamp(max(bottom - threshold, 0.0) - max(prev - threshold, 0.0), -dInkMax, dInkMax);
    
    gl_FragColor = (vec4(1.0, 1.0, 1.0, 1.0) * ((prev + exchange) * 0.96)) + (sc * noise);

}