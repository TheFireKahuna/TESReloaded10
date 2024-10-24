uniform float3         DiffuseDir       : register(c0);    // c0
uniform float3         DiffuseColor     : register(c1);    // c1
uniform float3         ScaleMask        : register(c2);    // c2
uniform float4         WindData         : register(c4);    // c4
uniform float4         AlphaParam       : register(c5);    // c5
uniform float4         AmbientColor     : register(c6);    // c6
uniform float4         AddlParams       : register(c7);    // c7
uniform row_major float4x4 ModelViewProj : register(c9);   // c9-c12
uniform float4         FogColor         : register(c14);   // c14
uniform float4         FogParam         : register(c15);   // c15
uniform float4         InstanceData[2]  : register(c20);   // c20-c21

float4 TESR_LinearObjectColor : register(c22);
float4 TESR_ShaderBaseColors : register(c23);

#include "includes/Helpers.hlsl"

struct VS_INPUT
{
    //dcl_position v0
    //dcl_color v1
    //dcl_texcoord v2
    //dcl_texcoord1 v3
    float4 position : POSITION;
    float4 color : COLOR;
    float2 texcoord0 : TEXCOORD0;
    float1 texcoord1 : TEXCOORD1;
};

struct VS_OUTPUT
{
    //dcl_position o0
    //dcl_texcoord o1.xy
    //dcl_texcoord4 o2
    //dcl_texcoord5 o3
    //dcl_color o4
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord0 : TEXCOORD0;
    float4 texcoord4 : TEXCOORD4;
    float4 texcoord5 : TEXCOORD5;
};

VS_OUTPUT main(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    // Temporary register for r0
    float4 r0 = 0.0; // initialised for HLSL

    // mova a0.x, r0.x
    int a0 = (int)(IN.texcoord1.x - frac(IN.texcoord1.x));

    // mov r0.xyw, c3
    r0.xyw = float3(1.0, 0.0, -0.00999999978);

    // mad r1, c20[a0.x].xyzx, r0.xxxy, r0.yyyx
    float4 r1 = (InstanceData[a0].xyzx * r0.xxxy) + r0.yyyx; 

    // Transform position using ModelViewProj matrix
    // dp4 r2.x, c9, r1
    // dp4 r2.y, c10, r1
    // dp4 r2.z, c11, r1
    // dp4 r2.w, c12, r1
    float4 r2 = mul(ModelViewProj, r1);

    // add o3.w, -r0.y, 1.0
    OUT.texcoord5.w = 1.0 - saturate(((1.0 / rsqrt(dot(r2, r2))) + (-AlphaParam.z)) * (1.0 / AlphaParam.w));

    // frc r1, c20[a0.x]
    r1 = frac(InstanceData[a0]);

    // add r1.xyz, r1, -0.5
    r1.xyz -= 0.5;

    // mad r0.y, r1.w, c8.x, c8.y
    r0.y = min(0.0,(r1.w * 0.75) + 0.25);

    // add r1.xyz, r1, r1
    r1.xyz *= 2;

    float3 r1_abs = abs(r1.xyz);               // Compute only needed absolute values
    float2 comparison = step(r1_abs.x, r1_abs.yz); // Step comparison for r1_abs.x against y and z
    r0.z = comparison.x * comparison.y;        // Multiply the results to get final value
      
    // mul r2.xyz, r1.zzxw, c16
    r2.xyz = r1.zzx * float3(-1.0, 0.0, 1.0);

    // nrm r3, r2
    float3 r3 = normalize(((r0.z * ((r1.zzy * float3(0.0, -1.0, 1.0)) + (-r2.xyz))) + r2.xyz));

    // mad r0.xzw, r0.z, c2.xyyz, r0.x
    r0.xzw = (r0.w * InstanceData[a0].w * ScaleMask.xyz) + r0.x;

    // mul r0.xzw, r0, v0.xyyz
    r0.xzw = r0.xzw * IN.position.xyz;

    // mad r2.xyz, r2, r0.x, r3
    r2.xyz = (((r3.yzx * r1.zxy) - (r1.yzx * r3.zxy)) * r0.x) + (r3 * r0.z);

    // mad r0.xzw, r1.xyyz, r0.w, r2.xyyz
    r0.xzw = (r1.xyz * r0.w) + r2.xyz;

    // dp3_sat r1.x, c0, r1
    float sunDot = saturate(dot(DiffuseDir, r1.xyz));

    r1.y = ((((InstanceData[a0].y + InstanceData[a0].x) * 0.0078125) + WindData.w) * 0.159154937) + 0.5;

    // mul r2.xy, r1.y, c4
    r2.xy = ((sin((frac(r1.y) * TWO_PI) + -PI) * WindData.z * pow(IN.color.w, 2.0)) * WindData).xy;

    // mov r2.z, 0.0
    r2.z = 0.0;

    // add r0.xzw, r0, r2.xyyz
    r0.xzw = r0.xzw + r2.xyz;

    // add r2.xyz, r0.xzww, c20[a0.x]
    r2.xyz = r0.xzw + InstanceData[a0].xyz;

    // mov r2.w, 1.0
    r2.w = 1.0; // 1.0
    
    // mul o2, r0.y, c6
    float4 ambientColor = linearCheck(AmbientColor, TESR_LinearObjectColor.x);
    ambientColor.rgb *= TESR_ShaderBaseColors.y;
    OUT.texcoord4 = linearCheck(r0.y, TESR_LinearObjectColor.x) * ambientColor;

    // mul o3.xyz, r0, c7.x
    float3 sunColor = linearCheck(DiffuseColor.rgb, TESR_LinearObjectColor.x) * TESR_ShaderBaseColors.x;
    //float3 sunColor = DiffuseColor.rgb;
    OUT.texcoord5.xyz = ((sunDot * (linearCheck(r0.y, TESR_LinearObjectColor.x) * linearCheck(IN.color.xyz, TESR_LinearObjectColor.x))) * sunColor.rgb) * AddlParams.x;
  
    // dp4 r0.x, c9, r2
    // dp4 r0.y, c10, r2
    // dp4 r0.z, c11, r2
    // dp3 r0.w, r0, r0
    r0 = mul(ModelViewProj, r2);

    OUT.position = r0;

    float fogStrength = 1 - saturate((FogParam.x - length(r0.xyz)) / FogParam.y);
    OUT.color.a = exp2(log2(fogStrength) * FogParam.z);
    OUT.color.rgb = FogColor.rgb;

    OUT.texcoord0 = IN.texcoord0;

    return OUT;
}