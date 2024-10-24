//
//
#define	ScreenSpace	Src0
// Parameters:

sampler2D AlphaMap : register(s1);
sampler2D ScreenSpace : register(s0);
float4 TintColor : register(c0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   TintColor    const_0       1
//   ScreenSpace         texture_0       1
//   AlphaMap     texture_1       1
//


// Structures:

struct VS_INPUT {
    float2 ScreenOffset : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 r0;
    float4 r1;
    float4 t0;

    t0 = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    r0 = tex2D(AlphaMap, IN.texcoord_1.xy);
    r1 = t0 * TintColor;
    r1.w = r0.x * r1.w;
    OUT.color_0 = r1;

    return OUT;
};

// approximately 5 instruction slots used (2 texture, 3 arithmetic)
