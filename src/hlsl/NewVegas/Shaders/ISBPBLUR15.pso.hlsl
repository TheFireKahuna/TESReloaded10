//
//
#define	ScreenSpace	Src0
// Parameters:

sampler2D AvgLum : register(s1);
float4 BlurOffsets[16] : register(c1);
float2 BlurScale : register(c0);
float4 HDRParam : register(c17);
sampler2D ScreenSpace : register(s0);

#include "Includes/Helpers.hlsl"


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   BlurScale    const_0       1
//   BlurOffsets[0]  const_1      15
//   HDRParam     const_17      1
//   ScreenSpace         texture_0       1
//   AvgLum       texture_1       1
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

    float4 blurOffset15 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[14].xy) + IN.ScreenOffset.xy);
    float4 blurOffset14 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[13].xy) + IN.ScreenOffset.xy);
    float4 blurOffset13 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[12].xy) + IN.ScreenOffset.xy);
    float4 blurOffset12 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[11].xy) + IN.ScreenOffset.xy);
    float4 blurOffset11 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[10].xy) + IN.ScreenOffset.xy);
    float4 blurOffset10 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[9].xy) + IN.ScreenOffset.xy);
    float4 blurOffset9 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[8].xy) + IN.ScreenOffset.xy);
    float4 blurOffset8 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[7].xy) + IN.ScreenOffset.xy);
    float4 blurOffset7 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[6].xy) + IN.ScreenOffset.xy);
    float4 blurOffset6 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[5].xy) + IN.ScreenOffset.xy);
    float4 blurOffset5 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[4].xy) + IN.ScreenOffset.xy);
    float4 blurOffset4 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset.xy);
    float4 blurOffset3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset.xy);
    float4 blurOffset2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset.xy);
    float4 blurOffset1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset.xy);
    float4 AvgLuma = tex2D(AvgLum, IN.ScreenOffset.xy);
    float3 finalColor = (BlurOffsets[0].z * (max(blurOffset1.rgb - HDRParam.x, 0) * HDRParam.y));
    finalColor += (BlurOffsets[2].z * (max(blurOffset3.rgb - HDRParam.x, 0) * HDRParam.y)) + (BlurOffsets[1].z * (max(blurOffset2.rgb - HDRParam.x, 0) * HDRParam.y));
    finalColor += (BlurOffsets[4].z * (max(blurOffset5.rgb - HDRParam.x, 0) * HDRParam.y)) + (BlurOffsets[3].z * (max(blurOffset4.rgb - HDRParam.x, 0) * HDRParam.y));
    finalColor += (BlurOffsets[6].z * (max(blurOffset7.rgb - HDRParam.x, 0) * HDRParam.y)) + (BlurOffsets[5].z * (max(blurOffset6.rgb - HDRParam.x, 0) * HDRParam.y));
    finalColor += (BlurOffsets[8].z * (max(blurOffset9.rgb - HDRParam.x, 0) * HDRParam.y)) + (BlurOffsets[7].z * (max(blurOffset8.rgb - HDRParam.x, 0) * HDRParam.y));
    finalColor += (BlurOffsets[10].z * (max(blurOffset11.rgb - HDRParam.x, 0) * HDRParam.y)) + (BlurOffsets[9].z * (max(blurOffset10.rgb - HDRParam.x, 0) * HDRParam.y));
    finalColor += (BlurOffsets[12].z * (max(blurOffset13.rgb - HDRParam.x, 0) * HDRParam.y)) + (BlurOffsets[11].z * (max(blurOffset12.rgb - HDRParam.x, 0) * HDRParam.y));
    finalColor += (BlurOffsets[14].z * (max(blurOffset15.rgb - HDRParam.x, 0) * HDRParam.y)) + (BlurOffsets[13].z * (max(blurOffset14.rgb - HDRParam.x, 0) * HDRParam.y));
    OUT.color_0.a = weight(AvgLuma.rgb);
    OUT.color_0.rgb = finalColor;

    return OUT;
};

// approximately 94 instruction slots used (16 texture, 78 arithmetic)
