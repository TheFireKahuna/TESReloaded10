//
//
// Parameters:

sampler2D Src0 : register(s0);
float4 blurParams : register(c0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   blurParams   const_0       1
//   Src0         texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 texcoord_0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const float4 const_1 = {0.5, (1.0 / 18), (1.0 / 9), 0.05};

    float2 q0;
    float2 q1;
    float2 q2;
    float2 q3;
    float2 q4;
    float2 q5;
    float3 q6;
    float3 q7;
    float2 q8;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;
    float4 r6;
    float4 r7;
    float4 r8;

    r0.xyz = const_1.xyz;
    r1.y = (blurParams.y * r0.y) + (IN.texcoord_0.y - (blurParams.y * r0.x));
    r1.x = (blurParams.x * r0.y) + (IN.texcoord_0.x - (blurParams.x * r0.x));
    q8.xy = (r0.z * blurParams.xy) + r1.xy;
    r1.xyzw = tex2D(Src0, r1.xy);
    q0.xy = (r0.z * blurParams.xy) + q8.xy;
    r2.xyzw = tex2D(Src0, q0.xy);
    q1.xy = (r0.z * blurParams.xy) + q0.xy;
    r3.xyzw = tex2D(Src0, q1.xy);
    q2.xy = (r0.z * blurParams.xy) + q1.xy;
    r4.xyzw = tex2D(Src0, q2.xy);
    q3.xy = (r0.z * blurParams.xy) + q2.xy;
    r5.xyzw = tex2D(Src0, q3.xy);
    q4.xy = (r0.z * blurParams.xy) + q3.xy;
    r6.xyzw = tex2D(Src0, q4.xy);
    q5.xy = (r0.z * blurParams.xy) + q4.xy;
    r7.xyzw = tex2D(Src0, q5.xy);
    r8.xyzw = tex2D(Src0, (r0.z * blurParams.xy) + q5.xy);
    r0.xyzw = tex2D(Src0, q8.xy);
    q6.xyz = (0.3 * r4.xyz) + ((0.15 * r3.xyz) + ((0.075 * r2.xyz) + ((0.025 * r1.xyz) + (r0.xyz * 0.05))));
    q7.xyz = (0.025 * r8.xyz) + ((0.05 * r7.xyz) + ((0.075 * r6.xyz) + ((0.15 * r5.xyz) + q6.xyz)));
    OUT.color_0.a = 1;
    OUT.color_0.rgb = q7.xyz;

    return OUT;
};

// approximately 33 instruction slots used (9 texture, 24 arithmetic)
