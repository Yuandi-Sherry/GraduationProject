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

uniform int type;
/*float hash(vec2 p) { 
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
}*/
// --------------------------------
/*float hash1(vec2 p)
{
    p = 50.0 * fract(p * 0.3183099);
    return fract(p.x * p.y * (p.x + p.y));
}
float hash1(float n)
{
    return fract(n * 17.0 * fract(n * 0.3183099));
}

vec3 noised(in vec2 x)
{
    vec2 p = floor(x);
    vec2 w = fract(x);

    vec2 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);
    vec2 du = 30.0 * w * w * (w * (w - 2.0) + 1.0);

    float a = hash1(p + vec2(0, 0));
    float b = hash1(p + vec2(1, 0));
    float c = hash1(p + vec2(0, 1));
    float d = hash1(p + vec2(1, 1));

    float k0 = a;
    float k1 = b - a;
    float k2 = c - a;
    float k4 = a - b - c + d;

    return vec3(-1.0 + 2.0 * (k0 + k1 * u.x + k2 * u.y + k4 * u.x * u.y),
        2.0 * du * vec2(k1 + k4 * u.y,
            k2 + k4 * u.x));
}



float noise(in vec2 x)
{
    vec2 p = floor(x);
    vec2 w = fract(x);
    vec2 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);

#if 0
    p *= 0.3183099;
    float kx0 = 50.0 * fract(p.x);
    float kx1 = 50.0 * fract(p.x + 0.3183099);
    float ky0 = 50.0 * fract(p.y);
    float ky1 = 50.0 * fract(p.y + 0.3183099);

    float a = fract(kx0 * ky0 * (kx0 + ky0));
    float b = fract(kx1 * ky0 * (kx1 + ky0));
    float c = fract(kx0 * ky1 * (kx0 + ky1));
    float d = fract(kx1 * ky1 * (kx1 + ky1));
#else
    float a = hash1(p + vec2(0, 0));
    float b = hash1(p + vec2(1, 0));
    float c = hash1(p + vec2(0, 1));
    float d = hash1(p + vec2(1, 1));
#endif

    return -1.0 + 2.0 * (a + (b - a) * u.x + (c - a) * u.y + (a - b - c + d) * u.x * u.y);
}

float noise(in vec3 x)
{
    vec3 p = floor(x);
    vec3 w = fract(x);

    vec3 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);

    float n = p.x + 317.0 * p.y + 157.0 * p.z;

    float a = hash1(n + 0.0);
    float b = hash1(n + 1.0);
    float c = hash1(n + 317.0);
    float d = hash1(n + 318.0);
    float e = hash1(n + 157.0);
    float f = hash1(n + 158.0);
    float g = hash1(n + 474.0);
    float h = hash1(n + 475.0);

    float k0 = a;
    float k1 = b - a;
    float k2 = c - a;
    float k3 = e - a;
    float k4 = a - b - c + d;
    float k5 = a - c - e + g;
    float k6 = a - b - e + f;
    float k7 = -a + b + c - d + e - f - g + h;

    return -1.0 + 2.0 * (k0 + k1 * u.x + k2 * u.y + k3 * u.z + k4 * u.x * u.y + k5 * u.y * u.z + k6 * u.z * u.x + k7 * u.x * u.y * u.z);
}


vec2 smoothstepd(float a, float b, float x)
{
    if (x < a) return vec2(0.0, 0.0);
    if (x > b) return vec2(1.0, 0.0);
    float ir = 1.0 / (b - a);
    x = (x - a) * ir;
    return vec2(x * x * (3.0 - 2.0 * x), 6.0 * x * (1.0 - x) * ir);
}



vec4 noised(in vec3 x)
{
    vec3 p = floor(x);
    vec3 w = fract(x);

    vec3 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);
    vec3 du = 30.0 * w * w * (w * (w - 2.0) + 1.0);

    float n = p.x + 317.0 * p.y + 157.0 * p.z;

    float a = hash1(n + 0.0);
    float b = hash1(n + 1.0);
    float c = hash1(n + 317.0);
    float d = hash1(n + 318.0);
    float e = hash1(n + 157.0);
    float f = hash1(n + 158.0);
    float g = hash1(n + 474.0);
    float h = hash1(n + 475.0);

    float k0 = a;
    float k1 = b - a;
    float k2 = c - a;
    float k3 = e - a;
    float k4 = a - b - c + d;
    float k5 = a - c - e + g;
    float k6 = a - b - e + f;
    float k7 = -a + b + c - d + e - f - g + h;

    return vec4(-1.0 + 2.0 * (k0 + k1 * u.x + k2 * u.y + k3 * u.z + k4 * u.x * u.y + k5 * u.y * u.z + k6 * u.z * u.x + k7 * u.x * u.y * u.z),
        2.0 * du * vec3(k1 + k4 * u.y + k6 * u.z + k7 * u.y * u.z,
            k2 + k5 * u.z + k4 * u.x + k7 * u.z * u.x,
            k3 + k6 * u.x + k5 * u.y + k7 * u.x * u.y));
}

//==========================================================================================
// fbm constructions
//==========================================================================================

const mat3 m3 = mat3(0.00, 0.80, 0.60,
    -0.80, 0.36, -0.48,
    -0.60, -0.48, 0.64);
const mat3 m3i = mat3(0.00, -0.80, -0.60,
    0.80, 0.36, -0.48,
    0.60, -0.48, 0.64);
const mat2 m2 = mat2(0.80, 0.60,
    -0.60, 0.80);
const mat2 m2i = mat2(0.80, -0.60,
    0.60, 0.80);

//------------------------------------------------------------------------------------------
vec4 fbmd_8(in vec3 x)
{
    float f = 1.92;
    float s = 0.5;
    float a = 0.0;
    float b = 0.5;
    vec3  d = vec3(0.0);
    mat3  m = mat3(1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0);
    for (int i = 0; i < 7; i++)
    {
        vec4 n = noised(x);
        a += b * n.x;          // accumulate values		
        d += b * m * n.yzw;      // accumulate derivatives
        b *= s;
        x = f * m3 * x;
        m = f * m3i * m;
    }
    return vec4(a, d);
}
float fbm_9(in vec2 x)
{
    float f = 1.9;
    float s = 0.55;
    float a = 0.0;
    float b = 0.5;
    for (int i = 0; i < 9; i++)
    {
        float n = noise(x);
        a += b * n;
        b *= s;
        x = f * m2 * x;
    }
    return a;
}
vec3 fbmd_9(in vec2 x)
{
    float f = 1.9;
    float s = 0.55;
    float a = 0.0;
    float b = 0.5;
    vec2  d = vec2(0.0);
    mat2  m = mat2(1.0, 0.0, 0.0, 1.0);
    for (int i = 0; i < 9; i++)
    {
        vec3 n = noised(x);
        a += b * n.x;          // accumulate values		
        d += b * m * n.yz;       // accumulate derivatives
        b *= s;
        x = f * m2 * x;
        m = f * m2i * m;
    }
    return vec3(a, d);
}

// terrain generation
vec2 terrainMap(in vec2 p)
{
    const float sca = 0.0010;
    const float amp = 300.0;

    p *= sca;
    float e = fbm_9(p + vec2(1.0, -2.0));
    float a = 1.0 - smoothstep(0.12, 0.13, abs(e + 0.12)); // flag high-slope areas (-0.25, 0.0)
    e = e + 0.15 * smoothstep(-0.08, -0.01, e);
    e *= amp;
    return vec2(e, a);
}

vec4 terrainMapD(in vec2 p)
{
    const float sca = 0.0010;
    const float amp = 300.0;
    p *= sca;
    vec3 e = fbmd_9(p + vec2(1.0, -2.0));
    vec2 c = smoothstepd(-0.08, -0.01, e.x);
    e.x = e.x + 0.15 * c.x;
    e.yz = e.yz + 0.15 * c.y * e.yz;
    e.x *= amp;
    e.yz *= amp * sca;
    return vec4(e.x, normalize(vec3(-e.y, 1.0, -e.z)));
}

vec3 terrainNormal(in vec2 pos)
{
#if 1
    return terrainMapD(pos).yzw;
#else    
    vec2 e = vec2(0.03, 0.0);
    return normalize(vec3(terrainMap(pos - e.xy).x - terrainMap(pos + e.xy).x,
        2.0 * e.x,
        terrainMap(pos - e.yx).x - terrainMap(pos + e.yx).x));
#endif    
}

float terrainShadow(in vec3 ro, in vec3 rd, in float mint)
{
    float res = 1.0;
    float t = mint;
#ifdef LOWQUALITY
    for (int i = ZERO; i < 32; i++)
    {
        vec3  pos = ro + t * rd;
        vec2  env = terrainMap(pos.xz);
        float hei = pos.y - env.x;
        res = min(res, 32.0 * hei / t);
        if (res < 0.0001) break;
        t += clamp(hei, 1.0 + t * 0.1, 50.0);
    }
#else
    for (int i = 0; i < 128; i++)
    {
        vec3  pos = ro + t * rd;
        vec2  env = terrainMap(pos.xz);
        float hei = pos.y - env.x;
        res = min(res, 32.0 * hei / t);
        if (res < 0.0001) break;
        t += clamp(hei, 0.5 + t * 0.05, 25.0);
    }
#endif
    return clamp(res, 0.0, 1.0);
}

vec2 raymarchTerrain(in vec3 ro, in vec3 rd, float tmin, float tmax)
{
    //float tt = (150.0-ro.y)/rd.y; if( tt>0.0 ) tmax = min( tmax, tt );

    float dis, th;
    float t2 = -1.0;
    float t = tmin;
    float ot = t;
    float odis = 0.0;
    float odis2 = 0.0;
    for (int i = 0; i < 400; i++)
    {
        th = 0.001 * t;

        vec3  pos = ro + t * rd;
        vec2  env = terrainMap(pos.xz);
        float hei = env.x;

        // tree envelope
        float dis2 = pos.y - hei;
        if (dis2 < th)
        {
            if (t2 < 0.0)
            {
                t2 = ot + (th - odis2) * (t - ot) / (dis2 - odis2); // linear interpolation for better accuracy
            }
        }
        odis2 = dis2;

        // terrain
        dis = pos.y - hei;
        if (dis < th) break;

        ot = t;
        odis = dis;
        t += dis * 0.8 * (1.0 - 0.75 * env.y); // slow down in step areas
        if (t > tmax) break;
    }

    if (t > tmax) t = -1.0;
    else t = ot + (th - odis) * (t - ot) / (dis - odis); // linear interpolation for better accuracy
    return vec2(t, t2);
}*/

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

void main()
{
    if (type == 0 || type == 1) {
        vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
        vs_out.aPos = aPos;
        vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
        vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

        gl_Position = projection * view * model * (vec4(aPos, 1.0));
    }
    else {
       vs_out.aPos = aPos;
       vs_out.FragPos = vec3(model * vec4(vs_out.aPos, 1.0));
       //vs_out.aPos = aPos;
       vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
       vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
       //vs_out.TestCoord = view * (FragPos.)
       gl_Position = projection * view * model * (vec4(vs_out.aPos, 1.0));
    }
    /**/
}