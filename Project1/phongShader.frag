	#version 330 core
out vec4 FragColor;

in vec4 LightingColor; 
in vec4 interPos;

uniform int type; // 1 -> vessel, 2 -> tummor, 3 -> bones
uniform int isPlane; // 1 -> white, 0 -> color
uniform vec4 plane;
uniform int cut; // 0 -> no cut, 1 -> upper part, 2 -> lower part

void main()
{
	vec4 color;
	if(type == 1)
		color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	else if(type == 2)
		color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	else if(type == 3)
		color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	else
		color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// plane
	if(isPlane == 1) {
		FragColor = vec4(1.0f, 1.0f, 1.0f, 0.3f);
	} else {
		// no cut 
		if(cut == 0) {
			FragColor = LightingColor * color;
		} 
		// upper part
		else if(cut == 1) {
			if(plane.x * interPos.x + plane.y * interPos.y + plane.z * interPos.z + plane.w > 0) {
				if(LightingColor.w < 1.0f) {
					FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
				} else {
					FragColor = LightingColor * color;
				}
			} else {
				discard;
			}
		} 
		// lower part
		else {
			if(plane.x * interPos.x + plane.y * interPos.y + plane.z * interPos.z + plane.w < 0) {
				if(LightingColor.w < 1.0f) {
					FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
				} else {
					FragColor = LightingColor * color;
				}
			} else {
				discard;
			}
		}	
	}
}