//
//
#define	ScreenSpace	Src0
// Parameters:

float4 BlurOffsets[16] : register(c3);
float3 BlurScale : register(c2);
float4 HDRParam : register(c1);
sampler2D ScreenSpace : register(s0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   HDRParam     const_1       1
//   BlurScale    const_2       1
//   BlurOffsets[0]  const_3      16
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

    /*float4 blurOffset15 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[15].xy) + IN.ScreenOffset.xy);
    float4 blurOffset14 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[14].xy) + IN.ScreenOffset.xy);
    float4 blurOffset13 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[13].xy) + IN.ScreenOffset.xy);
    float4 blurOffset12 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[12].xy) + IN.ScreenOffset.xy);
    float4 blurOffset11 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[11].xy) + IN.ScreenOffset.xy);
    float4 blurOffset10 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[10].xy) + IN.ScreenOffset.xy);
    float4 blurOffset9 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[9].xy) + IN.ScreenOffset.xy);
    float4 blurOffset8 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[8].xy) + IN.ScreenOffset.xy);
    float4 blurOffset7 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[7].xy) + IN.ScreenOffset.xy);
    float4 blurOffset6 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[6].xy) + IN.ScreenOffset.xy);
    float4 blurOffset5 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[5].xy) + IN.ScreenOffset.xy);
    float4 blurOffset4 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[4].xy) + IN.ScreenOffset.xy);
    float4 blurOffset3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset.xy);
    float4 blurOffset2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset.xy);
    float4 blurOffset0 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset.xy);
    float4 blurOffset1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset.xy);
    float3 finalColor = (BlurOffsets[3].z * blurOffset3.rgb) + ((BlurOffsets[2].z * blurOffset2.rgb) + ((BlurOffsets[0].z * blurOffset1.rgb) + (blurOffset0.rgb * BlurOffsets[1].z)));
    finalColor = (BlurOffsets[7].z * blurOffset7.rgb) + ((BlurOffsets[6].z * blurOffset6.rgb) + ((BlurOffsets[5].z * blurOffset5.rgb) + ((BlurOffsets[4].z * blurOffset4.rgb) + finalColor)));
    finalColor = (BlurOffsets[10].z * blurOffset10.rgb) + ((BlurOffsets[9].z * blurOffset9.rgb) + ((BlurOffsets[8].z * blurOffset8.rgb) + finalColor));
    finalColor = (BlurOffsets[13].z * blurOffset13.rgb) + ((BlurOffsets[12].z * blurOffset12.rgb) + ((BlurOffsets[11].z * blurOffset11.rgb) + finalColor));
    finalColor = (BlurOffsets[15].z * blurOffset15.rgb) + ((BlurOffsets[14].z * blurOffset14.rgb) + finalColor);
    OUT.color_0.a = BlurScale.z;
    OUT.color_0.rgb = finalColor;*/
    OUT.color_0.rgb = tex2D(ScreenSpace, IN.ScreenOffset.xy).rgb;
    OUT.color_0.a = BlurScale.z;

    //OUT.color_0.rgb = q4.xyz * (min(max(0.01, length(q4.xyz)), HDRParam.x) / max(0.01, length(q4.xyz)));

    return OUT;
};

// approximately 59 instruction slots used (16 texture, 43 arithmetic)
