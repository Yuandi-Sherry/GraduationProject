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
uniform int type; // 0-> model, 1-> remove range, 2->noise

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

vec3 random3( vec3 p )  
{
    p = vec3( dot(p,vec3(127.1,311.7, 74.7)),
              dot(p,vec3(269.5,183.3,246.1)),
              dot(p,vec3(113.5,271.9,124.6)));

    return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise3d( in vec3 p )
{
    vec3 i = floor( p );
    vec3 f = fract( p );

    vec3 u = f*f*(3.0-2.0*f);

    return mix( mix( mix( dot( random3( i + vec3(0.0,0.0,0.0) ), f - vec3(0.0,0.0,0.0) ), 
                          dot( random3( i + vec3(1.0,0.0,0.0) ), f - vec3(1.0,0.0,0.0) ), u.x),
                     mix( dot( random3( i + vec3(0.0,1.0,0.0) ), f - vec3(0.0,1.0,0.0) ), 
                          dot( random3( i + vec3(1.0,1.0,0.0) ), f - vec3(1.0,1.0,0.0) ), u.x), u.y),
                mix( mix( dot( random3( i + vec3(0.0,0.0,1.0) ), f - vec3(0.0,0.0,1.0) ), 
                          dot( random3( i + vec3(1.0,0.0,1.0) ), f - vec3(1.0,0.0,1.0) ), u.x),
                     mix( dot( random3( i + vec3(0.0,1.0,1.0) ), f - vec3(0.0,1.0,1.0) ), 
                          dot( random3( i + vec3(1.0,1.0,1.0) ), f - vec3(1.0,1.0,1.0) ), u.x), u.y), u.z );
}
float hash(vec3 p)  // replace this by something better
{
    p  = 50.0*fract( p*0.3183099 + vec3(0.71,0.113,0.419));
    return -1.0+2.0*fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

vec4 noised( in vec3 x )
 {
    vec3 p = floor(x);
    vec3 w = fract(x);

    vec3 u = w*w*w*(w*(w*6.0-15.0)+10.0);
    vec3 du = 30.0*w*w*(w*(w-2.0)+1.0);

    float a = hash( p+vec3(0,0,0) );
    float b = hash( p+vec3(1,0,0) );
    float c = hash( p+vec3(0,1,0) );
    float d = hash( p+vec3(1,1,0) );
    float e = hash( p+vec3(0,0,1) );
    float f = hash( p+vec3(1,0,1) );
    float g = hash( p+vec3(0,1,1) );
    float h = hash( p+vec3(1,1,1) );

    float k0 =   a;
    float k1 =   b - a;
    float k2 =   c - a;
    float k3 =   e - a;
    float k4 =   a - b - c + d;
    float k5 =   a - c - e + g;
    float k6 =   a - b - e + f;
    float k7 = - a + b + c - d + e - f - g + h;

    return vec4( -1.0+2.0*(k0 + k1*u.x + k2*u.y + k3*u.z + k4*u.x*u.y + k5*u.y*u.z + k6*u.z*u.x + k7*u.x*u.y*u.z),
                 2.0* du * vec3( k1 + k4*u.y + k6*u.z + k7*u.y*u.z,
                                 k2 + k5*u.z + k4*u.x + k7*u.z*u.x,
                                 k3 + k6*u.x + k5*u.y + k7*u.x*u.y ) );
}

const mat3 m3  = mat3( 0.00,  0.80,  0.60,
                      -0.80,  0.36, -0.48,
                      -0.60, -0.48,  0.64 );
const mat3 m3i = mat3( 0.00, -0.80, -0.60,
                       0.80,  0.36, -0.48,
                       0.60, -0.48,  0.64 );
vec4 fbm( in vec3 x, int octaves )
{
    float f = 1.98;  // could be 2.0
    float s = 0.49;  // could be 0.5
    float a = 0.0;
    float b = 0.5;
    vec3  d = vec3(0.0);
    mat3  m = mat3(1.0,0.0,0.0,
    0.0,1.0,0.0,
    0.0,0.0,1.0);
    for( int i=0; i < octaves; i++ )
    {
        vec4 n = noised(x);
        a += b*n.x;          // accumulate values
        d += b*m*n.yzw;      // accumulate derivatives
        b *= s;
        x = f*m3*x;
        m = f*m3i*m;
    }
    return vec4( a, d );
}

vec3 getLightingColor (vec3 color) {
	//float wave = ;
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
	vec3 diffuse;
	if(type == 0 || type == 1) {
		diffuse = diffuseStrength * diff * lightColor;
		//diffuse = diffuseStrength * diff * lightColor - vec3(wave)* 0.5f;
	} else {
		diffuse = diffuseStrength * diff * lightColor;
		//diffuse =  diffuseStrength * diff * lightColor + 0.2f* vec3(fbm(fs_in.aPos, 5));
	}
    

    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = specularStrength * spec * lightColor;

    // ¼ÆËãÒõÓ°
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
	
	if(isPlane == 1) {
		FragColor = vec4(1.0f, 1.0f, 1.0f, 0.3f);
		return;
	}
	// no cut 
	if(type == 0)
        FragColor =  vec4( getLightingColor(color) * color, 1.0f);
    else if(type == 1)
        FragColor =  vec4( getLightingColor(color) * color, 0.8f);
	else
		FragColor =  vec4( getLightingColor(color) * color, 1.0f);

}