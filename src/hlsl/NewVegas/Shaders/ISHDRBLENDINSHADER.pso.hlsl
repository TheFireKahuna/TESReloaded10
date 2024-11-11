//
//
#define	ScreenSpace	Src0
// Parameters:

float3 BlurScale : register(c2);
sampler2D DestBlend : register(s1);
float4 HDRParam : register(c1);
sampler2D ScreenSpace : register(s0);
sampler2D TESR_BloomBuffer : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

float4 TESR_HDRBloomData : register(c25); //
float4 TESR_HDRData : register(c27); //
float4 TESR_LotteData : register(c28);
float4 TESR_ToneMapping : register(c29); //
float4 TESR_BloomExtraData : register(c31); // .x - NVR bloom on/off.

#include "Includes/Helpers.hlsl"
#include "Includes/Tonemapping.hlsl"

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

    float4 final = tex2D(DestBlend, IN.texcoord_1.xy);
    
	[branch]
    if (TESR_BloomExtraData.x){
        // NVR bloom
        float4 NVRbloom = tex2D(TESR_BloomBuffer, IN.texcoord_1.xy); // already linear
        final.rgb = lerp(final.rgb, NVRbloom.rgb * TESR_HDRBloomData.y, saturate(TESR_HDRBloomData.x));
    }else{
        // vanilla bloom
        // scale bloom while maintaining color
        float4 bloom = tex2D(Src0, IN.ScreenOffset.xy);
        bloom.rgb = TESR_HDRBloomData.x * pows(bloom.rgb, TESR_HDRBloomData.y);

        float bloomModifier = 1.0 / max(bloom.w, HDRParam.x); // HDRParam.x is brights cutoff (clamp)
        final.rgb = ((bloomModifier * HDRParam.x) * final.rgb) + bloom.rgb * (bloomModifier * 0.5); // blend image and bloom
    }
    
    final.rgb = tonemap(final.rgb); // exposure & tonemap using provided tonemapper
    
    OUT.color_0 = float4(final.rgb,BlurScale.z); 

    return OUT;
};

// approximately 11 instruction slots used (2 texture, 9 arithmetic)
