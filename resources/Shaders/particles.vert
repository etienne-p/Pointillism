#version 120

//uniform float size;

void main() {
    gl_Position   = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_PointSize  = gl_Normal.x;
    gl_FrontColor = gl_Color;
}