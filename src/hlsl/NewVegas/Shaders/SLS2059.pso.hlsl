//
//
// Parameters:

float4 AmbientColor : register(c1);
float4 EnvToggles : register(c27);
samplerCUBE EnvironmentCubeMap : register(s1);
float4 PSLightColor[10] : register(c3);
float4 TESR_PBRData : register(c32);
float4 TESR_PBRExtraData : register(c33);


// Registers:
//
//   Name               Reg   Size
//   ------------------ ----- ----
//   AmbientColor       const_1       1
//   PSLightColor[0]       const_3       1
//   EnvToggles         const_27      1
//   EnvironmentCubeMap texture_1       1
//


// Structures:

struct VS_INPUT {
    float4 texcoord_1 : TEXCOORD1;			// partial precision
    float3 texcoord_2 : TEXCOORD2;			// partial precision
    float3 color_0 : COLOR0;
    float texcoord_4 : TEXCOORD4;
    float4 worldPos : TEXCOORD5;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

#include "includes/Helpers.hlsl"
#include "includes/DirectShadow.hlsl"

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float3 shadowMultiplier = GetLightAmount(IN.worldPos);
    float3 sunColor = PSLightColor[0].rgb  * TESR_PBRData.z * shadowMultiplier;
    float3 color = IN.color_0.rgb;

    float3 normTexCoords = normalize(IN.texcoord_2.xyz);			// partial precision
    float3 envMap = texCUBE(EnvironmentCubeMap, envreflect(normalize(expand(IN.texcoord_1.xyz)), normTexCoords)).rgb;			// partial precision
    
    envMap *= AmbientColor.a * TESR_PBRData.w;			// partial precision
    envMap = (EnvToggles.x <= 0.0 ? envMap : (envMap * color)) * sunColor;			// partial precision
    OUT.color_0.a = 1.0;			// partial precision
    OUT.color_0.rgb = (envMap * IN.texcoord_1.w) * IN.texcoord_4.x;			// partial precision

    return OUT;
};

// approximately 22 instruction slots used (1 texture, 21 arithmetic)
