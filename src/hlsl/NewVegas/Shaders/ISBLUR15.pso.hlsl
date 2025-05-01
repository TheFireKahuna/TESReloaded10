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
//   BlurOffsets[0]  const_1      15
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
    float3 q1;
    float3 q2;
    float3 q3;
    float4 r0;
    float4 r1;
    float4 r10;
    float4 r11;
    float4 r12;
    float4 r13;
    float4 r14;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;
    float4 r6;
    float4 r7;
    float4 r8;
    float4 r9;

    r9 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[8].xy) + IN.ScreenOffset.xy);
    r8 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[7].xy) + IN.ScreenOffset.xy);
    r7 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[6].xy) + IN.ScreenOffset.xy);
    r6 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[5].xy) + IN.ScreenOffset.xy);
    r5 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[4].xy) + IN.ScreenOffset.xy);
    r4 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[3].xy) + IN.ScreenOffset.xy);
    r3 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[2].xy) + IN.ScreenOffset.xy);
    r1 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[1].xy) + IN.ScreenOffset.xy);
    r0 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[14].xy) + IN.ScreenOffset.xy);
    r14 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[13].xy) + IN.ScreenOffset.xy);
    r13 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[12].xy) + IN.ScreenOffset.xy);
    r12 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[11].xy) + IN.ScreenOffset.xy);
    r11 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[10].xy) + IN.ScreenOffset.xy);
    r10 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[9].xy) + IN.ScreenOffset.xy);
    r2 = tex2D(ScreenSpace, (BlurScale.xy * BlurOffsets[0].xy) + IN.ScreenOffset.xy);
    q0.xyz = (BlurOffsets[3].z * r4.xyz) + ((BlurOffsets[2].z * r3.xyz) + ((BlurOffsets[0].z * r2.xyz) + (r1.xyz * BlurOffsets[1].z)));
    q1.xyz = (BlurOffsets[7].z * r8.xyz) + ((BlurOffsets[6].z * r7.xyz) + ((BlurOffsets[5].z * r6.xyz) + ((BlurOffsets[4].z * r5.xyz) + q0.xyz)));
    q2.xyz = (BlurOffsets[10].z * r11.xyz) + ((BlurOffsets[9].z * r10.xyz) + ((BlurOffsets[8].z * r9.xyz) + q1.xyz));
    q3.xyz = (BlurOffsets[13].z * r14.xyz) + ((BlurOffsets[12].z * r13.xyz) + ((BlurOffsets[11].z * r12.xyz) + q2.xyz));
    OUT.color_0.a = r0.w;
    OUT.color_0.rgb = (BlurOffsets[14].z * r0.xyz) + q3.xyz;

    return OUT;
};

// approximately 47 instruction slots used (15 texture, 32 arithmetic)
