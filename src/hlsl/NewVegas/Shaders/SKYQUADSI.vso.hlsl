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

    const int4 const_0 = {1, 0, 0, 0};

    OUT.position.xyzw = (IN.position.xyzx * const_0.xxxy) + const_0.yyyx;

    return OUT;
};

// approximately 1 instruction slot used
