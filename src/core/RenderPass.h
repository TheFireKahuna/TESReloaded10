#pragma once


enum ShadowMapTypeEnum {
	MapNear = 0,
	MapMiddle = 1,
	MapFar = 2,
	MapLod = 3,
	MapOrtho = 4,
};
enum ShadowMapVisibility {
	None = 0x0,
	Near = 0x1,
	Middle = 0x10,
	Far = 0x100,
	Lod = 0x1000,
	Ortho = 0x10000
};
class RenderPass {
public:
	RenderPass() {};
	virtual ~RenderPass() {
		VertexShader = NULL;
		PixelShader = NULL;
	};
	virtual bool AccumObject(NiGeometry* Geo, UInt32& visibility) { return true; };

	ShaderRecordVertex* VertexShader;
	ShaderRecordPixel* PixelShader;

	std::vector<std::tuple<NiGeometry*, UInt32>> GeometryList;
	virtual void UpdateConstants(NiGeometry* Geo) {};
	virtual void RegisterConstants() {};
	bool IsVisible(ShadowMapTypeEnum type, UInt32 visibility);
};


class ShadowRenderPass : public RenderPass {
public:
	ShadowRenderPass();

	struct ConstantsStruct {
	};
	ConstantsStruct Constants;

	void RegisterConstants();
	void UpdateConstants(NiGeometry* Geo);
	void RenderNormalPass(ShadowMapTypeEnum ShadowMapType);
	bool AccumObject(NiGeometry* Geo, UInt32& visibility);
};


class InteriorShadowRenderPass : public RenderPass {
public:
	InteriorShadowRenderPass();

	struct ConstantsStruct {
	};
	ConstantsStruct Constants;

	void RegisterConstants() {};
	void UpdateConstants(NiGeometry* Geo) {};
	void RenderInteriorPass(NiGeometry* Geo);
	bool AccumObject(NiGeometry* Geo, UInt32& visibility) { return true; };
};

class AlphaShadowRenderPass : public RenderPass {
public:
	AlphaShadowRenderPass();

	struct ConstantsStruct {
		IDirect3DBaseTexture9* DiffuseMap;
	};
	ConstantsStruct Constants;

	void RegisterConstants();
	void UpdateConstants(NiGeometry* Geo);
	void RenderAlphaPass(ShadowMapTypeEnum ShadowMapType);
	bool AccumObject(NiGeometry* Geo, UInt32& visibility);
};


class SkinnedGeoShadowRenderPass : public RenderPass {
public:
	SkinnedGeoShadowRenderPass();

	struct ConstantsStruct {
		D3DXVECTOR4* BoneMatrices;
	};
	ConstantsStruct Constants;

	void RegisterConstants();
	void UpdateConstants(NiGeometry* Geo);
	void RenderSkinnedPass(ShadowMapTypeEnum ShadowMapType);
	bool AccumObject(NiGeometry* Geo, UInt32& visibility);
};


class SkinnedAlphaGeoShadowRenderPass: public RenderPass{
public:
	SkinnedAlphaGeoShadowRenderPass();

	struct ConstantsStruct {
		D3DXVECTOR4* BoneMatrices;
	};
	ConstantsStruct Constants;

	void RegisterConstants();
	void UpdateConstants(NiGeometry* Geo);
	void RenderSkinnedAlphaPass(ShadowMapTypeEnum ShadowMapType);
	bool AccumObject(NiGeometry* Geo, UInt32& visibility);
};


class SpeedTreeShadowRenderPass : public RenderPass {
public:
	SpeedTreeShadowRenderPass();

	struct ConstantsStruct {
		D3DXVECTOR4* BillboardRight;
		D3DXVECTOR4* BillboardUp;
		D3DXVECTOR4* LeafBase;
		IDirect3DBaseTexture9* DiffuseMap;
	};
	ConstantsStruct Constants;

	void RegisterConstants(); 
	void UpdateConstants(NiGeometry* Geo);
	void RenderSpeedTreePass(ShadowMapTypeEnum ShadowMapType);
	bool AccumObject(NiGeometry* Geo, UInt32& visibility);
};