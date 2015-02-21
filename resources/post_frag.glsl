#version 120

uniform sampler2D tex0; // scene rendered in a fbo
uniform sampler2D previousFrame; // previous ink cells state
//uniform sampler2D noise; // gives textures, used for propagation


void main(void)
{
    vec4 sc = texture2D(tex0, gl_TexCoord[0].xy);
    
    vec4 prev = texture2D(previousFrame, gl_TexCoord[0].xy);
              
    // compute new cells state
              
    // propagation, evaporation, add
              
    // do ping pong
              
    gl_FragColor = (prev * .98f) + (sc * .04f);
    //gl_FragColor = texture2D(tex0, gl_TexCoord[0].xy);
}