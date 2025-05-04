float4x4 TESR_InvViewProjectionTransform : register(c100);
float4 TESR_SmoothedSunDir : register(c105); // x,y,z: center (world space), w: radius
float4x4 TESR_InvViewTransform : register(c106);
float4x4 TESR_ViewProjectionTransform : register(c110);
float4x4 TESR_ViewTransform : register(c114);
float4x4 TESR_ProjectionTransform : register(c119);
float4 TESR_ViewSpaceLightDir : register(c123);
float4 TESR_SunDirection : register(c124); // World Space 
float4 TESR_SunPosition : register(c125); // World Space 
float4x4 TESR_InvProjectionTransform : register(c126);
float4 TESR_DebugVar : register(c40);
float4 TESR_GameTime : register(c41);

float4 TESR_CameraPosition : register(c20); // x,y,z: center (world space), w: radius
static const float NormalBias = 1.0f;

float4 clipToWorld(float4 coord){
    float4 world = coord;
    if (TESR_DebugVar.z >= 4) {
	    world = mul(world, TESR_InvViewProjectionTransform);
    }
    else {
	    world = mul(TESR_InvViewProjectionTransform, world);
    }
    return world / world.w;
}
float4 clipToWorldWithOffset(float4 coord, float3 normal){
    float4 world = coord;
    if (TESR_DebugVar.z >= 4) {
	    world = mul(world, TESR_InvViewProjectionTransform);
    }
    else {
	    world = mul(TESR_InvViewProjectionTransform, world);
    }
    world = world / world.w;
    float NdotL = dot(normal, TESR_SunDirection.xyz);
    float offsetScale = saturate(1 - NdotL);
    float3 normalOffset = offsetScale * NormalBias * normal;
    
    return float4(world.xyz + normalOffset, 1.0f);
}
float3 worldVector(float4 coord, float4 worldPos){
    return coord.xyz - worldPos.xyz; 
}
float3 sunDirection(){
    float4 direction = TESR_SunDirection;
    return direction.xyz;
}