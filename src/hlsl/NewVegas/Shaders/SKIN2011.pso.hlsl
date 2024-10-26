//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
float4 EmittanceColor : register(c2);
sampler2D FaceGenMap0 : register(s2);
sampler2D FaceGenMap1 : register(s3);
sampler2D NormalMap : register(s1);
float4 PSLightColor[4] : register(c3);
float4 PSLightPosition[8] : register(c19);
float4 Toggles : register(c27);
float4 TESR_LinearObject : register(c28);
float4 TESR_ShaderBaseColors : register(c29);
float4 TESR_LinearObjectExtra : register(c30);

#include "includes/Helpers.hlsl"


// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   AmbientColor    const_1       1
//   EmittanceColor  const_2       1
//   PSLightColor[0]    PSLightColor[0]       4
//   PSLightPosition[0] PSLightPosition[0]      3
//   Toggles         const_27      1
//   BaseMap         texture_0       1
//   NormalMap       texture_1       1
//   FaceGenMap0     texture_2       1
//   FaceGenMap1     texture_3       1
//


// Structures:

struct VS_INPUT {
    float3 texcoord_3 : TEXCOORD3;
    float3 texcoord_4 : TEXCOORD4;
    float3 texcoord_5 : TEXCOORD5;
    float3 texcoord_6 : TEXCOORD6;
    float2 texcoord_0 : TEXCOORD0;
    float3 texcoord_1 : TEXCOORD1;
    float3 texcoord_2 : TEXCOORD2;
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 ambientColor = AmbientColor.rgb * TESR_ShaderBaseColors.y;
    float4 sunColor = PSLightColor[0];
    sunColor.rgb *= TESR_ShaderBaseColors.x;
    float4 lightColor1 = PSLightColor[1];
    float4 lightColor2 = PSLightColor[2];
    float4 lightColor3 = PSLightColor[3];
    lightColor1.rgb *= TESR_ShaderBaseColors.z;
    lightColor2.rgb *= TESR_ShaderBaseColors.z;
    lightColor3.rgb *= TESR_ShaderBaseColors.z;

    const float4 const_0 = {-0.5, 2, 1, 0.3};
    const float4 const_7 = {0.769230783, -2, 3, 0};
    const float4 const_8 = {0.3, 0, 0, 1};

    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;
    float4 r6;
    float4 r7;
    float4 r8 = 0;
    float4 r9;

    r0.xyzw = tex2D(BaseMap, IN.texcoord_0.xy);			// partial precision
    r0 = linearCheck(r0, TESR_LinearObject.y);
    r1.xyz = const_0.xyz;
    r1.w = AmbientColor.a - r1.z;
    r1.w = (r1.w >= 0.0 ? 0 : 1);
    r2.w = r0.w - Toggles.w;
    r2.xyzw = r1.w * r2.w;
    clip(r2.xyzw);
    r2.xyzw = tex2D(NormalMap, IN.texcoord_0.xy);			// partial precision
    r3.xyzw = tex2D(FaceGenMap0, IN.texcoord_0.xy);			// partial precision
    //r3 = linearCheck(r3, TESR_LinearObject.y);
    r4.xyzw = tex2D(FaceGenMap1, IN.texcoord_0.xy);			// partial precision
    //r4 = linearCheck(r4, TESR_LinearObject.y);
    r5.xyz = PSLightPosition[0].xyz - IN.texcoord_1.xyz;
    r1.w = 1.0 / PSLightPosition[0].w;
    r5.xyz = r5.xyz * r1.w;			// partial precision
    r5.x = saturate(dot(r5.xyz, r5.xyz));			// partial precision
    r6.xyz = PSLightPosition[1].xyz - IN.texcoord_1.xyz;
    r1.w = 1.0 / PSLightPosition[1].w;
    r6.xyz = r6.xyz * r1.w;			// partial precision
    r5.y = saturate(dot(r6.xyz, r6.xyz));			// partial precision
    r6.xyz = PSLightPosition[2].xyz - IN.texcoord_1.xyz;
    r1.w = 1.0 / PSLightPosition[2].w;
    r6.xyz = r6.xyz * r1.w;			// partial precision
    r5.z = saturate(dot(r6.xyz, r6.xyz));			// partial precision
    r5.xyz = 1 - r5.xyz;			// partial precision
    r2.xyz = r2.xyz - 0.5;
    r2.xyz = 2 * r2.xyz;			// partial precision
    r6.xyz = normalize(r2.xyz);			// partial precision
    r2.xyz = normalize(IN.texcoord_4.xyz);			// partial precision
    r1.w = dot(r6.xyz, r2.xyz);
    r2.w = saturate(r1.w);			// partial precision
    r1.w = r1.w + 0.3;
    r1.w = saturate(r1.w * 0.769230783);
    r3.w = (r2.w * -2) + 3;
    r4.w = r2.w * r2.w;
    r3.w = r3.w * r4.w;
    r4.w = (r1.w * -2) + 3;
    r1.w = r1.w * r1.w;
    r1.w = saturate((r4.w * r1.w) - r3.w);			// partial precision
    r7.xyz = const_8.xyz;			// partial precision
    r8.xyz = lerp(lightColor1.rgb, r7.xyz, -r1.x);			// partial precision
    r9.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    r3.w = saturate(dot(r9.xyz, -r2.xyz));			// partial precision
    r4.w = saturate(dot(r6.xyz, r9.xyz));			// partial precision
    r4.w = 1 - r4.w;			// partial precision
    r4.w = r4.w * r4.w;			// partial precision
    r3.w = r3.w * r4.w;			// partial precision
    r2.xyz = r8.xyz * r3.w;			// partial precision
    r2.xyz = (r2.w * lightColor1.rgb) + r2.xyz;			// partial precision
    r2.xyz = (r1.w * const_8.xyz) + r2.xyz;			// partial precision
    r1.w = saturate(dot(r6.xyz, IN.texcoord_3.xyz));			// partial precision
    r8.xyz = r1.w * sunColor.rgb;			// partial precision
    r2.xyz = (r5.x * r2.xyz) + r8.xyz;			// partial precision
    r2.w = r1.z - EmittanceColor.a;
    r2.xyz = (r2.w >= 0.0 ? r8.xyz : r2.xyz);			// partial precision
    r8.xyz = normalize(IN.texcoord_5.xyz);			// partial precision
    r2.w = dot(r6.xyz, r8.xyz);
    r3.w = saturate(dot(r9.xyz, -r8.xyz));			// partial precision
    r3.w = r4.w * r3.w;			// partial precision
    r6.w = saturate(r2.w);			// partial precision
    r2.w = r2.w + 0.3;
    r2.w = saturate(r2.w * 0.769230783);
    r7.w = (r6.w * -2) + 3;
    r9.w = r6.w * r6.w;
    r7.w = r7.w * r9.w;
    r9.w = (r2.w * -2) + 3;
    r2.w = r2.w * r2.w;
    r2.w = saturate((r9.w * r2.w) - r7.w);			// partial precision
    r8.xyz = lerp(lightColor2.rgb, r7.xyz, -r1.x);			// partial precision
    r8.xyz = r3.w * r8.xyz;			// partial precision
    r8.xyz = (r6.w * lightColor2.rgb) + r8.xyz;			// partial precision
    r8.xyz = (r2.w * const_8.xyz) + r8.xyz;			// partial precision
    r8.xyz = (r5.y * r8.xyz) + r2.xyz;			// partial precision
    r2.w = r1.y - EmittanceColor.a;
    r1.yzw = (r2.w >= 0.0 ? r2.wzyx : r8.wzyx);			// partial precision
    r2.xyz = normalize(IN.texcoord_6.xyz);			// partial precision
    r2.w = saturate(dot(r9.xyz, -r2.xyz));			// partial precision
    r3.w = dot(r6.xyz, r2.xyz);
    r4.w = r4.w * r2.w;			// partial precision
    r2.xyz = lerp(lightColor3.rgb, r7.xyz, -r1.x);			// partial precision
    r2.xyz = r4.w * r2.xyz;			// partial precision
    r2.w = saturate(r3.w);			// partial precision
    r3.w = r3.w + 0.3;
    r3.w = saturate(r3.w * 0.769230783);
    r2.xyz = (r2.w * lightColor3.rgb) + r2.xyz;			// partial precision
    r4.w = (r2.w * -2) + 3;
    r2.w = r2.w * r2.w;
    r2.w = r4.w * r2.w;
    r4.w = (r3.w * -2) + 3;
    r3.w = r3.w * r3.w;
    r2.w = saturate((r4.w * r3.w) - r2.w);			// partial precision
    r2.xyz = (r2.w * const_8.xyz) + r2.xyz;			// partial precision
    r2.xyz = (r5.z * r2.xyz) + r1.wzy;			// partial precision
    r2.w = 3;
    r2.w = r2.w - EmittanceColor.a;
    r1.xyz = (r2.w >= 0.0 ? r1.wzyx : r2.xyz);			// partial precision
    r1.xyz = r1.xyz + ambientColor;			// partial precision
    r2.xyz = max(r1.xyz, 0);			// partial precision
    r1.xyz = r3.xyz - 0.5;			// partial precision
    r0.xyz = (2 * r1.xyz) + r0.xyz;			// partial precision
    r1.w = r0.w * AmbientColor.a;			// partial precision
    r3.xyz = 2 * r4.xyz;			// partial precision
    r0.xyz = r0.xyz * r3.xyz;			// partial precision
    r0.xyz = 2 * r0.xyz;			// partial precision
    r3.xyz = r0.xyz * IN.color_0.rgb;			// partial precision
    r0.xyz = (Toggles.x <= 0.0 ? r0.xyz : r3.xyz);			// partial precision
    r3.xyz = (-r0.xyz * r2.xyz) + IN.color_1.rgb;
    r0.xyz = r2.xyz * r0.xyz;			// partial precision
    r2.xyz = (IN.color_1.a * r3.xyz) + r0.xyz;			// partial precision
    r1.xyz = (Toggles.y <= 0.0 ? r0.xyz : r2.xyz);			// partial precision
    OUT.color_0.rgba = r1.xyzw;			// partial precision

    return OUT;
};

// approximately 119 instruction slots used (4 texture, 115 arithmetic)
