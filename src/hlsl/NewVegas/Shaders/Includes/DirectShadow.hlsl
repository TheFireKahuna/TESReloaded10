sampler2D TESR_ShadowAtlas : register(s15) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

float4 TESR_ShadowFormatData : register(c62);
float4 TESR_ShadowNearCenter : register(c63); // x,y,z: center (world space), w: radius
float4 TESR_ShadowMiddleCenter : register(c64); // x,y,z: center (world space), w: radius
float4 TESR_ShadowFarCenter : register(c65); // x,y,z: center (world space), w: radius
float4 TESR_ShadowLodCenter : register(c66); // x,y,z: center (world space), w: radius
row_major float4x4 TESR_ShadowCameraToLightTransformNear : register(c68);
row_major float4x4 TESR_ShadowCameraToLightTransformMiddle : register(c72);
row_major float4x4 TESR_ShadowCameraToLightTransformFar : register(c76);
row_major float4x4 TESR_ShadowCameraToLightTransformLod : register(c80);

static const float Mode = TESR_ShadowFormatData.x;
static const float FormatBits = TESR_ShadowFormatData.y;

static const float BIAS = 0.018;
float Linstep(float a, float b, float v) {
    return saturate((v - a) / (b - a));
}

// Reduces VSM light bleedning
float ReduceLightBleeding(float pMax, float amount) {
  // Remove the [0, amount] tail and linearly rescale (amount, 1].
    return Linstep(amount, 1.0f, pMax);
}

float ChebyshevUpperBound(float2 moments, float mean, float minVariance,
                          float lightBleedingReduction) {
    // Compute variance
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, minVariance);

    // Compute probabilistic upper bound
    float d = mean - moments.x;
    float pMax = variance / (variance + (d * d));

    pMax = ReduceLightBleeding(pMax, lightBleedingReduction);

    // One-tailed Chebyshev
    return (mean <= moments.x ? 1.0f : pMax);
}


float2 GetEVSMExponents(in float positiveExponent, in float negativeExponent, in float formatBits) {
    const float maxExponent = formatBits == 0.0 ? 5.54f : 42.0f;

    float2 lightSpaceExponents = float2(positiveExponent, negativeExponent);

    // Clamp to maximum range of fp32/fp16 to prevent overflow/underflow
    return min(lightSpaceExponents, maxExponent);
}

// Applies exponential warp to shadow map depth, input depth should be in [0, 1]
float2 WarpDepth(float depth, float2 exponents) {
    // Rescale depth into [-1, 1]
    depth = 2.0f * depth - 1.0f;
    float pos = exp(exponents.x * depth);
    float neg = -exp(-exponents.y * depth);
    return float2(pos, neg);
}



float GetLightAmountValueVSM(float2 moments, float depth, float bias, float bleedReduction) {
    return ChebyshevUpperBound(moments, depth, bias, bleedReduction);
}
float GetLightAmountValueEVSM2(float2 moments, float depth, float bias, float bleedReduction, float formatBits) {
    float2 exponents = GetEVSMExponents(40.0f, 5.0f, formatBits);
    float2 warpedDepth = WarpDepth(depth, exponents);

    // Derivative of warping at depth
    float2 depthScale = bias * exponents * warpedDepth;
    float2 minVariance = depthScale * depthScale;

    return ChebyshevUpperBound(moments, warpedDepth.x, minVariance.x, bleedReduction);
}

float GetLightAmountValueEVSM4(float4 moments, float depth, float bias, float bleedReduction, float formatBits) {
    float2 exponents = GetEVSMExponents(40.0f, 5.0f, formatBits);
    float2 warpedDepth = WarpDepth(depth, exponents);

    // Derivative of warping at depth
    float2 depthScale = bias * exponents * warpedDepth;
    float2 minVariance = depthScale * depthScale;

    float posContrib = ChebyshevUpperBound(moments.xz, warpedDepth.x, minVariance.x, bleedReduction);
    float negContrib = ChebyshevUpperBound(moments.yw, warpedDepth.y, minVariance.y, bleedReduction);
    
    return min(posContrib, negContrib);
}


float4 ScreenCoordToTexCoord(float4 coord){
	// apply perspective (perspective division) and convert from -1/1 to range to 0/1 (shadowMap range);
	coord.xyz /= coord.w;
	coord.x = coord.x * 0.5f + 0.5f;
	coord.y = coord.y * -0.5f + 0.5f;

	return coord;
}

float GetLightAmountValue(float4x4 lightTransform, float4 coord, float offsetX, float offsetY, float bias, float bleedReduction) {
    float4 LightSpaceCoord = ScreenCoordToTexCoord(mul(coord, lightTransform));
	
	// Offset to the correct position in the atlas.
    LightSpaceCoord.xy *= 0.5;
    LightSpaceCoord.x += offsetX;
    LightSpaceCoord.y += offsetY;
	
    float4 shadowBufferValue = tex2D(TESR_ShadowAtlas, LightSpaceCoord.xy);
	
    float shadow;
	
	[branch]
    if (Mode == 0.0f)
        shadow = GetLightAmountValueVSM(shadowBufferValue.xy, LightSpaceCoord.z, bias, bleedReduction);
    else if (Mode == 1.0f)
        shadow = GetLightAmountValueEVSM2(shadowBufferValue.xy, LightSpaceCoord.z, bias, bleedReduction, FormatBits);
	else
        shadow = GetLightAmountValueEVSM4(shadowBufferValue, LightSpaceCoord.z, bias, bleedReduction, FormatBits);
	
    return shadow;
}

float GetLightAmount(float4 positionWS) {
    
    float4 samplePos = float4(positionWS.xyz, 1.0);
    //samplePos.x = samplePos.x *  0.5f + 0.5f;
    //samplePos.y = samplePos.y * -0.5f + 0.5f;
	
    const float bias = Mode == 0.0f ? 0.00001f : 0.01f;
	
    const float blend = 0.9f;
	
	float4 shadows = {
        GetLightAmountValue(TESR_ShadowCameraToLightTransformNear, samplePos, 0.0, 0.0, bias, 0.1f),
		GetLightAmountValue(TESR_ShadowCameraToLightTransformMiddle, samplePos, 0.5, 0.0, bias, 0.2f),
		GetLightAmountValue(TESR_ShadowCameraToLightTransformFar, samplePos, 0.0, 0.5, bias, 0.6f),
		GetLightAmountValue(TESR_ShadowCameraToLightTransformLod, samplePos, 0.5, 0.5, bias, 0.8f),
    };
	
    float4 distances = {
        length(samplePos.xyz - TESR_ShadowNearCenter.xyz),
		length(samplePos.xyz - TESR_ShadowMiddleCenter.xyz),
		length(samplePos.xyz - TESR_ShadowFarCenter.xyz),
		length(samplePos.xyz - TESR_ShadowLodCenter.xyz),
    };
	
    if (distances.x < TESR_ShadowNearCenter.w) {
        if (distances.x < TESR_ShadowNearCenter.w * blend)
            return shadows.x;
		
        return lerp(shadows.x, shadows.y, smoothstep(TESR_ShadowNearCenter.w * blend, TESR_ShadowNearCenter.w, distances.x));
    }
    else if (distances.y < TESR_ShadowMiddleCenter.w) {
        if (distances.y < TESR_ShadowMiddleCenter.w * blend)
            return shadows.y;
		
        return lerp(shadows.y, shadows.z, smoothstep(TESR_ShadowMiddleCenter.w * blend, TESR_ShadowMiddleCenter.w, distances.y));
    }
    else if (distances.z < TESR_ShadowFarCenter.w) {
        if (distances.z < TESR_ShadowFarCenter.w * blend)
            return shadows.z;
		
        return lerp(shadows.z, shadows.w, smoothstep(TESR_ShadowFarCenter.w * blend, TESR_ShadowFarCenter.w, distances.z));
    }
    else if (distances.w < TESR_ShadowLodCenter.w) {
        if (distances.w < TESR_ShadowLodCenter.w * blend)
            return shadows.w;
		
        return lerp(shadows.w, 1.0f, smoothstep(TESR_ShadowLodCenter.w * blend, TESR_ShadowLodCenter.w, distances.w));
    }
    else {
        return 1.0f;
    }
}