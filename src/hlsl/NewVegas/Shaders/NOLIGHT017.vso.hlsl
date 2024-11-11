//
//
// Parameters:

float4 FogParam : register(c13);
row_major float4x4 ModelViewProj : register(c0);
float4 SubTexOffsets[16] : register(c15);


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//   FogParam      const_13      1
//   SubTexOffsets[0] const_15     16
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
    float4 texcoord_1 : TEXCOORD1;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 mdl3 = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);
    OUT.color_0.rgba = IN.color_0.rgba;
    OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    OUT.position.xyz = mdl3;
    float q0 = 1 - saturate((FogParam.x - length(mdl3)) / FogParam.y);
    OUT.texcoord_0.xy = (SubTexOffsets[IN.texcoord_1.x].ywzw * IN.texcoord_0.xy) + SubTexOffsets[IN.texcoord_1.x].xzzw;
    OUT.texcoord_0.z = 1;
    OUT.texcoord_0.w = exp2(log2(q0) * FogParam.z);

    return OUT;
};

// approximately 23 instruction slots used
