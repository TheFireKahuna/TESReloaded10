//

// Structures:

struct VS_INPUT {
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    OUT.color_0 = IN.color_0;

    return OUT;
};

// approximately 1 instruction slot used
