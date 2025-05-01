row_major float4x4 TESR_InvViewProjectionTransform : register(c100);
float4 TESR_SmoothedSunDir : register(c105); // x,y,z: center (world space), w: radius
row_major float4x4 TESR_InvViewTransform : register(c106);
row_major float4x4 TESR_ViewProjectionTransform : register(c110);
row_major float4x4 TESR_ViewTransform : register(c114);
row_major float4x4 TESR_ProjectionTransform : register(c119);
float4 TESR_ViewSpaceLightDir : register(c123);
float4 TESR_SunDirection : register(c124);
float4 TESR_SunPosition : register(c125);
row_major float4x4 TESR_InvProjectionTransform : register(c126);
row_major float4x4 TESR_WorldTransform : register(c130);
float4 TESR_DebugVar : register(c40);

float4 TESR_CameraPosition : register(c20); // x,y,z: center (world space), w: radius
static const float NormalBias = 1.0f;

float4 clipToWorld(float4 coord){
	return mul(coord, TESR_InvViewProjectionTransform);
}
float3 worldToCamera(float4 coord){
    return coord.xyz - TESR_CameraPosition.xyz;
	float4 result = mul(float4(coord.xyz - TESR_CameraPosition.xyz,1.0f), TESR_ViewProjectionTransform);
    return result.xyz / result.w;
}
float3 sunToCamera(float4 coord){
    return mul(TESR_SunDirection,TESR_ViewTransform).xyz;
	float4 result = mul(float4(coord.xyz,1.0f), TESR_ViewProjectionTransform);
    return result.xyz;
}
float3 sunDirection(){
    float4 direction = TESR_SunDirection;
    if (TESR_DebugVar.y == 1.0) {
        direction = TESR_SunPosition;
    }
    if (TESR_DebugVar.y == 2.0) {
        direction = TESR_ViewSpaceLightDir;
    }
    if (TESR_DebugVar.y == 3.0) {
        direction = TESR_SmoothedSunDir;
    }
    if (TESR_DebugVar.z == 1.0) {
        direction.xyz = -(direction.xyz);
    }
    if (TESR_DebugVar.w == 1.0) {
        direction = mul(direction,TESR_WorldTransform);
    }
    if (TESR_DebugVar.x == 1.0) {
        return (mul(direction,TESR_ViewTransform).xyz);
    }
    if (TESR_DebugVar.x == 2.0) {
        return (mul(direction,TESR_InvViewTransform).xyz);
    }
    if (TESR_DebugVar.x == 3.0) {
        return (mul(direction,TESR_ViewProjectionTransform).xyz);
    }
    if (TESR_DebugVar.x == 4.0) {
        return (mul(direction,TESR_InvViewProjectionTransform).xyz);
    }
    if (TESR_DebugVar.x == 5.0) {
        return (mul(direction,TESR_ProjectionTransform).xyz);
    }
    if (TESR_DebugVar.x == 6.0) {
        return (mul(direction,TESR_InvProjectionTransform).xyz);
    }
    return direction.xyz;
}
float4 clipToWorldWithOffset(float4 coord, float3 normal){
    float4 worldPos = mul(coord, TESR_InvViewProjectionTransform);
    float NdotL = dot(normal, TESR_SmoothedSunDir.xyz);
    float offsetScale = saturate(1 - NdotL);
    float3 normalOffset = offsetScale * NormalBias * normal;
    
    return float4(worldPos.xyz + normalOffset, 1.0f);
}