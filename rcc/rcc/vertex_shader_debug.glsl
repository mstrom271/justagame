#version 330 core

layout(location = 2) in vec2 inPosition;
uniform mat4 matrix;

void main()
{
    gl_Position = matrix * vec4(inPosition, 1.0, 1.0);
}