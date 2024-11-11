//
//
// Parameters:

float4 FogColor : register(c2);
float4 MaterialColor : register(c0);
float4 Toggles : register(c1);

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
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float4 color = IN.color_0;
    float4 materialColor = MaterialColor;
    float3 fogColor = FogColor.rgb;

    float4 blendColorA = color * materialColor;			// partial precision
    float3 blendColorB = (saturate(IN.texcoord_0.y * 1.5) * (1.0 - (color.rgb * materialColor.rgb))) + blendColorA.rgb;
    blendColorB = (IN.texcoord_0.y * (fogColor - (color.rgb * materialColor.rgb))) + blendColorA.rgb;			// partial precision

    OUT.color_0.rgb = lerp(lerp(blendColorB, blendColorA.rgb - (IN.texcoord_0.y * blendColorA.rgb), Toggles.x), blendColorB, Toggles.y);			// partial precision
    OUT.color_0.a = blendColorA.a * IN.texcoord_0.x;			// partial precision

    return OUT;
};

// approximately 12 instruction slots used
