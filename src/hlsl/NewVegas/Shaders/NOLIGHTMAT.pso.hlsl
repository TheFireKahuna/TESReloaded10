//
//
// Parameters:

float4 FogColor : register(c2);
float4 MaterialColor : register(c0);
float4 Toggles : register(c1);
float4 TESR_LinearObjectExtra : register(c3);

#include "includes/Helpers.hlsl"

// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   MaterialColor const_0       1
//   Toggles       const_1       1
//   FogColor      const_2       1
//


// Structures:

struct VS_INPUT {
    float2 texcoord_0 : TEXCOORD0;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float4 materialColor = linearCheck(MaterialColor, TESR_LinearObjectExtra.x);
    float3 fogColor = linearCheck(FogColor.rgb, TESR_LinearObjectExtra.z);

    float3 blendColorA = (IN.texcoord_0.y * (fogColor - materialColor.rgb)) + materialColor.rgb;			// partial precision
    float3 blendColorB = lerp(blendColorA, materialColor.rgb - (IN.texcoord_0.y * materialColor.rgb), Toggles.x);			// partial precision
    
    OUT.color_0.rgb = lerp(blendColorB, (saturate(IN.texcoord_0.y * 1.5) * (1.0 - materialColor.rgb)) + materialColor.rgb, Toggles.y);			// partial precision
    OUT.color_0.a = IN.texcoord_0.x * materialColor.a;			// partial precision

    return OUT;
};

// approximately 11 instruction slots used
