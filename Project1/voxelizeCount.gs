#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 FragPos;

uniform mat4 projection[3];
out vec4 ProjectPos;
uniform mat4 model;
uniform mat4 view;

uint selectViewProject()
{
	vec3 vector0_1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 vector0_2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 faceNormal = cross(vector0_1, vector0_2);
	float nDX = abs(faceNormal.x);
	float nDY = abs(faceNormal.y);
	float nDZ = abs(faceNormal.z);

	if( nDX > nDY && nDX > nDZ )
    {
		// cameraX
		return 0;
	}
	else if( nDY > nDX && nDY > nDZ  )
    {
		// cameraY
	    return 1;
    }
	else
    {
		// cameraZ
	    return 2;
	}
} 

void main() {
	uint projectIndex = selectViewProject();
	FragPos = gl_in[0].gl_Position.xyz;
	gl_Position = projection[projectIndex] * view * model * gl_in[0].gl_Position;
    EmitVertex();

	FragPos = gl_in[1].gl_Position.xyz;
	gl_Position = projection[projectIndex] * view * model * gl_in[1].gl_Position;
    EmitVertex();

	FragPos = gl_in[2].gl_Position.xyz;
	gl_Position = projection[projectIndex] * view * model * gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}