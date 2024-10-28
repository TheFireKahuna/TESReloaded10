#if defined(__INTELLISENSE__)
    #include "Helpers.hlsl"
    #include "Pointlights.hlsl"
    #include "PBR_Alt.hlsl"
#else
    #include "includes/Pointlights.hlsl"
    #include "includes/PBR_Alt.hlsl"
#endif

float4 TESR_TerrainData : register(c32);
float4 TESR_TerrainExtraData : register(c33);
float4 TESR_LinearTerrain : register(c38);

float getRoughness(float gloss) {
    return saturate((1 - gloss) * TESR_TerrainData.y);
}
float3 blendDiffuseMaps(float3 vertexColor, float2 uv, int texCount, sampler2D tex[7], float blends[7]) {
    float3 color = tex2D(tex[0], uv).xyz * blends[0];
    color = linearCheck(color, TESR_LinearTerrain.y);
    
    float3 blendColor;
    [unroll] for (int i = 1; i < texCount; i++) {
        blendColor = tex2D(tex[i], uv).rgb;
        blendColor = linearCheck(blendColor, TESR_LinearTerrain.y);
        color += blendColor * blends[i];
    }

    vertexColor = linearCheck(vertexColor, TESR_LinearTerrain.z);
    return color * vertexColor;
}

float4 blendNormalMaps(float2 uv, int texCount, sampler2D tex[7], float blends[7]) {
    
    float4 normal = tex2D(tex[0], uv) * blends[0];
    
    [unroll] for (int i = 1; i < texCount; i++){
        normal += tex2D(tex[i], uv) * blends[i];
    }
    return float4(normalize(expand(normal.xyz)), normal.w);
}

float3 getPointLightLighting(float3 lightDir, float radius, float3 lightColor, float3 eyeDir, float3 normal, float3 albedo, float roughness = 1.0, float metallicness = 1.0) {
    float3 pointlightColor = lightColor * TESR_TerrainData.z;

    if (TESR_TerrainExtraData.x){
        // PBR.
        float att = vanillaAtt(lightDir, radius);
        
        float3 lighting = PBR(saturate(metallicness * TESR_TerrainData.x), roughness, albedo, normal, eyeDir, lightDir, pointlightColor);
        
        return max(0, lighting * att);
    } else {
        // Vanilla.
        float att = vanillaAtt(lightDir, radius);
        
        lightDir = normalize(lightDir);
        
        float3 lighting = LambertianDiffuse(albedo, (0).xxx) * shades(normal, lightDir) * pointlightColor * PI;
        
        return max(0, lighting * att);
    }
}

float3 getSunLighting(float3 lightDir, float3 sunColor, float3 eyeDir, float3 normal, float3 albedo, float roughness = 1.0, float metallicness = 1.0, float parallaxMultiplier = 1.0) {
    float3 color = albedo;
    color = lerp(luma(albedo), color, TESR_TerrainExtraData.y);
    float3 lightColor = sunColor * TESR_TerrainData.z * parallaxMultiplier;

    if (TESR_TerrainExtraData.x) {
        // PBR.
        return PBRSun(saturate(TESR_TerrainData.x), roughness, color, normal, eyeDir, lightDir, lightColor);
    } else {
        // Vanilla, no specular.
        return LambertianDiffuse(color, (0).xxx) * shades(normal, lightDir) * lightColor * PI;
    }
}

float3 getAmbientLighting(float3 ambient, float3 albedo) {
    return ambient * TESR_TerrainData.w * albedo;
}

float3 getAmbientLighting(float3 ambient, float3 sunLuminanceInLux, float3 albedo) {
    return (sunLuminanceInLux / 50.0) * TESR_TerrainData.w * albedo;
    //return float3(0.0f, 0.0f, 0.0f);
}
