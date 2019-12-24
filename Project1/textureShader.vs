#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord; // texture coordinate
out VS_OUT {
	vec3 aPos;
    vec3 FragPos;
    vec3 Normal;
} vs_out;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec2 TexCoord;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.aPos = aPos;
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}