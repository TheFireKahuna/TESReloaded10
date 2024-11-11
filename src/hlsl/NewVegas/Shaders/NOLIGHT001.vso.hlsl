//
//
// Parameters:

float4 FogParam : register(c13);
row_major float4x4 ModelViewProj : register(c0);


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//   FogParam      const_13      1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    const int4 const_4 = {0, 1, 0, 0};

    float4 r0;
    float4 dest;

    float3 mdl0 = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);
    r0.zw = FogParam.z;
    r0.xy = 1 - saturate((FogParam.x - length(mdl0)) / FogParam.y);

    dest.x = 1;
    dest.y = 0;
    dest.z = 0;
    dest.w = 1;

    float power = r0.w;
    const float MAXPOWER = 127.9961f;
    if (power < -MAXPOWER)
        power = -MAXPOWER;          // Fits into 8.8 fixed point format
    else if (power > MAXPOWER)
        power = MAXPOWER;          // Fits into 8.8 fixed point format

    if (r0.x > 0)
    {
        dest.y = r0.x;
        if (r0.y > 0)
        {
            // Allowed approximation is EXP(power * LOG(src.y))
            dest.z = (float)(pow(r0.y, power));
        }
    }
    OUT.color_0.rgba = IN.color_0.rgba;
    OUT.position.xyz = mdl0;
    OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    OUT.texcoord_0.xy = (dest.z * const_4.xy) + const_4.yx;

    return OUT;
};

// approximately 18 instruction slots used
