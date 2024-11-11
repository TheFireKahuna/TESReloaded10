//
//
// Parameters:

float4 Falloff : register(c40);
float4 FogParam : register(c13);
row_major float4x4 ModelViewProj : register(c0);
row_major float3x3 TexCoordTranform : register(c4);
row_major float4x4 WorldView : register(c36);


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
//   WorldView        WorldView[0]      3
//   Falloff          const_40      1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
    float4 color_0 : COLOR0;
    float3 normal : NORMAL;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

#define	weight(v)		dot(v, 1)
#define	sqr(v)			((v) * (v))

    const int4 const_6 = {1, 0, -2, 3};

    float3 r0;
    float3 r1;

    float3 mdl16 = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);
    OUT.color_0 = IN.color_0;
    OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    OUT.position.xyz = mdl16;
    float q3 = 1 - saturate((FogParam.x - length(mdl16)) / FogParam.y);
    float3 q0 = (IN.texcoord_0.xyx * const_6.xxy) + const_6.yyx;
    float4 q1 = (IN.position.xyzx * const_6.xxxy) + const_6.yyyx;
    r1.z = dot(WorldView[2].xyzw, q1.xyzw);
    r1.y = dot(WorldView[1].xyzw, q1.xyzw);
    r1.x = dot(WorldView[0].xyzw, q1.xyzw);
    r0.xyz = normalize(r1.xyz);
    r1.z = dot(WorldView[2].xyz, IN.normal.xyz);
    r1.y = dot(WorldView[1].xyz, IN.normal.xyz);
    r1.x = dot(WorldView[0].xyz, IN.normal.xyz);
    r0.x = dot(normalize(r1.xyz), r0.xyz);
    r0.yz = Falloff.yw - Falloff.xz;
    float q15 = saturate((abs(r0.x) - Falloff.x) / r0.y);
    OUT.texcoord_0.x = dot(TexCoordTranform[0].xyz, q0.xyz);
    OUT.texcoord_0.y = dot(TexCoordTranform[1].xyz, q0.xyz);
    OUT.texcoord_0.z = (((3 - (q15 * 2)) * sqr(q15)) * r0.z) + Falloff.z;
    OUT.texcoord_0.w = exp2(log2(q3) * FogParam.z);

    return OUT;
};

// approximately 46 instruction slots used
