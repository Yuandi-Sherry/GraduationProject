#version 330 core
out vec4 FragColor;

in VS_OUT {
	vec3 aPos;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;
uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;


uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int type; // 1 -> vessel, 2 -> tummor, 3 -> bones
uniform int isPlane; // 1 -> white, 0 -> color
uniform vec4 plane;
uniform int cut; // 0 -> no cut, 1 -> upper part, 2 -> lower part

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;


float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	return shadow;
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

vec3 getLightingColor (vec3 color) {
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = ambientStrength * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float diff;
	if(cut == 0) {
		diff = max(dot(lightDir, normal), 0.0);
	} else {
		diff = abs(dot(lightDir, normal));
	}
   // float diff = max(dot(lightDir, normal), 0.0);
	// float diff = abs(dot(lightDir, normal));
    vec3 diffuse = diffuseStrength * diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = specularStrength * spec * lightColor;
    // ¼ÆËãÒõÓ°
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);  
	vec3 lighting=  (ambient + (1.0 - shadow) * (diffuse + specular));
	//vec3 lighting=  (ambient + diffuse + specular);
	return lighting;
}


void main()
{   
	if(isPlane == 1) {
		FragColor = vec4(1.0f, 1.0f, 1.0f, 0.3f);
		return;
	}
	vec3 color;
	if(type == 1)
		color = vec3(0.7f, 0.0f, 0.0f);
	else if(type == 2)
		color = vec3(0.0f, 0.0f, 0.7);
	else if(type == 3)
		color = vec3(0.7f, 0.7f, 0.0f);
	else
		color = vec3(1.0f, 1.0f, 1.0f);
	// no cut 
	if(cut == 0) {
		FragColor = vec4( getLightingColor(color) * color, 1.0f);
	} 
	// upper part
	else if(cut == 1) {
		if(plane.x * fs_in.aPos.x + plane.y * fs_in.aPos.y + plane.z * fs_in.aPos.z + plane.w > 0) {
			FragColor = vec4(getLightingColor(color) * color, 1.0f);
		} else {
			discard;
		}
	} 
	// lower part
	else {
		if(plane.x * fs_in.aPos.x + plane.y * fs_in.aPos.y + plane.z * fs_in.aPos.z + plane.w < 0) {
			FragColor =  vec4(getLightingColor(color) * color, 1.0f);
		} else {
			discard;
		} 
	}
}