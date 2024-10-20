#pragma once

class LinearizePTEffect : public EffectRecord
{
public:
	LinearizePTEffect() : EffectRecord("LinearizePT") {};

	struct LinearizePTStruct {
		D3DXVECTOR4		Options;
		D3DXVECTOR4		OptsShared;
		D3DXVECTOR4		OptsObject;
		D3DXVECTOR4		OptsSky;
		D3DXVECTOR4		OptsTerrain;
		D3DXVECTOR4		OptsTerrainColor;
		D3DXVECTOR4		OptsObjectColor;
		D3DXVECTOR4		OptsLightColor;
		D3DXVECTOR4		OptsLightColor2;
	};
	LinearizePTStruct	Constants;

	bool linearizeGameColors;
	bool linearizeLights;

	void	UpdateConstants() {};
	void	RegisterConstants();
	void	UpdateSettings();

};