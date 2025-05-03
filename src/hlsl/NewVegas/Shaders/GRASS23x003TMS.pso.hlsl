//
//
// Parameters:

float4 AlphaTestRef : register(c3);
sampler2D AttMap : register(s1);
sampler2D DiffuseMap : register(s0);
float4 PointLightColor : register(c2);
sampler2D ShadowMap : register(s2);
sampler2D ShadowMaskMap : register(s3);


// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   PointLightColor const_2       1
//   AlphaTestRef    const_3       1
//   DiffuseMap      texture_0       1
//   AttMap          texture_1       1
//   ShadowMap       texture_2       1
//   ShadowMaskMap   texture_3       1
//


// Structures:

struct VS_INPUT {
    float2 DiffuseUV : TEXCOORD0;			// partial precision
    float3 texcoord_4 : TEXCOORD4_centroid;			// partial precision
    float4 texcoord_5 : TEXCOORD5_centroid;			// partial precision
    float4 texcoord_1 : TEXCOORD1;			// partial precision
    float2 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float3 texcoord_3 : TEXCOORD3_centroid;			// partial precision
    float4 color_0 : COLOR0;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float1 att3;
    float1 att4;
    float3 q2;
    float3 q5;
    float4 r0;
    float1 t0;
    float3 t1;

    t0.x = tex2D(ShadowMaskMap, IN.texcoord_3.xy);			// partial precision
    t1.xyz = tex2D(ShadowMap, IN.texcoord_2.xy);			// partial precision
    r0.xyzw = tex2D(DiffuseMap, IN.DiffuseUV.xy);			// partial precision
    att4.x = tex2D(AttMap, IN.texcoord_1.zw);			// partial precision
    att3.x = tex2D(AttMap, IN.texcoord_1.xy);			// partial precision
    q2.xyz = (((IN.texcoord_3.z * ((t1.xyz - 1) * t0.x)) + 1) * IN.texcoord_5.xyz) + IN.texcoord_4.xyz;			// partial precision
    q5.xyz = (saturate((1 - att3.x) - att4.x) * (0.4 * PointLightColor.rgb)) + q2.xyz;
    OUT.color_0.a = (AlphaTestRef.x >= r0.w ? 0 : IN.texcoord_5.w);			// partial precision
    OUT.color_0.rgb = (IN.color_0.a * (IN.color_0.rgb - (r0.xyz * q5.xyz))) + (r0.xyz * q5.xyz);			// partial precision

    return OUT;
};

// approximately 21 instruction slots used (5 texture, 16 arithmetic)
