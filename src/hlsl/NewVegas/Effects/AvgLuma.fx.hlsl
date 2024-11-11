// AvgLuma for Oblivion Reloaded

float4 TESR_MotionBlurData;
float4 TESR_DepthOfFieldData;
float4 TESR_GameTime;
float4 TESR_ExposureData; // x:min brightness, y;max brightness, z:dark adapt speed, w: light adapt speed
float4 TESR_ReciprocalResolution;
float4 TESR_HistogramBufferData;
float4 TESR_HistogramTexelData;
float4 TESR_HistogramTexelExtraData;
float4 TESR_DebugVar;

sampler2D TESR_SourceBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_RenderedBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

sampler2D TESR_HistogramSampleBufferY : register(s3) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_HistogramSampleBufferXY : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_HistogramBinBufferY : register(s5) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_HistogramBinBufferXY : register(s6) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_HistogramBuffer : register(s7) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_AvgLumaBuffer : register(s8) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_HistogramLumaBuffer : register(s9) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_HistogramSubsampleBufferY : register(s10) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_HistogramSubsampleBufferXY : register(s11) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

static const float SampleSize = 2; // TESR_HistogramBufferData.x;
static const float SubsampleSize = 4; // TESR_HistogramBufferData.x;
static const float BinSize = 8; // TESR_HistogramBufferData.y / (SampleSize * SubsampleSize

static const float decreaseRate = -TESR_ExposureData.z; // max value for adaptation speed towards darker screens
// static const float decreaseRate = -TESR_ExposureData.z * 0.001; // max value for adaptation speed towards darker screens
static const float increaseRate = TESR_ExposureData.w; // max value for adaptation speed towards brighter screens
// static const float increaseRate = TESR_ExposureData.w * 0.001; // max value for adaptation speed towards brighter screens
static const float2 center = float2(0.5, 0.5); // this shader is to be applied on a 1x1 texture so we sample at the center

static const float2 Histogram_Position = 0.0f;
static const float2 Histogram_Size = 0.25f;
static const float Histogram_ScaleY = 25.0f;
static const float Histogram_BackgroundBlend = 0.5f;
static const float Histogram_Antialias = 2.0f;

static const bool Histogram_View = true;
static const bool Histogram_Mix = false;

#define MINLOG -16
#define MAXLOG 16
#define LOGRANGE 32
#define ONEOVERLOGRANGE 0.03125
// 1.0 / ((abs(-16) + 16))


#include "Includes/Helpers.hlsl"
#include "Includes/Depth.hlsl"

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

static float2 taps[12] =
{
    float2(-0.326212, -0.405810),
    float2(-0.840144, -0.073580),
    float2(-0.695914,  0.457137),
    float2(-0.203345,  0.620716),
    float2( 0.962340, -0.194983),
    float2( 0.473434, -0.480026),
    float2( 0.519456,  0.767022),
    float2( 0.185461, -0.893124),
    float2( 0.507431,  0.064425),
    float2( 0.896420,  0.412458),
    float2(-0.321940, -0.932615),
    float2(-0.791559, -0.597710)
};

// returns a value from start to end within a min and max step from start
float stepTo(float startValue, float endValue, float minStep, float maxStep){
	// make sure min is negative and max is positive
	minStep = abs(minStep) * -1;
	maxStep = abs(maxStep);

	float diff = clamp(endValue - startValue, minStep, maxStep);
	return startValue + diff;
}

static const float HyperFocalDistance = max(0.0000001, TESR_DepthOfFieldData.x * 1000); // the distance at which DOF is greatest (infinite towards the distance and closest to the camera)

// calculates the current focal distance for depth of field shader and animates the value.
float getFocalDistance() {
	float oldFocus = tex2D(TESR_AvgLumaBuffer, center).b;

	// gets the autofocus depth based on a local average, scaled with player movement speed (to avoid flicker during fast movement)
	float2 speed = saturate(abs(float2(TESR_MotionBlurData.x, TESR_MotionBlurData.y))) + 0.1;

	float centerDepth = readDepth(center);
	float depth = centerDepth;
	float2 samplingRange = speed;
	[fastopt][loop]
	for (int i=0; i<12; i++){
		depth += readDepth(center + 0.3 * taps[i] * samplingRange);
	}
	depth /= 12;

	depth = centerDepth > depth ? centerDepth:depth; //if pixel at the center is farther than the average, we use that value (fix for some iron sights)
	depth = saturate(depth/HyperFocalDistance); // remaps to [0-1] to save as color value in the texture

	float step = pow(max(oldFocus, depth), 0.1); // because of the nature of depth, we scale the speed with distance
	return stepTo(oldFocus, depth, 3 * step * TESR_GameTime.w, 1 * step * TESR_GameTime.w);
}


float4 Histogram_GetLuma(VSOUT IN): COLOR0 {
	float lum = luma(tex2D(TESR_SourceBuffer, IN.UVCoord).rgb);
	// Avoid taking the log of zero
	[branch]
	if (lum < 0.005) {
		return float4(0.0, 1.0, 0.0, 0.0);
	}
	else {
		return float4(clamp((log2(lum) - MINLOG) * ONEOVERLOGRANGE, 0.0, 1.0), 0.0, 0.0, 0.0);
	}
}


float4 Histogram_SamplePixelsX(VSOUT IN): COLOR0 {
	float sampleUV =  TESR_ReciprocalResolution.y;
	float4 luma = 0;
	float4 lumaSum = 0;
	float2 coords = IN.UVCoord;
	float highestVal = 0;

	[unroll]
	for(float i = 0; i < SampleSize; i += 1) {
		coords.y = IN.UVCoord.y + (i * sampleUV);
		luma = tex2D(TESR_HistogramLumaBuffer, coords);
		highestVal = max(luma.x, highestVal);
		lumaSum += luma;
	}
	lumaSum.z = highestVal;
	lumaSum.w = 1.0;
    return lumaSum;
}
float4 Histogram_SamplePixelsY(VSOUT IN): COLOR0 {
	float sampleUV =  TESR_ReciprocalResolution.x;
	float4 luma = 0;
	float4 lumaSum = 0;
	float2 coords = IN.UVCoord;
	float highestVal = 0;

	[unroll]
	for(float i = 0; i < SampleSize; i += 1) {
		coords.x = IN.UVCoord.x + (i * sampleUV);
		luma = tex2D(TESR_HistogramSampleBufferY, coords);
		highestVal = max(luma.z, highestVal);
		lumaSum += luma;
	}
	lumaSum.z = highestVal;
	lumaSum.w = 1.0;
    return lumaSum;
}


float4 Histogram_SubsamplePixelsX(VSOUT IN): COLOR0 {
	float sampleUV =  TESR_ReciprocalResolution.y;
	float4 luma = 0;
	float4 lumaSum = 0;
	float2 coords = IN.UVCoord;
	float highestVal = 0;

	[unroll]
	for(float i = 0; i < SubsampleSize; i += 1) {
		coords.y = IN.UVCoord.y + (i * sampleUV);
		luma = tex2D(TESR_HistogramSampleBufferXY, coords);
		highestVal = max(luma.x, highestVal);
		lumaSum += luma;
	}
	lumaSum.z = highestVal;
	lumaSum.w = 1.0;
    return lumaSum;
}
float4 Histogram_SubsamplePixelsY(VSOUT IN): COLOR0 {
	float sampleUV =  TESR_ReciprocalResolution.x;
	float4 luma = 0;
	float4 lumaSum = 0;
	float2 coords = IN.UVCoord;
	float highestVal = 0;

	[unroll]
	for(float i = 0; i < SubsampleSize; i += 1) {
		coords.x = IN.UVCoord.x + (i * sampleUV);
		luma = tex2D(TESR_HistogramSubsampleBufferY, coords);
		highestVal = max(luma.z, highestVal);
		lumaSum += luma;
	}
	lumaSum.z = highestVal;
	lumaSum.w = 1.0;
    return lumaSum;
}


float4 Histogram_SampleBinsX(VSOUT IN): COLOR0 {
	float binUV =  TESR_HistogramTexelData.x;
	float4 luma = 0;
	float4 lumaSum = 0;
	float2 coords = IN.UVCoord;
	float highestVal = 0;
	[unroll]
	for(float i = 0; i < BinSize; i += 1) {
		coords.y = IN.UVCoord.y + (i * binUV);
		luma = tex2D(TESR_HistogramSubsampleBufferXY, coords);
		highestVal = max(luma.z, highestVal);
		lumaSum += luma;
	}
	lumaSum.z = highestVal;
	lumaSum.w = 1.0;
    return lumaSum;
}


float4 Histogram_SampleBinsY(VSOUT IN): COLOR0 {
	float binUV =  TESR_HistogramTexelData.y;
	float4 luma = 0;
	float4 lumaSum = 0;
	float2 coords = IN.UVCoord;
	float highestVal = 0;
	[unroll]
	for(float i = 0; i < BinSize; i += 1) {
		coords.x = IN.UVCoord.x + (i * binUV);
		luma = tex2D(TESR_HistogramBinBufferY, coords);
		highestVal = max(luma.z, highestVal);
		lumaSum += luma;
	}
	lumaSum.z = highestVal;
	lumaSum.w = 1.0;
    return lumaSum;
}


float4 Histogram_1D(VSOUT IN): COLOR0 {
	float binCount = TESR_HistogramBufferData.z;
	float binUV =  TESR_HistogramTexelData.z;
	float4 luma = 0;
	float4 lumaSum = 0;
	float2 coords = IN.UVCoord;
	float highestVal = 0;
	[loop]
	for(float i = 0; i < binCount; i += 1) {
		coords.y = IN.UVCoord.y + (i * binUV);
		luma = tex2Dgrad(TESR_HistogramBinBufferXY, coords, 0, 0);
		highestVal = max(luma.z, highestVal);
		lumaSum += luma;
	}
	lumaSum.z = highestVal;
	lumaSum.w = 1.0;
    return lumaSum;
}

float4 Histogram_Display(VSOUT IN): COLOR0 {
	/*float maxLuma = tex2D(TESR_AvgLumaBuffer, center).g;
	float3 lumaSum = 0.1;
	if (TESR_DebugVar.x > 0.0) {
		if (TESR_DebugVar.x > 0.3) {
			lumaSum = tex2D(TESR_AvgLumaBuffer, IN.UVCoord).rgb;
			if (TESR_DebugVar.z > 0.0) {
				float sampleCount = (1.0 / TESR_ReciprocalResolution.x) * (1.0 / TESR_ReciprocalResolution.y);
				sampleCount -= lumaSum.g;
				lumaSum.r /= sampleCount;
			}
		}
		else if (TESR_DebugVar.x > 0.2) {
			lumaSum = tex2D(TESR_HistogramBinBufferXY, IN.UVCoord).rgb;
			if (TESR_DebugVar.z > 0.0) {
				float sampleCount = 256.0;
				if (TESR_DebugVar.w > 0.0) {
					sampleCount -= lumaSum.g;
				}
				lumaSum.r /= sampleCount;
				if (TESR_DebugVar.w > 0.0) {
					lumaSum.r /= lumaSum.b;
				}
			}
		}
		else if  (TESR_DebugVar.x > 0.1) {
			lumaSum = tex2D(TESR_HistogramSampleBufferXY, IN.UVCoord).rgb;
			if (TESR_DebugVar.z > 0.0) {
				float sampleCount = 16.0;
				sampleCount -= lumaSum.g;
				lumaSum.r /= sampleCount;
				if (TESR_DebugVar.w > 0.0) {
					lumaSum.r /= lumaSum.b;
				}
			}
		}
		else  {
			lumaSum = tex2D(TESR_HistogramLumaBuffer, IN.UVCoord).rgb;
			if (TESR_DebugVar.w > 0.0) {
				lumaSum.r /= maxLuma;
			}
		}
		lumaSum = lumaSum.rrr;
	}
	else {
		lumaSum = tex2D(TESR_SourceBuffer, IN.UVCoord).rgb;
	}
	if (TESR_DebugVar.z > 0.1) {
		lumaSum = exp2(lumaSum);
	}

	return float4(lumaSum.r, lumaSum.g, lumaSum.b, 1.0);*/
	
	return tex2D(TESR_SourceBuffer, IN.UVCoord);
}

float4 AvgLuma(VSOUT IN): COLOR0 {
	float binCount = TESR_HistogramBufferData.w;
	float binUV =  TESR_HistogramTexelData.w;
	float4 luma = 0;
	float4 lumaSum = 0;
	float2 coords = center;
	float highestVal = 0;

	[loop]
	for(float i = 0; i < binCount; i += 1) {
		coords.x = IN.UVCoord.x * (i * binUV);
		luma = tex2Dgrad(TESR_HistogramBuffer, coords, 0, 0);
		highestVal = max(luma.z, highestVal);
		lumaSum += luma;
	}
	float sampleCount = (1.0 / TESR_ReciprocalResolution.x) * (1.0 / TESR_ReciprocalResolution.y);
	float weightedLogAverage = lumaSum.r / (sampleCount - lumaSum.g);
	
	float weightedAverageLuminance = exp2(((weightedLogAverage * LOGRANGE) + MINLOG));
	float luminanceLastFrame = tex2D(TESR_AvgLumaBuffer, center).r;
	float adaptedLuminance = luminanceLastFrame + (weightedAverageLuminance - luminanceLastFrame) * (1 - exp(-TESR_GameTime.w * 3.0 * TESR_ExposureData.z));

	// gradually change average luma
	//float animatedLuma = stepTo(oldLuma.g, newLuma, TESR_GameTime.w * decreaseRate, TESR_GameTime.w * increaseRate);

	// texture will store the actual current luma, the animated current luma, and the animated focal distance for DoF.
	return float4(adaptedLuminance, highestVal, getFocalDistance(), 1.0);
}
 
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Histogram_GetLuma();
	}
}
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Histogram_SamplePixelsX();
	}
}
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Histogram_SamplePixelsY();
	}
}
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Histogram_SubsamplePixelsX();
	}
}
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Histogram_SubsamplePixelsY();
	}
}
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Histogram_SampleBinsX();
	}
}
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Histogram_SampleBinsY();
	}
}
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Histogram_1D();
	}
}
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 AvgLuma();
	}
}
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Histogram_Display();
	}
}
