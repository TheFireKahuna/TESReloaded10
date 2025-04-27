
float4 TESR_ShadowData : register(c0);
float4 TESR_CameraData : register(c1);
float4 TESR_DepthConstants : register(c2);
float4x4 TESR_InvProjectionTransform : register(c3);

sampler2D DiffuseMap : register(s0) = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = POINT; MINFILTER = POINT; MIPFILTER = POINT; };

struct VS_OUTPUT {
    float4 texcoord_0 : TEXCOORD0;
	float4 texcoord_1 : TEXCOORD1;
};

struct PS_OUTPUT {
    float4 color_0 : COLOR0;
};

static const float invertedDepth = TESR_DepthConstants.z;
static const float nearZ = TESR_CameraData.x;
static const float farZ = TESR_CameraData.y;

// convert back to usual depth buffer format for easier reconstruction
float packDepth(float viewZ, float nearZ, float farZ){
	float Q = farZ/(farZ - nearZ);
	return (Q* (viewZ - nearZ ))/ viewZ;
}

PS_OUTPUT main(VS_OUTPUT IN) {
    PS_OUTPUT OUT;
	float depth = IN.texcoord_0.z;

    float x = IN.texcoord_1.x * 2 - 1;
    float y = (1 - IN.texcoord_1.y) * 2 - 1;
    float4 clipSpace = float4(x, y, depth, 1.0f);

    float4 viewSpace = mul(clipSpace, TESR_InvProjectionTransform);
	
    viewSpace /= viewSpace.w;

	OUT.color_0 = float4(viewSpace.z / farZ, depth, 1.0, 1.0); // scale values back to 0 - 1 to avoid overflow
    return OUT;
};