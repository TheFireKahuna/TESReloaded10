//
//
#define	ScreenSpace	Src0
// Parameters:

float4 BlurOffsets[16] : register(c3);
float3 BlurScale : register(c2);
sampler2D ScreenSpace : register(s0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   BlurScale    const_2       1
//   BlurOffsets[0]  BlurOffsets[0]       9
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

    float2 offsetTest = frac(IN.ScreenOffset * 128);
    float2 blurCoords0 = (offsetTest <= 0.0 ? (2.5 - BlurOffsets[0].xy) : BlurOffsets[0].xy);
    float2 blurCoords1 = (offsetTest <= 0.0 ? (2.5 - BlurOffsets[1].xy) : BlurOffsets[1].xy);
    float2 blurCoords2 = (offsetTest <= 0.0 ? (2.5 - BlurOffsets[2].xy) : BlurOffsets[2].xy);
    float2 blurCoords3 = (offsetTest <= 0.0 ? (2.5 - BlurOffsets[3].xy) : BlurOffsets[3].xy);
    float2 blurCoords4 = (offsetTest <= 0.0 ? (2.5 - BlurOffsets[4].xy) : BlurOffsets[4].xy);
    float2 blurCoords5 = (offsetTest <= 0.0 ? (2.5 - BlurOffsets[5].xy) : BlurOffsets[5].xy);
    float2 blurCoords6 = (offsetTest <= 0.0 ? (2.5 - BlurOffsets[6].xy) : BlurOffsets[6].xy);
    float2 blurCoords7 = (offsetTest <= 0.0 ? (2.5 - BlurOffsets[7].xy) : BlurOffsets[7].xy);
    float2 blurCoords8 = (offsetTest <= 0.0 ? (2.5 - BlurOffsets[8].xy) : BlurOffsets[8].xy);

    float3 blurSample8 = tex2D(ScreenSpace, (BlurScale.xy * blurCoords8) + IN.ScreenOffset.xy).rgb;
    float3 blurSample7 = tex2D(ScreenSpace, (BlurScale.xy * blurCoords7) + IN.ScreenOffset.xy).rgb;
    float3 blurSample6 = tex2D(ScreenSpace, (BlurScale.xy * blurCoords6) + IN.ScreenOffset.xy).rgb;
    float3 blurSample5 = tex2D(ScreenSpace, (BlurScale.xy * blurCoords5) + IN.ScreenOffset.xy).rgb;
    float3 blurSample4 = tex2D(ScreenSpace, (BlurScale.xy * blurCoords4) + IN.ScreenOffset.xy).rgb;
    float3 blurSample3 = tex2D(ScreenSpace, (BlurScale.xy * blurCoords3) + IN.ScreenOffset.xy).rgb;
    float3 blurSample2 = tex2D(ScreenSpace, (BlurScale.xy * blurCoords2) + IN.ScreenOffset.xy).rgb;
    float3 blurSample1 = tex2D(ScreenSpace, (BlurScale.xy * blurCoords1) + IN.ScreenOffset.xy).rgb;
    float3 blurSample0 = tex2D(ScreenSpace, (BlurScale.xy * blurCoords0) + IN.ScreenOffset.xy).rgb;
    float3 finalColor = (BlurOffsets[3].z * blurSample4) + ((BlurOffsets[2].z * blurSample3) + ((BlurOffsets[0].z * blurSample2) + (blurSample1 * BlurOffsets[1].z)));
    finalColor += (BlurOffsets[7].z * blurSample8) + ((BlurOffsets[6].z * blurSample7) + ((BlurOffsets[5].z * blurSample6) + ((BlurOffsets[4].z * blurSample5))));
    OUT.color_0.a = BlurScale.z;
    OUT.color_0.rgb = (BlurOffsets[8].z * blurSample0) + finalColor;

    return OUT;
};

// approximately 70 instruction slots used (9 texture, 61 arithmetic)
