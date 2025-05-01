//
//
#define	OverlaySpace	Src1
#define	ScreenSpace	Src0
// Parameters:

sampler2D ScreenSpace : register(s0);
sampler2D OverlaySpace : register(s1);
float4 blendW : register(c0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   blendW       const_0       1
//   ScreenSpace         texture_0       1
//   OverlaySpace         texture_1       1
//


// Structures:

struct VS_INPUT {
    float2 ScreenOffset : TEXCOORD0;
    float2 OverlayOffset : TEXCOORD1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 t0 = tex2D(OverlaySpace, IN.OverlayOffset.xy).rgb;
    float3 r1 = tex2D(ScreenSpace, IN.ScreenOffset.xy).rgb;
    OUT.color_0.a = 1;
    OUT.color_0.rgb = (blendW.x * r1) + (t0 * blendW.y);

    return OUT;
};

// approximately 6 instruction slots used (2 texture, 4 arithmetic)
