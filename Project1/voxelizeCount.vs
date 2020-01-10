#version 430 core
layout (location = 0) in vec3 position;

out vec3 FragPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	FragPos = position;
	gl_Position = projection * view * model* vec4(position,1.0f);
}