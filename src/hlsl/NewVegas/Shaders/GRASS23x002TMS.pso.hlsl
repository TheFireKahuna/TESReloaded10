//
//
// Parameters:

sampler2D DiffuseMap : register(s0);
sampler2D ShadowMap : register(s1);
sampler2D ShadowMaskMap : register(s2);


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   DiffuseMap    texture_0       1
//   ShadowMap     texture_1       1
//   ShadowMaskMap texture_2       1
//


// Structures:

struct VS_INPUT {
    float2 DiffuseUV : TEXCOORD0;			// partial precision
    float3 texcoord_4 : TEXCOORD4_centroid;			// partial precision
    float4 texcoord_5 : TEXCOORD5_centroid;			// partial precision
    float2 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float3 texcoord_3 : TEXCOORD3_centroid;			// partial precision
    float4 color_0 : COLOR0;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
/*
    float3 q2;
    float4 r1;
    float1 t0;
    float3 t1;

    t0.x = tex2D(ShadowMaskMap, IN.texcoord_3.xy);			// partial precision
    t1.xyz = tex2D(ShadowMap, IN.texcoord_2.xy);			// partial precision
    r1.xyzw = tex2D(DiffuseMap, IN.DiffuseUV.xy);			// partial precision
    q2.xyz = (((IN.texcoord_3.z * ((t1.xyz - 1) * t0.x)) + 1) * IN.texcoord_5.xyz) + IN.texcoord_4.xyz;			// partial precision
    OUT.color_0.a = saturate(r1.w * 1.75) * IN.texcoord_5.w;			// partial precision
    OUT.color_0.rgb = (IN.color_0.a * (IN.color_0.rgb - (r1.xyz * q2.xyz))) + (q2.xyz * r1.xyz);			// partial precision
*/
    OUT.color_0 = IN.color_0;
    return OUT;
};

// approximately 13 instruction slots used (3 texture, 10 arithmetic)
