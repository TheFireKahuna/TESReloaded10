//
//
#define	ScreenSpace	Src0
// Parameters:

sampler2D AvgLum : register(s1);
float4 BlurOffsets[16] : register(c3);
float3 BlurScale : register(c2);
float4 HDRParam : register(c1);
sampler2D ScreenSpace : register(s0);
float4 TimingData : register(c0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   TimingData   const_0       1
//   HDRParam     const_1       1
//   BlurScale    const_2       1
//   BlurOffsets[0]  const_3       4
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

    //float4 blurOffset3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset.xy);
    //float4 blurOffset2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset.xy);
   // float4 blurOffset1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset.xy);
    //float4 blurOffset0 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset.xy);
    //float3 blurOffsetBlend = (BlurOffsets[3].z * blurOffset3.rgb) + ((BlurOffsets[2].z * blurOffset2.rgb) + ((BlurOffsets[0].z * blurOffset0.rgb) + (blurOffset1.rgb * BlurOffsets[1].z)));
    //float4 avgLuma = tex2D(AvgLum, IN.ScreenOffset.xy);
    //float3 adaptBlend.rgb = lerp(avgLuma.rgb, blurOffsetBlend.rgb, 1 - pow(abs(HDRParam.z), TimingData.z));
    float4 screenSpace = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    OUT.color_0.rgb = screenSpace.rgb;
    OUT.color_0.a = BlurScale.z;
    //OUT.color_0.rgb = adaptBlend.rgb * (min(max(0.01, length(adaptBlend.rgb)), HDRParam.w) / max(0.01, length(adaptBlend.rgb)));

    return OUT;
};

// approximately 30 instruction slots used (5 texture, 25 arithmetic)
