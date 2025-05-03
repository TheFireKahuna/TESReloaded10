//
//
// Parameters:

sampler2D DiffuseMap : register(s0);
float4 LightData : register(c4);
float4 PSLightColor : register(c2);
sampler2D ShadowMap : register(s1);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   PSLightColor const_2        1
//   LightData    const_4       1
//   DiffuseMap   texture_0       1
//   ShadowMap    texture_1       1
//


// Structures:

struct VS_INPUT {
    float2 DiffuseUV : TEXCOORD0;			// partial precision
    float4 texcoord_5 : TEXCOORD5_centroid;	// only .w			// partial precision
    float4 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;
    float4 color_0 : COLOR4;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
/*

#define	expand(v)		(((v) - 0.5) / 0.5)
#define	compress(v)		(((v) * 0.5) + 0.5)
#define	weight(v)		dot(v, 1)
#define	sqr(v)			((v) * (v))

    const float4 const_0 = {1.75, 1, 0.5, 0};
    const float4 const_1 = {0.5, -0.5, 0.001, -0.00136718748};
    const float4 const_10 = {0.5, 1, 0.5 - 0.01, 0};
    const float4 const_3 = {0.5 - 0.01, 0.01, 0.5 + 0.01, -0.01};
    const float4 const_5 = {0.5, 0, 0.5 + 0.01, 0.01};
    const float4 const_6 = {0.5, -0.01, 0.5 + 0.01, 0};
    const float4 const_7 = {0, 1, 2, 0.125};
    const float4 const_9 = {0.5 - 0.01, -0.01, 0.5, 0.01};

    float1 l6;
    float1 q0;
    float1 q1;
    float1 q2;
    float1 q3;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;

    r0.xyzw = tex2D(DiffuseMap, IN.DiffuseUV.xy);			// partial precision
    r0.x = saturate(r0.w * 1.75) * IN.texcoord_5.w;			// partial precision
    color IN.color_0.w;			// partial precision
    r2.yz = const_0.yz;
    r1.xy = IN.texcoord_1.xy / IN.texcoord_1.w;			// partial precision
    q0.x = compress(r1.x);			// partial precision	// [-1,+1] to [0,1]
    q1.x = 1 - (IN.texcoord_1.y / IN.texcoord_1.w);			// partial precision
    r1.zw = (r1.xy * const_1.xy) + 0.5;			// partial precision
    r3.xyzw = (r1.xwxw * r2.zyzy) + const_5.zwxy;			// partial precision
    r1.yz = saturate(r1.zw >= 0.0 ? r3.xzww : 0);			// partial precision
    r3.xyz = tex2D(ShadowMap, saturate(r3.xy));			// partial precision
    r4.xyz = tex2D(ShadowMap, r1.yz);			// partial precision
    r2.xw = (-0.00136718748 >= (r4.x - (IN.texcoord_1.z * 0.001)) ? const_7.xyzy : const_7.yyzz);			// partial precision
    r4.xyzw = max((r1.xwxw * r2.zyzy) + const_3.xyzw, 0);			// partial precision
    r0.z = (-0.00136718748 >= (r3.x - (IN.texcoord_1.z * 0.001)) ? r2.x : r2.w);			// partial precision
    r3.xyz = tex2D(ShadowMap, saturate(r4.xy));			// partial precision
    r0.z = (-0.00136718748 >= (r3.x - (IN.texcoord_1.z * 0.001)) ? r0.z : (r0.z + 1));			// partial precision
    r3.xyz = tex2D(ShadowMap, saturate(r4.zw));			// partial precision
    r4.xyzw = max((r1.xwxw * r2.zyzy) + const_9.xyzw, 0);			// partial precision
    r0.z = (-0.00136718748 >= (r3.x - (IN.texcoord_1.z * 0.001)) ? r0.z : (r0.z + 1));			// partial precision
    r3.xyz = tex2D(ShadowMap, saturate(r4.xy));			// partial precision
    r0.z = (-0.00136718748 >= (r3.x - (IN.texcoord_1.z * 0.001)) ? r0.z : (r0.z + 1));			// partial precision
    r3.xyz = tex2D(ShadowMap, saturate(r4.zw));			// partial precision
    r0.z = (-0.00136718748 >= (r3.x - (IN.texcoord_1.z * 0.001)) ? r0.z : (r0.z + 1));			// partial precision
    r3.xyzw = (r1.xwxw * r2.zyzy) + const_6.xyzw;			// partial precision
    r4.xyzw = max(r3.xyzw, 0);			// partial precision
    r3.xyz = tex2D(ShadowMap, saturate(r4.xy));			// partial precision
    r0.z = (-0.00136718748 >= (r3.x - (IN.texcoord_1.z * 0.001)) ? r0.z : (r0.z + 1));			// partial precision
    r3.xyz = tex2D(ShadowMap, saturate(r4.zw));			// partial precision
    r1.z = r0.z + 1;			// partial precision
    q2.x = ((q0.x >= 0.0 ? (1 >= q0.x ? 1 : 0) : 0) * (q1.x >= 0.0 ? 1 : 0)) * ((1 - (q1.x * 0.5)) >= 0.0 ? 1 : 0);
    r0.y = q2.x * (IN.texcoord_1.z >= 0.0 ? 1 : 0);
    r0.z = ((-0.00136718748 - (r3.x - (IN.texcoord_1.z * 0.001))) >= 0.0 ? r0.z : r1.z);			// partial precision
    r1.xyz = tex2D(ShadowMap, saturate(const_10.zwzw + (r1.xwzw * const_10.xy)));			// partial precision
    q3.x = ((-0.00136718748 - (r1.x - (IN.texcoord_1.z * 0.001))) >= 0.0 ? r0.z : (r0.z + 1));			// partial precision
    r1.xyz = (r0.y <= 0.0 ? 1 : ((PSLightColor.a * (saturate(q3.x * 0.125) - 1)) + r2.y));			// partial precision

    if (0 != r0.y) {
      r0.y = min(min(r0.w, r0.y), 1 - r1.x);			// partial precision
      r2.yz = const_0.yz;
      l6.x = saturate(length(IN.texcoord_2.xyz - LightData.xyz) / (r2.z * LightData.w));
      r2.xyw = r2.y - (((1 - r1.x) * 0.6) * PSLightColor.xyzz);			// partial precision
      OUT.color_0.rgb = (r0.x * (lerp(lerp(r2.xyww, 1, sqr(l6.x)), 1, IN.color_0.w) - 1)) + 1;			// partial precision
    }
    else {
      OUT.color_0.rgb = r1.xyz;			// partial precision
    }

    OUT.color_0.a = min(r0.y, r0.x);			// partial precision
*/

    OUT.color_0 = IN.color_0;
    return OUT;
};

// approximately 108 instruction slots used (10 texture, 98 arithmetic)
