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
//   BlurOffsets[0]  const_1       7
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

    float3 q0;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;
    float4 r6;

    r0 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[6].xy) + IN.ScreenOffset.xy);
    r6 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[5].xy) + IN.ScreenOffset.xy);
    r5 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[4].xy) + IN.ScreenOffset.xy);
    r4 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset.xy);
    r3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset.xy);
    r1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset.xy);
    r2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset.xy);
    q0.xyz = (BlurOffsets[3].z * r4.xyz) + ((BlurOffsets[2].z * r3.xyz) + ((BlurOffsets[0].z * r2.xyz) + (r1.xyz * BlurOffsets[1].z)));
    OUT.color_0.a = r0.w;
    OUT.color_0.rgb = (BlurOffsets[6].z * r0.xyz) + ((BlurOffsets[5].z * r6.xyz) + ((BlurOffsets[4].z * r5.xyz) + q0.xyz));

    return OUT;
};

// approximately 23 instruction slots used (7 texture, 16 arithmetic)