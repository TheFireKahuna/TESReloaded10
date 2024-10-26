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
    float4 texcoord_0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const int4 const_1 = {2, -2, 0, 0};

    OUT.position.xy = IN.position.xy - (const_1.xy * geometryOffset.xy);
    OUT.position.zw = IN.position.zw;
    OUT.texcoord_0.xy = IN.texcoord_0.xy;

    return OUT;
};

// approximately 4 instruction slots used
