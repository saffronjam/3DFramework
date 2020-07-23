#version 410

out vec4 FragColor;

in vec3 ourColor;
in vec2 ourTexCoord;

uniform sampler2D ourTexture;
uniform vec3 extraColor;

void main()
{
    FragColor = vec4(extraColor, 1.0f) + texture(ourTexture, ourTexCoord);
}