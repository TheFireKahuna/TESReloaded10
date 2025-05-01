//
//
#define	ScreenSpace	Src0
// Parameters:

sampler2D ScreenSpace : register(s0);
float4 TintColor : register(c0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   TintColor    const_0       1
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

    float4 r0;
    float4 t0;

    t0 = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    r0 = t0 * TintColor;
    OUT.color_0.a = r0.w;
    OUT.color_0.rgb = r0.xyz;

    return OUT;
};

// approximately 3 instruction slots used (1 texture, 2 arithmetic)
