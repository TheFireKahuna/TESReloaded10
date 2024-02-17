//
// Generated by Microsoft (R) D3DX9 Shader Compiler 9.08.299.0000
//
//   vsa shaderdump19/SLS2068.pso /Fcshaderdump19/SLS2068.pso.dis
//
//
// Parameters:
//
float4 AmbientColor : register(c1);
float4 PSLightColor[4] : register(c2);
float4 TESR_TerrainData : register(c8);

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
sampler2D NoiseMap : register(s2);
sampler2D TESR_Noise : register(s10) < string ResourceName = "Effects\TerrainNoise_n.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = LINEAR; MINFILTER = MINDEF; MIPFILTER = MIPDEF; };

//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   PSLightColor[0] const_2        1
//   BaseMap      texture_0       1
//   NormalMap    texture_1       1
//   NoiseMap    texture_2       1
//


// Structures:

struct VS_OUTPUT {
    float2 BaseUV : TEXCOORD0;
    float2 NormalUV : TEXCOORD1;
    float4 texcoord_2 : TEXCOORD2_centroid;
    float3 Light0Dir : TEXCOORD3_centroid;
};

struct PS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

#include "Includes/Color.hlsl"

PS_OUTPUT main(VS_OUTPUT IN) {
    PS_OUTPUT OUT;

#define	expand(v)		(((v) - 0.5) / 0.5)
#define	compress(v)		(((v) * 0.5) + 0.5)
#define	shades(n, l)	saturate(dot(n, l))

    float3 q0;
    float3 q1;
    float q2;
    float4 r0;
	float3 r1;
    float4 r2;
	float spclr;
	
// SMOOTH NORMALS

    r2.xyzw  = tex2D(NormalMap, IN.NormalUV.xy);
    r2.xyz   = expand(r2.xyz);

    r2.xyz  += expand(tex2D(NormalMap, IN.NormalUV.xy + float2(0,  1.0 / 1024.0)).xyz) * 0.666;
    r2.xyz  += expand(tex2D(NormalMap, IN.NormalUV.xy + float2( 1.0 / 1024.0, 0)).xyz) * 0.666;
    r2.xyz  += expand(tex2D(NormalMap, IN.NormalUV.xy + float2(0, -1.0 / 1024.0)).xyz) * 0.666;
    r2.xyz  += expand(tex2D(NormalMap, IN.NormalUV.xy + float2(-1.0 / 1024.0, 0)).xyz) * 0.666;

    r2.xyz  += expand(tex2D(NormalMap, IN.NormalUV.xy + float2( 1.0 / 1536.0,  1.0 / 1536.0)).xyz) * 0.333;
    r2.xyz  += expand(tex2D(NormalMap, IN.NormalUV.xy + float2(-1.0 / 1536.0,  1.0 / 1536.0)).xyz) * 0.333;
    r2.xyz  += expand(tex2D(NormalMap, IN.NormalUV.xy + float2( 1.0 / 1536.0, -1.0 / 1536.0)).xyz) * 0.333;
    r2.xyz  += expand(tex2D(NormalMap, IN.NormalUV.xy + float2(-1.0 / 1536.0, -1.0 / 1536.0)).xyz) * 0.333;

    r2.xyz   = normalize(r2.xyz);

// SMOOTH COLORS

    r0.xyz  = GetYUV(tex2D(BaseMap, IN.BaseUV.xy).rgb);

    r0.xyz += GetYUV(tex2D(BaseMap, IN.BaseUV.xy + float2(0,  1.0 / 2048.0)).rgb).rgb * float3(0.0, 0.666, 0.666);
    r0.xyz += GetYUV(tex2D(BaseMap, IN.BaseUV.xy + float2( 1.0 / 2048.0, 0)).rgb).rgb * float3(0.0, 0.666, 0.666);
    r0.xyz += GetYUV(tex2D(BaseMap, IN.BaseUV.xy + float2(0, -1.0 / 2048.0)).rgb).rgb * float3(0.0, 0.666, 0.666);
    r0.xyz += GetYUV(tex2D(BaseMap, IN.BaseUV.xy + float2(-1.0 / 2048.0, 0)).rgb).rgb * float3(0.0, 0.666, 0.666);

    r0.yz += GetYUV(tex2D(BaseMap, IN.BaseUV.xy + float2( 1.0 / 3072.0,  1.0 / 3072.0)).rgb).gb * 0.333;
    r0.yz += GetYUV(tex2D(BaseMap, IN.BaseUV.xy + float2(-1.0 / 3072.0,  1.0 / 3072.0)).rgb).gb * 0.333;
    r0.yz += GetYUV(tex2D(BaseMap, IN.BaseUV.xy + float2( 1.0 / 3072.0, -1.0 / 3072.0)).rgb).gb * 0.333;
    r0.yz += GetYUV(tex2D(BaseMap, IN.BaseUV.xy + float2(-1.0 / 3072.0, -1.0 / 3072.0)).rgb).gb * 0.333;

    r0.xyz  = GetRGB(r0.xyz / float3(1.0, 5.0, 5.0)) * float3(1.1,1.2,1.09);

// BETTER NOISE

	float3 noisec = (tex2D(NoiseMap, IN.BaseUV.xy * 20).xyz * 2 + tex2D(NoiseMap, IN.BaseUV.xy * 2).xyz) * 0.3333;
    r1.x   = saturate(0.50 + 1.25 * (smoothstep(1.0, 0.0, pow(noisec.x, 0.5))));
    r1.xyz = lerp(r1.x, 1, pow(length(r0.rgb) / length(1), 2));
	
    float3 noisen = expand(tex2D(TESR_Noise, IN.BaseUV.xy * 40).xyz);
    r2.xyz = normalize(r2.xyz + float3(noisen.xy * TESR_TerrainData.y, 0));
	
	float3 LightDir = IN.Light0Dir.xyz;
	LightDir.x = LightDir.x < 0.4 ? max(LightDir.y, 0.8) : LightDir.x; // Trick to avoid to flat the bumpmap when midday
	
    q0.xyz = (shades(r2.xyz, LightDir) * PSLightColor[0].rgb) + AmbientColor.rgb;
    q1.xyz = r0.xyz * max(q0.xyz, 0);
	q1.xyz = q1.xyz * r1.x;
	
	spclr = smoothstep(0.0, 0.25, length(r0.rgb)) * (r0.b * 2.0 * TESR_TerrainData.w) + 1.0;
	
    OUT.color_0.a = IN.texcoord_2.w;
    OUT.color_0.rgb = q1.xyz * spclr;

    return OUT;
};

// approximately 18 instruction slots used (3 texture, 15 arithmetic)