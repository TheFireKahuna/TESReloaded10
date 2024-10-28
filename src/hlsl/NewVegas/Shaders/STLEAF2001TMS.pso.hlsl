//
//
// Parameters:

sampler2D DiffuseMap : register(s0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   DiffuseMap   texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 DiffuseUV : TEXCOORD0;			// partial precision
    float3 texcoord_1 : TEXCOORD1;			// partial precision
    float4 texcoord_2 : TEXCOORD2;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 r2;

    float4 r0 = tex2D(DiffuseMap, IN.DiffuseUV.xy);			// partial precision
    r2.w = 2 * r0.w;			// partial precision
    r2.xyz = (IN.texcoord_2.w * (IN.texcoord_2.xyz - (r0.xyz * IN.texcoord_1.xyz))) + (r0.xyz * IN.texcoord_1.xyz);			// partial precision
    OUT.color_0.rgba = r2.xyzw;			// partial precision

    return OUT;
};

// approximately 7 instruction slots used (1 texture, 6 arithmetic)
