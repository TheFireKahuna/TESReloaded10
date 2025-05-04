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
    float4 tangent : TEXCOORD3;
    float4 viewDir: TEXCOORD4;
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
    float4x4 modelMatrix;
    if (TESR_DebugVar.z >= 1) {
        modelMatrix =  mul(ModelViewProj, TESR_InvViewProjectionTransform);
    }
    else {
        modelMatrix =  mul(TESR_InvViewProjectionTransform, ModelViewProj);
    }
    float3x3 worldMatrix = (float3x3)modelMatrix;
    float3x3 normalMatrix = transpose(inverse3x3(worldMatrix)); // correct normal transform as learned 
    // Calculate the normal vector against the world matrix only and then normalize the final value.
    float3 T = IN.tangent;
    float3 B = IN.binormal;
    float3 N = IN.normal;
    //if (TESR_DebugVar.w == 1) {
    //    if (dot(cross(N, T) * -1, B) < 0.0) {
    //        T *= -1.0;
    //    }
    //}
    if (TESR_DebugVar.z >= 2) {
        if (TESR_DebugVar.y >= 2) {
            T = normalize(mul(normalMatrix, T));   // world-space tangent
            B = normalize(mul(normalMatrix, B));   // world-space binormal
        }
        else {
            T = normalize(mul(worldMatrix, T));   // world-space tangent
            B = normalize(mul(worldMatrix, B));   // world-space binormal
        }
        if (TESR_DebugVar.y >= 1) {
            N = normalize(mul(normalMatrix, N));   // world-space normal
        }
        else {
            N = normalize(mul(worldMatrix, N));   // world-space normal
        }
    }
    else {
        if (TESR_DebugVar.y >= 2) {
            T = normalize(mul(T, normalMatrix));   // world-space tangent
            B = normalize(mul(B, normalMatrix));   // world-space binormal
        }
        else {
            T = normalize(mul(T, worldMatrix));   // world-space tangent
            B = normalize(mul(B, worldMatrix));   // world-space binormal
        }
        if (TESR_DebugVar.y == 1) {
            N = normalize(mul(N, normalMatrix));   // world-space normal
        }
        else {
            N = normalize(mul(N, worldMatrix));   // world-space normal
        }
    }
    float sign = step(0.0f, dot(cross(N, T), B));   // 0 if −1, 1 if +1
    OUT.tangent = float4(T, sign);          // keep sign
    OUT.normal = N;                 // binormal no longer needed
    OUT.worldPos = clipToWorld(OUT.sPosition);

    OUT.blend_0 = IN.blend_0;
    OUT.blend_1 = IN.blend_1;

    OUT.uv.xy = IN.uv.xy;
    OUT.vertex_color.xyz = clamp(IN.vertex_color.rgb, 0.0f, 1.0f);
    
    float3 eye = EyePosition.xyz - IN.position.xyz;
    OUT.viewDir.xyz = mul(float3x3(IN.tangent.xyz, IN.binormal.xyz, IN.normal.xyz), eye);
    OUT.viewDir.w = length(eye);
    
    // Fog.
    float3 fogPos = OUT.sPosition.xyz;
    #ifdef REVERSED_DEPTH
        fogPos.z = OUT.sPosition.w - fogPos.z;
    #endif
    float fogStrength = 1 - saturate((FogParam.x - length(fogPos)) / FogParam.y);
    fogStrength = log2(fogStrength);
    OUT.fog.a = exp2(fogStrength * FogParam.z);
    OUT.fog.rgb = FogColor.rgb;

    return OUT;
};

#endif  // Vertex shader.

struct PS_INPUT
{
    float4 sPosition : POSITION1;
    float2 uv : TEXCOORD0;
    float3 vertex_color : TEXCOORD1;
    float4 worldPos : TEXCOORD2;
    float4 tangent : TEXCOORD3;
    float4 viewDir: TEXCOORD4;
    float3 normal : TEXCOORD5;
    float4 blend_0 : COLOR0;
    float4 blend_1 : COLOR1;
    float4 fog : TEXCOORD6;
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

    float3 T = normalize(IN.tangent.xyz);
    float3 N = normalize(IN.normal.xyz);
    T = normalize(T - N * dot(T, N));

    float sign = IN.tangent.w * 2.0f - 1.0f;          // 0/1 → −1/+1
    float3 B = sign * cross(N, T);                    // correct orientation

    float3x3 tbn = float3x3(T, B, N);                 // columns

    float3 eyeDir = normalize(TESR_CameraPosition.xyz - worldPos.xyz);
    if (TESR_DebugVar.z >= 5) {
        eyeDir = normalize(mul(tbn, TESR_CameraPosition.xyz - worldPos.xyz));
    }
    float distance = length(TESR_CameraPosition.xyz);

    float2 dx = ddx(IN.uv);
    float2 dy = ddy(IN.uv);
    
    float weights[7] = { 0, 0, 0, 0, 0, 0, 0 };
    float blends[7] = { IN.blend_0.x, IN.blend_0.y, IN.blend_0.z, IN.blend_0.w, IN.blend_1.x, IN.blend_1.y, IN.blend_1.z };
    float2 offsetUV = getParallaxCoords(distance, IN.uv, dx, dy, eyeDir, texCount, BaseMap, blends, weights);

    float roughness = 1.f;
    float3 baseColor = blendDiffuseMaps(IN.vertex_color, offsetUV, texCount, BaseMap, weights);
    float3 combinedNormal = blendNormalMaps(offsetUV, texCount, NormalMap, weights, roughness);
    if (TESR_DebugVar.z >= 5) {
        combinedNormal.xyz = normalize(combinedNormal.xyz);
    }
    else if (TESR_DebugVar.z >= 3) {
        combinedNormal.xyz = normalize(mul(tbn, combinedNormal.xyz));
    }
    else {
        combinedNormal.xyz = normalize(mul(combinedNormal.xyz, tbn));
    }


    float3 sunDir = sunDirection();
    if (TESR_DebugVar.z >= 5) {
        eyeDir = normalize(mul(tbn, sunDir));
    }
    float parallaxShadowMultiplier = getParallaxShadowMultipler(distance, offsetUV, dx, dy, sunDir, texCount, blends, BaseMap);
    parallaxShadowMultiplier = normalize(parallaxShadowMultiplier);
    
    float3 shadowMultiplier = GetLightAmount(worldPos);
    float3 lighting = getSunLighting(sunDir, PSLightColor[0].xyz * shadowMultiplier, eyeDir, combinedNormal, AmbientColor.rgb, baseColor, roughness, 1.0, parallaxShadowMultiplier);

    #if defined(POINTLIGHT)
        float3 pointlightDir;
        float3 pointlightPosition;
        [unroll] for (int i = 0; i < NUM_PT_LIGHTS; i++) {
        pointlightPosition = worldVector(TESR_ShadowLightPosition[i], worldPos);
        pointlightDir = pointlightPosition - worldPos.xyz;
            lighting += getPointLightLighting(pointlightDir, TESR_ShadowLightPosition[i].w, TESR_LightColor[i].rgb, eyeDir, combinedNormal, baseColor, roughness, 1.0);
        }
    #endif
    
    float3 finalColor = lighting;
    finalColor = lerp(finalColor, IN.fog.rgb, IN.fog.a); // Apply fog.

    OUT.color_0.a = 1;
    OUT.color_0.rgb = finalColor;
    if (TESR_DebugVar.x == 1) {
        float3 col  = N * 0.5 + 0.5;             // base = normal

        // tint magenta when sign = –1 so mirrored parts pop
        col = lerp(col, float3(1,0,1), step(0.0, -sign));

        OUT.color_0.rgb = col;
    }
    else if (TESR_DebugVar.x == 2) {
        OUT.color_0.rgb = combinedNormal.xyz * 0.5 + 0.5;
    }
    else if (TESR_DebugVar.x == 3) {
        float3 outRGB =
            abs(T) * float3(1,0,0) +   // red shows |T|
            abs(B) * float3(0,1,0) +   // green shows |B|
            abs(N) * float3(0,0,1);    // blue shows |N|

        OUT.color_0.rgb = saturate(outRGB);
    }
    else if (TESR_DebugVar.x == 4) {
        OUT.color_0.rgb = T.xyz * 0.5 + 0.5;
    }
    else if (TESR_DebugVar.x == 5) {
        OUT.color_0.rgb = B.xyz * 0.5 + 0.5;
    }
    else if (TESR_DebugVar.x == 6) {
        OUT.color_0.rgb = N.xyz * 0.5 + 0.5;
    }
    else if (TESR_DebugVar.x == 7) {
        float3 pos = clipToWorld(IN.sPosition).xyz - IN.worldPos.xyz;
        OUT.color_0.rgb = saturate(abs(pos) * 0.1);   // magenta if mismatch
    }
    else if (TESR_DebugVar.x == 8) {
        float2 test = ddx(IN.uv) * sign;
        OUT.color_0.rgb = float3(test.x, test.y, 0) * 10 + 0.5;
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

#endif  // Pixel shader.
