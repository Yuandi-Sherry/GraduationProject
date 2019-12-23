#version 330 core
out vec4 FragColor;

uniform vec3 cylinderColor;

void main()
{
    FragColor = vec4(cylinderColor, 1.0f);
}
