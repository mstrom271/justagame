#version 330 core

uniform vec4 externalColor;
out vec4 FragColor;

void main()
{
    FragColor = externalColor;
}
