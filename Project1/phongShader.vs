#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec4 LightingColor; // resulting color from lighting calculations
out vec4 interPos; // to frag

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int withLight;


void main()
{
    interPos = vec4(aPos, 1.0);
	gl_Position = projection *view * model * vec4(aPos, 1.0);
    vec3 Position = vec3(model * vec4(aPos, 1.0));
	if(withLight == 1) {
		vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
    
		// ambient
		float ambientStrength = 0.7;
		vec3 ambient = ambientStrength * lightColor;
  	
		// diffuse 
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - Position);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;
    
		// specular
		float specularStrength = 1.0; // this is set higher to better show the effect of Gouraud shading 
		vec3 viewDir = normalize(viewPos - Position);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = specularStrength * spec * lightColor;      

		LightingColor = vec4(ambient + diffuse + specular, 1.0f);
	} else {
		LightingColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}