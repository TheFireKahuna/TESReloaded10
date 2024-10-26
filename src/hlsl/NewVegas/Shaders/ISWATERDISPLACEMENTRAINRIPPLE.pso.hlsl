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

    const float4 const_0 = {1, 0.5, 0.5, 0.5};

    OUT.color_0.rgba = const_0.xyzw;

    return OUT;
};

// approximately 1 instruction slot used
