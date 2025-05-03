// Template for object lighting shaders.
//
// VS
//
// AD
// SLS2028 - LIGHTS = 2 (ONLY_LIGHT)
// SLS2029 - LIGHTS = 2, SKIN (ONLY_LIGHT)
// SLS2030 - LIGHTS = 2, PROJ_SHADOW (ONLY_LIGHT)
// SLS2031 - LIGHTS = 2, PROJ_SHADOW, SKIN (ONLY_LIGHT)
// SLS2032 - LIGHTS = 3 (ONLY_LIGHT)
// SLS2033 - LIGHTS = 3, SKIN (ONLY_LIGHT)
// SLS2034 - LIGHTS = 3, PROJ_SHADOW (ONLY_LIGHT)
// SLS2035 - LIGHTS = 3, PROJ_SHADOW, SKIN (ONLY_LIGHT)
//
// ADTS
// SLS2000 - Base
// SLS2001 - LOD
// SLS2003 - SKIN
// SLS2004 - PROJ_SHADOW
// SLS2006 - PROJ_SHADOW, SKIN
// SLS2007 - STBB
// SLS2008 - LIGHTS = 2
// SLS2009 - LIGHTS = 2, SKIN
// SLS2010 - LIGHTS = 2, PROJ_SHADOW
// SLS2011 - LIGHTS = 2, PROJ_SHADOW, SKIN
// SLS2012 - SPECULAR
// SLS2013 - SPECULAR, SKIN
// SLS2014 - SPECULAR, PROJ_SHADOW
// SLS2015 - SPECULAR, PROJ_SHADOW, SKIN
// SLS2016 - SPECULAR, LIGHTS = 2
// SLS2017 - SPECULAR, LIGHTS = 2, SKIN
// SLS2018 - SPECULAR, LIGHTS = 2, PROJ_SHADOW
// SLS2019 - SPECULAR, LIGHTS = 2, PROJ_SHADOW, SKIN
// (ignored) SLS2102 - LIGHTS = 0
//
// ADTS10
// SLS2020 - LIGHTS = 9
// SLS2021 - LIGHTS = 9, SKIN
// SLS2022 - LIGHTS = 4
// SLS2023 - LIGHTS = 4, OPT
// SLS2024 - LIGHTS = 4, SKIN
// SLS2025 - LIGHTS = 4, SPECULAR
// SLS2026 - LIGHTS = 4, SPECULAR, OPT
// SLS2027 - LIGHTS = 4, SPECULAR, SKIN
//
// DiffusePt
// SLS2036 - LIGHTS = 2 (DIFFUSE)
// SLS2037 - LIGHTS = 2, SKIN (DIFFUSE)
// SLS2038 - LIGHTS = 3 (DIFFUSE)
// SLS2039 - LIGHTS = 3, SKIN (DIFFUSE)
//
// Specular
// SLS2040 - Base (ONLY_SPECULAR)
// SLS2041 - SKIN (ONLY_SPECULAR)
// SLS2042 - PROJ_SHADOW (ONLY_SPECULAR)
// SLS2043 - PROJ_SHADOW, SKIN (ONLY_SPECULAR)
// SLS2044 - POINT (ONLY_SPECULAR)
// SLS2045 - POINT, SKIN (ONLY_SPECULAR)
// SLS2046 - POINT, NUM_PT_LIGHTS = 2 (ONLY_SPECULAR)
// SLS2047 - POINT, NUM_PT_LIGHTS = 2, SKIN (ONLY_SPECULAR)
// SLS2048 - POINT, NUM_PT_LIGHTS = 3 (ONLY_SPECULAR)
// SLS2049 - POINT, NUM_PT_LIGHTS = 3, SKIN (ONLY_SPECULAR)
//
// PS
//
// AD
// SLS2037 - LIGHTS = 2 (ONLY_LIGHT, OPT)
// SLS2038 - LIGHTS = 2, SI (ONLY_LIGHT, OPT)
// SLS2039 - LIGHTS = 2, PROJ_SHADOW (ONLY_LIGHT, OPT)
// SLS2040 - LIGHTS = 2, SI, PROJ_SHADOW (ONLY_LIGHT, OPT)
// SLS2041 - LIGHTS = 3 (ONLY_LIGHT, OPT)
// SLS2042 - LIGHTS = 3, SI (ONLY_LIGHT, OPT)
// SLS2043 - LIGHTS = 3, PROJ_SHADOW (ONLY_LIGHT, OPT)
// SLS2044 - LIGHTS = 3, SI, PROJ_SHADOW (ONLY_LIGHT, OPT)
//
// ADTS
// SLS2000 - Default
// SLS2001 - OPT
// SLS2002 - OPT, LOD
// SLS2004 - SI 
// SLS2005 - PROJ_SHADOW
// SLS2007 - SI, PROJ_SHADOW 
// SLS2008 - STBB
// SLS2009 - HAIR
// SLS2010 - HAIR, PROJ_SHADOW
// SLS2011 - LIGHTS = 2
// SLS2012 - LIGHTS = 2, SI
// SLS2013 - LIGHTS = 2, HAIR
// SLS2014 - LIGHTS = 2, PROJ_SHADOW
// SLS2015 - LIGHTS = 2, SI, PROJ_SHADOW
// SLS2016 - LIGHTS = 2, HAIR, PROJ_SHADOW
// SLS2017 - SPECULAR
// SLS2018 - SPECULAR, SI
// SLS2019 - SPECULAR, HAIR
// SLS2020 - SPECULAR, PROJ_SHADOW
// SLS2021 - SPECULAR, SI, PROJ_SHADOW
// SLS2022 - SPECULAR, HAIR, PROJ_SHADOW
// SLS2023 - SPECULAR, LIGHTS = 2
// SLS2024 - SPECULAR, LIGHTS = 2, SI
// SLS2026 - SPECULAR, LIGHTS = 2, PROJ_SHADOW
// SLS2027 - SPECULAR, LIGHTS = 2, SI, PROJ_SHADOW
// (ignored) SLS2151 - LIGHTS = 0, SILHOUETTE
//
// ADTS10
// SLS2029 - LIGHTS = 9
// SLS2030 - LIGHTS = 9, SI
// SLS2031 - LIGHTS = 4
// SLS2032 - LIGHTS = 4, OPT 
// SLS2033 - LIGHTS = 4, SI
// SLS2034 - LIGHTS = 4, SPECULAR
// SLS2035 - LIGHTS = 4, SPECULAR, OPT
// SLS2036 - LIGHTS = 4, SPECULAR, SI
//
// DiffusePt
// SLS2045 - LIGHTS = 2 (DIFFUSE)
// SLS2046 - LIGHTS = 3 (DIFFUSE)
//
// Specular
// SLS2047 - Base (ONLY_SPECULAR)
// SLS2048 - HAIR (ONLY_SPECULAR)
// SLS2049 - PROJ_SHADOW (ONLY_SPECULAR)
// SLS2050 - PROJ_SHADOW, HAIR (ONLY_SPECULAR)
// SLS2051 - POINT (ONLY_SPECULAR)
// SLS2052 - POINT, HAIR (ONLY_SPECULAR)
// SLS2053 - NUM_PT_LIGHTS = 2, POINT (ONLY_SPECULAR)
// SLS2054 - NUM_PT_LIGHTS = 2, POINT, HAIR (ONLY_SPECULAR)
// SLS2055 - NUM_PT_LIGHTS = 3, POINT (ONLY_SPECULAR)
// SLS2056 - NUM_PT_LIGHTS = 3, POINT, HAIR (ONLY_SPECULAR)

#if defined(__INTELLISENSE__)
    #define VS
    #define DIFFUSE
    #define LIGHTS 2
#endif

#if defined(DIFFUSE)
    #define ONLY_LIGHT
    #define OPT
#endif

#if defined(ONLY_SPECULAR)
    #define ONLY_LIGHT
    #define SPECULAR
#endif

#ifdef ONLY_LIGHT
    #define NO_FOG
    #define NO_VERTEX_COLOR
#endif

#include "includes/Helpers.hlsl"
#include "includes/Object.hlsl"
#include "includes/DirectShadow.hlsl"
#include "includes/Position.hlsl"

#ifdef SKIN
    #include "includes/SkinHelpers.hlsl"
#endif

// Toggles.
#define useVertexColor Toggles.x
#define useFog Toggles.y
#define glossPower Toggles.z
#define alphaTestRef Toggles.w

struct VS_INPUT {
    float4 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
#ifndef NO_VERTEX_COLOR
    float4 vertexColor : COLOR0;
#endif
#ifdef SKIN
    float3 blendWeight : BLENDWEIGHT;
    float4 blendIndices : BLENDINDICES;
#endif
};

#if defined(VS) && LIGHTS < 4

struct VS_OUTPUT {
#ifndef NO_VERTEX_COLOR
    float4 vertexColor : COLOR0;
#endif
#ifndef NO_FOG
    float4 fogColor : COLOR1;
#endif
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 binormal : TEXCOORD4;
    
    float4 worldPos: TEXCOORD6;
};

#ifndef NO_FOG
    float3 FogColor : register(c15);
    float4 FogParam : register(c14);
#endif

float4 LightData[10] : register(c25);

#ifndef SKIN
    row_major float4x4 ModelViewProj : register(c0);
#else
    row_major float4x4 SkinModelViewProj : register(c1);
    float4 Bones[54] : register(c44);
#endif

float4 EyePosition : register(c16);


VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    
    OUT.uv = IN.uv.xy;
    
    float4 position = IN.position;
    
    #ifndef SKIN
        OUT.sPosition = mul(ModelViewProj, position);
        float4x4 modelMatrix = mul(TESR_ViewProjectionTransform, ModelViewProj);
        float3x3 worldMatrix = (float3x3)modelMatrix;
        float3x3 normalMatrix = transpose(inverse3x3(worldMatrix)); // correct normal transform as learned 
        // Calculate the normal vector against the world matrix only and then normalize the final value.
        OUT.normal = mul(normalMatrix, IN.normal);
        OUT.tangent = mul(worldMatrix, IN.tangent);
        OUT.binormal = mul(worldMatrix, IN.binormal);
        if (TESR_DebugVar.z == 2.0) {
            OUT.worldPos = clipToWorldWithOffset(OUT.sPosition, IN.normal.xyz);
        }
        else if (TESR_DebugVar.z == 1.0) {
            OUT.worldPos = clipToWorldWithOffset(OUT.sPosition, OUT.normal.xyz);
        }
        else {
            OUT.worldPos = clipToWorld(OUT.sPosition);
        }
        
    #else
        float4 offset = IN.blendIndices.zyxw * 765.01001;
        float4 blend = IN.blendWeight.xyzz;
        blend.w = 1 - weight(IN.blendWeight.xyz);
    
        position.w = 1;
        position.xyz = BonesTransformPosition(Bones, offset, blend, position);
    
        OUT.sPosition = mul(SkinModelViewProj, position);
        float4x4 modelMatrix = mul(TESR_ViewProjectionTransform, SkinModelViewProj);
        float3x3 worldMatrix = (float3x3)modelMatrix;
        float3x3 normalMatrix = transpose(inverse3x3(worldMatrix)); // correct normal transform as learned 
        // Calculate the normal vector against the world matrix only and then normalize the final value.
        OUT.normal = mul(normalMatrix, IN.normal);
        OUT.tangent = mul(worldMatrix, IN.tangent);
        OUT.binormal = mul(worldMatrix, IN.binormal);
        if (TESR_DebugVar.z == 2.0) {
            OUT.worldPos = clipToWorldWithOffset(OUT.sPosition, IN.normal.xyz);
        }
        else if (TESR_DebugVar.z == 1.0) {
            OUT.worldPos = clipToWorldWithOffset(OUT.sPosition, OUT.normal.xyz);
        }
        else {
            OUT.worldPos = clipToWorld(OUT.sPosition);
        }
    #endif
    
    #ifndef NO_VERTEX_COLOR
        OUT.vertexColor = clamp(IN.vertexColor, 0.0f, 1.0f);
    #endif

    #ifndef NO_FOG
        float3 fogPos = OUT.sPosition.xyz;
    
        #ifdef REVERSED_DEPTH
            fogPos.z = OUT.sPosition.w - fogPos.z;
        #endif
    
        float fogStrength = 1 - saturate((FogParam.x - length(fogPos)) / FogParam.y);
        fogStrength = log2(fogStrength);
        OUT.fogColor.a = exp2(fogStrength * FogParam.z);
        OUT.fogColor.rgb = FogColor.rgb;
    #endif

    return OUT;
};

#elif defined(VS) && LIGHTS >= 4

#if LIGHTS > 4
    #define MAX_LIGHTS 6
#elif LIGHTS > 3 && !defined(SPECULAR)
    #define MAX_LIGHTS 4
#else
    #define MAX_LIGHTS 3
#endif

struct VS_OUTPUT {
    float4 vertexColor : COLOR0;
    float4 fogColor : COLOR1;
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 binormal : TEXCOORD4;
    
    float4 worldPos: TEXCOORD6;
};

float3 FogColor : register(c15);
float4 FogParam : register(c14);
float4 LightData[10] : register(c25);
#ifndef SKIN
row_major float4x4 ModelViewProj : register(c0);
#else
    row_major float4x4 SkinModelViewProj : register(c1);
    float4 Bones[54] : register(c44);
#endif
float4 EyePosition : register(c16);
#ifndef OPT
    float4 fvars0 : register(c17);
#endif

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    
    OUT.uv.xy = IN.uv.xy;
    
    float4 position = IN.position;
    
    #ifndef SKIN
        OUT.sPosition = mul(ModelViewProj, position);
        float4x4 modelMatrix = mul(TESR_ViewProjectionTransform, ModelViewProj);
        float3x3 worldMatrix = (float3x3)modelMatrix;
        float3x3 normalMatrix = transpose(inverse3x3(worldMatrix)); // correct normal transform as learned 
        // Calculate the normal vector against the world matrix only and then normalize the final value.
        OUT.normal = mul(normalMatrix, IN.normal);
        OUT.tangent = mul(worldMatrix, IN.tangent);
        OUT.binormal = mul(worldMatrix, IN.binormal);
        if (TESR_DebugVar.z == 2.0) {
            OUT.worldPos = clipToWorldWithOffset(OUT.sPosition, IN.normal.xyz);
        }
        else if (TESR_DebugVar.z == 1.0) {
            OUT.worldPos = clipToWorldWithOffset(OUT.sPosition, OUT.normal.xyz);
        }
        else {
            OUT.worldPos = clipToWorld(OUT.sPosition);
        }
    #else
        float4 offset = IN.blendIndices.zyxw * 765.01001;
        float4 blend = IN.blendWeight.xyzz;
        blend.w = 1 - weight(IN.blendWeight.xyz);
    
        position.w = 1;
        position.xyz = BonesTransformPosition(Bones, offset, blend, position);
        OUT.sPosition = mul(SkinModelViewProj, position);
        float4x4 modelMatrix = mul(TESR_ViewProjectionTransform, SkinModelViewProj);
        float3x3 worldMatrix = (float3x3)modelMatrix;
        float3x3 normalMatrix = transpose(inverse3x3(worldMatrix)); // correct normal transform as learned 
        // Calculate the normal vector against the world matrix only and then normalize the final value.
        OUT.normal = mul(normalMatrix, IN.normal);
        OUT.tangent = mul(worldMatrix, IN.tangent);
        OUT.binormal = mul(worldMatrix, IN.binormal);
        if (TESR_DebugVar.z == 2.0) {
            OUT.worldPos = clipToWorldWithOffset(OUT.sPosition, IN.normal.xyz);
        }
        else if (TESR_DebugVar.z == 1.0) {
            OUT.worldPos = clipToWorldWithOffset(OUT.sPosition, OUT.normal.xyz);
        }
        else {
            OUT.worldPos = clipToWorld(OUT.sPosition);
        }
    #endif
    
    OUT.vertexColor = clamp(IN.vertexColor, 0.0f, 1.0f);

    float3 fogPos = OUT.sPosition.xyz;
    #ifdef REVERSED_DEPTH
        fogPos.z = OUT.sPosition.w - fogPos.z;
    #endif
    float fogStrength = 1 - saturate((FogParam.x - length(fogPos)) / FogParam.y);
    fogStrength = log2(fogStrength);
    OUT.fogColor.a = exp2(fogStrength * FogParam.z);
    OUT.fogColor.rgb = FogColor.rgb;

    return OUT;
};
#endif // Vertex shaders.

#if defined(PS) && (!defined(LIGHTS) || LIGHTS < 4)

struct PS_INPUT {
#ifndef NO_VERTEX_COLOR
    float3 vertexColor : COLOR0;
#endif
#ifndef NO_FOG
    float4 fogColor : COLOR1;
#endif
    float2 uv : TEXCOORD0;
    float3 normal : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 binormal : TEXCOORD4;
    
    float4 worldPos: TEXCOORD6;
};

struct PS_OUTPUT {
    float4 color : COLOR0;
};

#if defined(DIFFUSE) || defined(ONLY_SPECULAR)
    sampler2D NormalMap : register(s0);
#else
    sampler2D BaseMap : register(s0);
    sampler2D NormalMap : register(s1);
#endif

#if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
    float4 AmbientColor : register(c1);
#endif

float4 PSLightColor[10] : register(c3);
float4 TESR_SunAmount : register(c147);
float4 TESR_SunColor : register(c149);
float4 TESR_ShadowLightPosition[12] : register(c150);
float4 TESR_LightColor[24] : register(c162);

#if (defined(SI) || defined(HAIR)) && !defined(ONLY_SPECULAR)
    #ifdef ONLY_LIGHT
        sampler2D GlowMap : register(s3);
    #else
        sampler2D GlowMap : register(s4);
    #endif
    float4 EmittanceColor : register(c2);
#endif

#ifndef OPT
    float4 Toggles : register(c27);
#endif

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;
    float4 worldPos = IN.worldPos;
    float3 eyeDir = normalize(TESR_CameraPosition.xyz - worldPos.xyz);
    float3x3 tbn = float3x3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
    
    #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
        float4 baseColor = tex2D(BaseMap, IN.uv.xy);
    
        #if defined(ONLY_LIGHT)
            baseColor.rgb = 1;
        #endif
    #else
        float4 baseColor = 1;
    #endif
    
    #if !defined(OPT) && !defined(ONLY_SPECULAR)
        clip(AmbientColor.a >= 1 ? 0 : (baseColor.a - alphaTestRef));
    #endif
    
    float4 normal = tex2D(NormalMap, IN.uv.xy);
    normal.xyz = normalize(normal.xyz * 2.0 - 1.0);
    normal.xyz = normalize(mul(normal.xyz,tbn));

    
    float roughness = getRoughness(normal.a);
    
    
    #ifndef NO_VERTEX_COLOR
        #if defined(HAIR)
            float4 glow = tex2D(GlowMap, IN.uv.xy);
            baseColor.rgb = (2 * ((IN.vertexColor.g * (EmittanceColor.rgb - 0.5)) + 0.5)) * lerp(baseColor.rgb, glow.rgb, glow.a);
        #elif !defined(OPT)
            baseColor.rgb = useVertexColor <= 0 ? baseColor.rgb : (baseColor.rgb * IN.vertexColor.rgb);
        #else
            baseColor.rgb = baseColor.rgb * IN.vertexColor.rgb;
        #endif
    #endif
    
    // Vanilla shadows.
    float3 shadowMultiplier = GetLightAmount(worldPos);
    float3 pointlightDir;
    float3 pointlightPosition;
    float3 lighting;

    #if !defined(DIFFUSE) && !defined(POINT)
        float3 sunDir = sunDirection();
        lighting = getSunLighting(sunDir, PSLightColor[0].xyz * shadowMultiplier, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    #else
        // Pointlights only.
        pointlightPosition = worldToCamera(TESR_ShadowLightPosition[0]);
        pointlightDir = pointlightPosition - worldPos.xyz;
        lighting = getPointLightLighting(pointlightDir, TESR_ShadowLightPosition[0].w, TESR_LightColor[0].rgb * shadowMultiplier, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    #endif
    
    // Self emmitance.
    #ifdef SI
        float3 glow = tex2D(GlowMap, IN.uv.xy).rgb;
        lighting += baseColor.rgb * glow.rgb * EmittanceColor.rgb;
    #endif
    
    #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
        lighting += getAmbientLighting(AmbientColor.rgb, baseColor.rgb);
    #endif
    
    // Other light sources.
    #if LIGHTS > 1 || NUM_PT_LIGHTS > 1
        pointlightPosition = worldToCamera(TESR_ShadowLightPosition[1]);
        pointlightDir = pointlightPosition - worldPos.xyz;
        lighting += getPointLightLighting(pointlightDir, TESR_ShadowLightPosition[1].w, TESR_LightColor[1].rgb, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    #endif
    
    #if LIGHTS > 2 || NUM_PT_LIGHTS > 2
        pointlightPosition = worldToCamera(TESR_ShadowLightPosition[2]);
        pointlightDir = pointlightPosition - worldPos.xyz;
        lighting += getPointLightLighting(pointlightDir, TESR_ShadowLightPosition[2].w, TESR_LightColor[2].rgb, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    #endif
    
    float3 finalColor = lighting.rgb;
    
    // Fog.
    #ifndef NO_FOG
        #ifndef OPT
            finalColor.rgb = (useFog <= 0.0 ? finalColor.rgb : lerp(finalColor.rgb, IN.fogColor.rgb, IN.fogColor.a));
        #else
            finalColor.rgb = lerp(finalColor.rgb, IN.fogColor.rgb, IN.fogColor.a);
        #endif
    #endif
    
    OUT.color.rgb = finalColor.rgb;
    
    #if defined(DIFFUSE)
        OUT.color.a = 1;
    #elif defined(ONLY_SPECULAR)
        OUT.color.a = weight(finalColor.rgb);
    #elif defined(ONLY_LIGHT)
        OUT.color.a = baseColor.a;
    #else
        OUT.color.a = baseColor.a * AmbientColor.a;
    #endif

    return OUT;
}

#elif defined(PS) && LIGHTS >= 4

#if LIGHTS > 4
    #define MAX_LIGHTS 6
#elif LIGHTS > 3 && !defined(SPECULAR)
    #define MAX_LIGHTS 4
#else
    #define MAX_LIGHTS 3
#endif

struct PS_INPUT {
    float4 vertexColor : COLOR0;
    float4 fogColor : COLOR1;
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0; // .z and .w are .x and .y of worldPos
    float3 normal : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 binormal : TEXCOORD4;
    
    float4 worldPos: TEXCOORD6;
};

struct PS_OUTPUT {
    float4 color : COLOR0;
};

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);

float4 AmbientColor : register(c1);

float4 PSLightColor[10] : register(c3);
float4 PSLightPosition[8] : register(c19);
float4 TESR_SunAmount : register(c147);
float4 TESR_SunColor : register(c149);
float4 TESR_ShadowLightPosition[12] : register(c150);
float4 TESR_LightColor[24] : register(c162);

#ifndef OPT
    float4 EmittanceColor : register(c2);

    float4 Toggles : register(c27);

    #define lightsUsed EmittanceColor.a
    #define lightOffset 0
    #define glossPow Toggles.z
#else
    #define lightsUsed PSLightColor[0].a
    #define lightOffset 1
    #define glossPow PSLightColor[1].w
#endif

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;
    float4 worldPos = IN.worldPos;
    float3 eyeDir = normalize(TESR_CameraPosition.xyz - worldPos.xyz);
    float3x3 tbn = float3x3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

    float4 baseColor = tex2D(BaseMap, IN.uv.xy);
    
    #ifndef OPT
        clip(AmbientColor.a >= 1 ? 0 : (baseColor.a - alphaTestRef));
    #endif
    
    #ifndef OPT
        baseColor.rgb = useVertexColor <= 0 ? baseColor.rgb : (baseColor.rgb * IN.vertexColor.rgb);
    #else
        baseColor.rgb = baseColor.rgb * IN.vertexColor.rgb;
    #endif
    
    float4 normal = tex2D(NormalMap, IN.uv.xy);
    normal.xyz = normalize(normal.xyz * 2.0 - 1.0);
    normal.xyz = normalize(mul(normal.xyz,tbn));

    float roughness = getRoughness(normal.a);
        
    // Lighting.
    
    float att;
    float3 shadowMultiplier = GetLightAmount(worldPos);
    
    float3 pointlightDir;
    float3 pointlightPosition;
    float3 lighting;
    #ifndef OPT
        float3 sunDir = sunDirection();
        lighting = getSunLighting(sunDir, PSLightColor[0].xyz * shadowMultiplier, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    #else
        pointlightPosition = worldToCamera(TESR_ShadowLightPosition[0]);
        pointlightDir = pointlightPosition - worldPos.xyz;
        att = vanillaAtt(pointlightDir, TESR_ShadowLightPosition[0].w);
        lighting = getPointLightLightingAtt(pointlightDir, att, TESR_LightColor[0].rgb, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    #endif
    
    pointlightPosition = worldToCamera(TESR_ShadowLightPosition[lightOffset + 0]);
    pointlightDir = pointlightPosition - worldPos.xyz;
    att = vanillaAtt(pointlightDir, TESR_ShadowLightPosition[lightOffset + 0].w);
    lighting += (1 >= lightsUsed ? 0.0 : 1.0) * getPointLightLightingAtt(pointlightDir, att, TESR_LightColor[lightOffset + 0].rgb, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    
    pointlightPosition = worldToCamera(TESR_ShadowLightPosition[lightOffset + 1]);
    pointlightDir = pointlightPosition - worldPos.xyz;
    att = vanillaAtt(pointlightDir, TESR_ShadowLightPosition[lightOffset + 1].w);
    lighting += (2 > lightsUsed ? 0.0 : 1.0) * getPointLightLightingAtt(pointlightDir, att, TESR_LightColor[lightOffset + 1].rgb, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    
    #if MAX_LIGHTS > 3
        pointlightPosition = worldToCamera(TESR_ShadowLightPosition[lightOffset + 2]);
        pointlightDir = pointlightPosition - worldPos.xyz;
        att = vanillaAtt(pointlightDir, TESR_ShadowLightPosition[lightOffset + 2].w);
        lighting += (3 > lightsUsed ? 0.0 : 1.0) * getPointLightLightingAtt(pointlightDir, att, TESR_LightColor[lightOffset + 2].rgb, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    #endif
    
    #if MAX_LIGHTS > 4
        pointlightPosition = worldToCamera(TESR_ShadowLightPosition[lightOffset + 3]);
        pointlightDir = pointlightPosition - worldPos.xyz;
        att = vanillaAtt(pointlightDir, TESR_ShadowLightPosition[lightOffset + 3].w);
        lighting += (4 > lightsUsed ? 0.0 : 1.0) * getPointLightLightingAtt(pointlightDir, att, TESR_LightColor[lightOffset + 3].rgb, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    
        pointlightPosition = worldToCamera(TESR_ShadowLightPosition[lightOffset + 4]);
        pointlightDir = pointlightPosition - worldPos.xyz;
        att = vanillaAtt(pointlightDir, TESR_ShadowLightPosition[lightOffset + 4].w);
        lighting += (5 > lightsUsed ? 0.0 : 1.0) * getPointLightLightingAtt(pointlightDir, att, TESR_LightColor[lightOffset + 4].rgb, eyeDir.xyz, normal.xyz, baseColor.rgb, roughness);
    #endif
    
    lighting += getAmbientLighting(AmbientColor.rgb, baseColor.rgb);
    
    // TODO: Vanilla attenuates the full specular term by IN.lPosition.w for some reason. Is this a problem?
    float3 finalColor = lighting;
    
    #ifndef OPT
        finalColor.rgb = (useFog <= 0.0 ? finalColor.rgb : lerp(finalColor.rgb, IN.fogColor.rgb, IN.fogColor.a));
    #else
        finalColor.rgb = lerp(finalColor.rgb, IN.fogColor.rgb, IN.fogColor.a);
    #endif
    
    OUT.color.rgb = finalColor.rgb;
    OUT.color.a = baseColor.a * AmbientColor.a;

    return OUT;
}

#endif // Pixel shaders.
