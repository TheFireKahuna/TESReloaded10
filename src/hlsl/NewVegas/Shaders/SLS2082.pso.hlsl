// Shader for LOD terrain during transition with near terrain
//
// Parameters:
#include "includes/DirectShadow.hlsl"
#include "includes/Position.hlsl"

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
sampler2D LODLandNoise : register(s2);

float4 AmbientColor : register(c1);
float4 PSLightColor[10] : register(c3);
//float4 TESR_SmoothedSunDir : register(c67); // x,y,z: center (world space), w: radius
// float4 TESR_DebugVar;


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   PSLightColor[0] const_3       1
//   BaseMap      texture_0       1
//   NormalMap    texture_1       1
//   LODLandNoise texture_2       1
//


// Structures:

struct VS_INPUT {
    
    float2 BaseUV : TEXCOORD0;
    float3 worldMatrix_0: TEXCOORD1;
    float3 worldMatrix_1: TEXCOORD2;
    float3 worldMatrix_2: TEXCOORD3;
    float texcoord_4 : TEXCOORD4;
    float4 texcoord_5 : TEXCOORD5;
    float4 worldPos : TEXCOORD8;
};

    float3 texcoord_1 : TEXCOORD1;
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

    float3 eyeDir = normalize(TESR_CameraPosition.xyz - worldPos.xyz);
    float distance = length(TESR_CameraPosition.xyz);

    float noise = tex2D(LODLandNoise, IN.BaseUV.xy * TESR_TerrainExtraData.w).r;
    noise = lerp(0, noise, IN.texcoord_4.x);

    float4 normal = tex2D(NormalMap, IN.BaseUV.xy);
    normal.rgb = normalize(expand(normal.rgb));
    normal.xyz = normalize( mul( normal.xyz, worldMatrix ) );

    float3 baseColor = tex2D(BaseMap, IN.BaseUV.xy).rgb;

    float roughness = saturate(TESR_TerrainData.y * (1 - normal.a));

    //float NdotL = dot(normal.xyz, TESR_SmoothedSunDir.xyz);
    //float offsetScale = saturate(1 - NdotL);
    //float3 normalOffset = offsetScale * NormalBias * normal.xyz;
    //float4 worldPos = float4(IN.worldPos.xyz + normalOffset, 1.0f);

    //float3 shadowMultiplier = GetLightAmount(worldPos);
    float3 sunDir = sunDirection();
    float3 shadowMultiplier = GetLightAmount(worldPos);
    float3 lighting = getSunLighting(sunDir, PSLightColor[0].rgb * shadowMultiplier, eyeDir, normal.rgb, AmbientColor.rgb, baseColor, roughness);

    float3 final = lighting;
    final = lerp(final, final * (0.8 * noise + 0.55), saturate(TESR_TerrainExtraData.z));  // Apply noise.
    final = lerp(final, IN.texcoord_5.rgb, IN.texcoord_5.a);  // Apply fog.

    OUT.color_0.rgb = final;
    OUT.color_0.a = IN.texcoord_4.x;
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

// approximately 16 instruction slots used (3 texture, 13 arithmetic)
