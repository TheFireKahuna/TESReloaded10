//
//
#define	ScreenSpace	Src0
// Parameters:

float4 BlurOffsets[16] : register(c3);
float3 BlurScale : register(c2);
sampler2D ScreenSpace : register(s0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   BlurScale    const_2       1
//   BlurOffsets[0]  const_3      15
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

    float4 blurOffset15 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[14].xy) + IN.ScreenOffset);
    float4 blurOffset14 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[13].xy) + IN.ScreenOffset);
    float4 blurOffset13 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[12].xy) + IN.ScreenOffset);
    float4 blurOffset12 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[11].xy) + IN.ScreenOffset);
    float4 blurOffset11 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[10].xy) + IN.ScreenOffset);
    float4 blurOffset10 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[9].xy) + IN.ScreenOffset);
    float4 blurOffset9 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[8].xy) + IN.ScreenOffset);
    float4 blurOffset8 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[7].xy) + IN.ScreenOffset);
    float4 blurOffset7 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[6].xy) + IN.ScreenOffset);
    float4 blurOffset6 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[5].xy) + IN.ScreenOffset);
    float4 blurOffset5 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[4].xy) + IN.ScreenOffset);
    float4 blurOffset4 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset);
    float4 blurOffset3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset);
    float4 blurOffset2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset);
    float4 blurOffset1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset);
    float3 finalColor = (BlurOffsets[3].b * blurOffset4.rgb) + ((BlurOffsets[2].b * blurOffset3.rgb) + ((BlurOffsets[0].b * blurOffset1.rgb) + (blurOffset2.rgb * BlurOffsets[1].b)));
    finalColor = (BlurOffsets[7].b * blurOffset8.rgb) + ((BlurOffsets[6].b * blurOffset7.rgb) + ((BlurOffsets[5].b * blurOffset6.rgb) + ((BlurOffsets[4].b * blurOffset5.rgb) + finalColor.rgb)));
    finalColor = (BlurOffsets[10].b * blurOffset11.rgb) + ((BlurOffsets[9].b * blurOffset10.rgb) + ((BlurOffsets[8].b * blurOffset9.rgb) + finalColor.rgb));
    finalColor = (BlurOffsets[13].b * blurOffset14.rgb) + ((BlurOffsets[12].b * blurOffset13.rgb) + ((BlurOffsets[11].b * blurOffset12.rgb) + finalColor.rgb));
    OUT.color_0.a = BlurScale.b;
    OUT.color_0.rgb = (BlurOffsets[14].b * blurOffset15.rgb) + finalColor;

    return OUT;
};

// approximately 48 instruction slots used (15 texture, 33 arithmetic)
