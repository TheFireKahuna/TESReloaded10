// Shader for LOD terrain
//
// Parameters:
#include "includes/DirectShadow.hlsl"
#include "includes/Position.hlsl"

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
sampler2D LODParentTex : register(s4);
sampler2D LODParentNormals : register(s6);
sampler2D LODLandNoise : register(s7);

float4 AmbientColor : register(c1);
float4 PSLightColor[10] : register(c3);
float4 LODTexParams : register(c31);
//float4 TESR_SmoothedSunDir : register(c67); // x,y,z: center (world space), w: radius

// float4 TESR_DebugVar;


// Registers:
//
//   Name             Reg   Size
//   ---------------- ----- ----
//   AmbientColor     const_1       1
//   PSLightColor[0]     const_3       1
//   LODTexParams     const_31      1
//   BaseMap          texture_0       1
//   NormalMap        texture_1       1
//   LODParentTex     texture_4       1
//   LODParentNormals texture_6       1
//   LODLandNoise     texture_7       1
//


// Structures:

struct VS_INPUT {
    float2 NormalUV : TEXCOORD0;
    float4 color_1 : COLOR1;
    float4 worldPos : TEXCOORD4;
    float3 worldMatrix_0: TEXCOORD5;
    float3 worldMatrix_1: TEXCOORD6;
    float3 worldMatrix_2: TEXCOORD7;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float4 worldPos = IN.worldPos;

    float3x3 worldMatrix = float3x3(IN.worldMatrix_0, IN.worldMatrix_1, IN.worldMatrix_2);

    float3 r0 = LODTexParams.xyw;

    float4 normal = tex2D(NormalMap, IN.NormalUV);
    float3 parentNormal = tex2D(LODParentNormals, (IN.NormalUV * 0.5) + r0.xy).xyz;

    float noise = tex2D(LODLandNoise, IN.NormalUV * TESR_TerrainExtraData.w).r;

    normal.xyz = r0.z >= 1 ? normal.xyz : lerp(parentNormal, normal.xyz, LODTexParams.w);
    normal.xyz = expand(normal.xyz);
    normal.xyz = normalize(normal.xyz);
    normal.xyz = normalize( mul( normal.xyz, worldMatrix ) );

    float2 uv = (IN.NormalUV * 0.9921875) + (1.0 / 256);
    float3 blendColor = tex2D(LODParentTex, (0.5 * uv) + lerp(r0.xy, 0.25, (1.0 / 128))).rgb;
    float3 baseColor = tex2D(BaseMap, uv).rgb;
    float3 eyeDir = normalize(TESR_CameraPosition.xyz - worldPos.xyz);

    // blending between parent tex and basemap + apply noise
    baseColor = r0.z >= 1 ? baseColor : lerp(blendColor, baseColor, LODTexParams.w);
    
    float roughness = saturate(TESR_TerrainData.y * (1 - normal.a));

    float3 sunDir = sunDirection();
    float3 shadowMultiplier = GetLightAmount(worldPos);
    float3 lighting = getSunLighting(sunDir, PSLightColor[0].rgb * shadowMultiplier, eyeDir, normal.xyz, AmbientColor.rgb, baseColor.rgb, roughness);
    float distance = length(TESR_CameraPosition.xyz);

    float3 final = lighting;
    final = lerp(final, final * (0.8 * noise + 0.55), saturate(TESR_TerrainExtraData.z));  // Apply noise.
    final = lerp(final, IN.color_1.rgb, IN.color_1.a);  // Apply fog.

    OUT.color_0.rgb = final;
    OUT.color_0.a = 1;
    if (TESR_DebugVar.x >= 1 && TESR_DebugVar.x <= 8) {
        OUT.color_0.rgb = normal.xyz * 0.5 + 0.5;
    }
    else if (TESR_DebugVar.x == 9)   // coherence check
    {
        OUT.color_0.rgb = saturate(distance / 2048.0).xxx; // should fade smoothly
    }
    else if (TESR_DebugVar.x == 10) {
        OUT.color_0.rgb = lighting;
    }

    return OUT;
};

// approximately 29 instruction slots used (5 texture, 24 arithmetic)
