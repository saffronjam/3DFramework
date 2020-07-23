#version 330 core

in vec3 outColor;
out vec4 FragColor;

uniform vec3 color;

void main()
{
    FragColor = vec4(outColor + color, 1.0f);
}