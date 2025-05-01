//
//
#define	ScreenSpace	Src0
// Parameters:

sampler2D AvgLum : register(s1);
float3 BlurScale : register(c2);
float4 HDRParam : register(c1);
sampler2D ScreenSpace : register(s0);
float4 TimingData : register(c0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   TimingData   const_0       1
//   HDRParam     const_1       1
//   BlurScale    const_2       1
//   ScreenSpace         texture_0       1
//   AvgLum       texture_1       1
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

    float3 q1;
    float4 r0;
    float4 r1;

    r0 = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    r1 = tex2D(AvgLum, IN.ScreenOffset.xy);
    q1.xyz = lerp(r0.xyz, r1.xyz, 1 - pow(abs(HDRParam.z), TimingData.z));
    OUT.color_0.a = BlurScale.z;
    OUT.color_0.rgb = r0.rgb;
   //OUT.color_0.rgb = q1.xyz * (min(max(0.01, length(q1.xyz)), HDRParam.w) / max(0.01, length(q1.xyz)));

    return OUT;
};

// approximately 18 instruction slots used (2 texture, 16 arithmetic)
