//
//
// Parameters:

sampler2D AttMapXY : register(s1);
sampler2D AttMapZ : register(s2);
samplerCUBE NormalCubeMap : register(s3);
sampler2D NormalMap : register(s0);
float4 PSLightColor[10] : register(c3);
float4 TESR_LinearObject : register(c28);
float4 TESR_PBRData : register(c29);
float4 TESR_LinearObjectColor : register(c30);
float4 TESR_ShaderBaseColors : register(c32);

#include "includes/Helpers.hlsl"

// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   PSLightColor[0]  const_3       1
//   NormalMap     texture_0       1
//   AttMapXY      texture_1       1
//   AttMapZ       texture_2       1
//   NormalCubeMap texture_3       1
//


// Structures:

struct VS_INPUT {
    float2 NormalUV : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
    float2 texcoord_2 : TEXCOORD2;
    float3 texcoord_3 : TEXCOORD3;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 sunColor = linearCheck(PSLightColor[0].rgb, TESR_LinearObjectColor.x)  * TESR_ShaderBaseColors.x;

    float3 noxel2 = texCUBE(NormalCubeMap, IN.texcoord_3).rgb;
    float3 noxel3 = tex2D(NormalMap, IN.NormalUV).rgb;
    float3 att0 = tex2D(AttMapZ, IN.texcoord_2);
    float3 att1 = tex2D(AttMapXY, IN.texcoord_1);
    float3 finalColor = (shades(expand(noxel3), expand(noxel2)) * sunColor) * (att1 * att0);
    OUT.color_0.a = PSLightColor[0].a;
    OUT.color_0.rgb = finalColor;

    return OUT;
};

// approximately 14 instruction slots used (4 texture, 10 arithmetic)
