// PBR calculation.
#if defined(__INTELLISENSE__)
    #include "Helpers.hlsl"
#endif

// Fresnel
// Schlick approximation
float3 Fresnel(float3 f0, float3 f90, float cosine) {
    return f0 + (f90 - f0) * pow(1 - cosine, 5.f);
}

// Diffuse
// Lambert
float3 LambertianDiffuse(float3 albedo, float3 fresnel) {
    return (1 - fresnel) * albedo / PI;
}

float3 DisneyDiffuse(float3 albedo, float roughness, float NdotV, float NdotL, float LdotH) {
    const float linearRoughness = roughness * roughness;
    
    const float energyBias = lerp (0, 0.5 , linearRoughness);
    const float energyFactor = lerp (1.0, 1.0 / 1.51, linearRoughness);
    const float fd90 = energyBias + 2.0 * LdotH * LdotH * linearRoughness;
    const float3 f0 = float(1.0).xxx;
    const float lightScatter = Fresnel(f0, fd90, NdotL).r;
    const float viewScatter = Fresnel(f0, fd90, NdotV).r;

    return (albedo / PI) * lightScatter * viewScatter * energyFactor;
}

// Specular
// D (normal distribution function)
float GGX(float NdotH, float roughness) {
    float alpha = roughness * roughness;
    float a2 = pow(roughness, 4);
    float d = max((NdotH * a2 - NdotH) * NdotH + 1, 1e-5);
    return a2 / (PI * d * d);
}

// G1
float ShlickBeckmann(float NdotX, float roughness) {
    float k = pow(roughness + 1, 2) / 8.0;
    return NdotX/max(NdotX * (1 - k) + k, 0.00000001);
}

// Smith
float GeometryShadowing(float roughness, float NdotV, float NdotL) {
    return ShlickBeckmann(NdotV, roughness) * ShlickBeckmann(NdotL, roughness);
}

// F
float3 FresnelShlick(float3 reflectance, float3 halfway, float3 eyeDir) {
    return reflectance + (1 - reflectance) * pow(1 - shades(halfway, eyeDir), 5.0);
}

// BRDF
float3 BRDF(float roughness, float3 fresnel, float NdotV, float NdotL, float NdotH){
    float3 num = GGX(NdotH, roughness) * GeometryShadowing(roughness, NdotV, NdotL) * fresnel;
    float denom = 4.0 * NdotV * NdotL;
    return num/denom;
}

float3 PBR(float metallicness, float roughness, float3 albedo, float3 normal, float3 eyeDir, float3 lightDir, float3 lightColor) {
    const float3 reflectance = lerp(float(0.04).rrr, albedo, metallicness);
    
    normal = normalize(normal);
    eyeDir = normalize(eyeDir);
    lightDir = normalize(lightDir);
    
    const float3 halfway = normalize(eyeDir + lightDir);
    const float NdotL = max(shades(normal, lightDir), 0.00001);
    const float NdotV = max(shades(normal, eyeDir), 0.00001);
    const float NdotH = shades(normal, halfway);
    const float LdotH = shades(lightDir, halfway);

    const float3 fresnel = Fresnel(reflectance, (1.0).xxx, LdotH);
    
    float3 diffuse = (1 - metallicness) * LambertianDiffuse(albedo, fresnel);
    
    const float3 spec = BRDF(roughness, fresnel, NdotV, NdotL, NdotH);

    return (diffuse + spec) * NdotL * lightColor * PI;
}

#define SUN_RADIUS 0.00918043

float3 PBRSun(float metallicness, float roughness, float3 albedo, float3 normal, float3 eyeDir, float3 lightDir, float3 lightColor) {
    const float3 reflectance = lerp(float(0.04).rrr, albedo, metallicness);
    
    normal = normalize(normal);
    eyeDir = normalize(eyeDir);
    lightDir = normalize(lightDir);
    
    const float3 reflectDir = reflect(lightDir, normal);

    const float radius = sin(SUN_RADIUS);
    const float dist = cos(SUN_RADIUS);
    
    const float3 LdotR = dot(lightDir, reflectDir);
    const float3 closestPoint = reflectDir - LdotR * lightDir;
    const float3 sunDir = LdotR < dist ? normalize(dist * lightDir + normalize(closestPoint) * radius) : reflectDir;
    
    const float3 halfway = normalize(eyeDir + sunDir);
    const float NdotS = max(shades(normal, sunDir), 0.00001);
    const float NdotV = max(shades(normal, eyeDir), 0.00001);
    const float NdotH = shades(normal, halfway);
    const float NdotL = shades(normal, lightDir);

    const float3 fresnel = FresnelShlick(reflectance, halfway, eyeDir);
    const float3 diffuse = (1 - metallicness) * LambertianDiffuse(albedo, fresnel);
    
    const float3 spec = BRDF(roughness, fresnel, NdotV, NdotS, NdotH);

    return (diffuse * NdotL + spec * NdotS) * lightColor * PI;
}
