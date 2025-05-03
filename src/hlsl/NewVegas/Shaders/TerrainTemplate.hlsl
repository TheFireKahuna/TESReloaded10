//  Template for terrain shaders for blending up to 7 textures and using up to 8 pointlights.

#if defined(__INTELLISENSE__)
    #define PS
    #define POINTLIGHT
#endif

#define TERRAIN

#ifndef ILS
    #define NUM_PT_LIGHTS 3
#else
    #define NUM_PT_LIGHTS 12
#endif

#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"
#include "includes/Parallax.hlsl"
#include "includes/DirectShadow.hlsl"
#include "includes/Position.hlsl"

struct VS_INPUT {
    float4 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float4 uv : TEXCOORD0;
    float4 vertex_color : COLOR0;
    float4 blend_0 : TEXCOORD1;
    float4 blend_1 : TEXCOORD2;
};

struct VS_OUTPUT {
    float4 blend_0 : COLOR0;
    float4 blend_1 : COLOR1;
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0;
    float3 vertex_color : TEXCOORD1;
    float4 worldPos : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 binormal : TEXCOORD4;
    float3 normal : TEXCOORD5;
    float4 fog : TEXCOORD6;
};

#ifdef VS

row_major float4x4 ModelViewProj : register(c0);

float4 FogParam : register(c14);
float3 FogColor : register(c15);
float4 EyePosition : register(c16);

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float4 position = IN.position;

    OUT.sPosition = mul(ModelViewProj, position);
    float4x4 modelMatrix = mul(TESR_ViewProjectionTransform, ModelViewProj);
    float3x3 worldMatrix = (float3x3)modelMatrix;
    float4 worldPos = clipToWorld(OUT.sPosition);
    float3x3 normalMatrix = transpose(inverse3x3(worldMatrix)); // correct normal transform as learned 
    // Calculate the normal vector against the world matrix only and then normalize the final value.
    OUT.normal = mul(normalMatrix, IN.normal.xyz);
    OUT.tangent = mul(worldMatrix, IN.tangent.xyz);
    OUT.binormal = mul(worldMatrix, IN.binormal.xyz);

    OUT.blend_0 = IN.blend_0;
    OUT.blend_1 = IN.blend_1;

    OUT.uv.xy = IN.uv.xy;
    OUT.vertex_color.xyz = clamp(IN.vertex_color.rgb, 0.0f, 1.0f);
    
    // Fog.
    float3 fogPos = OUT.sPosition.xyz;
    #ifdef REVERSED_DEPTH
        fogPos.z = OUT.sPosition.w - fogPos.z;
    #endif
    float fogStrength = 1 - saturate((FogParam.x - length(fogPos)) / FogParam.y);
    fogStrength = log2(fogStrength);
    OUT.fog.a = exp2(fogStrength * FogParam.z);
    OUT.fog.rgb = FogColor.rgb;

    if (TESR_DebugVar.z == 2.0) {
        OUT.worldPos = clipToWorldWithOffset(OUT.sPosition, IN.normal.xyz);
    }
    else if (TESR_DebugVar.z == 1.0) {
        OUT.worldPos = clipToWorldWithOffset(OUT.sPosition, OUT.normal.xyz);
    }
    else {
        OUT.worldPos = clipToWorld(OUT.sPosition);
    }

    return OUT;
};

#endif  // Vertex shader.

struct PS_INPUT
{
    float4 uv : TEXCOORD0;
    float3 vertex_color : TEXCOORD1;
    float4 worldPos : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 binormal : TEXCOORD4;
    float3 normal : TEXCOORD5;
    float4 blend_0 : COLOR0;
    float4 blend_1 : COLOR1;
    float4 fog : TEXCOORD6;
    float4 sPosition : POSITION1;
};

struct PS_OUTPUT {
    float4 color_0 : COLOR0;
};

#ifdef PS

sampler2D BaseMap[7]:register(s0);
sampler2D NormalMap[7]:register(s7);

float4 AmbientColor : register(c1);
float4 PSLightColor[13] : register(c3);
float4 PSLightDir : register(c18);
float4 PSLightPosition[12] : register(c19);
float4 TESR_SunDiskColor : register(c146);
float4 TESR_SunAmount : register(c147);
float4 TESR_SunColor : register(c149);
float4 TESR_ShadowLightPosition[12] : register(c150);
float4 TESR_LightColor[24] : register(c162);

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;
    float4 worldPos = IN.worldPos;
    int texCount = TEX_COUNT;  // Macro.
    float3x3 tbn = float3x3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
    float3 eyeDir = normalize(TESR_CameraPosition.xyz - worldPos.xyz);
    float dist = length(TESR_CameraPosition.xyz - worldPos.xyz);

    float2 dx, dy;
    dx = ddx(IN.uv.xy);
    dy = ddy(IN.uv.xy);
    
    float weights[7] = { 0, 0, 0, 0, 0, 0, 0 };
    float blends[7] = { IN.blend_0.x, IN.blend_0.y, IN.blend_0.z, IN.blend_0.w, IN.blend_1.x, IN.blend_1.y, IN.blend_1.z };
    float2 offsetUV = getParallaxCoords(dist, IN.uv.xy, dx, dy, eyeDir, texCount, BaseMap, blends, weights);

    float roughness = 1.f;
    float3 baseColor = blendDiffuseMaps(IN.vertex_color, offsetUV, texCount, BaseMap, weights);
    float3 combinedNormal = blendNormalMaps(offsetUV, texCount, NormalMap, weights, roughness);
    combinedNormal.xyz = normalize(mul(combinedNormal.xyz,tbn));

    float3 lightTS = sunDirection();
    float parallaxShadowMultiplier = getParallaxShadowMultipler(dist, offsetUV, dx, dy, lightTS, texCount, blends, BaseMap);
    
    float3 shadowMultiplier = GetLightAmount(worldPos);
    float3 lighting = getSunLighting(lightTS, PSLightColor[0].xyz * shadowMultiplier, eyeDir, combinedNormal, AmbientColor.rgb, baseColor, roughness, 1.0, parallaxShadowMultiplier);

    #if defined(POINTLIGHT)
        float3 pointlightDir;
        float3 pointlightPosition;
        [unroll] for (int i = 0; i < NUM_PT_LIGHTS; i++) {
        pointlightPosition = worldToCamera(TESR_ShadowLightPosition[i]);
        pointlightDir = pointlightPosition - worldPos.xyz;
            lighting += getPointLightLighting(pointlightDir, TESR_ShadowLightPosition[i].w, TESR_LightColor[i].rgb, eyeDir, combinedNormal, baseColor, roughness, 1.0);
        }
    #endif
    
    float3 finalColor = lighting;
    finalColor = lerp(finalColor, IN.fog.rgb, IN.fog.a); // Apply fog.

    OUT.color_0.a = 1;
    OUT.color_0.rgb = finalColor;

    return OUT;
};

#endif  // Pixel shader.
