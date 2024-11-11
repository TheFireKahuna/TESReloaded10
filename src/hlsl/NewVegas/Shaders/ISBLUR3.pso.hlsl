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
//   BlurOffsets[0]  const_1       3
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

    float4 blurOffset3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset.xy);
    float4 blurOffset2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset.xy);
    float4 blurOffset1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset.xy);
    
    OUT.color_0.a = BlurOffsets[2].a;
    OUT.color_0.rgb = (BlurOffsets[2].z * blurOffset3.rgb) + (BlurOffsets[1].z * blurOffset2.rgb) + (BlurOffsets[0].z * blurOffset1.rgb);

    return OUT;
};

// approximately 11 instruction slots used (3 texture, 8 arithmetic)
