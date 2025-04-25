//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
float4 Toggles : register(c27);
float4 TESR_PBRData : register(c32);
float4 TESR_PBRExtraData : register(c33);

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
    float3 color = IN.color_1.rgb;

    float3 ambientColor = AmbientColor.rgb  * TESR_PBRData.w;

    float4 baseColor = tex2D(BaseMap, IN.BaseUV);			// partial precision

    clip((AmbientColor.a >= 1.0 ? 0.0 : 1.0) * (baseColor.w - Toggles.w));

    float4 finalColor;
    
    finalColor.rgb = lerp(ambientColor, color, IN.color_1.a);			// partial precision
    finalColor.w = baseColor.w * AmbientColor.a;			// partial precision
    OUT.color_0 = finalColor;			// partial precision

    return OUT;
};

// approximately 10 instruction slots used (1 texture, 9 arithmetic)
