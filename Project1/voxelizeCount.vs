#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 aNormal;
out VS_OUT {
    vec3 normal;
} vs_out;
void main(){
	// local coordinate
	gl_Position = vec4(position,1.0f);
	vs_out.normal = aNormal;
}
