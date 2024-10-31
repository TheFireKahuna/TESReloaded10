// Exposure shader for Oblivion/Skyrim Reloaded
#define debug 0

float4 TESR_ExposureData; // x:min brightness, y;max brightness, z:dark adapt speed, w: light adapt speed
float4 TESR_DebugVar; // x:min brightness, y;max brightness, z:dark adapt speed, w: light adapt speed
//float4 TESR_FrameTime; // x:min brightness, y;max brightness, z:dark adapt speed, w: light adapt speed

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; SRGBTEXTURE = FALSE; };
sampler2D TESR_AvgLumaBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; SRGBTEXTURE = FALSE; };

#include "Includes/Helpers.hlsl"

#define MIN_ISO 100.0
#define MAX_ISO 6400.0
#define MIN_SHUTTER (1.0/4000.0)
#define MAX_SHUTTER (1.0/30.0)
#define MIN_APERTURE 1.8
#define MAX_APERTURE 22.0

// References:
// http://en.wikipedia.org/wiki/Film_speed
// http://en.wikipedia.org/wiki/Exposure_value
// http://en.wikipedia.org/wiki/Light_meter
 
// Notes:
// EV below refers to EV at ISO 100
 
// Given an aperture, shutter speed, and exposure value compute the required ISO value
float ComputeISO(float aperture, float shutterSpeed, float ev)
{
    return (sqr(aperture) * 100.0f) / (shutterSpeed * pows(2.0f, ev));
}
 
// Given the camera settings compute the current exposure value
float ComputeEV(float aperture, float shutterSpeed, float iso)
{
    return log2((sqr(aperture) * 100.0f) / (shutterSpeed * iso));
}
 
// Using the light metering equation compute the target exposure value
float ComputeTargetEV(float averageLuminance)
{
    // K is a light meter calibration constant
    static const float K = 12.5f;
    return log2(averageLuminance * 100.0f / K);
}
 
float3 ApplyAperturePriority(float focalLength,
                           float targetEV)
{
	float aperture, shutterSpeed, iso;
    // Start with the assumption that we want a shutter speed of 1/f
    shutterSpeed = 1.0f / (focalLength * 1000.0f);
 
    // Compute the resulting ISO if we left the shutter speed here
    iso = clamp(ComputeISO(aperture, shutterSpeed, targetEV), MIN_ISO, MAX_ISO);
 
    // Figure out how far we were from the target exposure value
    float evDiff = targetEV - ComputeEV(aperture, shutterSpeed, iso);
 
    // Compute the final shutter speed
    shutterSpeed = clamp(shutterSpeed * pows(2.0f, -evDiff), MIN_SHUTTER, MAX_SHUTTER);

	return float3(aperture, shutterSpeed, iso);
}
 
float3 ApplyShutterPriority(float focalLength,
                          float targetEV)
{
	float aperture, shutterSpeed, iso;
    // Start with the assumption that we want an aperture of 4.0
    aperture = 4.0f;
 
    // Compute the resulting ISO if we left the aperture here
    iso = clamp(ComputeISO(aperture, shutterSpeed, targetEV), MIN_ISO, MAX_ISO);
 
    // Figure out how far we were from the target exposure value
    float evDiff = targetEV - ComputeEV(aperture, shutterSpeed, iso);
 
    // Compute the final aperture
    aperture = clamp(aperture * pows(sqrt(2.0f), evDiff), MIN_APERTURE, MIN_APERTURE);
	
	return float3(aperture, shutterSpeed, iso);
}
 
float3 ApplyProgramAuto(float focalLength,
                      float targetEV)
{
	float aperture, shutterSpeed, iso;
    // Start with the assumption that we want an aperture of 4.0
    aperture = 4.0f * TESR_ExposureData.z;
 
    // Start with the assumption that we want a shutter speed of 1/f
    shutterSpeed = 1.0f / (focalLength * 1000.0f * TESR_ExposureData.w);
 
    // Compute the resulting ISO if we left both shutter and aperture here
    iso = clamp(ComputeISO(aperture, shutterSpeed, targetEV), MIN_ISO, MAX_ISO);
 
    // Apply half the difference in EV to the aperture
    float evDiff = targetEV - ComputeEV(aperture, shutterSpeed, iso);
    aperture = clamp(aperture * pows(sqrt(2.0f), evDiff * 0.5f), MIN_APERTURE, MIN_APERTURE);
 
    // Apply the remaining difference to the shutter speed
    evDiff = targetEV - ComputeEV(aperture, shutterSpeed, iso);
    shutterSpeed = clamp(shutterSpeed * pows(2.0f, -evDiff), MIN_SHUTTER, MAX_SHUTTER);
	
	return float3(aperture, shutterSpeed, iso);
}

struct VSOUT
{
	float4 vertPos : POSITION;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN
{
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

VSOUT FrameVS(VSIN IN)
{
	VSOUT OUT = (VSOUT)0.0f;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}

float convertEV100ToExposure(float EV100)
{
    // Compute the maximum luminance possible with H_sbs sensitivity
    // maxLum = 78 / ( S * q ) * N^2 / t
    // = 78 / ( S * q ) * 2^EV_100
    // = 78 / (100 * 0.65) * 2^EV_100
    // = 1.2 * 2^EV
    // Reference: http://en.wikipedia.org/wiki/Film_speed
    return 1.0f / (1.2f * pow(2.0f, EV100));
}

float4 Exposure(VSOUT IN) : COLOR0
{
	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
	float Lavg = tex2D(TESR_AvgLumaBuffer, float2(0.5, 0.5)).r;

	float targetEV = ComputeTargetEV(Lavg);
	
	//float EV100 = computeEV100(aperture, shutterTime, ISO); // frostbite calc
	// https://placeholderart.wordpress.com/2014/11/21/implementing-a-physically-based-camera-manual-exposure/
	//float3 ASI = ApplyProgramAuto(35.0 * TESR_ExposureData.y, targetEV);

	//float EV = ComputeEV(ASI.x, ASI.y, ASI.z);

	return float4(color.rgb * convertEV100ToExposure(targetEV), 1.0);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader  = compile ps_3_0 Exposure();
	}
}