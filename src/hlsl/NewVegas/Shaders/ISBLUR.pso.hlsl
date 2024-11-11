//
//
// Parameters:

sampler2D Src0 : register(s0);
float4 blurParams : register(c0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   blurParams   const_0       1
//   Src0         texture_0       1
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

    const float4 const_1 = {0.5, (1.0 / 18.0), (1.0 / 9.0), 0.05};

    float2 offset = (const_1.z * blurParams.xy);

    float2 coord0 = float2((blurParams.x * const_1.y) + (IN.texcoord_0.x - (blurParams.x * const_1.x)), (blurParams.y * const_1.y) + (IN.texcoord_0.y - (blurParams.y * const_1.x)));
    float2 coord1 = offset + coord0;
    float2 coord2 = offset + coord1;
    float2 coord3 = offset + coord2;
    float2 coord4 = offset + coord3;
    float2 coord5 = offset + coord4;
    float2 coord6 = offset + coord5;
    float2 coord7 = offset + coord6;
    float2 coord8 = offset + coord7;
    float3 sample0 = tex2D(Src0, coord0).rgb;
    float3 sample1 = tex2D(Src0, coord1).rgb;
    float3 sample2 = tex2D(Src0, coord2).rgb;
    float3 sample3 = tex2D(Src0, coord3).rgb;
    float3 sample4 = tex2D(Src0, coord4).rgb;
    float3 sample5 = tex2D(Src0, coord5).rgb;
    float3 sample6 = tex2D(Src0, coord6).rgb;
    float3 sample7 = tex2D(Src0, coord7).rgb;
    float3 sample8 = tex2D(Src0, coord8).rgb;
    float3 finalColor = (0.3 * sample4) + ((0.15 * sample3) + ((0.075 * sample2) + ((0.05 * sample1) + (0.025 * sample0))));
    finalColor = (0.025 * sample8) + ((0.05 * sample7) + ((0.075 * sample6) + ((0.15 * sample5) + finalColor)));
    OUT.color_0.a = 1.0;
    OUT.color_0.rgb = finalColor;

    return OUT;
};

// approximately 33 instruction slots used (9 texture, 24 arithmetic)
