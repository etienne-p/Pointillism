#version 120

uniform sampler2D particleTex;

void main () {
    
    //gl_FragColor = texture2D(particleTex, gl_TexCoord[0].st) * gl_Color;
    gl_FragColor = texture2D(particleTex, gl_PointCoord) * gl_Color;
    
    
}