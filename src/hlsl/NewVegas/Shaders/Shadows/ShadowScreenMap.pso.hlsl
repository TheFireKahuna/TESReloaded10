
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
	if (TESR_ShadowData.y == 1.0f) { // Leaves (Speedtrees) or alpha is required
		float4 diffuse = tex2D(DiffuseMap, IN.texcoord_1.xy);
        if (diffuse.a < 0.5f)
            discard;
    }

    float depthValue = IN.texcoord_0.z / IN.texcoord_0.w;

    // First 10% of the depth buffer color red.
    if(depthValue < 0.9f)
    {
        OUT.color_0 = float4(depthValue, 0.0f, 0.0f, 1.0f);
    }
	
    // The next 0.025% portion of the depth buffer color green.
    if(depthValue > 0.9f)
    {
        OUT.color_0 = float4(depthValue, 1.0f, 0.0f, 1.0f);
    }

    // The remainder of the depth buffer color blue.
    if(depthValue > 0.925f)
    {
        OUT.color_0 = float4(depthValue, 0.0f, 1.0f, 1.0f);
    }

    return OUT;
};