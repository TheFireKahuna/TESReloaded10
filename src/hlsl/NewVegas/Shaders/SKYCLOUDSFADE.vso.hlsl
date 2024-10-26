//
//
// Parameters:

float4 BlendColor[3] : register(c4);
float3 EyePosition : register(c7);
row_major float4x4 Model : register(c8);
row_major float4x4 ModelViewProj : register(c0);
float TexCoordYOff : register(c12);


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//   BlendColor[0]    const_4        1
//   BlendColor[1]    const_5        1
//   BlendColor[2]    const_6        1
//   EyePosition   const_7       1
//   Model[0]         const_8        1
//   Model[1]         const_9        1
//   Model[2]         const_10        1
//   TexCoordYOff  const_12      1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
    float texcoord_2 : TEXCOORD2;
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 color = IN.color_0;
    
	float3 blendColor0 = BlendColor[0].rgb;
	float3 blendColor1 = BlendColor[1].rgb;
	float3 blendColor2 = BlendColor[2].rgb;

    OUT.color_0.rgb = (color.r * blendColor0) + (color.g * blendColor1) + (color.b * blendColor2);
    OUT.color_0.a = BlendColor[0].a * IN.color_0.a;
    OUT.position.xy = mul(float2x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw), IN.position.xyzw);
    OUT.position.zw = dot(ModelViewProj[3].xyzw, IN.position.xyzw) * ModelViewProj[2].yz;
    float3 r0;
    r0.y = TexCoordYOff.x + IN.texcoord_0.y;
    r0.x = IN.texcoord_0.x;
    OUT.texcoord_0.xy = r0.xy;
    OUT.texcoord_1.xy = r0.xy;
    OUT.texcoord_2.x = saturate((dot(Model[2].xyzw, IN.position.xyzw) - EyePosition.z) / 17);

    return OUT;
};

// approximately 15 instruction slots used
