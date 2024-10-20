//
//
// Parameters:

row_major float4x4 ModelViewProj : register(c0);
float4 BlendColor[3] : register(c4);
float4 TESR_DepthConstants : register(c13);
float4 TESR_DebugVar : register(c14);
float4 TESR_LinearSky: register(c38);

#include "Includes/Helpers.hlsl"


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//   BlendColor[0]    const_4        1
//   BlendColor[1]    const_5        1
//   BlendColor[2]    const_6        1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float3 eye : TEXCOORD0_centroid;
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 color = linearCheck(IN.color_0.rgb, TESR_LinearSky.y);
    
	float3 blendColor0 = linearCheck(BlendColor[0].rgb, TESR_LinearSky.w);
	float3 blendColor1 = linearCheck(BlendColor[1].rgb, TESR_LinearSky.w);
	float3 blendColor2 = linearCheck(BlendColor[2].rgb, TESR_LinearSky.w);

    OUT.color_0.rgb = (color.r * blendColor0) + (color.g * blendColor1) + (color.b * blendColor2);
    OUT.color_0.a = BlendColor[0].a * IN.color_0.a;


    OUT.position.xyzw = mul(ModelViewProj, IN.position).xyww;

    if (TESR_DepthConstants.z > 0) {
        OUT.position.z = 0; // invert depth
    }
 
    OUT.eye.xyz = IN.position.xyz;


    return OUT;
};

// approximately 8 instruction slots used
