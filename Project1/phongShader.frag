#version 330 core
out vec4 FragColor;

in vec4 LightingColor; 

uniform int type; // 1 -> vessel, 2 -> tummor, 3 -> bones
uniform int isPlane; // 1 -> white, 0 -> color
void main()
{
	if(isPlane == 1) {
		FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	} else {
		FragColor = LightingColor;
	}
    /*if(LightingColor.w < 1.0f) {
		FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	} else {
		// linearly interpolate between both textures (80% container, 20% awesomeface)
		if(type == 1)
			FragColor = LightingColor * vec4(1.0f, 0.0f, 0.0f, 1.0f);
		else if(type == 2)
			FragColor = LightingColor * vec4(0.0f, 0.0f, 1.0f, 1.0f);
		else if(type == 3)
			FragColor = LightingColor * vec4(1.0f, 1.0f, 0.0f, 1.0f);
		else
			FragColor = LightingColor * vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}*/
	
   

}
