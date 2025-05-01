//
//
// Parameters:

sampler2D Image : register(s0);
float4 center : register(c1);
float4 params : register(c0);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   params       const_0       1
//   center       const_1       1
//   Image        texture_0       1
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

    float2 q0;
    float4 q17;
    float4 q19;
    float1 q3;
    float1 q4;
    float1 q48;
    float4 r0;
    float4 t10;
    float4 t11;
    float4 t12;
    float4 t13;
    float4 t14;
    float4 t15;
    float4 t16;
    float4 t18;
    float4 t26;
    float4 t6;
    float4 t7;
    float4 t8;
    float4 t9;

    t16 = tex2D(Image, IN.texcoord_0.xy);
    q0.xy = center.xy - IN.texcoord_0.xy;
    r0.xy = normalize(q0.xy);
    q3.x = max(length(q0.xy) - params.z, 0);
    q4.x = max(length(q0.xy) - center.z, 0);
    r0.w = (q4.x <= 0.0 ? 0 : ((1 - (1.0 / ((params.w * q4.x) + 1))) * params.x));
    q48.x = max((q3.x <= 0.0 ? 0 : ((1 - (1.0 / ((params.y * q3.x) + 1))) * params.x)) - r0.w, 0);
    t26 = tex2D(Image, IN.texcoord_0.xy - (q48.x * r0.xy));
    t18 = tex2D(Image, (q48.x * r0.xy) + IN.texcoord_0.xy);
    r0.zw = r0.yx * q48.x;
    t10 = tex2D(Image, IN.texcoord_0.xy - (r0.wz / 3));
    t13 = tex2D(Image, IN.texcoord_0.xy - (r0.wz * 0.833333313));
    t12 = tex2D(Image, IN.texcoord_0.xy - (r0.wz * 0.666666687));
    t11 = tex2D(Image, IN.texcoord_0.xy - (r0.wz * 0.5));
    t9 = tex2D(Image, IN.texcoord_0.xy - (r0.wz * 0.166666672));
    t15 = tex2D(Image, (r0.wz * 0.833333313) + IN.texcoord_0.xy);
    t14 = tex2D(Image, (r0.wz * 0.666666687) + IN.texcoord_0.xy);
    t6 = tex2D(Image, (r0.wz * 0.5) + IN.texcoord_0.xy);
    t8 = tex2D(Image, (r0.wz * 0.166666672) + IN.texcoord_0.xy);
    t7 = tex2D(Image, (r0.wz * -(1.0 / 3)) + IN.texcoord_0.xy);
    q17 = t16 + (t9 + (t10 + (t11 + (t12 + (t26 + t13)))));
    q19 = t18 + (t15 + (t14 + (t6 + (t7 + (t8 + q17)))));
    r0 = q19 * 0.0769230798;
    OUT.color_0.a = r0.w;
    OUT.color_0.rgb = r0.xyz;

    return OUT;
};

// approximately 63 instruction slots used (13 texture, 50 arithmetic)
