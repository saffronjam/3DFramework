#version 410

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

out vec3 ourColor;
out vec2 ourTexCoord;
out vec3 ourNormal;
out vec3 ourFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    ourColor = color;
    ourTexCoord = texCoord;
    ourNormal = mat3(transpose(inverse(model))) * normal;
    ourFragPos = vec3(model * vec4(pos, 1.0f));
    gl_Position = projection * view * model * vec4(pos, 1.0);
}