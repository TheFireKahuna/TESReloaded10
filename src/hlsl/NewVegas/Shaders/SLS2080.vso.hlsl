// Vertex Shader for LOD terrain during transition with near terrain
//
// Parameters:
#include "includes/Position.hlsl"

row_major float4x4 ModelViewProj : register(c0);
float4 FogParam : register(c14);
float3 FogColor : register(c15);
float4 EyePosition : register(c16);
float4 LandBlendParams : register(c19);
float4 LightData[10] : register(c25);


// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   ModelViewProj[0]   const_0        1
//   ModelViewProj[1]   const_1        1
//   ModelViewProj[2]   const_2        1
//   ModelViewProj[3]   const_3        1
//   FogParam        const_14      1
//   FogColor        const_15      1
//   LandBlendParams const_19      1
//   LightData[0]       const_25      1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float3 worldMatrix_0: TEXCOORD1;
    float3 worldMatrix_1: TEXCOORD2;
    float3 worldMatrix_2: TEXCOORD3;
    float texcoord_4 : TEXCOORD4;
    float4 texcoord_5 : TEXCOORD5;
    float4 worldPos : TEXCOORD8;
};


#include "includes/Helpers.hlsl"
// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 r0;

    float4 position = IN.position;

    OUT.position = mul(ModelViewProj, position);

    r0.xy = (IN.texcoord_0.xy * 0.015625) + LandBlendParams.xy;
    r0.z = 1 - r0.x;
    
    OUT.texcoord_0.xy = (r0.zy * 0.9921875) + (1.0 / 256);
    OUT.texcoord_4.x = 1 - saturate((9625.59961 - sqrt(r0.y + r0.x)) * 0.000375600968);
    
    // Fog.
    float3 fogPos = OUT.position.xyz;
    #ifdef REVERSED_DEPTH
        fogPos.z = OUT.position.w - fogPos.z;
    #endif
    float fogStrength = 1 - saturate((FogParam.x - length(fogPos)) / FogParam.y);
    fogStrength = log2(fogStrength);
    OUT.texcoord_5.a = exp2(fogStrength * FogParam.z);
    OUT.texcoord_5.rgb = FogColor.rgb;

    OUT.worldPos = clipToWorld(OUT.position);
    
    float3x3 worldMatrix = (float3x3)mul(TESR_InvViewProjectionTransform, ModelViewProj);
    OUT.worldMatrix_0 = worldMatrix[0];
    OUT.worldMatrix_1 = worldMatrix[1];
    OUT.worldMatrix_2 = worldMatrix[2];

    return OUT;
};

// approximately 33 instruction slots used
