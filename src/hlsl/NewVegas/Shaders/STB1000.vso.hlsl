//
//
// Parameters:

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
//   WindMatrices[0]  const_43     16
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
    float4 blendindices : BLENDINDICES;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 r0;

    r0.w = dot(WindMatrices[3 + IN.blendindices.y], IN.position);
    r0.z = dot(WindMatrices[2 + IN.blendindices.y], IN.position);
    r0.y = dot(WindMatrices[1 + IN.blendindices.y], IN.position);
    r0.x = dot(WindMatrices[0 + IN.blendindices.y], IN.position);
    OUT.position.xyzw = mul(ModelViewProj, (IN.blendindices.x * (r0.xyzw - IN.position.xyzw)) + IN.position.xyzw);
    OUT.texcoord_0.xy = IN.texcoord_0.xy;

    return OUT;
};

// approximately 13 instruction slots used
