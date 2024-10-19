//
// Generated by Microsoft (R) HLSL Shader Compiler 9.23.949.2378
//
// Parameters:

row_major float4x4 ModelViewProj : register(c0);
row_major float4x4 WorldMat : register(c4);
float TexScale : register(c8);
float4 QPosAdjust : register(c9);
float ObjectUV : register(c10);
float4 TESR_CameraPosition : register(c11);

// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//   WorldMat      const_4       3
//   TexScale      const_8       1
//   QPosAdjust    const_9       1
//   ObjectUV      const_10      1
//

#include "Includes/Helpers.hlsl"

// Structures:

struct VS_INPUT {
    float4 LPOSITION : POSITION;
    float4 LTEXCOORD_0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
    float4 texcoord_1 : TEXCOORD1;
    float4 texcoord_2 : TEXCOORD2; // modelviewproj matrix 1st row 
    float4 texcoord_3 : TEXCOORD3; // modelviewproj matrix 2nd row
    float4 texcoord_4 : TEXCOORD4; // modelviewproj matrix 3rd row
    float4 texcoord_5 : TEXCOORD5; // modelviewproj matrix 4th row
    float4 texcoord_6 : TEXCOORD6;
    float4 texcoord_7 : TEXCOORD7;
    float4 worldPosition : TEXCOORD8; // world position of surface point
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

#define	weight(v)		dot(v, 1.0)
#define	sqr(v)			((v) * (v))

    const float4 const_7 = {0.5, 0.001, 1, 0};

    float4 mdl0;
    float2 q4;
    float2 r0;
    float2 r1;

    OUT.position = mul(ModelViewProj, IN.LPOSITION); // screen position
    r1.y = dot(WorldMat[1], IN.LPOSITION);
    r1.x = dot(WorldMat[0], IN.LPOSITION);
    OUT.texcoord_0.w = length(WorldMat[0].xyz);
    OUT.texcoord_0.z = dot(WorldMat[2], IN.LPOSITION);
    OUT.texcoord_0.xy = r1.xy;
    r0.xy = const_7.xy;
    mdl0 = r0.x * ModelViewProj[3];
    OUT.texcoord_1 = IN.LPOSITION;
    q4.xy = (r1.xy + QPosAdjust.xy) / TexScale.x;
    OUT.texcoord_2 = (r0.x * ModelViewProj[0]) + mdl0;
    OUT.texcoord_3 = (r0.x * ModelViewProj[1]) + mdl0;
    OUT.texcoord_4 = (r0.x * ModelViewProj[2]) + mdl0;
    OUT.texcoord_5 = ModelViewProj[3];
    OUT.texcoord_6 = (IN.LTEXCOORD_0.xyzx * const_7.zzzw) + const_7.wwwz;
    r0.x = (-(sqr(ObjectUV.x)) < sqr(ObjectUV.x) ? 1.0 : 0.0);
    OUT.texcoord_7.zw = 0;
    OUT.texcoord_7.xy = (r0.x * ((IN.LTEXCOORD_0.xy / (r0.y * TexScale.x)) - q4.xy)) + q4.xy;
    
    OUT.worldPosition = mul(WorldMat, IN.LPOSITION) + TESR_CameraPosition;

    return OUT;
	
};