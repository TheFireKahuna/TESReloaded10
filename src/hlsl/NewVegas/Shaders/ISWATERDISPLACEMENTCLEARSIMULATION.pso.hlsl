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

    float4 r0;

    r0.xyzw = 0.5;
    OUT.color_0.a = r0.w;
    OUT.color_0.rgb = r0.xyz;

    return OUT;
};

// approximately 2 instruction slots used
