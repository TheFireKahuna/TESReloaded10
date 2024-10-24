//
//
#define	ScreenSpace	Src0
// Parameters:

sampler2D ScreenSpace : register(s0);
float4 alphaAdd : register(c0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   alphaAdd     const_0       1
//   ScreenSpace         texture_0       1
//

#include "Includes/Helpers.hlsl"

// Structures:

struct VS_INPUT {
    float2 ScreenOffset : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const float4 const_1 = {0.6, 0.73, 0.8, 1};

    float4 r0;

    r0 = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    OUT.color_0.a = 1;
    OUT.color_0.rgb = r0.xyz * ((sqr(shades(const_1.xyz, r0.xyz) * alphaAdd.y) + (r0.w * alphaAdd.z)) + alphaAdd.x);

    return OUT;
};

// approximately 9 instruction slots used (1 texture, 8 arithmetic)
