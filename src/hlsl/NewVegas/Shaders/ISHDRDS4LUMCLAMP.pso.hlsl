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
//   BlurOffsets[0]  const_3       4
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

    /*float4 blurOffset3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset.xy);
    float4 blurOffset2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset.xy);
    float4 blurOffset1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset.xy);
    float4 blurOffset0 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset.xy);
    float3 blurOffsetBlend = (BlurOffsets[3].z * blurOffset3.rgb) + ((BlurOffsets[2].z * blurOffset2.rgb) + ((BlurOffsets[0].z * blurOffset0.rgb) + (blurOffset1.rgb * BlurOffsets[1].z)));
    OUT.color_0.a = BlurScale.z;
    OUT.color_0.rgb = blurOffsetBlend;*/
    //OUT.color_0.rgb = q1.xyz * (min(max(0.01, length(q1.xyz)), HDRParam.x) / max(0.01, length(q1.xyz)));
    OUT.color_0.rgb = tex2D(ScreenSpace, IN.ScreenOffset.xy).rgb;
    OUT.color_0.a = BlurScale.z;

    return OUT;
};

// approximately 23 instruction slots used (4 texture, 19 arithmetic)
