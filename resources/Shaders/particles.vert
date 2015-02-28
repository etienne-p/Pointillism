#version 120

uniform sampler2D perlin;
uniform float pointSizeMul;

void main() {
    gl_Position   = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_PointSize  = texture2D(perlin, gl_Vertex.xy).r * pointSizeMul;//gl_Normal.x;
    gl_FrontColor = gl_Color;
}