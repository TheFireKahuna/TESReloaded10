//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D AttenuationMap : register(s4);
sampler2D BaseMap : register(s0);
sampler2D GlowMap : register(s3);
sampler2D NormalMap : register(s1);
float4 PSLightColor[3] : register(c3);
float4 TESR_PBRData : register(c28);

#include "includes/Helpers.hlsl"

// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   AmbientColor   const_1       1
//   PSLightColor[0]   PSLightColor[0]       3
//   BaseMap        texture_0       1
//   NormalMap      texture_1       1
//   GlowMap        texture_3       1
//   AttenuationMap texture_4       1
//


// Structures:

struct VS_INPUT {
    float3 texcoord_1 : TEXCOORD1;			// partial precision
    float3 texcoord_2 : TEXCOORD2;			// partial precision
    float3 texcoord_3 : TEXCOORD3;			// partial precision
    float4 texcoord_4 : TEXCOORD4;			// partial precision
    float4 texcoord_5 : TEXCOORD5;			// partial precision
    float2 texcoord_0 : TEXCOORD0;			// partial precision
    float3 texcoord_7 : TEXCOORD7;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const float4 const_0 = {-0.5, 1, 0.5, 0.3};
    const float4 const_2 = {0.769230783, -2, 3, 0};
    float3 ambientColor = AmbientColor.rgb * TESR_PBRData.w;
    float4 sunColor = PSLightColor[0];
    sunColor.rgb *= TESR_PBRData.z;
    float4 lightColor1 = PSLightColor[1];
    float4 lightColor2 = PSLightColor[2];
    lightColor1.rgb *= TESR_PBRData.z;
    lightColor2.rgb *= TESR_PBRData.z;

    float4 r0;
    float4 r1;
    float4 r2 = 0;
    float4 r3 = 0;
    float4 r4;
    float4 r5;
    float4 r6;
    float4 r7;
    float4 r8;

    r0.x = IN.texcoord_4.z;			// partial precision
    r0.y = IN.texcoord_4.w;			// partial precision
    r1.x = IN.texcoord_5.z;			// partial precision
    r1.y = IN.texcoord_5.w;			// partial precision
    r1.xyzw = tex2D(AttenuationMap, r1.xy);			// partial precision
    r4.xyzw = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    r5.xyzw = tex2D(NormalMap, IN.texcoord_0.xy);			// partial precision
    r6.xyzw = tex2D(GlowMap, IN.texcoord_0.xy);			// partial precision
    r2.xyzw = tex2D(AttenuationMap, r2.xy);			// partial precision
    r7.xyzw = tex2D(AttenuationMap, IN.texcoord_5.xy);			// partial precision
    r8.xyzw = tex2D(BaseMap, IN.texcoord_0.xy);			// partial precision
    r3.w = 1 - r2.x;			// partial precision
    r3.w = saturate(r3.w - r0.x);			// partial precision
    r0.xyz = r3.xyz - 0.5;
    r0.xyz = 2 * r0.xyz;			// partial precision
    r2.xyz = normalize(r0.xyz);			// partial precision
    r0.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    r0.w = dot(r2.xyz, r0.xyz);
    r2.w = saturate(r0.w);			// partial precision
    r0.w = r0.w + 0.3;
    r0.w = saturate(r0.w * 0.769230783);
    r4.w = (r2.w * -2) + 3;
    r1.y = r2.w * r2.w;
    r4.w = r4.w * r1.y;
    r1.y = (r0.w * -2) + 3;
    r0.w = r0.w * r0.w;
    r0.w = saturate((r1.y * r0.w) - r4.w);			// partial precision
    r3.xyz = normalize(IN.texcoord_7.xyz);			// partial precision
    r4.w = saturate(dot(r3.xyz, -r0.xyz));			// partial precision
    r0.x = saturate(dot(r2.xyz, r3.xyz));			// partial precision
    r0.x = 1 - r0.x;			// partial precision
    r0.x = r0.x * r0.x;			// partial precision
    r4.w = r4.w * r0.x;			// partial precision
    r0.z = 0.5;
    r1.yzw = lerp(lightColor1.wzyx, r4.wzyx, r0.z);			// partial precision
    r1.yzw = r4.w * r1.yzw;			// partial precision
    r1.yzw = (lightColor1.wzyx * r2.w) + r1.yzw;			// partial precision
    r1.yzw = (r0.w * r4.wzyx) + r1.yzw;			// partial precision
    r2.w = saturate(dot(r2.xyz, IN.texcoord_1.xyz));			// partial precision
    r4.w = saturate(dot(r3.xyz, -IN.texcoord_1));			// partial precision
    r4.w = r0.x * r4.w;			// partial precision
    r5.yzw = r4.w * sunColor.wzyx;			// partial precision
    r5.yzw = r5.yzw * 0.5;			// partial precision
    r5.yzw = (sunColor.wzyx * r2.w) + r5.yzw;			// partial precision
    r1.yzw = (r3.w * r1.yzw) + r5.yzw;			// partial precision
    r2.w = 1 - r5.x;			// partial precision
    r2.w = saturate(r2.w - r1.x);			// partial precision
    r5.xyz = normalize(IN.texcoord_3.xyz);			// partial precision
    r4.w = saturate(dot(r3.xyz, -r5.xyz));			// partial precision
    r0.y = dot(r2.xyz, r5.xyz);
    r4.w = r0.x * r4.w;			// partial precision
    r2.xyz = lerp(lightColor2.rgb, r4.xyz, r0.z);			// partial precision
    r2.xyz = r4.w * r2.xyz;			// partial precision
    r4.w = saturate(r0.y);			// partial precision
    r0.x = r0.y + 0.3;
    r0.x = saturate(r0.x * 0.769230783);
    r0.yzw = (lightColor2.wzyx * r4.w) + r2.wzyx;			// partial precision
    r1.x = (r4.w * -2) + 3;
    r4.w = r4.w * r4.w;
    r4.w = r1.x * r4.w;
    r1.x = (r0.x * -2) + 3;
    r0.x = r0.x * r0.x;
    r4.w = saturate((r1.x * r0.x) - r4.w);			// partial precision
    r0.xyz = (r4.w * r4.xyz) + r0.wzy;			// partial precision
    r0.xyz = (r2.w * r0.xyz) + r1.wzy;			// partial precision
    r6.xyz = r0.xyz + ambientColor;			// partial precision
    OUT.color_0.rgba = r6.xyzw;			// partial precision

    return OUT;
};

// approximately 75 instruction slots used (7 texture, 68 arithmetic)
