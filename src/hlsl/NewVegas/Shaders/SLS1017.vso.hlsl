//
//
// Parameters:

float3 FogColor : register(c15);
float4 FogParam : register(c14);
row_major float4x4 ModelViewProj : register(c0);


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
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
    float4 position : POSITION;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 mdl3 = mul(float3x4(ModelViewProj[0], ModelViewProj[1], ModelViewProj[2]), IN.position);
    float q0 = 1 - saturate((FogParam.x - length(mdl3)) / FogParam.y);
    q0 = log2(q0);
    OUT.color_0.rgb = FogColor.rgb;
    OUT.position.xyz = mdl3;
    OUT.color_0.a = exp2(q0 * FogParam.z);
    OUT.position.w = dot(ModelViewProj[3], IN.position);

    return OUT;
};

// approximately 18 instruction slots used
