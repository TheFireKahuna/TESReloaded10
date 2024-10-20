// GDECAL - DECAL, GEOMDECAL, MAXDECALS = 1

sampler2D DecalMap : register(s1);
float4 PSDecalOffset : register(c15);
float4 TESR_LinearObject : register(c30);

#include "includes/Helpers.hlsl"

struct PS_INPUT {
    float3 Normal : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Color : COLOR0;
};

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;

    float2 vUV;
    float2 r0 = saturate(IN.Normal.xy);
    vUV.y = (PSDecalOffset.w * r0.y) + PSDecalOffset.z;
    vUV.x = (PSDecalOffset.y * r0.x) + PSDecalOffset.x;
    float4 vDiffuse = tex2D(DecalMap, vUV.xy);
    vDiffuse = linearCheck(vDiffuse, TESR_LinearObject.y);
    OUT.Color.rgba = vDiffuse * IN.Normal.z;

    return OUT;
};