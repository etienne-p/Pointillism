#version 120

//uniform float size;

void main() {
    
    gl_Position   = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_PointSize  = 20.0;//size;
    gl_FrontColor = gl_Color;
    
}