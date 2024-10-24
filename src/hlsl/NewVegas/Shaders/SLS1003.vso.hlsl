//
//
// Parameters:

float4 LightData[10] : register(c25);
row_major float4x4 ModelViewProj : register(c0);

#include "includes/Helpers.hlsl"


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//   LightData[0]     const_25      1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float3 normal : NORMAL;
    float4 texcoord_0 : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;

#define	TanSpaceProj	float3x3(IN.tangent.xyz, IN.binormal.xyz, IN.normal.xyz)
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
    float2 texcoord_2 : TEXCOORD2;
    float3 texcoord_3 : TEXCOORD3;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    OUT.position = mul(ModelViewProj, IN.position);
    float3 l0 = LightData[0].xyz - IN.position.xyz;
    float3 r0 = l0.xyz / LightData[0].w;
    OUT.texcoord_0.xy = IN.texcoord_0.xy;
    OUT.texcoord_1.xy = compress(r0.xy);	// [-1,+1] to [0,1]
    OUT.texcoord_2.x = compress(r0.z);	// [-1,+1] to [0,1]
    OUT.texcoord_2.y = 0.5;
    OUT.texcoord_3.xyz = normalize(mul(TanSpaceProj, normalize(l0.xyz)));

    return OUT;
};

// approximately 20 instruction slots used
