
row_major float4x4 TESR_ShadowWorldTransform : register(c0);
row_major float4x4 TESR_ShadowViewProjTransform : register(c4);
float4 TESR_ShadowData : register(c8);
float4 Bones[54] : register(c9);
float4 TESR_ShadowCubeMapLightPosition : register(c63);

struct VS_INPUT {
    float4 position : POSITION;
	float4 texcoord_0 : TEXCOORD0;
    float4 blendweight : BLENDWEIGHT;
    float4 blendindexes : BLENDINDICES;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
	float4 texcoord_1 : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

#define	weight(v)		dot(v, 1.0)

	const float4 const_0 = {1.0f, 765.01001f, 0.0f, 0.0f};
	
	float4 offset;
    float4 q0;
    float4 q4;
    float4 q5;
    float4 q6;
    float4 q7;
    float4 q8;
	float4 r1 = 0.0f;
	float4 r0 = IN.position;
	
	if (TESR_ShadowData.x == 1.0f) { // Skinned (Actors)
		offset = IN.blendindexes.zyxw * const_0.y;
		r0.w = 1;
		q0 = (IN.position.xyzx * const_0.xxxz) + const_0.zzzx;
		q8.xyz = mul(float3x4(Bones[0 + offset.w], Bones[1 + offset.w], Bones[2 + offset.w]), q0);
		q6.xyz = mul(float3x4(Bones[0 + offset.z], Bones[1 + offset.z], Bones[2 + offset.z]), q0);
		q5.xyz = mul(float3x4(Bones[0 + offset.x], Bones[1 + offset.x], Bones[2 + offset.x]), q0);
		q4.xyz = mul(float3x4(Bones[0 + offset.y], Bones[1 + offset.y], Bones[2 + offset.y]), q0);
		q7.xyz = (IN.blendweight.z * q6.xyz) + ((IN.blendweight.x * q5.xyz) + (q4.xyz * IN.blendweight.y));
		r0.xyz = ((1 - weight(IN.blendweight.xyz)) * q8.xyz) + q7.xyz;
	}
    if (TESR_ShadowData.x != 1.0f) r0 = mul(r0, TESR_ShadowWorldTransform);
	r1.xyz = TESR_ShadowCubeMapLightPosition.xyz - r0.xyz;
	r0 = mul(r0, TESR_ShadowViewProjTransform);
	OUT.position = r0;
    OUT.texcoord_0 = r1;
	OUT.texcoord_1 = IN.texcoord_0;
    return OUT;
	
};