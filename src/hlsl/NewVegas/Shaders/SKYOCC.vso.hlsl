//
//
// Parameters:

row_major float4x4 ModelViewProj : register(c0);


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
};

struct VS_OUTPUT {
    float4 position : POSITION;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    OUT.position.xy = mul(float2x4(ModelViewProj[0], ModelViewProj[1]), IN.position);
    OUT.position.zw = dot(ModelViewProj[3], IN.position) * ModelViewProj[2].xy;

    return OUT;
};

// approximately 4 instruction slots used
