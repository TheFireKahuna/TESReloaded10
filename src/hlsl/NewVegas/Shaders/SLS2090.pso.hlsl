//
//
// Parameters:

float4 FogParams : register(c14);
float4 FogPlane : register(c16);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   FogParams    const_14      1
//   FogPlane     const_16      1
//


// Structures:

struct VS_INPUT {
    float3 texcoord_0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float fogPlane = 1 - (FogPlane.w / dot(-IN.texcoord_0, normalize(FogPlane.xyz)));
    float4 r1 = 0;
    r1.y = (abs(dot(-IN.texcoord_0, normalize(FogPlane.xyz))) * fogPlane) / FogParams.x;
    r1.x = (1.0 / FogParams.x) * (fogPlane * length(-IN.texcoord_0));
    OUT.color_0 = r1;

    return OUT;
};

// approximately 18 instruction slots used
