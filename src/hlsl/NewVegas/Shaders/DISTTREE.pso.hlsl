//
//
// Parameters:

sampler2D DiffuseMap : register(s0);
float4 TESR_LinearObject : register(c0);
float4 TESR_LinearObjectExtra : register(c1);

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
    float3 color = linearCheck(IN.color_0, TESR_LinearObjectExtra.y);

    float4 baseColor = tex2D(DiffuseMap, IN.DiffuseUV);			// partial precision
    baseColor = linearCheck(baseColor, TESR_LinearObjectExtra.w);

    OUT.color_0.rgb = (IN.texcoord_2.w * (linearCheck(IN.texcoord_2.xyz,TESR_LinearObject.x)  - (color * baseColor.rgb))) + (baseColor.rgb * color);			// partial precision
    OUT.color_0.a = baseColor.a;			// partial precision

    return OUT;
};

// approximately 6 instruction slots used (1 texture, 5 arithmetic)
