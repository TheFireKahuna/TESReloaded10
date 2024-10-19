#pragma once

class LinearizePTEffect : public EffectRecord
{
public:
	LinearizePTEffect() : EffectRecord("LinearizePT") {};

	struct LinearizePTStruct {
		D3DXVECTOR4		Options;
		D3DXVECTOR4		TextureOpts;
	};
	LinearizePTStruct	Constants;

	bool linearizeGameColors;

	void	UpdateConstants() {};
	void	RegisterConstants();
	void	UpdateSettings();

};