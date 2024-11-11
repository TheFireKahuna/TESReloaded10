//
//
// Parameters:

sampler2D TexMap : register(s0);
float4 TESR_LinearSky : register(c1);

#include "includes/Helpers.hlsl"


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   TexMap       texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 TexUV : TEXCOORD0;
    float texcoord_2 : TEXCOORD2;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 baseColor = tex2D(TexMap, IN.TexUV.xy);
    OUT.color_0.a = (baseColor.w * IN.color_0.a) * IN.texcoord_2.x;
    OUT.color_0.rgb = baseColor.rgb * IN.color_0.rgb;

    return OUT;
};

// approximately 4 instruction slots used (1 texture, 3 arithmetic)
