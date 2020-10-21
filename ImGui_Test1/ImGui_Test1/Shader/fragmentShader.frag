#version 330 core
in vec3 ourColor;
out vec4 fragColor;
uniform vec4 triangleColor;

void main()
{
    fragColor = triangleColor;
}