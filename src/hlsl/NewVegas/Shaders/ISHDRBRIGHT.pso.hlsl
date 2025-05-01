//
//
#define	ScreenSpace	Src0
// Parameters:

float3 BlurScale : register(c2);
float4 HDRParam : register(c1);
sampler2D ScreenSpace : register(s0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   HDRParam     const_1       1
//   BlurScale    const_2       1
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

    float4 screenSpace = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    OUT.color_0.a = BlurScale.z;
    //OUT.color_0.rgb = max(r0.xyz - HDRParam.x, 0) * HDRParam.y;
    OUT.color_0.rgb = screenSpace.rgb;

    return OUT;
};

// approximately 6 instruction slots used (1 texture, 5 arithmetic)
