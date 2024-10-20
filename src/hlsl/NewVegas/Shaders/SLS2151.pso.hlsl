//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
float4 Toggles : register(c27);
float4 TESR_LinearObject : register(c28);
float4 TESR_PBRData : register(c29);
float4 TESR_LinearObjectColor : register(c30);
float4 TESR_LinearObjectTerrain : register(c31);
float4 TESR_ShaderBaseColors : register(c32);

#include "includes/Helpers.hlsl"


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   Toggles      const_27      1
//   BaseMap      texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;			// partial precision
    float4 color_1 : COLOR1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float3 color = linearCheck(IN.color_1.rgb, TESR_LinearObjectTerrain.y);

    float3 ambientColor = linearCheck(AmbientColor.rgb, TESR_LinearObjectColor.x)  * TESR_ShaderBaseColors.y;

    float4 baseColor = tex2D(BaseMap, IN.BaseUV);			// partial precision
    baseColor = linearCheck(baseColor, TESR_LinearObject.y);

    clip((AmbientColor.a >= 1.0 ? 0.0 : 1.0) * (baseColor.w - Toggles.w));

    float4 finalColor;
    
    finalColor.rgb = lerp(ambientColor, color, IN.color_1.a);			// partial precision
    finalColor.w = baseColor.w * AmbientColor.a;			// partial precision
    OUT.color_0 = finalColor;			// partial precision

    return OUT;
};

// approximately 10 instruction slots used (1 texture, 9 arithmetic)
