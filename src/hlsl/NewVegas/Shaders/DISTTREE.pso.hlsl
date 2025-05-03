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
    float3 color_0 : COLOR0;
    float4 texcoord_2 : TEXCOORD2;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 q0;
    float4 r0;

    r0.xyzw = tex2D(DiffuseMap, IN.DiffuseUV.xy);			// partial precision
    q0.xyz = (IN.texcoord_2.w * (IN.texcoord_2.xyz - ((IN.color_0.rgb * TESR_PBRData.w) * r0.xyz))) + (r0.xyz * (IN.color_0.rgb * TESR_PBRData.w));			// partial precision
    OUT.color_0.a = r0.w;			// partial precision
    OUT.color_0.rgb = q0.xyz;			// partial precision

    return OUT;
};

// approximately 6 instruction slots used (1 texture, 5 arithmetic)
