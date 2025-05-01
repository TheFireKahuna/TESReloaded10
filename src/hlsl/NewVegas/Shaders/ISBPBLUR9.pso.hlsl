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
//   BlurOffsets[0]  const_1       9
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

    float3 q11;
    float3 q4;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;
    float4 r6;
    float4 r7;
    float4 r8;
    float4 r9;

    r0 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[8].xy) + IN.ScreenOffset.xy);
    r8 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[7].xy) + IN.ScreenOffset.xy);
    r7 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[6].xy) + IN.ScreenOffset.xy);
    r6 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[5].xy) + IN.ScreenOffset.xy);
    r5 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[4].xy) + IN.ScreenOffset.xy);
    r4 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset.xy);
    r3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset.xy);
    r1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset.xy);
    r2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset.xy);
    r9 = tex2D(AvgLum, IN.ScreenOffset.xy);
    r2.xyz = max(r2.xyz - HDRParam.x, 0) * HDRParam.y;
    r0.xyz = max(r0.xyz - HDRParam.x, 0) * HDRParam.y;
    r1.xyz = (BlurOffsets[0].z * r2.xyz) + ((max(r1.xyz - HDRParam.x, 0) * HDRParam.y) * BlurOffsets[1].z);
    r2.xyz = max(r5.xyz - HDRParam.x, 0) * HDRParam.y;
    r1.xyz = (BlurOffsets[2].z * (max(r3.xyz - HDRParam.x, 0) * HDRParam.y)) + r1.xyz;
    r1.xyz = (BlurOffsets[4].z * r2.xyz) + ((BlurOffsets[3].z * (max(r4.xyz - HDRParam.x, 0) * HDRParam.y)) + r1.xyz);
    r2.xyz = max(r7.xyz - HDRParam.x, 0) * HDRParam.y;
    q11.xyz = (BlurOffsets[6].z * r2.xyz) + ((BlurOffsets[5].z * (max(r6.xyz - HDRParam.x, 0) * HDRParam.y)) + r1.xyz);
    q4.xyz = (BlurOffsets[8].z * r0.xyz) + ((BlurOffsets[7].z * (max(r8.xyz - HDRParam.x, 0) * HDRParam.y)) + q11.xyz);
    OUT.color_0.a = weight(r9.xyz);
    OUT.color_0.rgb = q4.xyz;

    return OUT;
};

// approximately 58 instruction slots used (10 texture, 48 arithmetic)
