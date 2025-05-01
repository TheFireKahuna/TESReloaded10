//

// Structures:

struct VS_INPUT {
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    OUT.color_0 = 1.0;

    return OUT;
};

// approximately 2 instruction slots used
