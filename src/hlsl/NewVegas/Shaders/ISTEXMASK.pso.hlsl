//
//
#define	ScreenSpace	Src0
// Parameters:

sampler2D ScreenSpace : register(s0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   ScreenSpace         texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 ScreenOffset : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const int4 const_0 = {0, 0, 0, 1};
    const int4 const_1 = {1, 1, 1, -1};

    float4 r0;
    float4 r1;
    float4 r2;

    r0.xyzw = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    r1.xyzw = const_1.xyzw;
    r2.xyz = (r1.xyz * r0.xyz) + const_0.xyz;
    r2.w = (r0.x * r1.w) + 1;
    OUT.color_0.rgba = r2.xyzw;

    return OUT;
};

// approximately 5 instruction slots used (1 texture, 4 arithmetic)
