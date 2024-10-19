// Ambient Occlusion fullscreen shader for Oblivion/Skyrim Reloaded
float4 TESR_AmbientOcclusionAOData;
float4 TESR_AmbientOcclusionData;
float4 TESR_ReciprocalResolution;
float4 TESR_FogData; // x: fog start, y: fog end, z: sun glare, w: fog power
float4 TESR_FogColor;

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; SRGBTEXTURE = FALSE; };
sampler2D TESR_DepthBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; SRGBTEXTURE = FALSE; };
sampler2D TESR_SourceBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; SRGBTEXTURE = FALSE; };
sampler2D TESR_BlueNoiseSampler : register(s3) < string ResourceName = "Effects\bluenoise256.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; SRGBTEXTURE = FALSE; };
sampler2D TESR_NormalsBuffer : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; SRGBTEXTURE = FALSE; };

//static const float AOsamples = TESR_AmbientOcclusionAOData.x;
//static const float AOstrength = TESR_AmbientOcclusionAOData.y;
//static const float AOrange = TESR_AmbientOcclusionAOData.w;
//static const float AOangleBias = TESR_AmbientOcclusionData.x;

/*static const float AOclamp = TESR_AmbientOcclusionAOData.z;
static const float AOlumThreshold = TESR_AmbientOcclusionData.y;
static const float blurDrop = TESR_AmbientOcclusionData.z;
static const float blurRadius = TESR_AmbientOcclusionData.w;
static const int startFade = 2000;
static const int endFade = 8000;
static const float2 io = float2(1.0f, 0.0f);*/

static const int sampleCount = 8 + 1;
static const float sampleRadius = 4.0;
static const int sliceCount = 4;
static const float hitThickness = 0.2;
static const int BIT_COUNT = 31;
#define SECTOR_COUNT 32
#define halfres 0
static const int num_to_bits[16] = { 0, 1, 1, 2, 1, 2, 2, 3,
                        1, 2, 2, 3, 2, 3, 3, 4 };
 
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
 
#include "Includes/Depth.hlsl"
#include "Includes/BlurDepth.hlsl"
#include "Includes/Helpers.hlsl"
#include "Includes/Normals.hlsl"

/*int BIT_NOT(int x, int y) {
	int input = int2(x, y);
    int result = 0;
    int n = 1;
    int i = 0;
    while (i < 31)  {
        if (!any(input % 2)) {
            result += n;    
        }
        input /= 2;
        n = n * 2;
        i++;
    }
    return result;
}*/


int BIT_OR(int a, int b) {
    int result = 0;
    int n = 1;

	[loop]
    for(int i = 0; i < BIT_COUNT; i++) {
        if (((a % 2) == 1) || ((b % 2) == 1)) {
            result += n;
        }
        a = a / 2;
        b = b / 2;
        n = n * 2;
        if(!(a > 0 || b > 0)) {
            break;
        }
    }
    return result;
}

int BIT_LEFT(int x, int y) {
	return x * int(pow(2, y));
}

int BIT_RIGHT(int x, int y) {
	return x / int(pow(2, y));
}

int BIT_AND(int a, int b) {
    int result = 0;
    int n = 1;

	[loop]
    for(int i = 0; i < BIT_COUNT; i++) {
        if (((a % 2) == 1) && ((b % 2) == 1)) {
            result += n;
        }

        a = a / 2;
        b = b / 2;
        n = n * 2;

        if(!(a > 0 && b > 0)) {
            break;
        }
    }
    return result;
}

int BIT_FLIP(int x)
{
    return -x - 1;
}

int bitCount(int value)
{
    int count = 0;
    while (value != 0)
    {
        count++;
        value = BIT_AND(value, value - 1);
    }
    return count;
}

// https://cdrinmatane.github.io/posts/ssaovb-code/
int updateSectors(float minHorizon, float maxHorizon, int outBitfield) { 
    int startBit = int(minHorizon * float(SECTOR_COUNT));
	int horizonAngle = int((ceil((maxHorizon - minHorizon) * float(SECTOR_COUNT))));
    int angleBit = horizonAngle > 0 ? BIT_RIGHT(0xFFFFFFFF, (SECTOR_COUNT - horizonAngle)) : 0;
    int currentBitfield = BIT_LEFT(angleBit, startBit);
    return BIT_OR(outBitfield, currentBitfield);
}

// get indirect lighting and ambient occlusion 
float4 SSILVB(VSOUT IN, float2 pixelPos : VPOS) : COLOR0 {
	float2 uv = IN.UVCoord;
#if halfres
	clip ((uv < 0.5 && uv < 0.5)-1); // discard half the screen to render at half resolution
	uv *= 2;
#endif
    int indirect = 0;
    int globalOccludedBitfield = 0;

    float visibility = 0.0;
    float3 lighting = 0.0;
    float2 frontBackHorizon = 0.0;
    float3 position = reconstructPosition(uv);
    float3 camera = normalize(-position);
    float3 normal = normalize(GetNormal(uv));

    float sliceRotation = twoPI / (float(sliceCount) - 1.0);
    float sampleScale = (-sampleRadius * TESR_ProjectionTransform[0][0]) / position.z;
    float jitter = randomIGN(int2(pixelPos)) - 0.5;

	[loop]
    for (float slice = 0.0; slice < float(sliceCount) + 0.5; slice += 1.0) {
        float phi = sliceRotation * (slice + jitter) + PI;
        float2 omega = float2(cos(phi), sin(phi));
        float3 direction = float3(omega.x, omega.y, 0.0);
        float3 orthoDirection = direction - dot(direction, camera) * camera;
        float3 axis = cross(direction, camera);
        float3 projNormal = normal - axis * dot(normal, axis);
        float projLength = length(projNormal);

        float n = sign(dot(orthoDirection, projNormal)) * acos(clamp(dot(projNormal, camera) / projLength, 0.0, 1.0));

		[loop]
        for (float currentSample = 0.0; currentSample < float(sampleCount)-0.5; currentSample += 1.0) {
            float sampleStep = (currentSample + jitter) / float(sampleCount) + 0.01;
            float2 sampleUV = uv - sampleStep * sampleScale * omega * TESR_ReciprocalResolution.yx / TESR_ReciprocalResolution.x;
            float3 samplePosition = reconstructPosition(sampleUV);
            float3 sampleNormal = normalize(GetNormal(sampleUV));
            float3 sampleLight = tex2D(TESR_SourceBuffer, sampleUV).rgb;
            float3 sampleDistance = samplePosition - position;
            float3 sampleHorizon = sampleDistance / length(sampleDistance);

            frontBackHorizon.x = dot(sampleHorizon, camera);
            frontBackHorizon.y = dot(normalize(sampleDistance - camera * hitThickness), camera);

            frontBackHorizon = clamp((acos(frontBackHorizon) + n + twoPI) / PI, 0.0, 1.0);

            indirect = updateSectors(frontBackHorizon.x, frontBackHorizon.y, globalOccludedBitfield);
            lighting += (1.0 - float(bitCount(BIT_AND(indirect, BIT_FLIP(globalOccludedBitfield)))) / float(SECTOR_COUNT)) *
                sampleLight * clamp(dot(normal, sampleHorizon), 0.0, 1.0) *
                clamp(dot(sampleNormal, -sampleHorizon), 0.0, 1.0);
            globalOccludedBitfield = BIT_OR(globalOccludedBitfield , indirect);
        }
        visibility += 1.0 - float(bitCount(globalOccludedBitfield )) / float(SECTOR_COUNT);
    }

    visibility /= float(sliceCount);
    lighting /= float(sliceCount);

    return float4(lighting, visibility);
}
/*
float4 SSAO(VSOUT IN, uniform float2 OffsetMask) : COLOR0
{
	float2 uv = IN.UVCoord.xy;
	float4 color = tex2D(TESR_RenderedBuffer, uv);
	color = OffsetMask.y?color:float(1).xxxx; // use previous rendered buffer if not first pass

#if halfres
	clip ((IN.UVCoord.x < 0.5 && IN.UVCoord.y < 0.5)-1); // discard half the screen to render at half resolution
	uv *= 2;
#endif
	
	// generate the sampling kernel with random points in a hemisphere
	// int kernelSize = clamp(AOsamples, 0, 32);
	float uRadius = (AOrange);
	float bias = saturate(AOangleBias);

	float3 origin = reconstructPosition(uv);
	if (origin.z > endFade) return 1.0;

	//reorient our sample kernel along the origin's normal
	float3 normal = GetNormal(uv);

	float angle = -randomIGN(uv).x / 2 * PI; // random angle between 0 and 90degrees
	float3 kernelRotation = float3( -sin(angle), cos(angle), 0);
	float3 tangent = normalize(kernelRotation - normal * dot(kernelRotation, normal));
	float3 bitangent = cross(normal, tangent);
	float3x3 tbn = float3x3(tangent, bitangent, normal);

	// calculate occlusion by sampling depth of each point from the kernel
	float occlusion = 0.0;
	for (int i = 0; i < kernelSize; ++i) {
		// generate random samples in a unit sphere (random vector coordinates from -1 to 1);
		float randomNoise = randomIGN(uv + i * TESR_ReciprocalResolution.x);
		float3 rand = float3(randomNoise,randomNoise,randomNoise);
		float3 sampleVector = float3 (expand(rand.xy), rand.z) * float3(OffsetMask, 1); // separate kernel
		sampleVector = mul(normalize(sampleVector), tbn);

		//randomize points distance to sphere center, making them more concentrated towards the center
		sampleVector *= randomIGN(uv * i/2);
		float scale = 1 + float(i) / float(kernelSize);
		scale = lerp(bias, 1.0f, scale * scale);
		sampleVector *= scale; 

		// get sample positions around origin:
		sampleVector *= dot(normal, sampleVector) < 0.0 ? -1.0 : 1.0; // if our sample vector goes inside the geometry, we flip it
		float3 samplePoint = origin + sampleVector * uRadius;
		
		// compare depth of the projected sample with the value from depthbuffer
		float3 screenSpaceSample = projectPosition (samplePoint);
		float sampleDepth = readDepth(screenSpaceSample.xy);
		float actualDepth = samplePoint.z;

		// range check & accumulate:
		float distance = (actualDepth - sampleDepth);
		float rangeCheck = distance < uRadius ? 1.0 : 0.0;
		float influence = (sampleDepth < actualDepth ? 1.0 : 0.0 ) * rangeCheck;

		// stronger strength curve in close vectors (replacing an if statement with a lerp)
		influence *= lerp(1.0 - distance * distance/(uRadius * uRadius), 1.0 - distance /uRadius, i < kernelSize / 4);
		occlusion += influence;
	}
	
	occlusion = 1.0 - occlusion/kernelSize * AOstrength;

	float fogColor = luma(TESR_FogColor.rgb);
	float darkness = clamp(lerp(occlusion, fogColor, fogCoeff(origin.z)), occlusion, 1.0);

	darkness = lerp(darkness, 1.0, saturate(invlerp(startFade, endFade, origin.z))) * color.x;

	return convertsRGBBuffer(float2(darkness, 1.0).xxxy);
}
*/

float4 Expand(VSOUT IN) : COLOR0
{
	float2 coord = IN.UVCoord * 0.5;
	return convertsRGBBuffer(tex2D(TESR_RenderedBuffer, coord));
}

float4 Combine(VSOUT IN) : COLOR0
{
	float3 color = tex2D(TESR_SourceBuffer, IN.UVCoord).rgb;
	/*float ao = lerp(AOclamp, 1.0, tex2D(TESR_RenderedBuffer, IN.UVCoord).w);

	float luminance = luma(color);
	float lt = luminance - AOlumThreshold;
	luminance = saturate(lt * 3.0);
	ao = lerp(ao, 1.0, luminance);
	color *= ao;

    #if viewao
		return float4(ao, ao, ao, 1.0f);
	#endif
	
	return float4(color.rgb, 1.0f);*/
	// rgb is the ambient lighting, w is the AO.
	float4 IBL = tex2D(TESR_RenderedBuffer, IN.UVCoord);

	return float4(IBL.www, 1.0);
}


technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 SSILVB();
	}

#if halfres
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Expand();
	}
#endif
	
	//pass
	//{ 
	//	VertexShader = compile vs_3_0 FrameVS();
	//	PixelShader = compile ps_3_0 NormalBlurRChannel(io.xy, blurRadius, blurDrop, endFade);
	//}
	
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Combine();
	}
}