//
//
#define	ScreenSpace	Src0
// Parameters:

float3 BlurScale : register(c2);
sampler2D DestBlend : register(s1);
float4 HDRParam : register(c1);
sampler2D ScreenSpace : register(s0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   HDRParam     const_1       1
//   BlurScale    const_2       1
//   ScreenSpace         texture_0       1
//   DestBlend    texture_1       1
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

    float1 q0;
    float4 r0;
    float4 r1;

    r0 = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    r1 = tex2D(DestBlend, IN.texcoord_1.xy);
    q0.x = 1.0 / max(r0.w, HDRParam.x);
    OUT.color_0.a = BlurScale.z;
    OUT.color_0.rgb = ((q0.x * HDRParam.x) * r1.xyz) + max(r0.xyz * (q0.x * 0.5), 0);

    return OUT;
};

// approximately 11 instruction slots used (2 texture, 9 arithmetic)
