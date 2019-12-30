#version 330 core
out vec4 FragColor;
// 1 -> vessel, 2 -> tummor, 3 -> bones
uniform vec3 color;
void main()
{
	FragColor = vec4(color, 1.0f);
}