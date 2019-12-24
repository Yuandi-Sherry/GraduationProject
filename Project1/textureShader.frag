#version 330 core
out vec4 FragColor;
in VS_OUT {
	vec3 aPos;
    vec3 FragPos;
    vec3 Normal;
} fs_in;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture; 
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = ambientStrength * vec3(1,1,1);

    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float diff;
	diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = specularStrength * spec * lightColor;

	vec3 lighting=  (/*ambient + */diffuse + specular);
    FragColor = vec4(lighting, 1.0f) * texture(ourTexture, TexCoord); // combine texture and coordinate
}
