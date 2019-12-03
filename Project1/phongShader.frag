/*#version 330 core
out vec4 FragColor;

in vec4 LightingColor; 
in vec4 interPos;

uniform int type; // 1 -> vessel, 2 -> tummor, 3 -> bones
uniform int isPlane; // 1 -> white, 0 -> color
uniform vec4 plane;
uniform int cut; // 0 -> no cut, 1 -> upper part, 2 -> lower part

void main()
{
	vec4 color;
	if(type == 1)
		color = vec4(0.7f, 0.0f, 0.0f, 1.0f);
	else if(type == 2)
		color = vec4(0.0f, 0.0f, 0.7f,  1.0f);
	else if(type == 3)
		color = vec4(0.7f, 0.7f, 0.0f,  1.0f);
	else
		color = vec4(1.0f, 1.0f, 1.0f,  1.0f);
	// plane
	if(isPlane == 1) {
		FragColor = vec4(1.0f, 1.0f, 1.0f, 0.3f);
	} else {
		// no cut 
		if(cut == 0) {
			FragColor = LightingColor * color;
		} 
		// upper part
		else if(cut == 1) {
			if(plane.x * interPos.x + plane.y * interPos.y + plane.z * interPos.z + plane.w > 0) {
				if(LightingColor.w < 1.0f) {
					FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
				} else {
					FragColor = LightingColor * color;
				}
			} else {
				discard;
			}
		} 
		// lower part
		else {
			if(plane.x * interPos.x + plane.y * interPos.y + plane.z * interPos.z + plane.w < 0) {
				if(LightingColor.w < 1.0f) {
					FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
				} else {
					FragColor = LightingColor * color;
				}
			} else {
				discard;
			}
		}	
	}
}*/

#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int type; // 1 -> vessel, 2 -> tummor, 3 -> bones
uniform int isPlane; // 1 -> white, 0 -> color
uniform vec4 plane;
uniform int cut; // 0 -> no cut, 1 -> upper part, 2 -> lower part



float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	return shadow;
}

void main()
{           
    //vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 color;
	if(type == 1)
		color = vec3(0.7f, 0.0f, 0.0f);
	else if(type == 2)
		color = vec3(0.0f, 0.0f, 0.7);
	else if(type == 3)
		color = vec3(0.7f, 0.7f, 0.0f);
	else
		color = vec3(1.0f, 1.0f, 1.0f);
   
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = 0.5 * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // º∆À„“ı”∞
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);  
	vec3 lighting=  (ambient + (1.0 - shadow) * (diffuse + specular))  * color;
    FragColor = vec4(lighting, 1.0f);
}