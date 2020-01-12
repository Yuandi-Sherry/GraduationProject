#version 430 core
layout (location = 0) in vec3 position;

void main(){
	// local coordinate
	gl_Position = vec4(position,1.0f);
}
