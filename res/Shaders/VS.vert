#version 410

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;

out vec3 ourColor;
out vec2 ourTexCoord;

uniform mat4 transform;

void main()
{
    ourColor = color;
    ourTexCoord = texCoord;
    gl_Position = transform * vec4(pos, 1.0);
}