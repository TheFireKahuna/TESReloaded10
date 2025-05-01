//
//
// Parameters:

sampler2D HeightMap : register(s0);
float3 WadingVars : register(c5);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   WadingVars   const_5       1
//   HeightMap    texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 texcoord_0 : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
    float2 texcoord_2 : TEXCOORD2;
    float2 texcoord_3 : TEXCOORD3;
    float2 texcoord_4 : TEXCOORD4;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float1 q0;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;

    r0.xyzw = tex2D(HeightMap, IN.texcoord_0.xy);
    r1.xyzw = tex2D(HeightMap, IN.texcoord_1.xy);
    r2.xyzw = tex2D(HeightMap, IN.texcoord_2.xy);
    r3.xyzw = tex2D(HeightMap, IN.texcoord_3.xy);
    r4.xyzw = tex2D(HeightMap, IN.texcoord_4.xy);
    q0.x = r1.x + r2.x;
    r1.w = ((r3.x + (r1.x - r2.x)) - r4.x) * 0.5;
    r1.x = (r4.x + (r3.x + q0.x)) - (r0.x * 4);
    r0.xy = r0.xy - 0.5;
    r1.y = (WadingVars.x * r1.x) + r0.y;
    r1.x = (WadingVars.y * r1.y) + r0.x;
    r1.xy = r1.xy * WadingVars.z;
    r1.z = ((q0.x - r3.x) - r4.x) * 0.5;
    r0.xyzw = r1.xyzw + 0.5;
    OUT.color_0.a = r0.w;
    OUT.color_0.rgb = r0.xyz;

    return OUT;
};

// approximately 22 instruction slots used (5 texture, 17 arithmetic)
