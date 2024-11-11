//
//
#define	ScreenSpace	Src0
// Parameters:

float4 BlurOffsets[16] : register(c1);
float2 BlurScale : register(c0);
sampler2D ScreenSpace : register(s0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   BlurScale    const_0       1
//   BlurOffsets[0]  const_1       5
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

    float4 blurOffset5 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[4].xy) + IN.ScreenOffset.xy);
    float4 blurOffset4 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset.xy);
    float4 blurOffset3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset.xy);
    float4 blurOffset2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset.xy);
    float4 blurOffset1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset.xy);
    
    float3 finalColor = (BlurOffsets[0].z * blurOffset1.rgb);
    finalColor += (BlurOffsets[2].z * blurOffset3.rgb) + (BlurOffsets[1].z * blurOffset2.rgb);
    finalColor += (BlurOffsets[4].z * blurOffset5.rgb) + (BlurOffsets[3].z * blurOffset4.rgb);
    OUT.color_0.a = BlurOffsets[4].a;
    OUT.color_0.rgb = finalColor;

    return OUT;
};

// approximately 17 instruction slots used (5 texture, 12 arithmetic)
