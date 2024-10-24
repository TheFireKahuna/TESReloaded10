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
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 r0;
    float4 r1;

    r0 = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    r1.xyz = r0.xyz * TintColor.rgb;
    r1.w = r0.w * IN.color_0.a;
    OUT.color_0 = r1;

    return OUT;
};

// approximately 4 instruction slots used (1 texture, 3 arithmetic)
