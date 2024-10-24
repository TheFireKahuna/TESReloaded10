row_major float4x4  ModelViewProj               : register(c0);
float4              DecalFade                   : register(c31);

// GDECAL - DECAL, GEOMDECAL, MAXDECALS = 1

struct VS_INPUT {
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
};

struct VS_OUTPUT {
    float4 Position     : POSITION;
    float3 texcoord_0   : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    OUT.Position = mul(ModelViewProj, IN.Position);
    OUT.texcoord_0.xy = IN.Normal.xy;
    OUT.texcoord_0.z = (DecalFade.y * DecalFade.x) * IN.Normal.z; // y is fade,

    return OUT;
};