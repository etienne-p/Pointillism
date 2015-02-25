#version 120

uniform vec3 lightLookAt;

varying vec3 normal;

void main()
{
    gl_FragColor = gl_Color * dot(lightLookAt, normal);
}