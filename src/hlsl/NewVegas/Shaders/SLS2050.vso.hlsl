//
//
// Parameters:

float4 FogParam : register(c14);
row_major float4x4 ModelViewProj : register(c0);
row_major float4x4 ObjToCubeSpace : register(c8);


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   ModelViewProj[0]  const_0        1
//   ModelViewProj[1]  const_1        1
//   ModelViewProj[2]  const_2        1
//   ModelViewProj[3]  const_3        1
//   ObjToCubeSpace[0] const_8        1
//   ObjToCubeSpace[1] const_9        1
//   ObjToCubeSpace[2] const_10        1
//   FogParam       const_14      1
//

#include "includes/Helpers.hlsl"


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float3 color_0 : COLOR0;
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float4 texcoord_1 : TEXCOORD1;
    float4 texcoord_2 : TEXCOORD2;
    float4 texcoord_3 : TEXCOORD3;
    float texcoord_4 : TEXCOORD4;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 mdl8;
    float3 q0;
    float3 q1;
    float3 q10;
    float1 q2;
    float3 r1;

    q10.xyz = mul(float3x4(ObjToCubeSpace[0], ObjToCubeSpace[1], ObjToCubeSpace[2]), IN.position);
    r1.xyz = normalize(-q10.xyz);
    mdl8.xyz = mul(float3x4(ModelViewProj[0], ModelViewProj[1], ModelViewProj[2]), IN.position);
    q1.xyz = IN.tangent.xyz * 0.1;
    OUT.color_0.rgb = IN.color_0.rgb;
    OUT.position.w = dot(ModelViewProj[3], IN.position);
    OUT.position.xyz = mdl8.xyz;
    OUT.texcoord_0.xy = IN.texcoord_0.xy;
    q0.xyz = IN.binormal.xyz * 0.1;
    OUT.texcoord_1.w = r1.x;
    OUT.texcoord_1.xyz = mul(float3x3(q1.xyz, q0.xyz, IN.normal.xyz), ObjToCubeSpace[0].xyz);
    OUT.texcoord_2.w = r1.y;
    OUT.texcoord_2.xyz = mul(float3x3(q1.xyz, q0.xyz, IN.normal.xyz), ObjToCubeSpace[1].xyz);
    OUT.texcoord_3.w = r1.z;
    OUT.texcoord_3.xyz = mul(float3x3(q1.xyz, q0.xyz, IN.normal.xyz), ObjToCubeSpace[2].xyz);
    q2.x = 1 - saturate((FogParam.x - length(mdl8.xyz)) / FogParam.y);
    OUT.texcoord_4.x = 1 - pow(abs(q2.x), FogParam.z);

    return OUT;
};

// approximately 40 instruction slots used
