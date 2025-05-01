//
//
// Parameters:

float4 Bones[54] : register(c44);
row_major float4x4 SkinModelViewProj : register(c1);
#include "includes/Helpers.hlsl"
#include "includes/SkinHelpers.hlsl"


// Registers:
//
//   Name              Reg   Size
//   ----------------- ----- ----
//   SkinModelViewProj[0] const_1        1
//   SkinModelViewProj[1] const_2        1
//   SkinModelViewProj[2] const_3        1
//   SkinModelViewProj[3] const_4        1
//   Bones[0]             const_44     54
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
    float3 blendweight : BLENDWEIGHT;
    float4 blendindices : BLENDINDICES;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
    float2 texcoord_2 : TEXCOORD2;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 position = IN.position.xyzw;

    float4 offset = IN.blendindices.zyxw * 765.01001;
    float4 blend = IN.blendweight.xyzz;
    blend.w = 1 - weight(IN.blendweight.xyz);
    position.w = 1;
    position.xyz = BonesTransformPosition(Bones, offset, blend, position);
    OUT.position = mul(SkinModelViewProj, position);

    OUT.texcoord_0.xy = IN.texcoord_0.xy;
    OUT.texcoord_1.xy = IN.texcoord_0.xy;
    OUT.texcoord_2.xy = IN.texcoord_0.xy;

    return OUT;
};

// approximately 31 instruction slots used
