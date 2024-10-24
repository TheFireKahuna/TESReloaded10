// SLS1000 - none, SLS1S000
// SLS1003 - TEX, SLS1005
// SLS1004 - TEX, VC, SLS1006
// SLS1005 - TEX, FACEGENBLEND, SLS1007
// SLS1006 - TEX, HAIR
// SLS1031 - FOG, TEX, SLS1018
// SLS1039 - none
// SLS1047 - CONSTALPHA
// SLS2150 - FOG, FOGANDAMBIENT

// STB1000 - none
// STB1006 - TEX, VC
// STB1009 - FOG

#include "includes/Helpers.hlsl"

#if !defined(TEX)
float4 AmbientColor     : register(c1);
#endif

sampler2D DiffuseMap    : register(s0);

#if defined(HAIR)
sampler2D LayerMap      : register(s1);
float4 EmittanceColor   : register(c2);
#elif defined(FACEGENBLEND)
sampler2D DecalMap      : register(s1);
sampler2D Decal2Map     : register(s2);
#endif

float4 TESR_LinearObject : register(c3);
float4 TESR_LinearObjectColor : register(c4);
float4 TESR_ShaderBaseColors : register(c5);
float4 TESR_LinearObjectExtra : register(c7);

struct PS_INPUT {
    float2 DiffuseUV    : TEXCOORD0;
#if   defined(FACEGENBLEND)
    float2 DecalUV_1    : TEXCOORD1;
    float2 Decal2UV_2   : TEXCOORD2;
#elif defined(VC)
    float3 VertexColor  : COLOR0;
#elif defined(HAIR)
    float2 Color        : COLOR0;
#elif defined(FOG)
    float4 FogColor     : COLOR0;
#endif
};

struct PS_OUTPUT {
    float4 Color : COLOR0;
};


PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;
#if !defined(TEX)
    float3 ambientColor = linearCheck(AmbientColor.rgb, TESR_LinearObjectColor.x)  * TESR_ShaderBaseColors.y;
#endif
#if defined(FOG)
    float4 fogColor = linearCheck(IN.FogColor, TESR_LinearObjectExtra.z);
#endif

    float3 finalColor;
    float4 baseColor = tex2D(DiffuseMap, IN.DiffuseUV);
    baseColor = linearCheck(baseColor, TESR_LinearObject.y);

#if defined(FOG) && defined(TEX)
    OUT.Color.rgb = fogColor.rgb;
    OUT.Color.a = fogColor.a * baseColor.a;
    return OUT;
#elif defined(FOG) && defined(FOGANDAMBIENT)
    OUT.Color.rgb = lerp(ambientColor, fogColor.rgb, fogColor.a);
    OUT.Color.a = 1.0;
    return OUT;
#elif defined(FOG)
    OUT.Color = fogColor;
    return OUT;
#endif

#if defined(HAIR)
    float4 layerMap = tex2D(LayerMap, IN.DiffuseUV);
    layerMap = linearCheck(layerMap, TESR_LinearObject.y);
    //float inputColor = linearCheck(IN.Color.g, TESR_LinearObject.x);
    
    float3 emittanceColor = linearCheck(EmittanceColor.rgb, TESR_LinearObjectColor.y) * TESR_ShaderBaseColors.w;
    baseColor.rgb = (2 * ((IN.Color.g * (emittanceColor - 0.5)) + 0.5)) * lerp(baseColor.rgb, layerMap.rgb, layerMap.w);

#elif defined(FACEGENBLEND)
    float3 decalMap0 = tex2D(DecalMap, IN.DecalUV_1).rgb;
    float3 decalMap1 = tex2D(Decal2Map, IN.Decal2UV_2).rgb;
    decalMap0 = linearCheck(decalMap0, TESR_LinearObject.y);
    decalMap1 = linearCheck(decalMap1, TESR_LinearObject.y);

    baseColor.rgb = 2 * ((2 * decalMap1) * (expand(decalMap0) + baseColor.rgb));
#endif

#if defined(TEX)
    finalColor.rgb = baseColor.rgb;
#elif defined(CONSTALPHA)
    finalColor.b = ambientColor.b;
    finalColor.g = AmbientColor.a;
    finalColor.r = 1;
#else
    finalColor.rgb = ambientColor;
#endif

#if defined(VC)
    float3 vertexColor = linearCheck(IN.VertexColor.rgb, TESR_LinearObject.z);
    finalColor.rgb = finalColor.rgb * vertexColor;
#endif
    OUT.Color.rgb = finalColor.rgb;

#if defined(CONSTALPHA)
    OUT.Color.a = (ambientColor.b == 0 ? (0.2 >= baseColor.w ? 0 : 1) : 1);
#else
    OUT.Color.a = baseColor.w;
#endif
    return OUT;
};