#pragma once

class GrassShaders : public ShaderCollection
{
public:
	GrassShaders() : ShaderCollection("Grass") {};

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};