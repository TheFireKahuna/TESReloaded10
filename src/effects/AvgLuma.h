#pragma once

class AvgLumaEffect : public EffectRecord
{
public:
	AvgLumaEffect() : EffectRecord("AvgLuma") {};

	struct AvgLumaStruct {
		D3DXVECTOR4		BufferRes;
		D3DXVECTOR4		BufferTexel;
		D3DXVECTOR4		BufferExtraTexel;
	};
	AvgLumaStruct	Constants;

	struct AvgLumaTextures {
		IDirect3DTexture9* AvgLumaTexture;
		IDirect3DSurface9* AvgLumaSurface;
		IDirect3DTexture9* HistogramLumaTexture;
		IDirect3DSurface9* HistogramLumaSurface;
		IDirect3DTexture9* HistogramSampleTextureY;
		IDirect3DSurface9* HistogramSampleSurfaceY;
		IDirect3DTexture9* HistogramSampleTextureXY;
		IDirect3DSurface9* HistogramSampleSurfaceXY;
		IDirect3DTexture9* HistogramSubsampleTextureY;
		IDirect3DSurface9* HistogramSubsampleSurfaceY;
		IDirect3DTexture9* HistogramSubsampleTextureXY;
		IDirect3DSurface9* HistogramSubsampleSurfaceXY; 
		IDirect3DTexture9* HistogramBinTextureY;
		IDirect3DSurface9* HistogramBinSurfaceY;
		IDirect3DTexture9* HistogramBinTextureXY;
		IDirect3DSurface9* HistogramBinSurfaceXY;
		IDirect3DTexture9* HistogramTexture;
		IDirect3DSurface9* HistogramSurface;


		IDirect3DVertexBuffer9* HistogramLumaBuffer;
		IDirect3DVertexBuffer9* HistogramSampleBufferY;
		IDirect3DVertexBuffer9* HistogramSampleBufferXY;
		IDirect3DVertexBuffer9* HistogramSubsampleBufferY;
		IDirect3DVertexBuffer9* HistogramSubsampleBufferXY;
		IDirect3DVertexBuffer9* HistogramBinBufferY;
		IDirect3DVertexBuffer9* HistogramBinBufferXY;
		IDirect3DVertexBuffer9* HistogramBuffer;
		IDirect3DVertexBuffer9* AvgLumaBuffer;
	};
	AvgLumaTextures	Textures;

	void    RenderPass(IDirect3DDevice9* Device, UINT techniqueIndex, bool ClearRenderTarget);
	void	UpdateConstants() {};
	void	UpdateSettings();
	void	RegisterTextures();
	void	RegisterConstants();
	void	RenderAvgLumaBuffer(IDirect3DSurface9* RenderTarget, IDirect3DSurface9* RenderedBuffer, IDirect3DSurface9* SourceBuffer);
};
