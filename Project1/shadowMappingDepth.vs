#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

uniform int cut;
uniform vec4 plane;
void main()
{
	if(cut == 0)
	    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
	else if(cut == 1) {
		if(plane.x * aPos.x + plane.y * aPos.y + plane.z * aPos.z + plane.w > 0) {
			gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
		} else {
			gl_Position = vec4(-1,-1,-1,-1);
		}
	} else {
		if(plane.x * aPos.x + plane.y * aPos.y + plane.z * aPos.z + plane.w < 0) {
			gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
		} else {
			gl_Position = vec4(-1,-1,-1,-1);
		}
	}
}