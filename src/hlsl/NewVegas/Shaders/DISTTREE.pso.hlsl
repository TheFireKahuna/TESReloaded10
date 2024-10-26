//
//
// Parameters:

sampler2D DiffuseMap : register(s0);
float4 TESR_LinearObject : register(c0);

#include "includes/Helpers.hlsl"


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   DiffuseMap   texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 DiffuseUV : TEXCOORD0;			// partial precision
    float3 color_0 : COLOR0;
    float4 texcoord_2 : TEXCOORD2;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float3 color = IN.color_0;

    float4 baseColor = tex2D(DiffuseMap, IN.DiffuseUV);			// partial precision
    baseColor = linearCheck(baseColor, TESR_LinearObject.y);

    OUT.color_0.rgb = (IN.texcoord_2.w * (IN.texcoord_2.xyz  - (color * baseColor.rgb))) + (baseColor.rgb * color);			// partial precision
    //OUT.color_0.rgb = IN.color_0.rgb;			// partial precision
    OUT.color_0.a = baseColor.a;			// partial precision

    return OUT;
};

// approximately 6 instruction slots used (1 texture, 5 arithmetic)
