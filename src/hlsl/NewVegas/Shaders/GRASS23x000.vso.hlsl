// Parameters
float4 AdditionalParams : register(c7);
float4 AlphaParams : register(c5);
float4 AmbientColor : register(c6);
float3 DiffuseColor : register(c1);
float3 DiffuseDir : register(c0);
float4 FogColor : register(c14);
float4 FogParams : register(c15);
float4 InstanceData[2] : register(c20);
row_major float4x4 ModelViewProj : register(c9); // ModelViewProj stored in c9, c10, c11, c12
float3 ScaleMask : register(c2);
float4 WindData : register(c4);

float4 TESR_LinearObjectColor : register(c22);
float4 TESR_ShaderBaseColors : register(c23);

#include "includes/Helpers.hlsl"

// Constant definitions
struct VS_INPUT {
    float4 position : POSITION; // v0
    float4 color_0 : COLOR0;      // v1
    float4 texcoord_0 : TEXCOORD0; // v2
    float4 texcoord_1 : TEXCOORD1; // v3
};

struct VS_OUTPUT {
    float4 position : POSITION; // o0
    float2 texcoord0 : TEXCOORD0; // o1.xy
    float4 texcoord4 : TEXCOORD4_centroid; // o2
    float4 texcoord5 : TEXCOORD5_centroid; // o3
    float4 color : COLOR0;       // o4
};
VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const float4 const_3 = {1, 0, -0.5, 0.01};
    float4 finalColor = IN.color_0;

    float4 instanceData = float4(const_3.rgb, 1.0);

    float4 windCalc = (InstanceData[0 + IN.texcoord_1.x].xyzx * instanceData.xxxy) + instanceData.yyyx;
    OUT.texcoord5.w = 1 - saturate((length(mul(ModelViewProj, windCalc)) - AlphaParams.z) / AlphaParams.w);
    windCalc.z = 0;

    instanceData.y = fracr(((InstanceData[0 + IN.texcoord_1.x].y + InstanceData[0 + IN.texcoord_1.x].x) / 128) + WindData.w);	// [0,1] to [-PI,PI]
    windCalc.y = sin(windCalc.y);
    windCalc.xy = ((windCalc.y * WindData.z) * sqr(finalColor.a)) * WindData.xy;
    float3 maskedInstanceData = (IN.position.xyz * (((instanceData.w * InstanceData[0 + IN.texcoord_1.x].w) * ScaleMask.rgb) + instanceData.x)) + windCalc.xyz;
    instanceData.xyz = maskedInstanceData + InstanceData[0 + IN.texcoord_1.x].rgb;
    windCalc = frac(InstanceData[0 + IN.texcoord_1.x]);
    windCalc.xyz = expand(windCalc.xyz);	// [0,1] to [-1,+1]
    float3 mdl12 = mul(float3x4(ModelViewProj[0], ModelViewProj[1], ModelViewProj[2]), instanceData);
    float q2 = 1 - saturate((FogParams.x - length(mdl12)) / FogParams.y);
    float q1 = (windCalc.w * 0.75) + 0.25;

    // Lighting
    finalColor.rgb = (q1 * finalColor.rrgb).rgb;
    float3 sunColor = linearCheck(DiffuseColor.rgb, TESR_LinearObjectColor.x);
    OUT.texcoord5.rgb = ((shades(DiffuseDir.xyz, windCalc.xyz) * finalColor.rgb) * sunColor) * AdditionalParams.x;

    instanceData.x = log(instanceData.x);
    OUT.color.a = exp2(q2 * FogParams.z);
    OUT.color.rgb = FogColor.rgb;

    OUT.position.w = dot(ModelViewProj[3], instanceData);
    OUT.position.xyz = mdl12;

    OUT.texcoord0.xy = IN.texcoord_0.xy;

    float4 ambientColor = linearCheck(AmbientColor, TESR_LinearObjectColor.x) * TESR_ShaderBaseColors.y;
    OUT.texcoord4 = q1 * ambientColor;

    return OUT;
};
