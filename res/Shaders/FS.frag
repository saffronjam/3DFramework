#version 410

out vec4 FragColor;

in vec3 ourColor;
in vec2 ourTexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, ourTexCoord);
}