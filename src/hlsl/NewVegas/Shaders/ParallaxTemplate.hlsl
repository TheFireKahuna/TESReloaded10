// Template for PAR parallax shaders.
//
// VS
//
// AD
// PAR2008 - LIGHTS = 2, PARALLAX (AD)
// PAR2009 - LIGHTS = 2, PARALLAX, PROJ_SHADOW (AD)
// PAR2010 - LIGHTS = 3, PARALLAX (AD)
// PAR2011 - LIGHTS = 3, PARALLAX, PROJ_SHADOW (AD)
//
// ADTS
// PAR2000 - PARALLAX
// PAR2001 - PARALLAX, PROJ_SHADOW
// PAR2002 - PARALLAX, LIGHTS = 2
// PAR2003 - PARALLAX, LIGHTS = 2, PROJ_SHADOW
// PAR2004 - PARALLAX, SPECULAR
// PAR2005 - PARALLAX, SPECULAR, PROJ_SHADOW
// PAR2006 - PARALLAX, SPECULAR, LIGHTS = 2
// PAR2007 - PARALLAX, SPECULAR, LIGHTS = 2, PROJ_SHADOW
//
// Diffuse
// PAR2012 - LIGHTS = 2, PARALLAX (NO_FOG, DIFFUSE)
// PAR2013 - LIGHTS = 3, PARALLAX (NO_FOG, DIFFUSE)
//
// Specular
// PAR2015 - PARALLAX (NO_FOG, NO_VERTEX_COLOR, SPECULAR)
// PAR2016 - PROJ_SHADOW, PARALLAX (NO_FOG, NO_VERTEX_COLOR, SPECULAR)
// PAR2017 - POINT, PARALLAX (NO_FOG. NO_VERTEX_COLOR, SPECULAR)
// PAR2018 - POINT, NUM_PT_LIGHTS = 2, PARALLAX (NO_FOG, NO_VERTEX_COLOR, SPECULAR)
// PAR2019 - POINT, NUM_PT_LIGHTS = 3, PARALLAX (NO_FOG, NO_VERTEX_COLOR, SPECULAR)
//
// Texture
// PAR2014 - PARALLAX (NO_LIGHT, NO_FOG)
//
// PS
//
// AD
// PAR2013 - LIGHTS = 2, PARALLAX (AD)
// PAR2014 - LIGHTS = 2, PARALLAX, SI (AD)
// PAR2015 - LIGHTS = 2, PARALLAX, PROJ_SHADOW (AD)
// PAR2016 - LIGHTS = 2, PARALLAX, SI, PROJ_SHADOW (AD)
// PAR2017 - LIGHTS = 3, PARALLAX (AD)
// PAR2018 - LIGHTS = 3, PARALLAX, SI (AD)
// PAR2019 - LIGHTS = 3, PARALLAX, PROJ_SHADOW (AD)
// PAR2020 - LIGHTS = 3, PARALLAX, SI, PROJ_SHADOW (AD)
//
// ADTS
// PAR2000 - PARALLAX
// PAR2001 - PARALLAX, OPT
// PAR2002 - PARALLAX, SI
// PAR2003 - PARALLAX, PROJ_SHADOW
// PAR2004 - PARALLAX, SI, PROJ_SHADOW
// PAR2005 - PARALLAX, LIGHTS = 2
// PAR2006 - PARALLAX, LIGHTS = 2, SI
// PAR2007 - PARALLAX, LIGHTS = 2, PROJ_SHADOW
// PAR2008 - PARALLAX, LIGHTS = 2, SI, PROJ_SHADOW
// PAR2009 - PARALLAX, SPECULAR
// PAR2010 - PARALLAX, SPECULAR, SI
// PAR2011 - PARALLAX, SPECULAR, PROJ_SHADOW
// PAR2012 - PARALLAX, SPECULAR, SI, PROJ_SHADOW
// PAR2029 - PARALLAX, LIGHTS = 2, SPECULAR
// PAR2030 - PARALLAX, LIGHTS = 2, SPECULAR, SI
// PAR2031 - PARALLAX, LIGHTS = 2, SPECULAR, PROJ_SHADOW
// PAR2032 - PARALLAX, LIGHTS = 2, SPECULAR, SI, PROJ_SHADOW
//
// Diffuse
// PAR2021 - LIGHTS = 2, PARALLAX (NO_FOG, NO_VERTEX_COLOR, DIFFUSE, ONLY_LIGHT, OPT)
// PAR2022 - LIGHTS = 3, PARALLAX (NO_FOG, NO_VERTEX_COLOR, DIFFUSE, ONLY_LIGHT, OPT)
//
// Specular
// PAR2024 - PARALLAX (ONLY_SPECULAR)
// PAR2025 - PARALLAX, PROJ_SHADOW (ONLY_SPECULAR)
// PAR2026 - PARALLAX, POINT (ONLY_SPECULAR)
// PAR2027 - PARALLAX, NUM_PT_LIGHTS = 2, POINT (ONLY_SPECULAR)
// PAR2028 - PARALLAX, NUM_PT_LIGHTS = 3, POINT (ONLY_SPECULAR)
//
// Texture
// PAR2023 - PARALLAX (NO_LIGHT)

#if defined(__INTELLISENSE__)
    #define VS
    #define REVERSED_DEPTH
#endif

#if defined(AD)
    #define ONLY_LIGHT
    #define OPT
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
#include "includes/Parallax.hlsl"
#include "includes/Object.hlsl"
#include "includes/DirectShadow.hlsl"
#include "includes/Position.hlsl"

struct VS_INPUT
{
    float4 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float4 uv : TEXCOORD0;
#ifndef NO_VERTEX_COLOR
    float4 vertex_color : COLOR0;
#endif
};

struct VS_OUTPUT
{
#ifndef NO_VERTEX_COLOR
    float4 vertexColor : COLOR0;
#endif
#ifndef NO_FOG
    float4 fogColor : COLOR1;
#endif
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 tangent : TEXCOORD2;
    float3 binormal : TEXCOORD3;
    float4 worldPos: TEXCOORD6;
};

#ifdef VS

float4 EyePosition : register(c16);
row_major float4x4 ModelViewProj : register(c0);

#ifndef NO_LIGHT
    float4 LightData[10] : register(c25);
#endif

#ifndef NO_FOG
    float3 FogColor : register(c15);
    float4 FogParam : register(c14);
#endif

VS_OUTPUT main(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    float4 position = IN.position;
    
    OUT.uv = IN.uv.xy;
    OUT.sPosition = mul(ModelViewProj, position);
    float4x4 modelMatrix = mul(TESR_ViewProjectionTransform, ModelViewProj);
    float3x3 worldMatrix = (float3x3)modelMatrix;
    float4 worldPos = clipToWorld(OUT.sPosition);
    float3x3 normalMatrix = transpose(inverse3x3(worldMatrix)); // correct normal transform as learned 
    // Calculate the normal vector against the world matrix only and then normalize the final value.
    OUT.normal = mul(normalMatrix, IN.normal);
    OUT.tangent = mul(worldMatrix, IN.tangent);
    OUT.binormal = mul(worldMatrix, IN.binormal);
    OUT.worldPos = worldPos;
    
    #ifndef NO_VERTEX_COLOR
        OUT.vertexColor = clamp(IN.vertex_color, 0.0f, 1.0f);
    #endif
    
    #ifndef NO_FOG
        float3 fogPos = OUT.sPosition.xyz;
        #ifdef REVERSED_DEPTH
            fogPos.z = OUT.sPosition.w - fogPos.z;
        #endif
        float fogStrength = 1 - saturate((FogParam.x - length(fogPos)) / FogParam.y);
        fogStrength = log2(fogStrength);  // Unclear.
        OUT.fogColor.a = exp2(fogStrength * FogParam.z);
        OUT.fogColor.rgb = FogColor.rgb;
    #endif

    return OUT;
};

#endif // Vertex shader.

struct PS_INPUT
{
#ifndef NO_VERTEX_COLOR
    float3 vertexColor : COLOR0;
#endif
#ifndef NO_FOG
    float4 fogColor : COLOR1;
#endif
    float2 uv : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 tangent : TEXCOORD2;
    float3 binormal : TEXCOORD3;
    float4 worldPos: TEXCOORD6;
};

struct PS_OUTPUT {
    float4 color : COLOR0;
};

#ifdef PS

#if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
    #if !defined(NO_LIGHT)
        float4 AmbientColor : register(c1);
    #endif
    sampler2D BaseMap : register(s0);
#endif
#if !defined(NO_LIGHT)
    #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
        sampler2D NormalMap : register(s1);
    #else
        sampler2D NormalMap : register(s0);
    #endif
#endif
#if !defined(ONLY_LIGHT) && !defined(ONLY_SPECULAR) && !defined(NO_LIGHT)
    sampler2D HeightMap : register(s3);
#else
    sampler2D HeightMap : register(s2);
#endif
#ifdef SI
float4 EmittanceColor : register(c2);
    #ifndef ONLY_LIGHT
        sampler2D GlowMap : register(s4);
    #else
        sampler2D GlowMap : register(s3);
    #endif
#endif
#if LIGHTS > 1
    #ifdef DIFFUSE
        sampler2D AttenuationMap : register(s3);
    #elif defined(ONLY_LIGHT)
        sampler2D AttenuationMap : register(s4);
    #else
        sampler2D AttenuationMap : register(s5);
    #endif
#endif
#if !defined(NO_LIGHT)
    float4 PSLightColor[10] : register(c3);
#endif
#ifndef OPT
    float4 Toggles : register(c27);

    #define useVertexColor Toggles.x
    #define useFog Toggles.y
    #define glossPower Toggles.z
    #define alphaTestRef Toggles.w
#else
    #define glossPower 1  // OPT is never used in combination with specular in PAR.
#endif
float4 TESR_SunAmount : register(c147);
float4 TESR_SunColor : register(c149);
float4 TESR_ShadowLightPosition[12] : register(c150);
float4 TESR_LightColor[24] : register(c162);

#define	uvtile(w)		(((w) * 0.04) - 0.02)

PS_OUTPUT main(PS_INPUT IN)
{
    PS_OUTPUT OUT;
    
    #if !defined(ONLY_LIGHT) && !defined(ONLY_SPECULAR) && !defined(NO_LIGHT)
        float alpha = tex2D(BaseMap, IN.uv.xy).a;
    
        #ifndef OPT
            clip(AmbientColor.a >= 1 ? 0 : (alpha - alphaTestRef));
        #endif
    #endif
    
    // Parallax.
    float4 worldPos = IN.worldPos;
    float3 eyeDir = normalize(TESR_CameraPosition.xyz - worldPos.xyz);
    float3x3 tbn = float3x3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

    float2 dx, dy;
    dx = ddx(IN.uv.xy);
    dy = ddy(IN.uv.xy);
    
    float distance = length(TESR_CameraPosition.xyz - worldPos.xyz);
    float2 offsetUV = getParallaxCoords(distance, IN.uv.xy, dx, dy, eyeDir.xyz, HeightMap);

    #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
        float4 baseColor = tex2D(BaseMap, offsetUV.xy);
    
        #if defined(ONLY_LIGHT)
            baseColor.rgb = 1.f;
        #endif
    #else
        float4 baseColor = 1.f;
    #endif
    
    // Vertex color.
    #ifndef NO_VERTEX_COLOR
        #ifndef OPT
            // Apply vertex color if toggled.
            baseColor.xyz = (useVertexColor <= 0.0 ? baseColor.xyz : (baseColor.xyz * IN.vertexColor.rgb));
        #else
            baseColor.xyz = baseColor.xyz * IN.vertexColor.rgb;
        #endif
    #endif
    
    // Shadows.
    float3 shadowMultiplier = GetLightAmount(worldPos);
    float3 sunDir = sunDirection();
    
    #ifndef NO_LIGHT
        shadowMultiplier *= getParallaxShadowMultipler(distance, offsetUV, dx, dy, normalize(sunDir), HeightMap);
    #endif
    
    // Lighting.
    float3 lighting;
    float finalAtt;
    float3 pointlightDir;
    float3 pointlightPosition;
    
    #ifdef NO_LIGHT
        lighting = baseColor.rgb;
    #else
        float4 normal = tex2D(NormalMap, offsetUV.xy);
        normal.xyz = normalize(normal.xyz * 2.0 - 1.0);
        normal.xyz = normalize(mul(normal.xyz,tbn));

        float roughness = getRoughness(normal.a);
    
        #if !defined(DIFFUSE) && !defined(POINT)
            lighting = getSunLighting(sunDir, PSLightColor[0].rgb * shadowMultiplier, eyeDir, normal.xyz, baseColor.rgb, roughness);
        #elif defined(DIFFUSE)
            pointlightPosition = worldToCamera(TESR_ShadowLightPosition[0]);
            pointlightDir = pointlightPosition - worldPos.xyz;
            // Pointlight vanilla att.
            if (TESR_ParallaxData.y)
                lighting = getPointLightLighting(pointlightDir, TESR_ShadowLightPosition[0].w, TESR_LightColor[0].rgb, eyeDir, normal.xyz, baseColor.rgb, roughness);
            else {
                lighting = getVanillaLightingAtt(pointlightDir, TESR_ShadowLightPosition[0].w, TESR_LightColor[0].rgb, eyeDir, normal.xyz, baseColor.rgb, normal.a, glossPower);
            }
        #else
            pointlightPosition = worldToCamera(TESR_ShadowLightPosition[0]);
            pointlightDir = pointlightPosition - worldPos.xyz;
            if (TESR_ParallaxData.y)
                lighting = getPointLightLighting(pointlightDir, TESR_ShadowLightPosition[0].w, TESR_LightColor[0].rgb, eyeDir, normal.xyz, baseColor.rgb, roughness);
            else
                lighting = getVanillaLighting(pointlightDir, TESR_ShadowLightPosition[0].w, TESR_LightColor[0].rgb, eyeDir, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #endif
    
        // Self emmitance.
        #ifdef SI
            float3 glow = tex2D(GlowMap, IN.uv.xy).rgb;
            lighting += baseColor.rgb * glow.rgb * EmittanceColor.rgb;
        #endif
    
        #if !defined(DIFFUSE) && !defined(ONLY_SPECULAR)
            if (TESR_ParallaxData.y)
                lighting += getAmbientLighting(AmbientColor.rgb, baseColor.rgb);
            else
                lighting += baseColor.rgb * AmbientColor.rgb;
        #endif
    
        // Other light sources.
        #if LIGHTS > 1
            pointlightPosition = worldToCamera(TESR_ShadowLightPosition[1]);
            pointlightDir = pointlightPosition - worldPos.xyz;
        
            if (TESR_ParallaxData.y)
                lighting += getPointLightLightingAtt(pointlightDir, TESR_ShadowLightPosition[1].w, TESR_LightColor[1].rgb, eyeDir, normal.xyz, baseColor.rgb, roughness);
            else
                lighting += getVanillaLightingAtt(pointlightDir, TESR_ShadowLightPosition[1].w, TESR_LightColor[1].rgb, eyeDir, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #endif
    
        #if LIGHTS > 2
            pointlightPosition = worldToCamera(TESR_ShadowLightPosition[2]);
            pointlightDir = pointlightPosition - worldPos.xyz;
        
            if (TESR_ParallaxData.y)
                lighting += getPointLightLightingAtt(pointlightDir, TESR_ShadowLightPosition[2].w, TESR_LightColor[2].rgb, eyeDir, normal.xyz, baseColor.rgb, roughness);
            else
                lighting += getVanillaLightingAtt(pointlightDir, TESR_ShadowLightPosition[2].w, TESR_LightColor[2].rgb, eyeDir, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #endif
    
        #if NUM_PT_LIGHTS > 1
            pointlightPosition = worldToCamera(TESR_ShadowLightPosition[1]);
            pointlightDir = pointlightPosition - worldPos.xyz;
            if (TESR_ParallaxData.y)
                lighting += getPointLightLighting(pointlightDir, TESR_ShadowLightPosition[1].w, TESR_LightColor[1].rgb, eyeDir, normal.xyz, baseColor.rgb, roughness);
            else
                lighting += getVanillaLighting(pointlightDir, TESR_ShadowLightPosition[1].w, TESR_LightColor[1].rgb, eyeDir, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #endif
    
        #if NUM_PT_LIGHTS > 2
            pointlightPosition = worldToCamera(TESR_ShadowLightPosition[2]);
            pointlightDir = pointlightPosition - worldPos.xyz;
            if (TESR_ParallaxData.y)
                lighting += getPointLightLighting(pointlightDir, TESR_ShadowLightPosition[2].w, TESR_LightColor[2].rgb, eyeDir, normal.xyz, baseColor.rgb, roughness);
            else
                lighting += getVanillaLighting(pointlightDir, TESR_ShadowLightPosition[2].w, TESR_LightColor[2].rgb, eyeDir, normal.xyz, baseColor.rgb, normal.a, glossPower);
        #endif
    #endif
    
    // Fog.
    #ifndef NO_FOG
        #ifndef OPT
            lighting.rgb = (useFog <= 0.0 ? lighting.rgb : lerp(lighting.rgb, IN.fogColor.rgb, IN.fogColor.a));
        #else
            lighting.rgb = lerp(lighting.rgb, IN.fogColor.rgb, IN.fogColor.a);
        #endif
    #endif
    
    OUT.color.rgb = lighting.rgb;
    
    #if defined(DIFFUSE) || defined(NO_LIGHT)
        OUT.color.a = 1;
    #elif defined(ONLY_SPECULAR)
        if (!TESR_ParallaxData.y)
            OUT.color.rgb = saturate(OUT.color.rgb);
        OUT.color.a = weight(lighting.rgb);
    #elif defined(ONLY_LIGHT)
        OUT.color.a = baseColor.a;
    #else
        OUT.color.a = alpha * AmbientColor.a;
    #endif

    return OUT;
};

#endif // Pixel shader.