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
    float2 texcoord_1 : TEXCOORD1;
    float2 texcoord_2 : TEXCOORD2;
    float2 texcoord_3 : TEXCOORD3;
    float2 texcoord_4 : TEXCOORD4;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const float4 const_1 = {2.0, -2.0, -0.0009765625, 0.0009765625};

    OUT.position.xy = IN.position.xy - (const_1.xy * geometryOffset.xy);
    OUT.position.zw = IN.position.zw;
    OUT.texcoord_0.xy = IN.texcoord_0.xy;
    OUT.texcoord_1.xy = IN.texcoord_0.xy - 0.0009765625;
    OUT.texcoord_2.xy = IN.texcoord_0.xy + const_1.zw;
    OUT.texcoord_3.xy = IN.texcoord_0.xy + const_1.wz;
    OUT.texcoord_4.xy = IN.texcoord_0.xy + 0.0009765625;

    return OUT;
};

// approximately 8 instruction slots used
