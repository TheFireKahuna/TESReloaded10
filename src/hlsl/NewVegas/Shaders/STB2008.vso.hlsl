//
//
// Parameters:

float4 EyePosition : register(c16);
float3 FogColor : register(c15);
float4 FogParam : register(c14);
float4 LightData[10] : register(c25);
row_major float4x4 ModelViewProj : register(c0);
float4 WindMatrices[16] : register(c43);


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//   FogParam      const_14      1
//   FogColor      const_15      1
//   EyePosition   const_16      1
//   LightData[0]     const_25      1
//   WindMatrices[0]  const_43     16
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float4 texcoord_0 : TEXCOORD0;
    float4 blendindices : BLENDINDICES;

#define	TanSpaceProj	float3x3(IN.tangent.xyz, IN.binormal.xyz, IN.normal.xyz)
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float4 texcoord_1 : TEXCOORD1;
    float3 texcoord_3 : TEXCOORD3;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const int4 const_4 = {0, 1, 0, 0};

    float3 l11;
    float3 mdl12;
    float1 q3;
    float4 r0;

    r0.w = dot(WindMatrices[3 + IN.blendindices.y], IN.position);
    r0.z = dot(WindMatrices[2 + IN.blendindices.y], IN.position);
    r0.y = dot(WindMatrices[1 + IN.blendindices.y], IN.position);
    r0.x = dot(WindMatrices[0 + IN.blendindices.y], IN.position);
    OUT.color_0.rgba = (IN.blendindices.z * const_4.yyyx) + const_4.xxxy;
    r0.xyzw = (IN.blendindices.x * (r0.xyzw - IN.position.xyzw)) + IN.position.xyzw;
    mdl12.xyz = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), r0.xyzw);
    q3.x = 1 - saturate((FogParam.x - length(mdl12.xyz)) / FogParam.y);
    q3.x = log2(q3.x);
    l11.xyz = mul(TanSpaceProj, normalize(normalize(EyePosition.xyz - r0.xyz) + LightData[0].xyz));
    OUT.color_1.rgb = FogColor.rgb;
    OUT.texcoord_1.xyz = normalize(mul(TanSpaceProj, LightData[0].xyz));
    OUT.texcoord_1.w = LightData[0].w;
    OUT.color_1.a = exp2(q3.x * FogParam.z);
    OUT.position.xyz = mdl12.xyz;
    OUT.position.w = dot(ModelViewProj[3].xyzw, r0.xyzw);
    OUT.texcoord_0.xy = IN.texcoord_0.xy;
    OUT.texcoord_3.xyz = normalize(l11.xyz);

    return OUT;
};

// approximately 50 instruction slots used
