// vertex shader for cloud layer
//
// Parameters:

float4 BlendColor[3] : register(c4);
row_major float4x4 ModelViewProj : register(c0);
float TexCoordYOff : register(c12);
float4 TESR_DepthConstants : register(c13);
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
//   TexCoordYOff  const_12      1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
    float3 location : TEXCOORD2;
    float4 color_0 : COLOR0;
    float4 color_1 : COLOR1;
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

    OUT.location = IN.position.xyz;

    IN.position.z -= 5; // lower mesh to avoid visiible seam at the bottom
    OUT.position.xyzw = mul(ModelViewProj, IN.position.xyzw).xyww;

    if (TESR_DepthConstants.z)
        OUT.position.z = 0.0; // invert depth
    else {
        OUT.position.z *= 0.99998; // scale to appear in front of the moon mask
    }

    float2 r0;
    r0.y = IN.texcoord_0.y + TexCoordYOff.x; // scroll clouds
    r0.x = IN.texcoord_0.x;

    OUT.texcoord_0.xy = r0.xy;
    OUT.texcoord_1.xy = r0.xy;

    OUT.color_1 = float4(0, 1, 0, 1); // identify this object as being clouds

    return OUT;
};

// approximately 12 instruction slots used
