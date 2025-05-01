//
//
// Parameters:

sampler2D DiffuseMap : register(s0);
float4 MaterialColor : register(c0);

#include "includes/Helpers.hlsl"

// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   MaterialColor const_0       1
//   DiffuseMap    texture_0       1
//


// Structures:

struct VS_INPUT {
    float4 DiffuseUV : TEXCOORD0;			// partial precision
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

    float4 baseColor = tex2D(DiffuseMap, IN.DiffuseUV.xy);			// partial precision

    baseColor = (baseColor * color) * materialColor;			// partial precision

    OUT.color_0.rgb = (baseColor.rgb * (1.0 - IN.DiffuseUV.w)) * (baseColor.a * IN.DiffuseUV.z);			// partial precision
    OUT.color_0.a = 1.0;			// partial precision

    return OUT;
};

// approximately 9 instruction slots used (1 texture, 8 arithmetic)
