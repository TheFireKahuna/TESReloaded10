#include "LinearizePT.h"

void LinearizePTEffect::UpdateSettings() {
	Constants.Options.x = TheSettingManager->GetSettingI("Shaders.Linearization.NVR", "PreLinearize");
	Constants.Options.y = TheSettingManager->GetSettingI("Shaders.Linearization.NVR", "PreDelinearize");
	Constants.Options.z = TheSettingManager->GetSettingI("Shaders.Linearization.NVR", "PostLinearize");
	Constants.Options.w = TheSettingManager->GetSettingI("Shaders.Linearization.NVR", "PostDelinearize");

	linearizeGameColors = TheSettingManager->GetSettingI("Shaders.Linearization.NVR", "LinearWeathers");
	linearizeLights = TheSettingManager->GetSettingI("Shaders.Linearization.NVR", "LinearEffectLights");

	Constants.OptsShared.x = TheSettingManager->GetSettingI("Shaders.Linearization.Shaders", "LinearLight");
	Constants.OptsShared.y = TheSettingManager->GetSettingI("Shaders.Linearization.Shaders", "LinearDiffuse");
	Constants.OptsShared.z = TheSettingManager->GetSettingI("Shaders.Linearization.Shaders", "LinearVertex");
	Constants.OptsShared.w = TheSettingManager->GetSettingI("Shaders.Linearization.Shaders", "LinearRoughness");
	Constants.OptsTerrainColor.y = TheSettingManager->GetSettingI("Shaders.Linearization.Shaders", "LinearInputColor");

	if (Constants.OptsShared.x > 0) {
		Constants.OptsObject.x = TheSettingManager->GetSettingI("Shaders.Linearization.Object", "LinearLight");
		Constants.OptsTerrain.x = TheSettingManager->GetSettingI("Shaders.Linearization.Terrain", "LinearLight");

		if (Constants.OptsObject.x > 0) {
			Constants.OptsObjectColor.x = TheSettingManager->GetSettingI("Shaders.Linearization.Object", "LinearLightSunAmb");
			Constants.OptsObjectColor.y = TheSettingManager->GetSettingI("Shaders.Linearization.Object", "LinearLightEmittance");
			Constants.OptsObjectColor.z = TheSettingManager->GetSettingI("Shaders.Linearization.Object", "LinearLightPointLight");
			Constants.OptsObjectColor.w = TheSettingManager->GetSettingI("Shaders.Linearization.Object", "LinearLightEnvMap");
		}
		else {
			Constants.OptsObjectColor.x = 0;
			Constants.OptsObjectColor.y = 0;
			Constants.OptsObjectColor.z = 0;
			Constants.OptsObjectColor.w = 0;
		}

		if (Constants.OptsTerrain.x > 0) {
			Constants.OptsTerrainColor.x = TheSettingManager->GetSettingI("Shaders.Linearization.Terrain", "LinearLightSun");
			Constants.OptsTerrainColor.z = TheSettingManager->GetSettingI("Shaders.Linearization.Terrain", "LinearLightPointLight");
			Constants.OptsTerrainColor.w = TheSettingManager->GetSettingI("Shaders.Linearization.Terrain", "LinearLightAmbient");
		}
		else {
			Constants.OptsTerrainColor.x = 0;
			Constants.OptsTerrainColor.z = 0;
			Constants.OptsTerrainColor.w = 0;
		}
	}
	else {
		Constants.OptsObject.x = 0;
		Constants.OptsTerrain.x = 0;

		Constants.OptsObjectColor.x = 0;
		Constants.OptsObjectColor.y = 0;
		Constants.OptsObjectColor.z = 0;
		Constants.OptsObjectColor.w = 0;

		Constants.OptsTerrainColor.x = 0;
		Constants.OptsTerrainColor.z = 0;
		Constants.OptsTerrainColor.w = 0;
	}
	if (Constants.OptsShared.y > 0) {
		Constants.OptsObject.y = TheSettingManager->GetSettingI("Shaders.Linearization.Object", "LinearDiffuse");
		Constants.OptsTerrain.y = TheSettingManager->GetSettingI("Shaders.Linearization.Terrain", "LinearDiffuse");
	}
	else {
		Constants.OptsObject.y = 0;
		Constants.OptsTerrain.y = 0;
	}
	if (Constants.OptsShared.z > 0) {
		Constants.OptsObject.z = TheSettingManager->GetSettingI("Shaders.Linearization.Object", "LinearVertex");
		Constants.OptsTerrain.z = TheSettingManager->GetSettingI("Shaders.Linearization.Terrain", "LinearVertex");
	}
	else {
		Constants.OptsObject.z = 0;
		Constants.OptsTerrain.z = 0;
	}
	if (Constants.OptsShared.w > 0) {
		Constants.OptsObject.w = TheSettingManager->GetSettingI("Shaders.Linearization.Object", "LinearRoughness");
		Constants.OptsTerrain.w = TheSettingManager->GetSettingI("Shaders.Linearization.Terrain", "LinearRoughness");
	}
	else {
		Constants.OptsObject.w = 0;
		Constants.OptsTerrain.w = 0;
	}

	Constants.OptsSky.x = TheSettingManager->GetSettingI("Shaders.Linearization.Sky", "LinearClouds");
	Constants.OptsSky.y = TheSettingManager->GetSettingI("Shaders.Linearization.Sky", "LinearColor");
	Constants.OptsSky.z = TheSettingManager->GetSettingI("Shaders.Linearization.Sky", "LinearStars");
	Constants.OptsSky.w = TheSettingManager->GetSettingI("Shaders.Linearization.Sky", "LinearBlend");

	Constants.OptsLightColor.x = TheSettingManager->GetSettingF("Shaders.Linearization.Shaders", "GlobalDiffuse");
	Constants.OptsLightColor.y = TheSettingManager->GetSettingF("Shaders.Linearization.Shaders", "GlobalAmbient");
	Constants.OptsLightColor.z = TheSettingManager->GetSettingF("Shaders.Linearization.Shaders", "GlobalPointLight");
	Constants.OptsLightColor.w = TheSettingManager->GetSettingF("Shaders.Linearization.Shaders", "GlobalEmissiveLight");

	Constants.OptsLightColor2.x = TheSettingManager->GetSettingF("Shaders.Linearization.Shaders", "GlobalEnvMap");
	Constants.OptsLightColor2.y = TheSettingManager->GetSettingF("Shaders.Linearization.Shaders", "GlobalGlowMap");

}

void LinearizePTEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_Linearization", &Constants.Options);
	TheShaderManager->RegisterConstant("TESR_LinearShader", &Constants.OptsShared);
	TheShaderManager->RegisterConstant("TESR_LinearObject", &Constants.OptsObject);
	TheShaderManager->RegisterConstant("TESR_LinearSky", &Constants.OptsSky);
	TheShaderManager->RegisterConstant("TESR_LinearTerrain", &Constants.OptsTerrain);
	TheShaderManager->RegisterConstant("TESR_LinearObjectColor", &Constants.OptsObjectColor);
	TheShaderManager->RegisterConstant("TESR_LinearTerrainColor", &Constants.OptsTerrainColor);
	TheShaderManager->RegisterConstant("TESR_ShaderBaseColors", &Constants.OptsLightColor);
	TheShaderManager->RegisterConstant("TESR_ShaderExtraColors", &Constants.OptsLightColor2);
}