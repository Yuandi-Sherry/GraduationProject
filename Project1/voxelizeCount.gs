#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];// 每个三角面片的法向量,local
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
// 计算三角形包围盒
vec4 calAABB(vec4 pos[3], vec2 pixelDiagonal) {
	vec4 aabb;
	//包围盒左下角
	aabb.xy = min(pos[2].xy, min(pos[1].xy, pos[0].xy));
	// 包围盒右上角
	aabb.zw = max(pos[2].xy, max(pos[1].xy, pos[0].xy));
	// 扩大包围盒
	aabb.xy -= pixelDiagonal;
	aabb.zw += pixelDiagonal;
	return aabb;
}
void main() {
	// 确定应用的摄相机
	uint projectIndex = selectViewProject();

	// 裁剪空间的坐标
	vec4 pos[3] = vec4[3] (
		projection[projectIndex] * view * model * gl_in[0].gl_Position,
		projection[projectIndex] * view * model * gl_in[1].gl_Position,
		projection[projectIndex] * view * model * gl_in[2].gl_Position
	);

	// 世界空间的坐标
	vec4 worldPos[3] = vec4[3] (
		model * gl_in[0].gl_Position,
		model * gl_in[1].gl_Position,
		model * gl_in[2].gl_Position
	);

	// 世界空间的法向量
	vec3 worldNormal[3] = vec3[3] (
		transpose(inverse(mat3(model))) * gs_in[0].normal,
		transpose(inverse(mat3(model))) * gs_in[1].normal,
		transpose(inverse(mat3(model))) * gs_in[2].normal
	);

	BoundingBox = calAABB(pos, halfPixel[projectIndex]);

	vec4 trianglePlane;
	trianglePlane.xyz = normalize(cross(pos[1].xyz - pos[0].xyz, pos[2].xyz-pos[0].xyz));
	trianglePlane.w   = -dot(pos[0].xyz, trianglePlane.xyz);


	// 计算齐次平面法方向，即平面方程
	vec3 edgePlanes[3];
	edgePlanes[0] = normalize(cross(pos[0].xyw - pos[2].xyw, pos[2].xyw));
	edgePlanes[1] = normalize(cross(pos[1].xyw - pos[0].xyw, pos[0].xyw));
	edgePlanes[2] = normalize(cross(pos[2].xyw - pos[1].xyw, pos[1].xyw));

	// 对三个平面进行偏移：三条边在法向量方向挪一段距离
	edgePlanes[0].z -= dot(halfPixel[projectIndex], abs(edgePlanes[0].xy))/coef[projectIndex];
	edgePlanes[1].z -= dot(halfPixel[projectIndex], abs(edgePlanes[1].xy))/coef[projectIndex];
	edgePlanes[2].z -= dot(halfPixel[projectIndex], abs(edgePlanes[2].xy))/coef[projectIndex];
	
	

	// 计算三个其次平面的交线向量 -> 由相交于此的两个法向量叉乘得到
	vec3 intersection[3];
	intersection[0] = cross(edgePlanes[0], edgePlanes[1]);
	intersection[1] = cross(edgePlanes[1], edgePlanes[2]);
	intersection[2] = cross(edgePlanes[2], edgePlanes[0]);
	intersection[0] /= intersection[0].z;
	intersection[1] /= intersection[1].z;
	intersection[2] /= intersection[2].z;

	// 三条交线于三角形所在平面求交点，得到三个顶点，x,y不变，求z
	float z[3];
	z[0] = -(intersection[0].x * trianglePlane.x + intersection[0].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	z[1] = -(intersection[1].x * trianglePlane.x + intersection[1].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	z[2] = -(intersection[2].x * trianglePlane.x + intersection[2].y * trianglePlane.y + trianglePlane.w) / trianglePlane.z;
	
	// 扩大后三角形顶点坐标 裁剪空间
	pos[0].xyz = vec3(intersection[0].xy, z[0]);
	pos[1].xyz = vec3(intersection[1].xy, z[1]);
	pos[2].xyz = vec3(intersection[2].xy, z[2]);

	float coef1 = 6.0f;
	worldPos[0].xyz -= coef1 * worldNormal[0].xyz;
	worldPos[1].xyz -= coef1 * worldNormal[1].xyz;
	worldPos[2].xyz -= coef1 * worldNormal[2].xyz;
	
	// 从裁剪空间变换到世界空间
	vec4 voxelPos;
	ProjectPos = vec3(projection[projectIndex] *view * worldPos[0]); // 裁剪坐标系xyz
	gl_Position = projection[projectIndex] *view * worldPos[0]; // 裁剪坐标系
	voxelPos = worldPos[0];  // 世界坐标系
	FragPos = voxelPos.xyz;
	EmitVertex();

	ProjectPos = vec3(projection[projectIndex] *view * worldPos[1]); // 裁剪坐标系xyz
	gl_Position = projection[projectIndex] *view * worldPos[1]; // 裁剪坐标系
	voxelPos = worldPos[1];  // 世界坐标系
	FragPos = voxelPos.xyz;
	EmitVertex();

	ProjectPos = vec3(projection[projectIndex] *view * worldPos[2]); // 裁剪坐标系xyz
	gl_Position = projection[projectIndex] *view * worldPos[2]; // 裁剪坐标系
	voxelPos = worldPos[2];  // 世界坐标系
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