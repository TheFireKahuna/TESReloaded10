//
//
// Parameters:

float4 FogParam : register(c13);
row_major float4x4 ModelViewProj : register(c0);
row_major float3x3 TexCoordTranform : register(c4);


// Registers:
//
//   Name             Reg   Size
//   ---------------- ----- ----
//   ModelViewProj[0]    const_0        1
//   ModelViewProj[1]    const_1        1
//   ModelViewProj[2]    const_2        1
//   ModelViewProj[3]    const_3        1
//   TexCoordTranform const_4       2
//   FogParam         const_13      1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const int4 const_6 = {1, 0, 0, 0};

    float3 mdl6 = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);
    OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    OUT.position.xyz = mdl6;
    float q1 = 1 - saturate((FogParam.x - length(mdl6)) / FogParam.y);
    float3 q0 = (IN.texcoord_0.xyx * const_6.xxy) + const_6.yyx;
    OUT.texcoord_0.w = exp2(log2(q1) * FogParam.z);
    OUT.texcoord_0.z = 1;
    OUT.texcoord_0.x = dot(TexCoordTranform[0].xyz, q0.xyz);
    OUT.texcoord_0.y = dot(TexCoordTranform[1].xyz, q0.xyz);

    return OUT;
};

// approximately 21 instruction slots used
