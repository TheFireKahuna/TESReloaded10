//
//
// Parameters:

row_major float2x2 RotationMatrix : register(c8);
float4 geometryOffset : register(c0);
float4 texOffset0 : register(c1);


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   geometryOffset const_0       1
//   texOffset0     const_1       1
//   RotationMatrix[0] const_8        1
//   RotationMatrix[1] const_9        1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const float4 const_2 = {2, -2, -0.5, 0.5};

    float2 r0;

    OUT.position.xy = IN.position.xy - (const_2.xy * geometryOffset.xy);
    OUT.position.zw = IN.position.zw;
    r0.xy = (IN.texcoord_0.xy + texOffset0.xy) - 0.5;
    OUT.texcoord_0.xy = ((r0.x * RotationMatrix[0].xy) + (r0.y * RotationMatrix[1].xy)) + 0.5;

    return OUT;
};

// approximately 8 instruction slots used
