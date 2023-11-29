#version 330 core

layout(location = 0) in vec2 inPosition; // Входные координаты вершин
uniform mat4 matrix; // Матрица преобразования

void main()
{
    gl_Position = matrix * vec4(inPosition, 1.0, 1.0);
}