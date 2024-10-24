//
//
// Parameters:

sampler2D DiffuseMap : register(s0);
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
//   DiffuseMap    texture_0       1
//


// Structures:

struct VS_INPUT {
    float4 DiffuseUV : TEXCOORD0;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float4 materialColor = linearCheck(MaterialColor, TESR_LinearObjectExtra.x);
    float3 fogColor = linearCheck(FogColor.rgb, TESR_LinearObjectExtra.z);

    float4 baseColor = tex2D(DiffuseMap, IN.DiffuseUV.xy);			// partial precision
    baseColor = linearCheck(baseColor, TESR_LinearObjectExtra.w);

    float4 blendColorA = baseColor * materialColor;			// partial precision
    float3 blendColorB = (IN.DiffuseUV.w * (fogColor - (materialColor.rgb * baseColor.rgb))) + blendColorA.rgb;			// partial precision
    float3 blendColorC = lerp(blendColorB, blendColorA.rgb - (IN.DiffuseUV.w * blendColorA.rgb), Toggles.x);			// partial precision

    OUT.color_0.rgb = lerp(blendColorC.xyz, (saturate(IN.DiffuseUV.w * 1.5) * (1.0 - (materialColor.rgb * baseColor.rgb))) + blendColorA.rgb, Toggles.y);			// partial precision
    OUT.color_0.a = blendColorA.a * IN.DiffuseUV.z;			// partial precision

    return OUT;
};

// approximately 13 instruction slots used (1 texture, 12 arithmetic)
