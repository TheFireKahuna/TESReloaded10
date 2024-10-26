//
//
// Parameters:

sampler2D DiffuseMap : register(s0);
float4 TESR_LinearObject : register(c0);

float4 TESR_ShaderBaseColors : register(c1);

#include "includes/Helpers.hlsl"

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   DiffuseMap   texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 position : TEXCOORD0;			// partial precision
    float4 texcoord4 : TEXCOORD4_centroid;			// partial precision
    float4 texcoord5 : TEXCOORD5_centroid;			// partial precision
    float4 color : COLOR0;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    //float3 color = IN.color.rgb;

    //float4 baseColor = tex2D(DiffuseMap, IN.position);			// partial precision
    //baseColor = linearCheck(baseColor, TESR_LinearObject.y);

    //float3 colorBlend = ((IN.texcoord5.rgb * TESR_ShaderBaseColors.x) + (IN.texcoord4.rgb * TESR_ShaderBaseColors.y));			// partial precision

    //OUT.color_0.rgb = (IN.color_0.a * (IN.color_0.rgb - (baseColor.rgb * colorBlend.rgb))) + (colorBlend.rgb * baseColor.rgb);			// partial precision
    //OUT.color_0.a = saturate(baseColor.a * 1.75) * IN.texcoord5.w;			// partial precision
    OUT.color_0 = IN.color;

    return OUT;
};

// approximately 8 instruction slots used (1 texture, 7 arithmetic)
