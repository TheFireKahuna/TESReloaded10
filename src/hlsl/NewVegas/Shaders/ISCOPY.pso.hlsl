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

    OUT.color_0 = tex2D(ScreenSpace, IN.ScreenOffset.xy);

    return OUT;
};

// approximately 2 instruction slots used (1 texture, 1 arithmetic)
