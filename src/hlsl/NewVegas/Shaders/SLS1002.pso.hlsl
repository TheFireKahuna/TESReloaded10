//
//
// Parameters:

sampler2D NormalMap : register(s0);
float4 PSLightColor[10] : register(c3);
float4 TESR_LinearObjectColor : register(c4);
float4 TESR_ShaderBaseColors : register(c5);

#include "includes/Helpers.hlsl"


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   PSLightColor[0] const_3       1
//   NormalMap    texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 NormalUV : TEXCOORD0;
    float3 texcoord_1 : TEXCOORD1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float4 sunColor = linearCheck(PSLightColor[0], TESR_LinearObjectColor.x) * TESR_ShaderBaseColors.x;

    float3 noxel0 = tex2D(NormalMap, IN.NormalUV.xy).rgb;
    OUT.color_0.a = sunColor.a;
    OUT.color_0.rgb = shades(expand(noxel0.rgb), expand(IN.texcoord_1.xyz)) * sunColor.rgb;

    return OUT;
};

// approximately 9 instruction slots used (1 texture, 8 arithmetic)
