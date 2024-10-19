#include "LinearizePT.h"

void LinearizePTEffect::UpdateSettings() {
	Constants.Options.x = TheSettingManager->GetSettingI("Shaders.Linearization.PreTonemap", "Linearize");
	Constants.Options.y = TheSettingManager->GetSettingI("Shaders.Linearization.PreTonemap", "Delinearize");
	Constants.Options.z = TheSettingManager->GetSettingI("Shaders.Linearization.ImageSpace", "Linearize");
	Constants.Options.w = TheSettingManager->GetSettingI("Shaders.Linearization.ImageSpace", "Delinearize");
	linearizeGameColors = TheSettingManager->GetSettingF("Shaders.Linearization.Shaders", "LinearizeWeathers");
	Constants.TextureOpts.x = TheSettingManager->GetSettingI("Shaders.Linearization.Shaders", "LinearizeSkyTextures");
	Constants.TextureOpts.y = TheSettingManager->GetSettingI("Shaders.Linearization.Shaders", "LinearizeBaseColors");
	Constants.TextureOpts.z = TheSettingManager->GetSettingI("Shaders.Linearization.Shaders", "LinearizeDiffuseColors");
	Constants.TextureOpts.w = TheSettingManager->GetSettingI("Shaders.Linearization.Shaders", "LinearizeVertexColors");
}

void LinearizePTEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_Linearization", &Constants.Options);
	TheShaderManager->RegisterConstant("TESR_LinearTex", &Constants.TextureOpts);
}