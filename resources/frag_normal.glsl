#version 120

varying vec3 normal;

void main()
{
    gl_FragColor = gl_Color * vec4(normal, 1.0f);
}