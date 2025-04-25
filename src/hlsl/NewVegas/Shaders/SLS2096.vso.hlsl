//
//
// Parameters:

row_major float4x4 ModelViewProj : register(c0);
row_major float4x4 World : register(c8);


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//   World         const_8       3
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float3 texcoord_0 : TEXCOORD0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    OUT.position.xyzw = mul(ModelViewProj, IN.position.xyzw);
    OUT.texcoord_0.x = dot(World[0].xyzw, IN.position.xyzw);
    OUT.texcoord_0.y = dot(World[1].xyzw, IN.position.xyzw);
    OUT.texcoord_0.z = dot(World[2].xyzw, IN.position.xyzw);

    return OUT;
};

// approximately 7 instruction slots used
