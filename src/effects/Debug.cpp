#include "Debug.h"

void DebugEffect::UpdateConstants() {}

void DebugEffect::UpdateSettings() {
	Constants.DebugVar.x = TheSettingManager->GetSettingI("Main.Develop.Main", "DebugVar1");
	Constants.DebugVar.y = TheSettingManager->GetSettingI("Main.Develop.Main", "DebugVar2");
	Constants.DebugVar.z = TheSettingManager->GetSettingI("Main.Develop.Main", "DebugVar3");
	Constants.DebugVar.w = TheSettingManager->GetSettingI("Main.Develop.Main", "DebugVar4");
}

void DebugEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_DebugVar", &Constants.DebugVar);
}