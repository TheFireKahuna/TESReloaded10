#pragma once

class TextureRecord {
public:
	TextureRecord();
	
	enum TextureRecordType {
		None,
		PlanarBuffer,
		VolumeBuffer,
		CubeBuffer,
		SourceBuffer,
		RenderedBuffer,
		DepthBuffer,
		ShadowMapBufferNear,
		ShadowMapBufferFar,
		OrthoMapBuffer,
		ShadowCubeMapBuffer0,
		ShadowCubeMapBuffer1,
		ShadowCubeMapBuffer2,
		ShadowCubeMapBuffer3,
		WaterHeightMapBuffer,
	};

	bool					LoadTexture(TextureRecordType Type, const char* Filename);
	void					SetSamplerState(D3DSAMPLERSTATETYPE SamplerType, DWORD Value);

	IDirect3DBaseTexture9*	Texture;
	DWORD					SamplerStates[SamplerStatesMax];
};

typedef std::map<std::string, TextureRecord*> TextureList;

class TextureManager { // Never disposed
public:
	static void			Initialize();

	TextureRecord*		LoadTexture(const char* ShaderSource, UInt32 RegisterIndex);
	
	TextureList			Textures;
};