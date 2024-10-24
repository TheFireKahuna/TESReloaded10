//
//
#define	ScreenSpace	Src0
// Parameters:

float4 Cinematic : register(c0);
float4 Fade : register(c2);
sampler2D ScreenSpace : register(s0);
float4 Tint : register(c1);
float4 TESR_ToneMapping : register(c3);

#include "Includes/Helpers.hlsl"

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   Cinematic    const_0       1
//   Tint         const_1       1
//   Fade         const_2       1
//   ScreenSpace         texture_0       1
//


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

    //const float4 const_3 = {0.299, 0.587000012, 0.114, 0};

    float4 color = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    float3 tint = Tint.rgb;
    //q0.x = dot(color.xyz, const_3.xyz);
    //q3 = lerp(q0.x, color, Cinematic.x);
    color.rgb = lerp(color.rgb, tint * luma(color.rgb), saturate(Tint.a * TESR_ToneMapping.z)); // apply tint
    //float4 final = (Cinematic.w * lerp((Tint.a * ((q0.x * Tint) - q3)) + q3, Fade, Fade.w)) - Cinematic.y;
    //color = (Cinematic.z * final) + Cinematic.y;
    OUT.color_0.a = color.w;
    OUT.color_0.rgb = color.rgb;

    return OUT;
};

// approximately 9 instruction slots used (1 texture, 8 arithmetic)
