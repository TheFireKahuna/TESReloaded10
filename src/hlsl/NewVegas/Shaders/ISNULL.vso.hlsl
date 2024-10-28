//
//
// Parameters:

float4 geometryOffset : register(c0);


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   geometryOffset const_0       1
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

    const int4 const_1 = {2, -2, 0, 0};

    OUT.position.xy = IN.position.xy - (const_1.xy * geometryOffset.xy);
    OUT.position.zw = IN.position.zw;

    return OUT;
};

// approximately 3 instruction slots used
