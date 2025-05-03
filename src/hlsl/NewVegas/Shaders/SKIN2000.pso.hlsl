//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
sampler2D FaceGenMap0 : register(s2);
sampler2D FaceGenMap1 : register(s3);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10] : register(c3);
float4 Toggles : register(c27);
float4 TESR_PBRData : register(c28);

#include "includes/Helpers.hlsl"

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   PSLightColor[0] const_3       1
//   Toggles      const_27      1
//   BaseMap      texture_0       1
//   NormalMap    texture_1       1
//   FaceGenMap0  texture_2       1
//   FaceGenMap1  texture_3       1
//


// Structures:

struct VS_INPUT {
    float2 texcoord_0 : TEXCOORD0;			// partial precision
    float3 texcoord_6 : TEXCOORD6;			// partial precision
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float3 texcoord_1 : TEXCOORD1;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float3 ambientColor = AmbientColor.rgb * TESR_PBRData.w;
    float4 sunColor = PSLightColor[0];
    sunColor.rgb *= TESR_PBRData.z;

    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;

    const float4 const_0 = {-0.5, 2, 1, 0.5};
    const int4 const_2 = {0, 1, 0, 0};
    r0.xyzw = tex2D(BaseMap, IN.texcoord_0.xy);			// partial precision
    r0 = r0;
    r1.w = 1;
    r1.x = AmbientColor.a - r1.w;
    r1.x = (r1.x >= 0.0 ? 0 : 1);
    r1.y = r0.w - Toggles.w;
    r1.xyzw = r1.x * r1.y;
    clip(r1.xyzw);
    r1.xyzw = tex2D(NormalMap, IN.texcoord_0.xy);			// partial precision
    r2.xyzw = tex2D(FaceGenMap0, IN.texcoord_0.xy);			// partial precision
    r3.xyzw = tex2D(FaceGenMap1, IN.texcoord_0.xy);			// partial precision
    r1.xyz = r1.xyz - 0.5;
    r1.xyz = 2 * r1.xyz;			// partial precision
    r4.xyz = normalize(r1.xyz);			// partial precision
    r1.xyz = normalize(IN.texcoord_6.xyz);			// partial precision
    r1.w = saturate(dot(r4.xyz, r1.xyz));			// partial precision
    r2.w = saturate(dot(r4.xyz, IN.texcoord_1.xyz));			// partial precision
    r3.w = saturate(dot(r1.xyz, -IN.texcoord_1));			// partial precision
    r1.x = 1 - r1.w;			// partial precision
    r1.x = r1.x * r1.x;			// partial precision
    r3.w = r3.w * r1.x;			// partial precision
    r1.xyz = r3.w * sunColor.rgb;			// partial precision
    r1.xyz = r1.xyz * 0.5;			// partial precision
    r1.xyz = (r2.w * sunColor.rgb) + r1.xyz;			// partial precision
    r1.xyz = r1.xyz + ambientColor;			// partial precision
    r4.xyz = max(r1.xyz, 0);			// partial precision
    r1.xyz = r2.xyz - 0.5;			// partial precision
    r0.xyz = (2 * r1.xyz) + r0.xyz;			// partial precision
    r1.w = r0.w * AmbientColor.a;			// partial precision
    r2.xyz = 2 * r3.xyz;			// partial precision
    r0.xyz = r0.xyz * r2.xyz;			// partial precision
    r0.xyz = 2 * r0.xyz;			// partial precision
    r2.xyz = r0.xyz * IN.color_0.rgb;			// partial precision
    r0.xyz = (Toggles.x <= 0.0 ? r0.xyz : r2.xyz);			// partial precision
    r2.xyz = (-r0.xyz * r4.xyz) + IN.color_1.rgb;			// partial precision
    r0.xyz = r4.xyz * r0.xyz;			// partial precision
    r2.xyz = (IN.color_1.a * r2.xyz) + r0.xyz;			// partial precision
    r1.xyz = (Toggles.y <= 0.0 ? r0.xyz : r2.xyz);			// partial precision
    OUT.color_0.rgba = r1.xyzw;			// partial precision

    return OUT;
};

// approximately 42 instruction slots used (4 texture, 38 arithmetic)
