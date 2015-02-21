#version 120

varying vec3 normal;

void main(){
    normal = normalize(gl_NormalMatrix * gl_Normal);
    gl_FrontColor = gl_Color;
	gl_Position = ftransform(); // = gl_ModelViewProjectionMatrix * gl_Vertex;
}