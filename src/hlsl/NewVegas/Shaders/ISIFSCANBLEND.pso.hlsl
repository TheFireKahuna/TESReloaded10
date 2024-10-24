//
//
// Parameters:

sampler2D Colors : register(s1);
sampler2D Distort : register(s3);
float4 DistortParams : register(c1);
sampler2D MaskColors : register(s0);
float4 Offsets : register(c3);
float4 Params : register(c0);
sampler2D Scanlines : register(s2);
float4 Tint : register(c2);


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   Params        const_0       1
//   DistortParams const_1       1
//   Tint          const_2       1
//   Offsets       const_3       1
//   MaskColors    texture_0       1
//   Colors        texture_1       1
//   Scanlines     texture_2       1
//   Distort       texture_3       1
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

    const float4 const_4 = {1, 0.5, 0.349999994, 0};
    const float4 const_5 = {(1.0 / 9), -1, 1, 0};

    float1 q0;
    float4 q6;
    float4 q7;
    float3 q8;
    float4 r0;
    float2 r1;
    float4 r2;
    float2 r3;
    float2 r4;
    float2 r5;
    float2 r8;
    float4 r9;
    float4 t1;
    float4 t10;
    float4 t13;
    float4 t2;
    float4 t26;
    float4 t27;
    float4 t29;
    float4 t3;
    float4 t31;
    float4 t4;
    float4 t5;

    t4 = tex2D(Scanlines, IN.texcoord_0.xy * Params.w);
    r2.xw = const_4.xw;
    q0.x = IN.texcoord_0.y + Params.y;
    r8.x = IN.texcoord_0.x;
    r1.x = IN.texcoord_0.x;
    r5.x = r1.x + Offsets.x;
    r3.x = r1.x + Offsets.x;
    r0.x = IN.texcoord_0.x;
    r9.x = 0.5;
    r4.y = 0;
    r4.x = -Offsets.x;
    r1.y = (Params.z >= q0.x ? q0.x : (q0.x - Params.z));
    t10 = tex2D(MaskColors, r1.xy - Offsets.xy);
    t26 = tex2D(MaskColors, r1.xy + Offsets.xy);
    t27 = tex2D(MaskColors, (const_5.yz * Offsets.xy) + r1.xy);
    t13 = tex2D(Colors, r1.xy);
    r3.y = r1.y - Offsets.y;
    t3 = tex2D(MaskColors, r3.xy);
    t31 = tex2D(MaskColors, r1.xy);
    r0.y = r1.y - Offsets.y;
    t5 = tex2D(MaskColors, r0.xy);
    r8.y = r1.y + Offsets.y;
    r5.y = r1.y;
    t2 = tex2D(MaskColors, r5.xy);
    t29 = tex2D(MaskColors, r1.xy + r4.xy);
    r9.y = ((IN.texcoord_0.y * DistortParams.x) - DistortParams.y) + 1;
    r9 = tex2D(Distort, r9.xy);
    q8.xyz = (DistortParams.x <= 0.0 ? r2.w : (((r9.xyz * DistortParams.z) * Tint.rgb) * 0.349999994));
    r0 = ((1.0 / 9) * t10) + (t5 / 9);
    t1 = tex2D(MaskColors, r8.xy);
    q6 = ((1.0 / 9) * t31) + (((1.0 / 9) * t29) + (((1.0 / 9) * t3) + r0));
    q7 = ((1.0 / 9) * t1) + (((1.0 / 9) * t27) + (((1.0 / 9) * t2) + q6));
    r0 = (Tint * ((((1.0 / 9) * t26) + q7) * Params.x)) + t13;
    r0.xyz = r0.xyz + q8.xyz;
    r0 = r0 * (Params.w <= 0.0 ? r2.x : t4);
    OUT.color_0.a = r0.w;
    OUT.color_0.rgb = r0.xyz;

    return OUT;
};

// approximately 56 instruction slots used (12 texture, 44 arithmetic)
