#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
	vec3 aPos;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

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
void main()
{
    float r = 0.1f;
	vec2 uResolution = vec2(r, r);
	vec2 position = aPos.xy/uResolution.xy;
	float wave = noise(position);
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.aPos = aPos;
    ///vs_out.Normal = transpose(inverse(mat3(model))) * aNormal +2.0f * vec3(wave);
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    // gl_Position = projection * view * model * (vec4(aPos, 1.0)+3.0f*vec4(vec3(wave), 0.0f));
    gl_Position = projection * view * model * (vec4(aPos, 1.0));
}