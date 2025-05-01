//
//
// Parameters:

sampler2D DisplaySampler : register(s0);
float2 TextureOffset : register(c2);


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   TextureOffset  const_2       1
//   DisplaySampler texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 texcoord_0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    OUT.color_0 = lerp(tex2D(DisplaySampler, IN.texcoord_0.xy + TextureOffset.xy), 0.5, saturate((length(IN.texcoord_0.xy - 0.5) - 0.4) * 10));

    return OUT;
};

// approximately 11 instruction slots used (1 texture, 10 arithmetic)
