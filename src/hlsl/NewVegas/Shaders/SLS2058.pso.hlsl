//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D CustomEnvMask : register(s3);
float4 EnvToggles : register(c27);
samplerCUBE EnvironmentCubeMap : register(s1);
sampler2D NormalMap : register(s0);
float4 TESR_LinearObject : register(c28);
float4 TESR_LinearObjectColor : register(c30);
float4 TESR_LinearObjectTerrain : register(c31);
float4 TESR_ShaderExtraColors : register(c33);


// Registers:
//
//   Name               Reg   Size
//   ------------------ ----- ----
//   AmbientColor       const_1       1
//   EnvToggles         const_27      1
//   NormalMap          texture_0       1
//   EnvironmentCubeMap texture_1       1
//   CustomEnvMask      texture_3       1
//


// Structures:

struct VS_INPUT {
    float4 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float4 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float4 texcoord_3 : TEXCOORD3_centroid;			// partial precision
    float2 NormalUV : TEXCOORD0;			// partial precision
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
    float3 color = linearCheck(IN.color_0.rgb, TESR_LinearObjectTerrain.y);

    float4 normals = tex2D(NormalMap, IN.NormalUV);			// partial precision
    float4 customEnvMask = tex2D(CustomEnvMask, IN.NormalUV);			// partial precision
    normals = normalize(expand(normals));			// partial precision

    float3 normalCoords = float3(dot(normals.rgb, IN.texcoord_1.xyz), dot(normals.rgb, IN.texcoord_2.xyz), dot(normals.rgb, IN.texcoord_3.xyz));			// partial precision

    float3 envMapCoords = float3(-IN.texcoord_1.w, -IN.texcoord_2.w, -IN.texcoord_3.w);

    float3 envMap = texCUBE(EnvironmentCubeMap, envreflect(normalize(normalCoords), normalize(envMapCoords))).rgb;			// partial precision
    envMap = linearCheck(envMap, TESR_LinearObjectColor.w);

    float3 finalColor = (envMap * (lerp(normals.w, customEnvMask.x, EnvToggles.w) * EnvToggles.z)) * AmbientColor.a * TESR_ShaderExtraColors.x;			// partial precision
    OUT.color_0.a = 1;			// partial precision
    OUT.color_0.rgb = ((EnvToggles.x <= 0.0 ? finalColor : (finalColor * color)) * IN.texcoord_4);			// partial precision

    return OUT;
};

// approximately 34 instruction slots used (3 texture, 31 arithmetic)
