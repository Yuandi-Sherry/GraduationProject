#version 330 core
out vec4 FragColor;

in VS_OUT {
	vec3 aPos;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;
uniform float near_plane;
uniform float far_plane;

uniform vec3 mainLight;
uniform float mainLightCoef;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

uniform vec3 color;
uniform int type; // 0-> model, 1-> remove range, 2->noise

// debug
uniform float debugAmbient;
uniform float debugDiffuse;
uniform float debugSpecular;

uniform float neiDis;

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow =  currentDepth - bias > closestDepth ? 1.0 : 0.0; ;
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

vec3 eightNeighbors[8] = {
	{-neiDis, -neiDis, 0},
	{-neiDis, 0, 0},
	{-neiDis, neiDis, 0},
	{0, -neiDis, 0},
	{0, neiDis, 0},
	{neiDis, -neiDis, 0},
	{neiDis, 0, 0},
	{neiDis, neiDis, 0}
};
int numLight = 8;
vec3 getLightingColor (vec3 color) {
	vec3 normal =  normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = ambientStrength * color;

	// Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float diff = 1.0f/(numLight+1) * max(dot(lightDir, normal), 0.0);
	
	// Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = 1.0f/(numLight+1) * pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	
	// 假设一圈都是light
	for(int i = 0; i < numLight; i++) {
		vec3 neiLightPos = lightPos + eightNeighbors[i];
		diff += 1.0f/(numLight+1) * max(dot(normalize(neiLightPos - fs_in.FragPos), normal), 0.0);
		vec3 halfwayDir = normalize(normalize(neiLightPos - fs_in.FragPos) + viewDir);  
		spec += 1.0f/(numLight+1) * pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	}
	vec3 diffuse = diffuseStrength * diff * lightColor;
	vec3 specular = specularStrength * spec * lightColor;
    // 计算阴影
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);  
	vec3 lighting=  (ambient + (1.0 - shadow) * (diffuse + specular));

    if (type == 2) {
        // + UI
        lighting = (debugAmbient * ambient + (1.0 - shadow)*(debugDiffuse * diffuse + debugSpecular * specular));
    }
    return lighting;
}


void main()
{
	if(type == 0)
        FragColor =  vec4( getLightingColor(color) * color, 1.0f);
    else if(type == 1)
        FragColor =  vec4( getLightingColor(color) * color, 0.8f);
	else
		FragColor =  vec4( getLightingColor(color) * color, 1.0f);

}