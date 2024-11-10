#pragma once
extern bool(__thiscall* ReadSetting)(IniSettingCollection*, GameSetting*);
bool __fastcall ReadSettingHook(IniSettingCollection* This, UInt32 edx, GameSetting* Setting);

extern bool(__thiscall* WriteSetting)(IniSettingCollection*, GameSetting*);
bool __fastcall WriteSettingHook(IniSettingCollection* This, UInt32 edx, GameSetting* Setting);

extern bool(__thiscall* LoadGame)(TESSaveLoadGame*, char*, UInt8);
bool __fastcall LoadGameHook(TESSaveLoadGame* This, UInt32 edx, char* FileName, UInt8 Arg2);

void MainMenuMusicFix(char* DestinationBuffer, size_t Size, char* SourceBuffer);