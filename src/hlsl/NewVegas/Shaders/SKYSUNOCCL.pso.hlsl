//

// Structures:

struct VS_INPUT {
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
    float4 depth : COLOR1;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    OUT.color_0.rgba = float4(0.1, 0.1, 0.1, 1);
    OUT.depth = 1.0;

    return OUT;
};

// approximately 2 instruction slots used
