#version 330 core
out vec4 FragColor;
uniform vec4 ourColor; // uniform means global

void main()
{
    FragColor = ourColor;
}