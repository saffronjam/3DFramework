#version 330 core

layout (location = 0) in vec3 aPos;
uniform vec3 color;

void main()
{
    gl_Position = vec4(aPos.x + color.r, aPos.y + color.g, aPos.z +  + color.b, 1.0);
}