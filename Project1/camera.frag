#version 330 core
out vec4 FragColor;
uniform int type;
// 1 -> vessel, 2 -> tummor, 3 -> bones

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	if(type == 1)
		FragColor = vec4(1.0f, 0.0f, 0.0f, 0.0001f);
	else if(type == 2)
		FragColor = vec4(0.0f, 0.0f, 1.0f, 0.0001f);
	else if(type == 3)
		FragColor = vec4(0.0f, 1.0f, 1.0f, 0.0001f);
	else
		FragColor = vec4(1.0f, 1.0f, 1.0f, 0.0001f);
}