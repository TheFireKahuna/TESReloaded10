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
//   BlurOffsets[0]  BlurOffsets[0]       9
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

    float1 q0;
    float1 q1;
    float3 q2;
    float3 q3;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;
    float4 r6;
    float4 r7;
    float4 r8;

    q1.x = frac(IN.ScreenOffset.x * 128);
    q0.x = frac(IN.ScreenOffset.y * 128);
    r7.x = (q1.x <= 0.0 ? (2.5 - BlurOffsets[6].x) : BlurOffsets[6].x);
    r6.x = (q1.x <= 0.0 ? (2.5 - BlurOffsets[5].x) : BlurOffsets[5].x);
    r5.x = (q1.x <= 0.0 ? (2.5 - BlurOffsets[4].x) : BlurOffsets[4].x);
    r4.x = (q1.x <= 0.0 ? (2.5 - BlurOffsets[3].x) : BlurOffsets[3].x);
    r3.x = (q1.x <= 0.0 ? (2.5 - BlurOffsets[2].x) : BlurOffsets[2].x);
    r1.x = (q1.x <= 0.0 ? (2.5 - BlurOffsets[1].x) : BlurOffsets[1].x);
    r2.x = (q1.x <= 0.0 ? (2.5 - BlurOffsets[0].x) : BlurOffsets[0].x);
    r0.x = (q1.x <= 0.0 ? (2.5 - BlurOffsets[8].x) : BlurOffsets[8].x);
    r8.x = (q1.x <= 0.0 ? (2.5 - BlurOffsets[7].x) : BlurOffsets[7].x);
    r7.y = (q0.x <= 0.0 ? (2.5 - BlurOffsets[6].y) : BlurOffsets[6].y);
    r7.xyzw = tex2D(ScreenSpace, (BlurScale.xy * r7.xy) + IN.ScreenOffset.xy);
    r6.y = (q0.x <= 0.0 ? (2.5 - BlurOffsets[5].y) : BlurOffsets[5].y);
    r6.xyzw = tex2D(ScreenSpace, (BlurScale.xy * r6.xy) + IN.ScreenOffset.xy);
    r5.y = (q0.x <= 0.0 ? (2.5 - BlurOffsets[4].y) : BlurOffsets[4].y);
    r5.xyzw = tex2D(ScreenSpace, (BlurScale.xy * r5.xy) + IN.ScreenOffset.xy);
    r4.y = (q0.x <= 0.0 ? (2.5 - BlurOffsets[3].y) : BlurOffsets[3].y);
    r4.xyzw = tex2D(ScreenSpace, (BlurScale.xy * r4.xy) + IN.ScreenOffset.xy);
    r3.y = (q0.x <= 0.0 ? (2.5 - BlurOffsets[2].y) : BlurOffsets[2].y);
    r3.xyzw = tex2D(ScreenSpace, (BlurScale.xy * r3.xy) + IN.ScreenOffset.xy);
    r1.y = (q0.x <= 0.0 ? (2.5 - BlurOffsets[1].y) : BlurOffsets[1].y);
    r1.xyzw = tex2D(ScreenSpace, (BlurScale.xy * r1.xy) + IN.ScreenOffset.xy);
    r2.y = (q0.x <= 0.0 ? (2.5 - BlurOffsets[0].y) : BlurOffsets[0].y);
    r2.xyzw = tex2D(ScreenSpace, (BlurScale.xy * r2.xy) + IN.ScreenOffset.xy);
    r0.y = (q0.x <= 0.0 ? (2.5 - BlurOffsets[8].y) : BlurOffsets[8].y);
    r0.xyzw = tex2D(ScreenSpace, (BlurScale.xy * r0.xy) + IN.ScreenOffset.xy);
    r8.y = (q0.x <= 0.0 ? (2.5 - BlurOffsets[7].y) : BlurOffsets[7].y);
    r8.xyzw = tex2D(ScreenSpace, (BlurScale.xy * r8.xy) + IN.ScreenOffset.xy);
    q2.xyz = (BlurOffsets[3].z * r4.xyz) + ((BlurOffsets[2].z * r3.xyz) + ((BlurOffsets[0].z * r2.xyz) + (r1.xyz * BlurOffsets[1].z)));
    q3.xyz = (BlurOffsets[7].z * r8.xyz) + ((BlurOffsets[6].z * r7.xyz) + ((BlurOffsets[5].z * r6.xyz) + ((BlurOffsets[4].z * r5.xyz) + q2.xyz)));
    OUT.color_0.a = BlurScale.z;
    OUT.color_0.rgb = (BlurOffsets[8].z * r0.xyz) + q3.xyz;

    return OUT;
};

// approximately 70 instruction slots used (9 texture, 61 arithmetic)
