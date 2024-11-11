// Template for Lighting30 shaders.
//
// VS (not touching these for now)
// SM3000 - Base
// SM3001 - Base
// SM3002 - SKIN
// SM3003 - VC
// SM3004 - VC
// SM3005 - VC, SKIN
// SM3006 - PROJSHADOW
// SM3007 - PROJSHADOW
// SM3008 - PROJSHADOW, SKIN
// SM3009 - PROJSHADOW, VC
// SM3010 - PROJSHADOW, VC
// SM3011 - PROJSHADOW, VC, SKIN
//
// PS (not touching any hair/facegen ones for now)
// SM3000   - MAXLIGHTS = 4
// SM3LL000 - MAXLIGHTS = 2
// SM3001   - MAXLIGHTS = 4, SPECULAR
// SM3LL001 - MAXLIGHTS = 2, SPECULAR
// SM3002   - MAXLIGHTS = 4, HAIR
// SM3LL002 - MAXLIGHTS = 2, HAIR
// SM3003   - MAXLIGHTS = 4, SPECULAR, HAIR
// SM3LL003 - MAXLIGHTS = 2, SPECULAR, HAIR
// SM3004   - MAXLIGHTS = 4, PARALLAX
// SM3LL004 - MAXLIGHTS = 2, PARALLAX
// SM3005   - MAXLIGHTS = 4, PARALLAXOCC = 30
// SM3LL005 - MAXLIGHTS = 2, PARALLAXOCC = 30
// SM3006   - MAXLIGHTS = 4, PARALLAX, SPECULAR
// SM3LL006 - MAXLIGHTS = 2, PARALLAX, SPECULAR
// SM3007   - MAXLIGHTS = 4, PARALLAXOCC = 30, SPECULAR
// SM3LL007 - MAXLIGHTS = 2, PARALLAXOCC = 30, SPECULAR
// SM3008   - MAXLIGHTS = 4, FACEGENBLEND
// SM3LL008 - MAXLIGHTS = 2, FACEGENBLEND
// SM3009   - MAXLIGHTS = 4, FACEGENBLEND, SPECULAR
// SM3LL009 - MAXLIGHTS = 2, FACEGENBLEND, SPECULAR
// SM3010   - MAXLIGHTS = 4, GLOW
// SM3LL010 - MAXLIGHTS = 2, GLOW
// SM3011   - MAXLIGHTS = 4, GLOW, SPECULAR
// SM3LL011 - MAXLIGHTS = 2, GLOW, SPECULAR
// SM3012   - MAXLIGHTS = 4, PROJSHADOW
// SM3LL012 - MAXLIGHTS = 2, PROJSHADOW
// SM3013   - MAXLIGHTS = 4, PROJSHADOW, SPECULAR
// SM3LL013 - MAXLIGHTS = 2, PROJSHADOW, SPECULAR
// SM3014   - MAXLIGHTS = 4, PROJSHADOW, HAIR
// SM3LL014 - MAXLIGHTS = 2, PROJSHADOW, HAIR
// SM3015   - MAXLIGHTS = 4, PROJSHADOW, HAIR, SPECULAR
// SM3LL015 - MAXLIGHTS = 2, PROJSHADOW, HAIR, SPECULAR
// SM3016   - MAXLIGHTS = 4, PROJSHADOW, PARALLAX
// SM3LL016 - MAXLIGHTS = 2, PROJSHADOW, PARALLAX
// SM3017   - MAXLIGHTS = 4, PROJSHADOW, PARALLAXOCC = 30
// SM3LL017 - MAXLIGHTS = 2, PROJSHADOW, PARALLAXOCC = 30
// SM3018   - MAXLIGHTS = 4, PROJSHADOW, PARALLAX, SPECULAR
// SM3LL018 - MAXLIGHTS = 2, PROJSHADOW, PARALLAX, SPECULAR
// SM3019   - MAXLIGHTS = 4, PROJSHADOW, PARALLAXOCC = 30, SPECULAR
// SM3LL019 - MAXLIGHTS = 2, PROJSHADOW, PARALLAXOCC = 30, SPECULAR
// SM3020   - MAXLIGHTS = 4, PROJSHADOW, FACEGENBLEND
// SM3LL020 - MAXLIGHTS = 2, PROJSHADOW, FACEGENBLEND
// SM3021   - MAXLIGHTS = 4, PROJSHADOW, FACEGENBLEND, SPECULAR
// SM3LL021 - MAXLIGHTS = 2, PROJSHADOW, FACEGENBLEND, SPECULAR
// SM3022   - MAXLIGHTS = 4, PROJSHADOW, GLOW
// SM3LL022 - MAXLIGHTS = 2, PROJSHADOW, GLOW
// SM3023   - MAXLIGHTS = 4, PROJSHADOW, GLOW, SPECULAR
// SM3LL023 - MAXLIGHTS = 2, PROJSHADOW, GLOW, SPECULAR
// SM3037   - MAXLIGHTS = 1, SILHOUETTE

#include "includes/Helpers.hlsl"

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
float4 MatAlpha : register(c3);
float4 AmbientColor : register(c0);
float4 ToggleNumLights : register(c20);
float4 LightData[8] : register(c21);

struct PS_INPUT {
    float2 uv : TEXCOORD0;
    float3 vertex_color : COLOR0;
    float3 tangent : TEXCOORD3_centroid;
    float3 binormal : TEXCOORD4_centroid;
    float3 normal : TEXCOORD5_centroid;
    float3 position : TEXCOORD6_centroid;
    float4 fogColor : TEXCOORD7_centroid;
};

struct PS_OUTPUT {
    float4 color : COLOR0;
};

#ifndef MAXLIGHTS
    #define MAXLIGHTS 4
#endif

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;
    
    const float4 const_1 = { -0.5, 0, -1, -2 };
    const int4 const_2 = { 4, -2, 2, 3 };
    const int4 const_4 = { 2, -4, -5, -6 };
    const int4 const_5 = { 2, -6, -7, 0 };
    const int4 const_6 = { 2, 1, 0, 3 };
    const int4 const_7 = { -2, -1, -0, 2 };
    
    float3 tangent = normalize(IN.tangent.xyz);
    float3 binormal = normalize(IN.binormal.xyz);
    float3 normal = normalize(IN.normal.xyz);
    float3x3 tbn = float3x3(tangent, binormal, normal);
    
    float4 normalSample = tex2D(NormalMap, IN.uv.xy);
    normalSample.xyz = normalize(expand(normalSample.xyz));
    
    float3 sunDir = mul(tbn, LightData[1].xyz);
    
    int numberOfLights = min(ToggleNumLights.x + ToggleNumLights.y, MAXLIGHTS);
    
    float3 lightDir = LightData[7].xyz - IN.position.xyz;
    float lightAtt = saturate(length(lightDir) / LightData[7].w);
    lightDir = mul(tbn, lightDir);
    
    float4 r2 = (ToggleNumLights.x <= 0.0 ? float4(-2, -1, 0, 2) : float4(2, 1, 0, 3));
    float3 r0 = float3(0, -1, -2);
    
    
    r2.xyzw = const_7.xyzw;
    r0.yzw = const_1.yzw;
    r2.xyzw = (ToggleNumLights.x <= 0.0 ? r2.zyxw : const_6.xyzw);
    r1.xyz = (r2.xyz >= 0.0 ? r2.xyz : (ToggleNumLights.x <= 0.0 ? abs(r0.yzww) : -abs(r0.wzyw)));
    r2.xyz = (r1.x <= 0.0 ? const_21.xyz : r0.y); // partial precision
    r1.xyw = (r1.y <= 0.0 ? const_22.xyzz : r2.xyzz); // partial precision
    r1.xyz = (r1.z <= 0.0 ? const_23.xyz : r1.xyww); // partial precision
    r3.xyzw = (ToggleNumLights.x <= 0.0 ? r0.yzwy : abs(r0.wzyz));
    r4.xyzw = (ToggleNumLights.x <= 0.0 ? -r0.yzwz : const_7.xyzw);
    r2.xyz = (r3.xyz >= 0.0 ? r3.xyz : r4.xyz);
    r5.xyzw = (r2.z <= 0.0 ? const_24.xyzw : (r2.y <= 0.0 ? const_23.xyzw : (r2.x <= 0.0 ? const_22.xyzw : r0.y)));
    q25.xyz = r5.xyz - IN.texcoord_6.xyz;
    q26.x = saturate(length(q25.xyz) / r5.w);
    m75.xyz = mul(float3x3(q2.xyz, q1.xyz, q0.xyz), q25.xyz);
    r3.xyz = (ToggleNumLights.x <= 0.0 ? r0.y : (shade(q4.xyz, m74.xyz) * const_21.xyz)); // partial precision
    r4.xyz = (max((1 - sqr(q26.x)) * shades(q4.xyz, normalize(m75.xyz)), 0) * r1.xyz) + r3.xyz; // partial precision
    q7.x = ((q5.x >= 0.0 ? abs(0) : abs(-1)) * (frac(q5.x) <= 0.0 ? abs(0) : abs(-1))) + (q5.x - frac(q5.x));
    q8.x = r4.w - q7.x;
    q27.x = r3.w - q7.x;
    q9.x = (q27.x >= 0.0 ? abs(0) : abs(-1)) * (q8.x >= 0.0 ? abs(0) : abs(-1));
    q10.x = (r2.w >= q7.x ? abs(0) : abs(-1)) * q9.x;
    r2.xyzw = (q27.x >= 0.0 ? r3.xyzw : r4.xyzw);
    r1.yzw = (r2.w * 2) - const_2.xzwx;
    r3.xyzw = (r1.w == 0.0 ? const_26.xyzw : (r1.z == 0.0 ? const_25.xyzw : (r1.y == 0.0 ? const_24.xyzw : r0.y)));
    q76.xyz = r3.xyz - IN.texcoord_6.xyz;
    m77.xyz = mul(float3x3(q2.xyz, q1.xyz, q0.xyz), q76.xyz);
    q227.x = max((1 - sqr(saturate(length(q76.xyz) / r3.w))) * shades(q4.xyz, normalize(m77.xyz)), 0); // partial precision
    r3.w = r2.w + 1;
    r3.xyz = (r1.z == 0.0 ? const_24.xyz : (r1.y == 0.0 ? const_23.xyz : r0.y)); // partial precision
    r3.xyz = ((r1.w == 0.0 ? const_25.xxyz : r3.xxyz) * q227.x) + r2.xyz; // partial precision
    r2.xyzw = (q9.x <= 0.0 ? r2.xyzw : r3.xyzw);
    r1.yzw = (r2.w * 2) + const_4.yzw;
    r3.xyzw = (r1.w == 0.0 ? const_28.xyzw : (r1.z == 0.0 ? const_27.xyzw : (r1.y == 0.0 ? const_26.xyzw : r0.y)));
    q32.xyz = r3.xyz - IN.texcoord_6.xyz;
    q55.x = saturate(length(q32.xyz) / r3.w);
    r3.w = r2.w + 1;
    m80.xyz = mul(float3x3(q2.xyz, q1.xyz, q0.xyz), q32.xyz);
    r1.yzw = (r1.w == 0.0 ? const_27.xxyz : (r1.z == 0.0 ? const_26.xxyz : (r1.y == 0.0 ? const_25.xxyz : r0.y))); // partial precision
    r3.xyz = (r1.yzw * max((1 - sqr(q55.x)) * shades(q4.xyz, normalize(m80.xyz)), 0)) + r2.xyz; // partial precision
    r2.xyzw = (q10.x <= 0.0 ? r2.xyzw : r3.xyzw);
    r1.xy = (r2.w * 2) + const_5.yz;
    r1.xzw = (r1.x == 0.0 ? const_27.xyyz : r0.y); // partial precision
    r1.xyz = (r1.y == 0.0 ? const_28.xyz : r1.xzww); // partial precision
    r1.xyz = (max(shades(q4.xyz, normalize(m82.xyz)) * (1 - sqr(q12.x)), 0) * r1.xyz) + r2.xyz; // partial precision
    r0.xyw = ((((q8.x + 2) >= 0.0 ? abs(0) : abs(-1)) * q10.x) <= 0.0 ? r2.xyzz : r1.xyzz); // partial precision
    r1.xyzw = tex2D(BaseMap, IN.BaseUV.xy); // partial precision
    r1.xyz = r1.xyz * IN.color_0.rgb; // partial precision
    q13.xyz = r0.xy + AmbientColor.rgb; // partial precision
    r3.xyz = q13.xyz * r1.xyz; // partial precision
    r2.xyz = r3.xyz - (IN.texcoord_7.w * r3.xyz);
    q15.xyz = lerp((IN.texcoord_7.w * (IN.texcoord_7.xyz - (q13.xyz * r1.xyz))) + r3.xyz, r2.xyz, MatAlpha.z); // partial precision
    r0.xyw = (IN.texcoord_7.w * (1 - (r1.xyzz * q13.xyz))) + r3.xyzz;
    OUT.color_0.a = r1.w * MatAlpha.x; // partial precision
    OUT.color_0.rgb = (MatAlpha.w * (r0.xy - q15.xyz)) + q15.xyz; // partial precision
    r0.xyzw = ((r0.z + MatAlpha.x) >= 0.0 ? abs(0) : abs(-1)) * (r1.w - MatAlpha.y);
    clip(r0.xyzw);

    return OUT;
}