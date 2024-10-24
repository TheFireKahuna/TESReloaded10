#pragma once
#define CommandPrefix "NVR"
#define SettingsFile "\\Data\\NVSE\\Plugins\\NewVegasReloaded.dll.config"
#define TomlSettingsFile "\\Data\\NVSE\\Plugins\\NewVegasReloaded.dll.toml"
#define DefaultsSettingsFile "\\Data\\NVSE\\Plugins\\NewVegasReloaded.dll.defaults.toml"
#define ShadersPath "Data\\Shaders\\NewVegasReloaded\\Shaders\\"
#define EffectsPath "Data\\Shaders\\NewVegasReloaded\\Effects\\"
#define RenderStateArgs 0, 0
#define TerrainShadersNames "SLS2002.vso SLS2003.pso SLS2006.pso SLS2080.vso SLS2082.pso SLS2100.vso SLS2092.pso SLS2096.pso SLS2100.pso SLS2104.pso SLS2108.pso SLS2112.pso SLS2116.pso SLS2120.pso SLS2124.pso SLS2128.pso SLS2132.pso SLS2136.pso SLS2140.pso SLS2144.pso"
#define ObjectShadersNames "GDECAL.pso GDECAL.vso NOLIGHT000.vso NOLIGHT001.vso NOLIGHT002.vso NOLIGHTMAT.pso NOLIGHTTEX.pso NOLIGHTTEXVC.pso NOLIGHTTEXVCPMA.pso NOLIGHTVCONLY.pso DISTTREE.pso SLS1000.vso SLS1002.pso SLS1003.vso SLS1004.vso SLS1005.vso SLS1006.vso SLS1007.vso SLS1017.vso SLS1018.vso SLS1024.vso SLS1030.pso SLS2050.vso SLS2057.pso SLS2058.pso SLS2059.pso SLS2102.vso SLS2151.pso STB1005.pso STB2004.vso"
#define GrassShadersNames "GRASS23x000.vso GRASS23x002.vso GRASS23x000TMS.pso"
#define BloodShaders ""

#define SamplerStatesMax 12
#define ShadowCubeMapsMax 12
#define TrackedLightsMax 12
#define SpotLightsMax 1

#define WordWaterHeightMapBuffer "TESR_WaterHeightMapBuffer"
#define WordWaterReflectionMapBuffer "TESR_WaterReflectionMapBuffer"

static const char* IntroMovie = "NVRGameStudios.bik";
static const char* MainMenuMovie = "\\Data\\Video\\NVRMainMenu.bik";
static const char* MainMenuMusic = "NVRMainMenu";
static char* TitleMenu = (char*)"New Vegas Reloaded - Settings";

// to review
static const char* WeatherColorTypes[TESWeather::kNumColorTypes] = { "SkyUpper", "Fog", "CloudsLower", "Ambient", "Sunlight", "Sun", "Stars", "SkyLower", "Horizon", "CloudsUpper" };
static const char* WeatherTimesOfDay[TESWeather::kNumTimeOfDay] = { "Sunrise", "Day", "Sunset", "Night" };
static const char* WeatherHDRTypes[14] = { "EyeAdaptation", "BlurRadius", "BlurPasses", "EmissiveMult", "TargetLUM", "UpperLUMClamp", "BrightScale", "BrightClamp", "LUMRampNoTex", "LUMRampMin", "LUMRampMax", "SunlightDimmer", "GrassDimmer", "TreeDimmer" };


