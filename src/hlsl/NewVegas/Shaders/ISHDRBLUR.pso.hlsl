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

    float4 blurOffset7 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[6].xy) + IN.ScreenOffset);
    float4 blurOffset6 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[5].xy) + IN.ScreenOffset);
    float4 blurOffset5 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[4].xy) + IN.ScreenOffset);
    float4 blurOffset4 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset);
    float4 blurOffset3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset);
    float4 blurOffset1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset);
    float4 blurOffset0 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[14].xy) + IN.ScreenOffset);
    float4 blurOffset14 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[13].xy) + IN.ScreenOffset);
    float4 blurOffset13 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[12].xy) + IN.ScreenOffset);
    float4 blurOffset12 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[11].xy) + IN.ScreenOffset);
    float4 blurOffset11 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[10].xy) + IN.ScreenOffset);
    float4 blurOffset10 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[9].xy) + IN.ScreenOffset);
    float4 blurOffset9 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[8].xy) + IN.ScreenOffset);
    float4 blurOffset8 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[7].xy) + IN.ScreenOffset);
    float4 blurOffset2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset);
    float3 finalColor = (BlurOffsets[3].z * blurOffset4.xyz) + ((BlurOffsets[2].z * blurOffset3.xyz) + ((BlurOffsets[0].z * blurOffset2.xyz) + (blurOffset1.xyz * BlurOffsets[1].z)));
    finalColor = (BlurOffsets[7].z * blurOffset8.xyz) + ((BlurOffsets[6].z * blurOffset7.xyz) + ((BlurOffsets[5].z * blurOffset6.xyz) + ((BlurOffsets[4].z * blurOffset5.xyz) + finalColor.xyz)));
    finalColor = (BlurOffsets[10].z * blurOffset11.xyz) + ((BlurOffsets[9].z * blurOffset10.xyz) + ((BlurOffsets[8].z * blurOffset9.xyz) + finalColor.xyz));
    finalColor = (BlurOffsets[13].z * blurOffset14.xyz) + ((BlurOffsets[12].z * blurOffset13.xyz) + ((BlurOffsets[11].z * blurOffset12.xyz) + finalColor.xyz));
    OUT.color_0.a = BlurScale.z;
    OUT.color_0.rgb = (BlurOffsets[14].z * blurOffset0.xyz) + finalColor;

    return OUT;
};

// approximately 48 instruction slots used (15 texture, 33 arithmetic)
