#version 120

uniform sampler2D previousFrame; // previous ink cells state
uniform sampler2D noise;
uniform sampler2D pen;
uniform float persistence;
uniform float threshold;
uniform float maxRate;

void main(void)
{
    vec2 prevCoord = gl_TexCoord[0].xy;
    
    // https://www.opengl.org/wiki/Common_Mistakes
    // For textures, GL considers the y-axis to be bottom to top,
    // the bottom being 0.0 and the top being 1.0.
    // Some people load their bitmap to GL texture and wonder why it appears inverted on their model.
    // The solution is simple: invert your bitmap or invert your model's texcoord by doing 1.0 - v.
    prevCoord.y = 1.0 - prevCoord.y;
    
    vec4 prev   = texture2D(previousFrame, prevCoord);
    float d = 1.0 / 1024.0;

    vec4 top    = texture2D(previousFrame, prevCoord + vec2(0.0,  -d));
    vec4 left   = texture2D(previousFrame, prevCoord + vec2( -d, 0.0));
    vec4 right  = texture2D(previousFrame, prevCoord + vec2(  d, 0.0));
    vec4 bottom = texture2D(previousFrame, prevCoord + vec2(0.0,   d));
              
    float exchangeR = 0.0;
    exchangeR += clamp(max(top.r    - threshold, 0.0) - max(prev.r - threshold, 0.0), -maxRate, maxRate);
    exchangeR += clamp(max(left.r   - threshold, 0.0) - max(prev.r - threshold, 0.0), -maxRate, maxRate);
    exchangeR += clamp(max(right.r  - threshold, 0.0) - max(prev.r - threshold, 0.0), -maxRate, maxRate);
    exchangeR += clamp(max(bottom.r - threshold, 0.0) - max(prev.r - threshold, 0.0), -maxRate, maxRate);
    
    float exchangeG = 0.0;
    exchangeG += clamp(max(top.g    - threshold, 0.0) - max(prev.g - threshold, 0.0), -maxRate, maxRate);
    exchangeG += clamp(max(left.g   - threshold, 0.0) - max(prev.g - threshold, 0.0), -maxRate, maxRate);
    exchangeG += clamp(max(right.g  - threshold, 0.0) - max(prev.g - threshold, 0.0), -maxRate, maxRate);
    exchangeG += clamp(max(bottom.g - threshold, 0.0) - max(prev.g - threshold, 0.0), -maxRate, maxRate);
    
    float exchangeB = 0.0;
    exchangeB += clamp(max(top.b    - threshold, 0.0) - max(prev.b - threshold, 0.0), -maxRate, maxRate);
    exchangeB += clamp(max(left.b   - threshold, 0.0) - max(prev.b - threshold, 0.0), -maxRate, maxRate);
    exchangeB += clamp(max(right.b  - threshold, 0.0) - max(prev.b - threshold, 0.0), -maxRate, maxRate);
    exchangeB += clamp(max(bottom.b - threshold, 0.0) - max(prev.b - threshold, 0.0), -maxRate, maxRate);

    vec4 sc = texture2D(pen, gl_TexCoord[0].xy);
    vec4 noise = texture2D(noise, gl_TexCoord[0].xy);
    gl_FragColor = ((prev + vec4(exchangeR, exchangeG, exchangeB, 1.0) * 0.25) * persistence) + sc * noise;

}