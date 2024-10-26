//
//
// Parameters:

float2 Params : register(c4);
sampler2D TexMap : register(s0);
sampler2D TexMapBlend : register(s1);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   Params       const_4       1
//   TexMap       texture_0       1
//   TexMapBlend  texture_1       1
//


// Structures:

struct VS_INPUT {
    float2 TexUV : TEXCOORD0;
    float2 TexBlendUV : TEXCOORD1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 cloudBlend = tex2D(TexMapBlend, IN.TexBlendUV.xy);
    float4 cloud = tex2D(TexMap, IN.TexUV.xy);
    OUT.color_0.a = (Params.x * (cloudBlend.w - cloud.w)) + cloud.w;
    OUT.color_0.rgb = 0;

    return OUT;
};

// approximately 5 instruction slots used (2 texture, 3 arithmetic)
