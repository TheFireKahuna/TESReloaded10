//
//
#define	ScreenSpace	Src0
// Parameters:

sampler2D AvgLum : register(s1);
float4 BlurOffsets[16] : register(c1);
float2 BlurScale : register(c0);
float4 HDRParam : register(c17);
sampler2D ScreenSpace : register(s0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   BlurScale    const_0       1
//   BlurOffsets[0]  const_1       5
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

#define	weight(v)		dot(v, 1)
#define	sqr(v)			((v) * (v))

    float3 q2;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;

    r0 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[4].xy) + IN.ScreenOffset.xy);
    r4 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset.xy);
    r3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset.xy);
    r1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset.xy);
    r2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset.xy);
    r5 = tex2D(AvgLum, IN.ScreenOffset.xy);
    r0.xyz = max(r0.xyz - HDRParam.x, 0) * HDRParam.y;
    r1.xyz = (max(r1.xyz - HDRParam.x, 0) * HDRParam.y) * BlurOffsets[1].z;
    r1.xyz = (BlurOffsets[0].z * (max(r2.xyz - HDRParam.x, 0) * HDRParam.y)) + r1.xyz;
    r1.xyz = (BlurOffsets[2].z * (max(r3.xyz - HDRParam.x, 0) * HDRParam.y)) + r1.xyz;
    q2.xyz = (BlurOffsets[4].z * r0.xyz) + ((BlurOffsets[3].z * (max(r4.xyz - HDRParam.x, 0) * HDRParam.y)) + r1.xyz);
    OUT.color_0.a = weight(r5.xyz);
    OUT.color_0.rgb = q2.xyz;

    return OUT;
};

// approximately 34 instruction slots used (6 texture, 28 arithmetic)
