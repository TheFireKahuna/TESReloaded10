#include "AvgLuma.h"
#include <algorithm>

#define Histogram_SampleRes 8 //[32:253] //-Adjust sample resolution (effects performance)
#define Histogram_SubsampleRes 32 //[32:253] //-Adjust sample resolution (effects performance)
#define Histogram_BinCount 256 //[32:253] //-Adjust sample resolution (effects performance)

void AvgLumaEffect::RegisterTextures() {
	TheTextureManager->InitTexture("TESR_AvgLumaBuffer", &Textures.AvgLumaTexture, &Textures.AvgLumaSurface, 1, 1, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_HistogramLumaBuffer", &Textures.HistogramLumaTexture, &Textures.HistogramLumaSurface, TheRenderManager->width / 2, TheRenderManager->height / 2, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_HistogramSampleBufferY", &Textures.HistogramSampleTextureY, &Textures.HistogramSampleSurfaceY, TheRenderManager->width, TheRenderManager->height / Histogram_SampleRes, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_HistogramSampleBufferXY", &Textures.HistogramSampleTextureXY, &Textures.HistogramSampleSurfaceXY, TheRenderManager->width / Histogram_SampleRes, TheRenderManager->height / Histogram_SampleRes, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_HistogramSubsampleBufferY", &Textures.HistogramSubsampleTextureY, &Textures.HistogramSubsampleSurfaceY, TheRenderManager->width / Histogram_SampleRes, TheRenderManager->height / Histogram_SubsampleRes, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_HistogramSubsampleBufferXY", &Textures.HistogramSubsampleTextureXY, &Textures.HistogramSubsampleSurfaceXY, TheRenderManager->width / Histogram_SubsampleRes, TheRenderManager->height / Histogram_SubsampleRes, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_HistogramBinBufferY", &Textures.HistogramBinTextureY, &Textures.HistogramBinSurfaceY, TheRenderManager->width / Histogram_SubsampleRes, TheRenderManager->height / Histogram_BinCount, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_HistogramBinBufferXY", &Textures.HistogramBinTextureXY, &Textures.HistogramBinSurfaceXY, TheRenderManager->width / Histogram_BinCount, TheRenderManager->height / Histogram_BinCount, D3DFMT_A16B16G16R16F);
	TheTextureManager->InitTexture("TESR_HistogramBuffer", &Textures.HistogramTexture, &Textures.HistogramSurface, TheRenderManager->width / Histogram_BinCount, 1, D3DFMT_A16B16G16R16F);

	TheShaderManager->CreateFrameVertex(TheRenderManager->width / 2, TheRenderManager->height / 2, &Textures.HistogramLumaBuffer);
	TheShaderManager->CreateFrameVertex(TheRenderManager->width, TheRenderManager->height / Histogram_SampleRes, &Textures.HistogramSampleBufferY);
	TheShaderManager->CreateFrameVertex(TheRenderManager->width / Histogram_SampleRes, TheRenderManager->height / Histogram_SampleRes, &Textures.HistogramSampleBufferXY);
	TheShaderManager->CreateFrameVertex(TheRenderManager->width / Histogram_SampleRes, TheRenderManager->height / Histogram_SubsampleRes, &Textures.HistogramSubsampleBufferY);
	TheShaderManager->CreateFrameVertex(TheRenderManager->width / Histogram_SubsampleRes, TheRenderManager->height / Histogram_SubsampleRes, &Textures.HistogramSubsampleBufferXY);
	TheShaderManager->CreateFrameVertex(TheRenderManager->width / Histogram_SubsampleRes, TheRenderManager->height / Histogram_BinCount, &Textures.HistogramBinBufferY);
	TheShaderManager->CreateFrameVertex(TheRenderManager->width / Histogram_BinCount, TheRenderManager->height / Histogram_BinCount, &Textures.HistogramBinBufferXY);
	TheShaderManager->CreateFrameVertex(TheRenderManager->width / Histogram_BinCount, 1, &Textures.HistogramBuffer);
	TheShaderManager->CreateFrameVertex(1, 1, &Textures.AvgLumaBuffer);
}

/**
* Renders the given effect shader.
*/
void AvgLumaEffect::RenderPass(IDirect3DDevice9* Device, UINT techniqueIndex, bool ClearRenderTarget) {
	try {
		D3DXHANDLE technique = Effect->GetTechnique(techniqueIndex);
		Effect->SetTechnique(technique);
		SetCT(); // update the constant table
		UINT Passes;
		Effect->Begin(&Passes, NULL);
		for (UINT p = 0; p < Passes; p++) {
			if (ClearRenderTarget) Device->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0L);
			Effect->BeginPass(p);
			Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			Effect->EndPass();
		}
		Effect->End();
	}
	catch (const std::exception& e) {
		Logger::Log("Error during rendering of effect %s: %s", Name, e.what());
	}
}


void AvgLumaEffect::RenderAvgLumaBuffer(IDirect3DSurface9* RenderTarget, IDirect3DSurface9* RenderedSurface, IDirect3DSurface9* SourceBuffer) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	Device->StretchRect(RenderTarget, NULL, SourceBuffer, NULL, D3DTEXF_LINEAR);

	// first, calc log2 luma for every pixel, and set .G to 1 if its invalid
	// this is at either full res, or could be done at half-res for perf
	Device->SetStreamSource(0, Textures.HistogramLumaBuffer, 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
	Device->SetRenderTarget(0, Textures.HistogramLumaSurface);
	Render(Device, Textures.HistogramLumaSurface, Textures.HistogramLumaSurface, 0, false, NULL);

	// next, sample 16x16 pixels into a Screen/16 grid. sum up the total luminance into .R, sum up invalid pixels into .G
	// this is done by summing 16x in the Y axis every Screen/16 * gridPosY into a single value in the Screen/16 Sample Buffer
	// this is then repeated in the X axis, which sums the results of the previous pass
	Device->SetStreamSource(0, Textures.HistogramSampleBufferY, 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
	Device->SetRenderTarget(0, Textures.HistogramSampleSurfaceY);
	Render(Device, Textures.HistogramSampleSurfaceY, Textures.HistogramSampleSurfaceY, 1, false, NULL);
	Device->SetStreamSource(0, Textures.HistogramSampleBufferXY, 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
	Device->SetRenderTarget(0, Textures.HistogramSampleSurfaceXY);
	Render(Device, Textures.HistogramSampleSurfaceXY, Textures.HistogramSampleSurfaceXY, 2, false, NULL);

	Device->SetStreamSource(0, Textures.HistogramSubsampleBufferY, 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
	Device->SetRenderTarget(0, Textures.HistogramSubsampleSurfaceY);
	Render(Device, Textures.HistogramSubsampleSurfaceY, Textures.HistogramSubsampleSurfaceY, 3, false, NULL);
	Device->SetStreamSource(0, Textures.HistogramSubsampleBufferXY, 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
	Device->SetRenderTarget(0, Textures.HistogramSubsampleSurfaceXY);
	Render(Device, Textures.HistogramSubsampleSurfaceXY, Textures.HistogramSubsampleSurfaceXY, 4, false, NULL);

	// calculate histogram bins
	// we once more sample 16x16 pixels this time into a Screen/(16*16) grid. sum up the total luminance, sum up invalid pixels
	// this time we will have a buffer (HistogramBinSurfaceXY, that has within a sum of each bin and the sum of its invalid pixels)
	Device->SetStreamSource(0, Textures.HistogramBinBufferY, 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
	Device->SetRenderTarget(0, Textures.HistogramBinSurfaceY);
	Render(Device, Textures.HistogramBinSurfaceY, Textures.HistogramBinSurfaceY, 5, false, NULL);
	Device->SetStreamSource(0, Textures.HistogramBinBufferXY, 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
	Device->SetRenderTarget(0, Textures.HistogramBinSurfaceXY);
	Render(Device, Textures.HistogramBinSurfaceXY, Textures.HistogramBinSurfaceXY, 6, false, NULL);

	// the with this summarised, we sum up a 1D Bins*1 histogram which rolls up the Y axis
	Device->SetStreamSource(0, Textures.HistogramBuffer, 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
	Device->SetRenderTarget(0, Textures.HistogramSurface);
	Render(Device, Textures.HistogramSurface, Textures.HistogramSurface, 7, false, NULL);

	// finally, we sum up the X bins into a single 1x1 value, Average Log2 Luminance
	Device->SetStreamSource(0, Textures.AvgLumaBuffer, 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
	Device->SetRenderTarget(0, Textures.AvgLumaSurface);
	Render(Device, Textures.AvgLumaSurface, Textures.AvgLumaSurface, 8, false, NULL);

	// then we optionally render a histogram display
	Device->SetStreamSource(0, TheShaderManager->FrameVertex, 0, sizeof(FrameVS)); // Set correct vertex buffer for given resolution.
	Device->SetRenderTarget(0, RenderTarget);
	Render(Device, RenderTarget, RenderTarget, 9, false, NULL);
}

void AvgLumaEffect::RegisterConstants() {
	TheShaderManager->ConstantsTable["TESR_HistogramBufferData"] = &Constants.BufferRes;
	TheShaderManager->ConstantsTable["TESR_HistogramTexelData"] = &Constants.BufferTexel;
	TheShaderManager->ConstantsTable["TESR_HistogramTexelExtraData"] = &Constants.BufferExtraTexel;
}

void AvgLumaEffect::UpdateSettings() {
	Constants.BufferRes.x = Histogram_SampleRes;
	Constants.BufferRes.y = Histogram_BinCount;
	Constants.BufferRes.z = (int)(TheRenderManager->height / Histogram_BinCount);
	Constants.BufferRes.w = (int)(TheRenderManager->width / Histogram_BinCount);

	Constants.BufferTexel.x = 1.0 / (TheRenderManager->height / Histogram_SampleRes);
	Constants.BufferTexel.y = 1.0 / (TheRenderManager->width / Histogram_SampleRes);
	Constants.BufferTexel.z = 1.0 / (TheRenderManager->height / Histogram_BinCount);
	Constants.BufferTexel.w = 1.0 / (TheRenderManager->width / Histogram_BinCount);

	Constants.BufferExtraTexel.x = 1.0 / (TheRenderManager->height / Histogram_SubsampleRes);
	Constants.BufferExtraTexel.y = 1.0 / (TheRenderManager->width / Histogram_SubsampleRes);
	Constants.BufferExtraTexel.z = 1.0;
	Constants.BufferExtraTexel.w = 1.0;
}
