// Exposure shader for Oblivion/Skyrim Reloaded
#define debug 0

float4 TESR_ExposureData; // x:min brightness, y;max brightness, z:dark adapt speed, w: light adapt speed
float4 TESR_DebugVar; // x:min brightness, y;max brightness, z:dark adapt speed, w: light adapt speed
//float4 TESR_FrameTime; // x:min brightness, y;max brightness, z:dark adapt speed, w: light adapt speed

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; SRGBTEXTURE = FALSE; };
sampler2D TESR_AvgLumaBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; SRGBTEXTURE = FALSE; };

#include "Includes/Helpers.hlsl"

float computeEV100(float aperture, float shutterTime, float ISO) {
	// EV number is defined as:
	// 2^ EV_s = N^2 / t and EV_s = EV_100 + log2 (S /100)
	// This gives
	// EV_s = log2 (N^2 / t)
	// EV_100 + log2 (S /100) = log2 (N^2 / t)
	// EV_100 = log2 (N^2 / t) - log2 (S /100)
	// EV_100 = log2 (N^2 / t . 100 / S)
	return log2(sqr(aperture) / shutterTime * 100 / ISO );
}

float computeEV100FromAvgLuminance(float avgLuminance) {
	// We later use the middle gray at 12.7% in order to have
	// a middle gray at 18% with a sqrt (2) room for specular highlights
	// But here we deal with the spot meter measuring the middle gray
	// which is fixed at 12.5 for matching standard camera
	// constructor settings (i.e. calibration constant K = 12.5)
	// Reference : http :// en. wikipedia . org / wiki / Film_speed
	return log2(avgLuminance * (100.0f / (12.5f * TESR_ExposureData.x)));
}

float convertEV100toExposure(float EV100) {
	// Compute the maximum luminance possible with H_sbs sensitivity
	// maxLum = 78 / ( S * q ) * N^2 / t
	// = 78 / ( S * q ) * 2^ EV_100
	// = 78 / (100 * 0.65) * 2^ EV_100
	// = 1.2 * 2^ EV
	// Reference : http :// en. wikipedia . org / wiki / Film_speed
	float maxLuminance = 1.2f * TESR_ExposureData.y * pows(2.0f, EV100);
	return 1.0f / maxLuminance;
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

float4 Exposure(VSOUT IN) : COLOR0
{
	bool useAutoExposure = true;
	float aperture = 16.0/5.6;
	float shutterTime = 100.0 * TESR_ExposureData.x;
	float ISO = 100.0 * TESR_ExposureData.y;
	
	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
	float Lavg = tex2D(TESR_AvgLumaBuffer, float2(0.5, 0.5)).g;

	float EV100 = computeEV100(aperture, shutterTime, ISO);

	float AutoEV100 = computeEV100FromAvgLuminance(Lavg);

	float currentEV = useAutoExposure ? AutoEV100 : EV100;
	float exposure = convertEV100toExposure(currentEV);

	//float exposedLuminance = luminance * exposure;

	return float4(color.rgb * exposure, 1.0);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader  = compile ps_3_0 Exposure();
	}
}