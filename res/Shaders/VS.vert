#version 330 core

in vec3 pos;
in vec3 color;

out vec3 outColor;

uniform mat4 transform;

void main()
{
    outColor = color;
    gl_Position = transform * vec4(pos.x, pos.y, pos.z, 1.0);
}