row_major float4x4 TESR_InvViewProjectionTransform : register(c100);
float4 TESR_SmoothedSunDir : register(c105); // x,y,z: center (world space), w: radius
static const float NormalBias = 1.0f;

float4 clipToWorld(float4 coord){
	return mul(coord, TESR_InvViewProjectionTransform);
}
float4 clipToWorldWithOffset(float4 coord, float3 normal){
    float4 worldPos = mul(coord, TESR_InvViewProjectionTransform);
    float NdotL = dot(normal, TESR_SmoothedSunDir.xyz);
    float offsetScale = saturate(1 - NdotL);
    float3 normalOffset = offsetScale * NormalBias * normal;
    
    return float4(worldPos.xyz + normalOffset, 1.0f);
}