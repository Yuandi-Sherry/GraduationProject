#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];// ÿ��������Ƭ�ķ�����,local
out vec3 FragPos;
out vec3 ProjectPos;
out vec4 BoundingBox;

uniform vec2 halfPixel[3];

uniform mat4 projection[3];
uniform mat4 model;
uniform mat4 view;

uniform mat4 projectionInverse[3];
uniform mat4 modelInverse;
uniform mat4 viewInverse;

uniform float coef[3];


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
// ���������ΰ�Χ��
vec4 calAABB(vec4 pos[3], vec2 pixelDiagonal) {
	vec4 aabb;
	//��Χ�����½�
	aabb.xy = min(pos[2].xy, min(pos[1].xy, pos[0].xy));
	// ��Χ�����Ͻ�
	aabb.zw = max(pos[2].xy, max(pos[1].xy, pos[0].xy));
	// �����Χ��
	aabb.xy -= pixelDiagonal;
	aabb.zw += pixelDiagonal;
	return aabb;
}
void main() {
	// ȷ��Ӧ�õ������
	uint projectIndex = selectViewProject();

	// �ü��ռ������
	vec4 pos[3] = vec4[3] (
		projection[projectIndex] * view * model * gl_in[0].gl_Position,
		projection[projectIndex] * view * model * gl_in[1].gl_Position,
		projection[projectIndex] * view * model * gl_in[2].gl_Position
	);

	// ����ռ������
	vec4 worldPos[3] = vec4[3] (
		model * gl_in[0].gl_Position,
		model * gl_in[1].gl_Position,
		model * gl_in[2].gl_Position
	);

	// ����ռ�ķ�����
	vec3 worldNormal[3] = vec3[3] (
		transpose(inverse(mat3(model))) * gs_in[0].normal,
		transpose(inverse(mat3(model))) * gs_in[1].normal,
		transpose(inverse(mat3(model))) * gs_in[2].normal
	);

	BoundingBox = calAABB(pos, halfPixel[projectIndex]);

	vec4 trianglePlane;
	trianglePlane.xyz = normalize(cross(pos[1].xyz - pos[0].xyz, pos[2].xyz-pos[0].xyz));
	trianglePlane.w   = -dot(pos[0].xyz, trianglePlane.xyz);


	// �������ƽ�淨���򣬼�ƽ�淽��
	vec3 edgePlanes[3];
	edgePlanes[0] = normalize(cross(pos[0].xyw - pos[2].xyw, pos[2].xyw));
	edgePlanes[1] = normalize(cross(pos[1].xyw - pos[0].xyw, pos[0].xyw));
	edgePlanes[2] = normalize(cross(pos[2].xyw - pos[1].xyw, pos[1].xyw));

	// ������ƽ�����ƫ�ƣ��������ڷ���������Ųһ�ξ���
	edgePlanes[0].z -= dot(halfPixel[projectIndex], abs(edgePlanes[0].xy))/coef[projectIndex];
	edgePlanes[1].z -= dot(halfPixel[projectIndex], abs(edgePlanes[1].xy))/coef[projectIndex];
	edgePlanes[2].z -= dot(halfPixel[projectIndex], abs(edgePlanes[2].xy))/coef[projectIndex];
	
	

	// �����������ƽ��Ľ������� -> ���ཻ�ڴ˵�������������˵õ�
	vec3 intersection[3];
	intersection[0] = cross(edgePlanes[0], edgePlanes[1]);
	intersection[1] = cross(edgePlanes[1], edgePlanes[2]);
	intersection[2] = cross(edgePlanes[2], edgePlanes[0]);
	intersection[0] /= intersection[0].z;
	intersection[1] /= intersection[1].z;
	intersection[2] /= intersection[2].z;

	// ��������������������ƽ���󽻵㣬�õ��������㣬x,y���䣬��z
	float z[3];
	z[0] = -(intersection[0].x * trianglePlane.x + intersection[0].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	z[1] = -(intersection[1].x * trianglePlane.x + intersection[1].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	z[2] = -(intersection[2].x * trianglePlane.x + intersection[2].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	
	// ����������ζ������� �ü��ռ�
	pos[0].xyz = vec3(intersection[0].xy, z[0]);
	pos[1].xyz = vec3(intersection[1].xy, z[1]);
	pos[2].xyz = vec3(intersection[2].xy, z[2]);

	float coef1 = 6.0f;
	worldPos[0].xyz -= coef1 * worldNormal[0].xyz;
	worldPos[1].xyz -= coef1 * worldNormal[1].xyz;
	worldPos[2].xyz -= coef1 * worldNormal[2].xyz;
	
	// �Ӳü��ռ�任������ռ�
	vec4 voxelPos;
	ProjectPos = vec3(projection[projectIndex] *view * worldPos[0]); // �ü�����ϵxyz
	gl_Position = projection[projectIndex] *view * worldPos[0]; // �ü�����ϵ
	voxelPos = worldPos[0];  // ��������ϵ
	FragPos = voxelPos.xyz;
	EmitVertex();

	ProjectPos = vec3(projection[projectIndex] *view * worldPos[1]); // �ü�����ϵxyz
	gl_Position = projection[projectIndex] *view * worldPos[1]; // �ü�����ϵ
	voxelPos = worldPos[1];  // ��������ϵ
	FragPos = voxelPos.xyz;
	EmitVertex();

	ProjectPos = vec3(projection[projectIndex] *view * worldPos[2]); // �ü�����ϵxyz
	gl_Position = projection[projectIndex] *view * worldPos[2]; // �ü�����ϵ
	voxelPos = worldPos[2];  // ��������ϵ
	FragPos = voxelPos.xyz;
	EmitVertex();

	// -----------------------------------------------------------

	/*ProjectPos = pos[0].xyz;
	gl_Position = pos[0];
	voxelPos = viewInverse * projectionInverse[projectIndex] * gl_Position;  
	FragPos = voxelPos.xyz;
	EmitVertex();

	ProjectPos = pos[1].xyz;
	gl_Position = pos[1];
	voxelPos = viewInverse * projectionInverse[projectIndex] * gl_Position;  
	FragPos = voxelPos.xyz;
	EmitVertex();

	ProjectPos = pos[2].xyz;
	gl_Position = pos[2];
	voxelPos = viewInverse * projectionInverse[projectIndex] * gl_Position;  
	FragPos = voxelPos.xyz;
	EmitVertex();*/

	// -----------------------------------------------------------

    EndPrimitive();
}