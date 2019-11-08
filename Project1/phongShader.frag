#version 330 core
out vec4 FragColor;

in vec3 LightingColor; 

uniform int type;
// 1 -> vessel, 2 -> tummor, 3 -> bones
void main()
{
   
   	// linearly interpolate between both textures (80% container, 20% awesomeface)
	if(type == 1)
		FragColor = vec4(LightingColor * vec3(1.0f, 0.0f, 0.0f), 1.0);
	else if(type == 2)
		FragColor = vec4(LightingColor * vec3(0.0f, 0.0f, 1.0f), 1.0);
	else if(type == 3)
		FragColor = vec4(LightingColor * vec3(1.0f, 1.0f, 0.0f), 1.0);
	else
		FragColor = vec4(LightingColor * vec3(1.0f, 1.0f, 1.0f), 1.0);
}
