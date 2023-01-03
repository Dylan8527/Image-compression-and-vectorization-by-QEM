#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 vertexcolor;


void main()
{
    FragColor = vec4(vertexcolor, 1.0f);
}