//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D AttenuationMap : register(s5);
sampler2D BaseMap : register(s0);
sampler2D FaceGenMap0 : register(s2);
sampler2D FaceGenMap1 : register(s3);
sampler2D GlowMap : register(s4);
sampler2D NormalMap : register(s1);
float4 PSLightColor[2] : register(c3);
float4 Toggles : register(c27);
float4 TESR_PBRData : register(c28);

#include "includes/Helpers.hlsl"

// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   AmbientColor   const_1       1
//   PSLightColor[0]   PSLightColor[0]       2
//   Toggles        const_27      1
//   BaseMap        texture_0       1
//   NormalMap      texture_1       1
//   FaceGenMap0    texture_2       1
//   FaceGenMap1    texture_3       1
//   GlowMap        texture_4       1
//   AttenuationMap texture_5       1
//


// Structures:

struct VS_INPUT {
    float2 texcoord_0 : TEXCOORD0;			// partial precision
    float3 texcoord_6 : TEXCOORD6_centroid;			// partial precision
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float4 texcoord_4 : TEXCOORD4;			// partial precision
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
    float4 lightColor1 = PSLightColor[1];
    lightColor1.rgb *= TESR_PBRData.z;

    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;
    float4 r6;
    float4 r7;
    float4 r8;

    const float4 const_0 = {-0.5, 2, 1, 0.5};
    const int4 const_2 = {0, 1, 0, 0};
    const float4 const_5 = {0.3, 0.769230783, -2, 3};
    r0.xyzw = tex2D(BaseMap, IN.texcoord_0.xy);			// partial precision
    r1.zw = const_0.zw;
    r1.x = AmbientColor.a - r1.z;
    r1.x = (r1.x >= 0.0 ? 0 : 1);
    r1.y = r0.w - Toggles.w;
    r2.xyzw = r1.x * r1.y;
    r1.x = IN.texcoord_4.z;			// partial precision
    r1.y = IN.texcoord_4.w;			// partial precision
    clip(r2.xyzw);
    r2.xyzw = tex2D(NormalMap, IN.texcoord_0.xy);			// partial precision
    r3.xyzw = tex2D(GlowMap, IN.texcoord_0.xy);			// partial precision
    r4.xyzw = tex2D(AttenuationMap, r1.xy);			// partial precision
    r5.xyzw = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    r6.xyzw = tex2D(FaceGenMap0, IN.texcoord_0.xy);			// partial precision
    r7.xyzw = tex2D(FaceGenMap1, IN.texcoord_0.xy);			// partial precision
    r1.xyz = r2.xyz - 0.5;
    r1.xyz = 2 * r1.xyz;			// partial precision
    r2.xyz = normalize(r1.xyz);			// partial precision
    r1.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    r2.w = dot(r2.xyz, r1.xyz);
    r3.w = saturate(r2.w);			// partial precision
    r2.w = r2.w + 0.3;
    r2.w = saturate(r2.w * 0.769230783);
    r6.w = (r3.w * -2) + 3;
    r7.w = r3.w * r3.w;
    r6.w = r6.w * r7.w;
    r7.w = (r2.w * -2) + 3;
    r2.w = r2.w * r2.w;
    r2.w = saturate((r7.w * r2.w) - r6.w);			// partial precision
    r8.xyz = normalize(IN.texcoord_6.xyz);			// partial precision
    r6.w = saturate(dot(r2.xyz, r8.xyz));			// partial precision
    r7.w = saturate(dot(r2.xyz, IN.texcoord_1.xyz));			// partial precision
    r6.w = 1 - r6.w;			// partial precision
    r6.w = r6.w * r6.w;			// partial precision
    r8.w = saturate(dot(r8.xyz, -r1.xyz));			// partial precision
    r1.x = saturate(dot(r8.xyz, -IN.texcoord_1));			// partial precision
    r1.x = r6.w * r1.x;			// partial precision
    r6.w = r6.w * r8.w;			// partial precision
    r1.xyz = r1.x * sunColor.rgb;			// partial precision
    r1.xyz = r1.xyz * 0.5;			// partial precision
    r1.xyz = (r7.w * sunColor.rgb) + r1.xyz;			// partial precision
    r2.xyz = lerp(lightColor1.rgb, r3.xyz, r1.w);			// partial precision
    r2.xyz = r6.w * r2.xyz;			// partial precision
    r2.xyz = (r3.w * lightColor1.rgb) + r2.xyz;			// partial precision
    r2.xyz = (r2.w * r3.xyz) + r2.xyz;			// partial precision
    r1.w = 1 - r5.x;			// partial precision
    r1.w = saturate(r1.w - r4.x);			// partial precision
    r1.xyz = (r1.w * r2.xyz) + r1.xyz;			// partial precision
    r1.xyz = r1.xyz + ambientColor;			// partial precision
    r2.xyz = max(r1.xyz, 0);			// partial precision
    r1.xyz = r6.xyz - 0.5;			// partial precision
    r0.xyz = (2 * r1.xyz) + r0.xyz;			// partial precision
    r1.w = r0.w * AmbientColor.a;			// partial precision
    r3.xyz = 2 * r7.xyz;			// partial precision
    r0.xyz = r0.xyz * r3.xyz;			// partial precision
    r0.xyz = 2 * r0.xyz;			// partial precision
    r3.xyz = r0.xyz * IN.color_0.rgb;			// partial precision
    r0.xyz = (Toggles.x <= 0.0 ? r0.xyz : r3.xyz);			// partial precision
    r3.xyz = (-r0.xyz * r2.xyz) + IN.color_1.rgb;			// partial precision
    r0.xyz = r2.xyz * r0.xyz;			// partial precision
    r2.xyz = (IN.color_1.a * r3.xyz) + r0.xyz;			// partial precision
    r1.xyz = (Toggles.y <= 0.0 ? r0.xyz : r2.xyz);			// partial precision
    OUT.color_0.rgba = r1.xyzw;			// partial precision

    return OUT;
};

// approximately 69 instruction slots used (7 texture, 62 arithmetic)
