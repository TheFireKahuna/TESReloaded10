//
//
// Parameters:

float4 AmbientColor : register(c1);
float4 EnvToggles : register(c27);
samplerCUBE EnvironmentCubeMap : register(s1);
float4 PSLightColor[10] : register(c3);
float4 TESR_LinearObject : register(c28);
float4 TESR_PBRData : register(c29);
float4 TESR_LinearObjectColor : register(c30);
float4 TESR_LinearObjectTerrain : register(c31);
float4 TESR_ShaderBaseColors : register(c32);
float4 TESR_ShaderExtraColors : register(c33);


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
    float4 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float3 color_0 : COLOR0;
    float texcoord_4 : TEXCOORD4;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

#include "includes/Helpers.hlsl"

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float3 sunColor = linearCheck(PSLightColor[0].rgb, TESR_LinearObjectColor.x)  * TESR_ShaderBaseColors.x;
    float3 color = linearCheck(IN.color_0.rgb, TESR_LinearObjectTerrain.y);

    float3 normTexCoords = normalize(IN.texcoord_2.xyz);			// partial precision
    float3 envMap = texCUBE(EnvironmentCubeMap, envreflect(normalize(expand(IN.texcoord_1.xyz)), normTexCoords)).rgb;			// partial precision
    envMap = linearCheck(envMap, TESR_LinearObjectColor.w);
    
    envMap *= AmbientColor.a * TESR_ShaderExtraColors.x;			// partial precision
    envMap = (EnvToggles.x <= 0.0 ? envMap : (envMap * color)) * sunColor;			// partial precision
    OUT.color_0.a = 1.0;			// partial precision
    OUT.color_0.rgb = (envMap * IN.texcoord_1.w) * IN.texcoord_4.x;			// partial precision

    return OUT;
};

// approximately 22 instruction slots used (1 texture, 21 arithmetic)
