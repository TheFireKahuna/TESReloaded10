// PBR calculation.
#if defined(__INTELLISENSE__)
    #include "Helpers.hlsl"
#endif

#define SUN_RADIUS 0.00918043
#define invPI 1.0f / 3.14159265f

float3 LambertianDiffuse(float3 albedo, float3 fresnel) {
    return (1 - fresnel) * albedo / PI;
}

float V_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG) {
    // Original formulation of G_SmithGGX Correlated
    // lambda_v = (-1 + sqrt(alphaG2 * (1-NdotL2) / NdotL2 + 1)) * 0.5f;
    // lambda_l = (-1 + sqrt(alphaG2 * (1-NdotV2) / NdotV2 + 1)) * 0.5f;
    // G_SmithGGXCorrelated = 1 / (1 + lambda_v + lambda_l);
    // V_SmithGGXCorrelated = G_SmithGGXCorrelated / (4.0f * NdotL * NdotV);
    // This is the optimize version
    float alphaG2 = alphaG * alphaG;
    // Caution: the "NdotL " and "NdotV" are explicitely inversed, this is not a mistake.
    float Lambda_GGXV = NdotL * sqrt((-NdotV * alphaG2 + NdotV) * NdotV + alphaG2);
    float Lambda_GGXL = NdotV * sqrt((-NdotL * alphaG2 + NdotL) * NdotL + alphaG2);
    return 0.5f / (Lambda_GGXV + Lambda_GGXL);
}
float3 F_Schlick(in float3 f0, in float f90, in float u)
{
    return f0 + (f90- f0) * pow(1.f- u, 5.f);
}
// Specular
// D (N distribution function)
float D_GGX(float NdotH, float m) {
    float m2 = m * m;
    float f = (NdotH * m2 - NdotH) * NdotH + 1;
    return m2 / (f * f);
}
float3 SpecularBRDF(float3 f0, float f90, float3 L, float3 N, float3 V, float NdotV, float roughness) {
    float3 H = normalize(V + L);
    float LdotH = saturate(dot(L, H));
    float NdotH = saturate(dot(N, H));
    float NdotL = saturate(dot(N, L));

    float3 F = F_Schlick(f0, f90, LdotH);
    float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
    float D = D_GGX(NdotH, roughness);

    // Specular Element
    return (D * F * Vis) / PI; // Fr
}
float DiffuseBRDF(float3 L, float3 N, float3 V, float NdotV, float roughness) {
    float linearRoughness = roughness * roughness;

    float3 H = normalize(V + L);
    float LdotH = saturate(dot(L, H));
    float NdotL = saturate(dot(N, L));

    float energyBias = lerp(0, 0.5, linearRoughness);
    float energyFactor = lerp(1.0, 1.0 / 1.51, linearRoughness);
    float f90 = energyBias + 2.0 * LdotH * LdotH * linearRoughness;
    float3 f0 = float3(1.0f, 1.0f, 1.0f);
    float lightScatter = F_Schlick(f0, f90, NdotL).r;
    float viewScatter = F_Schlick(f0, f90, NdotV).r;

    // Diffuse element
    return (lightScatter * viewScatter * energyFactor) / PI; // Fd
}
float3 SpecularBRDFOpt(float3 f0, float f90, float NdotV, float LdotH, float NdotL, float NdotH, float roughness) {
    float3 F = F_Schlick(f0, f90, LdotH);
    float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
    float D = D_GGX(NdotH, roughness);

    // Specular Element
    return (D * F * Vis) / PI; // Fr
}
float DiffuseBRDFOpt(float NdotV, float LdotH, float NdotL, float roughness) {
    float linearRoughness = roughness * roughness;

    float energyBias = lerp(0, 0.5, linearRoughness);
    float energyFactor = lerp(1.0, 1.0 / 1.51, linearRoughness);
    float f90 = energyBias + 2.0 * LdotH * LdotH * linearRoughness;
    float3 f0 = float3(1.0f, 1.0f, 1.0f);
    float lightScatter = F_Schlick(f0, f90, NdotL).r;
    float viewScatter = F_Schlick(f0, f90, NdotV).r;

    // Diffuse element
    return (lightScatter * viewScatter * energyFactor) / PI; // Fd
}



float3 PBR(float metallicness, float roughness, float3 albedo, float3 N, float3 V, float3 L, float3 lightColor) {
	roughness = clamp(roughness, 0.0001f, 0.999f);
    float3 f0 = lerp(float(0.04).rrr, albedo, metallicness); // reflectance
    float f90 = 1.0;
	float3 diffuseColor = invPI * (albedo - albedo * metallicness);
	float3 specularColor = f0;
    //float f90 = clamp(dot(f0, 50.0 * 0.33), 0.0, 1.0);
    
    N = normalize(N); // normal vector
    V = normalize(V); // view/eye vector
    L = normalize(L); // lightDirection
    
    float NdotV = abs(dot(N, V)) + 1e-5f; // avoid artifacts
    float3 H = normalize(V + L);
    float LdotH = saturate(dot(L, H));
    float NdotH = saturate(dot(N, H));
    float NdotL = saturate(dot(N, L));

    //return (SpecularBRDFOpt(f0, f90, NdotV, LdotH, NdotL, NdotH, roughness) * specularColor) * NdotL * lightColor * PI;
    //return (DiffuseBRDFOpt(NdotV, LdotH, NdotL, roughness) * diffuseColor) * NdotL * lightColor * PI;
    return (DiffuseBRDFOpt(NdotV, LdotH, NdotL, roughness) * diffuseColor + SpecularBRDFOpt(f0, f90, NdotV, LdotH, NdotL, NdotH, roughness) * specularColor) * NdotL * lightColor * PI;
}
float3 PBRSpecular(float metallicness, float roughness, float3 albedo, float3 N, float3 V, float3 L, float3 lightColor) {
	roughness = clamp(roughness, 0.0001f, 0.999f);
    float3 f0 = lerp(float(0.04).rrr, albedo, metallicness); // reflectance
    float f90 = 1.0;
	float3 specularColor = f0;
    //float f90 = clamp(dot(f0, 50.0 * 0.33), 0.0, 1.0);
    
    N = normalize(N); // normal vector
    V = normalize(V); // view/eye vector
    L = normalize(L); // lightDirection
    
    float NdotV = abs(dot(N, V)) + 1e-5f; // avoid artifacts
    float3 H = normalize(V + L);
    float LdotH = saturate(dot(L, H));
    float NdotH = saturate(dot(N, H));
    float NdotL = saturate(dot(N, L));

    //return float3(0.0, 0.0, 0.0);
    return (SpecularBRDFOpt(f0, f90, NdotV, LdotH, NdotL, NdotH, roughness) * specularColor) * NdotL * lightColor * PI;
}
float3 PBRDiffuse(float metallicness, float roughness, float3 albedo, float3 N, float3 V, float3 L, float3 lightColor) {
	roughness = clamp(roughness, 0.0001f, 0.999f);
	float3 diffuseColor = invPI * (albedo - albedo * metallicness);
    N = normalize(N); // normal vector
    V = normalize(V); // view/eye vector
    L = normalize(L); // lightDirection
    
    float NdotV = abs(dot(N, V)) + 1e-5f; // avoid artifacts
    float3 H = normalize(V + L);
    float LdotH = saturate(dot(L, H));
    float NdotL = saturate(dot(N, L));

    //return float3(0.0f, 0.0f, 0.0f);
    return (DiffuseBRDFOpt(NdotV, LdotH, NdotL, roughness) * diffuseColor) * NdotL * lightColor * PI;
}
float3 PBRLight(float metallicness, float roughness, float3 albedo, float3 N, float3 V, float3 L, float3 lightColor) {
    N = normalize(N); // normal vector
    L = normalize(L); // lightDirection
    
    float NdotL = saturate(dot(N, L));

    return NdotL * lightColor * PI;
}


float3 PBRSun(float metallicness, float roughness, float3 albedo, float3 N, float3 V, float3 D, float3 illuminanceInLux) {
	roughness = clamp(roughness, 0.0001f, 0.999f);
    float3 f0 = lerp(float(0.04).rrr, albedo, metallicness); // reflectance
    float f90 = 1.0;
	float3 diffuseColor = invPI * (albedo - albedo * metallicness);
	float3 specularColor = f0;
    //float f90 = clamp(dot(f0, 50.0 * 0.33), 0.0, 1.0);
    
    N = normalize(N); // normal vector
    V = normalize(V); // view/eye vector
    D = normalize(D); // Sun Direction, punctual
    
    float NdotV = abs(dot(N, V)) + 1e-5f; // avoid artifacts

    float3 R = reflect(D, N);

    float radius = sin(SUN_RADIUS);  // Disk radius
    float dist = cos(SUN_RADIUS); // Distance to disk

    // Closest point to a disk (since the radius is small, this is a good approximation
    float3 DdotR = dot(D, R);
    float3 S = R - DdotR * D; // closest point
    float3 L = DdotR < dist ? normalize(dist * D + normalize(S) * radius) : R;

    float3 illuminance = illuminanceInLux * saturate(dot(N, D));

    //return (DiffuseBRDF(D, N, V, NdotV, roughness) * diffuseColor) * illuminance;
    //return (SpecularBRDF(f0, f90, L, N, V, NdotV, roughness) * specularColor) * illuminance;
    return (DiffuseBRDF(D, N, V, NdotV, roughness) * diffuseColor + SpecularBRDF(f0, f90, L, N, V, NdotV, roughness) * specularColor) * illuminance;
}


float3 PBRSunLight(float metallicness, float roughness, float3 albedo, float3 N, float3 V, float3 D, float3 illuminanceInLux) {

    N = normalize(N); // normal vector
    D = normalize(D); // Sun Direction, punctual

    float3 illuminance = illuminanceInLux * saturate(dot(N, D));
    return illuminance;
}


float3 PBRSunDiffuse(float metallicness, float roughness, float3 albedo, float3 N, float3 V, float3 D, float3 illuminanceInLux) {
	roughness = clamp(roughness, 0.0001f, 0.999f);
    float3 f0 = lerp(float(0.04).rrr, albedo, metallicness); // reflectance
    float f90 = 1.0;
	float3 diffuseColor = invPI * (albedo - albedo * metallicness);
    //float f90 = clamp(dot(f0, 50.0 * 0.33), 0.0, 1.0);
    
    N = normalize(N); // normal vector
    V = normalize(V); // view/eye vector
    D = normalize(D); // Sun Direction, punctual
    
    float NdotV = abs(dot(N, V)) + 1e-5f; // avoid artifacts

    float3 illuminance = illuminanceInLux * saturate(dot(N, D));

    //return float3(0.0f, 0.0f, 0.0f);
    return (DiffuseBRDF(D, N, V, NdotV, roughness) * diffuseColor) * illuminance;
}


float3 PBRSunSpecular(float metallicness, float roughness, float3 albedo, float3 N, float3 V, float3 D, float3 illuminanceInLux) {
	roughness = clamp(roughness, 0.0001f, 0.999f);
    float3 f0 = lerp(float(0.04).rrr, albedo, metallicness); // reflectance
    float f90 = 1.0;
	float3 specularColor = f0;
   //float f90 = clamp(dot(f0, 50.0 * 0.33), 0.0, 1.0);
    
    N = normalize(N); // normal vector
    V = normalize(V); // view/eye vector
    D = normalize(D); // Sun Direction, punctual
    
    float NdotV = abs(dot(N, V)) + 1e-5f; // avoid artifacts

    float3 R = reflect(D, N);

    float radius = sin(SUN_RADIUS);  // Disk radius
    float dist = cos(SUN_RADIUS); // Distance to disk

    // Closest point to a disk (since the radius is small, this is a good approximation
    float3 DdotR = dot(D, R);
    float3 S = R - DdotR * D; // closest point
    float3 L = DdotR < dist ? normalize(dist * D + normalize(S) * radius) : R;

    float3 illuminance = illuminanceInLux * saturate(dot(N, D));

    //return float3(0.0, 0.0, 0.0);
    return (SpecularBRDF(f0, f90, L, N, V, NdotV, roughness) * specularColor) * illuminance;
}

float3 preExposeLighting(float3 lighting) {
    return lighting;
}