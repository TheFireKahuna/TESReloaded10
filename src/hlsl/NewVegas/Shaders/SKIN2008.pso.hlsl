//
//
// Parameters:

sampler2D AttenuationMap : register(s3);
sampler2D NormalMap : register(s0);
float4 PSLightColor[10] : register(c3);

float4 TESR_PBRData : register(c28);

#include "includes/Helpers.hlsl"


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   PSLightColor[0]   PSLightColor[0]       2
//   NormalMap      texture_0       1
//   AttenuationMap texture_3       1
//


// Structures:

struct VS_INPUT {
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float4 texcoord_4 : TEXCOORD4;			// partial precision
    float4 texcoord_5 : TEXCOORD5;			// partial precision
    float2 texcoord_0 : TEXCOORD0;			// partial precision
    float3 texcoord_7 : TEXCOORD7_centroid;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 sunColor = PSLightColor[0];
    sunColor.rgb *= TESR_PBRData.z;
    float4 lightColor1 = PSLightColor[1];
    lightColor1.rgb *= TESR_PBRData.z;

    const float4 const_0 = {-0.5, 1, 0.3, 0.769230783};
    const int4 const_1 = {-2, 3, 0, 0};
    const float4 const_2 = {0.3, 0, 0, 0};
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;
    float4 r6;
    float4 r7;

    r0.xyzw = tex2D(NormalMap, IN.texcoord_0.xy);			// partial precision
    r0.xyz = r0.xyz - 0.5;
    r0.xyz = 2 * r0.xyz;			// partial precision
    r1.xyz = normalize(r0.xyz);			// partial precision
    r0.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    r0.w = dot(r1.xyz, r0.xyz);
    r1.w = saturate(r0.w);			// partial precision
    r0.w = r0.w + 0.3;
    r0.w = saturate(r0.w * 0.769230783);
    r2.w = (r1.w * -2) + 3;
    r2.x = r1.w * r1.w;
    r2.x = r2.w * r2.x;
    r2.y = (r0.w * -2) + 3;
    r0.w = r0.w * r0.w;
    r0.w = saturate((r2.y * r0.w) - r2.x);			// partial precision
    r2.xyz = normalize(IN.texcoord_7.xyz);			// partial precision
    r2.w = saturate(dot(r1.xyz, r2.xyz));			// partial precision
    r2.w = 1 - r2.w;			// partial precision
    r2.w = r2.w * r2.w;			// partial precision
    r0.x = saturate(dot(r2.xyz, -r0.xyz));			// partial precision
    r0.x = r2.w * r0.x;			// partial precision
    r3.xyz = const_2.xyz;			// partial precision
    r3.w = -0.5;
    r4.xyz = lerp(lightColor1.rgb, r3.xyz, -r3.w);			// partial precision
    r0.xyz = r0.x * r4.xyz;			// partial precision
    r0.xyz = (r1.w * lightColor1.rgb) + r0.xyz;			// partial precision
    r0.xyz = (r0.w * const_2.xyz) + r0.xyz;			// partial precision
    r4.x = IN.texcoord_5.z;			// partial precision
    r4.y = IN.texcoord_5.w;			// partial precision
    r5.x = IN.texcoord_4.z;			// partial precision
    r5.y = IN.texcoord_4.w;			// partial precision
    r4.xyzw = tex2D(AttenuationMap, r4.xy);			// partial precision
    r6.xyzw = tex2D(AttenuationMap, IN.texcoord_5.xy);			// partial precision
    r5.xyzw = tex2D(AttenuationMap, r5.xy);			// partial precision
    r7.xyzw = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    r0.w = 1 - r6.x;			// partial precision
    r0.w = saturate(r0.w - r4.x);			// partial precision
    r0.xyz = r0.xyz * r0.w;			// partial precision
    r4.xyz = normalize(IN.texcoord_1.xyz);			// partial precision
    r0.w = saturate(dot(r2.xyz, -r4.xyz));			// partial precision
    r1.x = dot(r1.xyz, r4.xyz);
    r0.w = r2.w * r0.w;			// partial precision
    r1.yzw = lerp(sunColor.wzyx, r3.wzyx, -r3.w);			// partial precision
    r1.yzw = r0.w * r1.yzw;			// partial precision
    r0.w = saturate(r1.x);			// partial precision
    r1.x = r1.x + 0.3;
    r1.x = saturate(r1.x * 0.769230783);
    r1.yzw = (sunColor.wzyx * r0.w) + r1.yzw;			// partial precision
    r2.x = (r0.w * -2) + 3;
    r0.w = r0.w * r0.w;
    r0.w = r2.x * r0.w;
    r2.x = (r1.x * -2) + 3;
    r1.x = r1.x * r1.x;
    r0.w = saturate((r2.x * r1.x) - r0.w);			// partial precision
    r1.xyz = (r0.w * const_2.xyz) + r1.wzy;			// partial precision
    r0.w = 1 - r7.x;			// partial precision
    r0.w = saturate(r0.w - r5.x);			// partial precision
    r0.xyz = (r0.w * r1.xyz) + r0.xyz;			// partial precision
    r0.w = 1;			// partial precision
    OUT.color_0.rgba = r0.xyzw;			// partial precision

    return OUT;
};

// approximately 68 instruction slots used (5 texture, 63 arithmetic)
