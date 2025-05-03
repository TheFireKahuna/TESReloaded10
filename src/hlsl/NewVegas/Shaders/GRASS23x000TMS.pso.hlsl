//
//
// Parameters:

sampler2D DiffuseMap : register(s0);

float4 TESR_PBRData : register(c22);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   DiffuseMap   texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 DiffuseUV : TEXCOORD0;			// partial precision
    float3 texcoord_4 : TEXCOORD4_centroid;			// partial precision
    float4 texcoord_5 : TEXCOORD5_centroid;			// partial precision
    float4 color_0 : COLOR0;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 q0;
    float4 r1;

    r1.xyzw = tex2D(DiffuseMap, IN.DiffuseUV.xy);			// partial precision
    q0.xyz = (IN.texcoord_5.xyz * TESR_PBRData.z) + (IN.texcoord_4.xyz * TESR_PBRData.w);			// partial precision
    OUT.color_0.a = saturate(r1.w * 1.75) * IN.texcoord_5.w;			// partial precision
    OUT.color_0.rgb = (IN.color_0.a * (IN.color_0.rgb - (r1.xyz * q0.xyz))) + (q0.xyz * r1.xyz);			// partial precision

    return OUT;
};

// approximately 8 instruction slots used (1 texture, 7 arithmetic)
