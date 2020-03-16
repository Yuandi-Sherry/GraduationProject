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

uniform int isPlane; // 1 -> white, 0 -> color
uniform vec4 plane;
uniform int cut; // 0 -> no cut, 1 -> upper part, 2 -> lower part

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

uniform vec3 color;
uniform int withShadow;
uniform int hasNoise;
float hash(vec2 p) { 
    return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x))));
}

float random (float n) {
    return fract(sin(n)*1000000.);
}
float noise(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    // Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
    vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}
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

vec3 getLightingColor (vec3 color) {
	/*float r = 0.5f;
	vec2 uResolution = vec2(r, r);
	vec2 position = fs_in.aPos.xy/uResolution.xy;
	float wave = noise(position);*/
	vec3 normal;
	normal =  normalize(fs_in.Normal);
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
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = specularStrength * spec * lightColor;

    // º∆À„“ı”∞
	if(withShadow == 1) {
		float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
		float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);  
		vec3 lighting=  (ambient + (1.0 - shadow) * (diffuse + specular));
		return lighting;
	} else {
		vec3 lighting=  (ambient + diffuse + specular);
		return lighting;
	}
	
}




void main()
{   
	float r = 0.5f;
	vec2 uResolution = vec2(r, r);
	vec2 position = fs_in.FragPos.xy/uResolution.xy;
	float wave = noise(position);
	if(isPlane == 1) {
		FragColor = vec4(1.0f, 1.0f, 1.0f, 0.3f);
		return;
	}
	// no cut 
	//float rand = rand();
	/*if(hasNoise == 1)
		FragColor =  vec4( getLightingColor(color) * color, 1.0f)+vec4(vec3(wave)* 0.2f, 0.0f) ;
	else*/
		FragColor =  vec4( getLightingColor(color) * color, 1.0f);
}