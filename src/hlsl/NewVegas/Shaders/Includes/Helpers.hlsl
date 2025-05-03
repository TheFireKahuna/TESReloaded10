#define	expand(v)	        (((v) - 0.5) / 0.5)  // from 0/1 to -1/1
#define	compress(v)         (((v) * 0.5) + 0.5)  // from -1/1 to 0/1 
#define	shade(n, l)         max(dot(n, l), 0)
#define	shades(n, l)        saturate(dot(n, l))
#define invlerp(a, b, t)    ((t-a)/(b-a))
#define invlerps(a, b, t)   saturate((t-a)/(b-a))
#define luma(color)         dot(color.rgb, float3(0.2126f, 0.7152f, 0.0722f)) // extract luminance (BT.709)
// #define luma(color)         dot(color, float3(0.3f, 0.59f, 0.11f)) // extract luminance (BT.601)
#define mix(colora, colorb) colora * colorb * 2 // mix two colors without darkening
#define	weight(v)           dot(v, 1)
#define	sqr(v)              ((v) * (v))
#define blendnormals(a, b)  float3(a.xy + b.xy, a.z)
#define rand(s)             (frac(sin(dot(s, float2(12.9898, 78.233))) * 43758.5453)) // pseudo random from https://gist.github.com/keijiro/ee7bc388272548396870
#define pows(a, b)          (pow(abs(a), b) * sign(a)) // no more pow/abs warning!
#define reconstructZ(normal)  (normalize(float3(normal.xy, sqrt(1 - saturate(dot(normal.xy, normal.xy)))))) // derives the Z component of a normal
#define bend(a, b)           (a * (1 + b) / (1 + a * b)) //bends the response curve in a symetric way 
#define scaledReinhard(a, b) ((a * b) / (1 + a * b)) // curve that reaches a maximum of 1 with a speed b
#define	envreflect(i, n)	((2 * dot(i, n)) * (i)) - ((n) * dot(i, i))

#define PI 3.1415926538
#define white     float4 (1, 1, 1, 1)
#define grey      float4 (0.5, 0.5, 0.5, 1)
#define black     float4 (0, 0, 0, 1)
#define red       float4 (1, 0, 0, 1)
#define green     float4 (0, 1, 0, 1)
#define blue      float4 (0, 0, 1, 1)
#define yellow    float4 (1, 1, 0, 1)
#define cyan      float4 (0, 1, 1, 1)
#define magenta   float4 (1, 0, 1, 1)

float3 linearize(float3 color) {
    float3 linearRGBLo = color / 12.92;
    float3 linearRGBHi = pow((color + 0.055) / 1.055, 2.4);
    float3 linearRGB = (color <= 0.04045) ? linearRGBLo : linearRGBHi;
    return linearRGB;
}
float4 linearize(float4 color) {
    float3 linearRGBLo = color.rgb / 12.92;
    float3 linearRGBHi = pow((color.rgb + 0.055) / 1.055, 2.4);
    float3 linearRGB = (color.rgb <= 0.04045) ? linearRGBLo : linearRGBHi;
    return float4(linearRGB, color.a);
}
float3 delinearize(float3 color) {
    float3 sRGBLo = color * 12.92;
    float3 sRGBHi = (pow(abs(color), 1.0 / 2.4) * 1.055) - 0.055;
    float3 sRGB = (color <= 0.0031308) ? sRGBLo : sRGBHi;
    return sRGB;
}
float4 delinearize(float4 color) {
    float3 sRGBLo = color.rgb * 12.92;
    float3 sRGBHi = (pow(abs(color.rgb), 1.0 / 2.4) * 1.055) - 0.055;
    float3 sRGB = (color.rgb <= 0.0031308) ? sRGBLo : sRGBHi;
    return float4(sRGB, color.a);
}

float4 selectColor(float selector, float4 color0, float4 color1, float4 color2, float4 color3, float4 color4, float4 color5, float4 color6, float4 color7, float4 color8, float4 color9){
    if (selector == 0.0) return color0;
    if (selector >= 0.1 && selector < 0.2) return color1;
    if (selector >= 0.2 && selector < 0.3) return color2;
    if (selector >= 0.3 && selector < 0.4) return color3;
    if (selector >= 0.4 && selector < 0.5) return color4;
    if (selector >= 0.5 && selector < 0.6) return color5;
    if (selector >= 0.6 && selector < 0.7) return color6;
    if (selector >= 0.7 && selector < 0.8) return color7;
    if (selector >= 0.8 && selector < 0.9) return color8;
    if (selector >= 0.9 && selector < 1.0) return color9;
    return black;
}

float3 selectColor(float selector, float3 color0, float3 color1, float3 color2, float3 color3, float3 color4, float3 color5, float3 color6, float3 color7, float3 color8, float3 color9){
    if (selector == 0.0) return color0;
    if (selector >= 0.1 && selector < 0.2) return color1;
    if (selector >= 0.2 && selector < 0.3) return color2;
    if (selector >= 0.3 && selector < 0.4) return color3;
    if (selector >= 0.4 && selector < 0.5) return color4;
    if (selector >= 0.5 && selector < 0.6) return color5;
    if (selector >= 0.6 && selector < 0.7) return color6;
    if (selector >= 0.7 && selector < 0.8) return color7;
    if (selector >= 0.8 && selector < 0.9) return color8;
    if (selector >= 0.9 && selector < 1.0) return color9;
    return black.rgb;
}

float3x3 inverse3x3( float3x3 M ) {
    // The original was written in HLSL, but this is GLSL, 
    // therefore 
    // - the array index selects columns, so M_t[0] is the 
    // first column of M_t, etc. 
    // - the float3x3 constructor assembles columns, so 
    // cross( M_t[1], M_t[2] ) becomes the first column 
    // of the adjugate, etc. 
    // - for the determinant, it does not matter whether it is 
    // computed with M or with M_t; but using M_t makes it 
    // easier to follow the derivation in the text
    float3x3 M_t = transpose( M ); 
    float det = dot( cross( M_t[0], M_t[1] ), M_t[2] ); 
    float3x3 adjugate = float3x3( cross( M_t[1], M_t[2] ), 
    cross( M_t[2], M_t[0] ), cross( M_t[0], M_t[1] ) ); 
    return adjugate / det;
}
float3x3 cotangent_frame( float3 N, float3 p, float2 uv )
{
    // get edge vectors of the pixel triangle
    float3 dp1 = ddx( p );
    float3 dp2 = ddy( p );
    float2 duv1 = ddx( uv );
    float2 duv2 = ddy( uv );
 
    // solve the linear system
    float3 dp2perp = cross( dp2, N );
    float3 dp1perp = cross( N, dp1 );
    float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    float3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = rsqrt( max( dot(T,T), dot(B,B) ) );
    return float3x3( T * invmax, B * invmax, N );
}
float3 perturb_normal( float3 N, float3 V, float2 texcoord, float3 normalMap )
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    float3 map = normalize(expand(normalMap));
    /*
#ifdef WITH_NORMALMAP_UNSIGNED
    map = map * 255.0f/127.0f - 128.0f/127.0f;
#endif
#ifdef WITH_NORMALMAP_2CHANNEL
    map.z = sqrt( 1.0f - dot( map.xy, map.xy ) );
#endif
#ifdef WITH_NORMALMAP_GREEN_UP
    map.y = -map.y;
#endif
*/
    float3x3 TBN = cotangent_frame( N, -V, texcoord );
    return normalize( mul(TBN, map) );
}