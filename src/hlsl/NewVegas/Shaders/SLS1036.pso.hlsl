//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D DiffuseMap : register(s0);
float4 EmittanceColor : register(c2);
sampler2D GlowMap : register(s1);


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   AmbientColor   const_1       1
//   EmittanceColor const_2       1
//   DiffuseMap     texture_0       1
//   GlowMap        texture_1       1
//


// Structures:

struct VS_INPUT {
    float2 DiffuseUV : TEXCOORD0;
    float2 GlowUV : TEXCOORD1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 r0;
    float3 r1;
    float4 t0;

    r0.xyzw = tex2D(GlowMap, IN.GlowUV.xy);
    t0.xyzw = tex2D(DiffuseMap, IN.DiffuseUV.xy);
    OUT.color_0.rgba = t0.xyzw;
    r1.xyz = (r0.xyz * EmittanceColor.rgb) + AmbientColor.rgb;

    return OUT;
};

// approximately 5 instruction slots used (2 texture, 3 arithmetic)
