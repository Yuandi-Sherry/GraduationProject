#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec4 LightingColor; // resulting color from lighting calculations
out vec4 interPos; // to frag
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform int withLight;
uniform sampler2D shadowMap;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片元是否在阴影中
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
	gl_Position = projection *view * model * vec4(aPos, 1.0);
	vec3 Position = vec3(model * vec4(aPos, 1.0));
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	interPos = vec4(aPos, 1.0);
	if(withLight == 1) {
		vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
		
		// ambient
		vec3 ambient = ambientStrength * lightColor;
  	
		// diffuse 
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - Position);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diffuseStrength * diff * lightColor;
    
		// specular
		vec3 viewDir = normalize(viewPos - Position);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = specularStrength * spec * lightColor;  
		
		// shadow
		float shadow = ShadowCalculation(vs_out.FragPosLightSpace);       
		vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));   

		LightingColor = vec4(lighting, 1.0f);
		//LightingColor = vec4(ambient + diffuse + specular, 1.0f);
	} else {
		LightingColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}