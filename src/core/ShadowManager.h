#pragma once


class ShadowManager { // Never disposed
public:
	static void Initialize();

	enum PlaneEnum {
		PlaneNear = 0,
		PlaneFar = 1,
		PlaneLeft = 2,
		PlaneRight = 3,
		PlaneTop = 4,
		PlaneBottom = 5,
	};


	D3DXMATRIX				GetViewMatrix(D3DXVECTOR3* At, D3DXVECTOR4* Dir);
	void					RenderShadowCubeMap(ShadowSceneLight** Lights, UInt32 LightIndex);
	void					RenderShadowSpotlight(NiSpotLight** Lights, UInt32 LightIndex);
	void					RenderShadowMaps();
    void                    BlurShadowMap(ShadowsExteriorEffect::ShadowMapSettings* ShadowMap);
	void					RenderShadowExteriorMaps(D3DXVECTOR3* At);
	TESObjectREFR*			GetRef(TESObjectREFR* Ref, ShadowsExteriorEffect::FormsStruct* Forms);
	void					AccumulateGeometry(NiAVObject* accum);
	void					SelectGeometry(NiGeometry* geo);
	bool					IsOutAllFrustums(NiNode* node);
	bool					ExcludeFromAllRadius(NiAVObject* node);
	void					RenderInterior(NiAVObject* Object, float MinRadius);
	void					RenderGeometry(NiGeometry* Geo);

	ShadowRenderPass*				 geometryPass;
	AlphaShadowRenderPass*			 alphaPass;
	SkinnedGeoShadowRenderPass*		 skinnedGeoPass;
	SkinnedAlphaGeoShadowRenderPass* skinnedAlphaPass;
	SpeedTreeShadowRenderPass*		 speedTreePass;
	InteriorShadowRenderPass*		 interiorPass;

	NiVector4				BillboardRight;
	NiVector4				BillboardUp;
	ShaderRecordVertex*		ShadowMapVertex;
	ShaderRecordPixel*		ShadowMapPixel;
	ShaderRecordVertex*		ShadowCubeMapVertex;
	ShaderRecordPixel*		ShadowCubeMapPixel;
    ShaderRecordVertex*		ShadowMapBlurVertex;
	ShaderRecordPixel*		ShadowMapBlurPixel;
	D3DVIEWPORT9			ShadowCubeMapViewPort;
	ShaderRecordVertex*		CurrentVertex;
	ShaderRecordPixel*		CurrentPixel;
	bool					AlphaEnabled;
	int						PointLightsNum;
	float					shadowMapsRenderTime;
	bool					ShadowShadersLoaded;
};