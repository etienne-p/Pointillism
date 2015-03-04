#version 120

uniform sampler2D perlin;
uniform sampler2D scene;
uniform float pointSizeMul;
uniform float pointSizeVariation;

void main() {
    vec4 p = gl_ModelViewProjectionMatrix * gl_Vertex;
    float c = texture2D(scene, vec2(p.x + 1.0, p.y + 1.0) * .5f).r;
    gl_Position   = p;
    gl_PointSize  = (c * pointSizeVariation + (1.0 - pointSizeVariation)) * pointSizeMul;
    gl_FrontColor = gl_Color * c;
}