
#include "Game.h"

Main* (__thiscall* NewMain)(Main*, HWND, HINSTANCE) = (Main* (__thiscall*)(Main*, HWND, HINSTANCE))Hooks::NewMain;
Main* __fastcall NewMainHook(Main* This, UInt32 edx, HWND Window, HINSTANCE Instance) {

	Global = (*NewMain)(This, Window, Instance);
	return Global;

}

TES* (__thiscall* NewTES)(TES*, char*, NiNode*, NiNode*, Sky*, NiNode*) = (TES* (__thiscall*)(TES*, char*, NiNode*, NiNode*, Sky*, NiNode*))Hooks::NewTES;
TES* __fastcall NewTESHook(TES* This, UInt32 edx, char* RootData, NiNode* ObjectLODRoot, NiNode* LandLOD, Sky* Sky, NiNode* WaterLOD) {
	
	Tes = (*NewTES)(This, RootData, ObjectLODRoot, LandLOD, Sky, WaterLOD);
	SceneNode = *(ShadowSceneNode**)0x011F91C8;
	return Tes;

}

MainDataHandler* (__thiscall* NewMainDataHandler)(MainDataHandler*) = (MainDataHandler* (__thiscall*)(MainDataHandler*))Hooks::NewMainDataHandler;
MainDataHandler* __fastcall NewMainDataHandlerHook(MainDataHandler* This, UInt32 edx) {

	DataHandler = (*NewMainDataHandler)(This);
	return DataHandler;

}

NiNode* TESObjectCELL::GetChildNode(CellRenderData::CellSubNodes aeNode) {
	NiNode* pMaster = GetNode();
	if (pMaster)
		return static_cast<NiNode*>(pMaster->m_children.data[aeNode]);
	else {
#if _DEBUG
		Logger::Log("[ TESObjectCELL::GetChildNode ] %s has no master node", fullName.name.m_data);
#endif
		return nullptr;
	}
}
GridCellArray* g_gridCellArray;

__declspec(naked) TESObjectCELL* __vectorcall GridCellArray::GetCellAtPos(__m128 pos) const
{
	__asm
	{
		pxor	xmm3, xmm3
		unpcklpd	xmm0, xmm3
		cvttps2dq	xmm0, xmm0
		psrad	xmm0, 0xC
		movq	xmm1, qword ptr[ecx + 4]
		psubd	xmm0, xmm1
		movd	xmm1, [ecx + 0xC]
		unpcklps	xmm1, xmm1
		movaps	xmm2, xmm1
		psrld	xmm2, 1
		paddd	xmm0, xmm2
		pcmpgtd	xmm1, xmm0
		movmskps	eax, xmm1
		cmp		al, 3
		jnz		retnNull
		movaps	xmm1, xmm0
		phaddd	xmm1, xmm3
		psrldq	xmm2, 4
		pslld	xmm0, xmm2
		paddd	xmm0, xmm1
		pextrw	edx, xmm0, 0
		mov		ecx, [ecx + 0x10]
		mov		eax, [ecx + edx * 4]
		retn
		retnNull :
		xor eax, eax
			retn
	}
}

__declspec(naked) TESObjectCELL* __vectorcall GridCellArray::GetCellAtCoord(__m128i cellXY) const
{
	__asm
	{
		push	ebx
		mov		ebx, ecx
		movd	eax, xmm0
		movsx	edx, ax
		sar		eax, 0x10
		mov		ecx, [ebx + 0xC]
		shr		ecx, 1
		sub		eax, [ebx + 4]
		add		eax, ecx
		cmp		eax, [ebx + 0xC]
		jnb		retnNull
		sub		edx, [ebx + 8]
		add		edx, ecx
		cmp		edx, [ebx + 0xC]
		jnb		retnNull
		add		edx, eax
		shl		eax, cl
		add		edx, eax
		mov		ecx, [ebx + 0x10]
		mov		eax, [ecx + edx * 4]
		pop		ebx
		retn
		retnNull :
		xor eax, eax
			pop		ebx
			retn
	}
}

__declspec(naked) void __vectorcall Coordinate::operator=(__m128 rhs)
{
	__asm
	{
		cvttps2dq	xmm0, xmm0
		psrad	xmm0, 0xC
		pshuflw	xmm1, xmm0, 2
		movd[ecx], xmm1
		retn
	}
}

__declspec(naked) void __fastcall NiReleaseObject(NiRefObject* toRelease)
{
	__asm
	{
		lock dec dword ptr[ecx + 4]
		jg		done
		mov		eax, [ecx]
		call	dword ptr[eax + 4]
		done:
		retn
	}
}

__declspec(naked) NiRefObject** __stdcall NiReplaceObject(void* toRelease, NiRefObject* toAdd)
{
	__asm
	{
		mov		eax, [esp + 4]
		mov		ecx, [eax]
		cmp		ecx, [esp + 8]
		jz		done
		test	ecx, ecx
		jz		doAdd
		lock dec dword ptr[ecx + 4]
		jg		doAdd
		mov		eax, [ecx]
		call	dword ptr[eax + 4]
		mov		eax, [esp + 4]
		doAdd:
		mov		ecx, [esp + 8]
			mov[eax], ecx
			test	ecx, ecx
			jz		done
			lock inc dword ptr[ecx + 4]
			done :
			retn	8
	}
}

ExtraCount* ExtraCount::Create(SInt32 count)
{
	CreateExtraType(ExtraCount)
		dataPtr[3] = (count > SHRT_MAX) ? SHRT_MAX : count;
	return (ExtraCount*)dataPtr;
}

ExtraCount* ExtraDataList::AddExtraCount(SInt32 count)
{
	return (ExtraCount*)AddExtra(ExtraCount::Create(count));
}

__declspec(naked) ExtraDataList* ExtraDataList::CreateCopy(bool bCopyAndRemove)
{
	__asm
	{
		push	esi
		mov		esi, ecx
		push	0x20
		call	Game_DoHeapAlloc
		xorps	xmm0, xmm0
		movups[eax], xmm0
		movups[eax + 0x10], xmm0
		mov		dword ptr[eax], kVtbl_ExtraDataList
		movzx	edx, byte ptr[esp + 8]
		push	edx
		push	esi
		mov		esi, eax
		mov		ecx, eax
		CALL_EAX(0x412490)
		mov		eax, esi
		pop		esi
		retn	4
	}
}

const char* kExtraDataNames[] =
{
	"Unknown00", "Havok", "Cell3D", "CellWaterType", "RegionList", "SeenData", "EditorID", "CellMusicType", "CellClimate",
	"ProcessMiddleLow", "CellCanopyShadowMask", "DetachTime", "PersistentCell", "Script", "Action", "StartingPosition",
	"Anim", "NoStack", "UsedMarkers", "DistantData", "RagdollData", "ContainerChanges", "Worn", "WornLeft", "PackageStartLocation",
	"Package", "TrespassPackage", "RunOncePacks", "ReferencePointer", "Follower", "LevCreaModifier", "Ghost", "OriginalReference",
	"Ownership", "Global", "Rank", "Count", "Health", "Uses", "JIP", "Charge", "Light", "Lock", "Teleport", "MapMarker",
	"Unknown2D", "LeveledCreature", "LeveledItem", "Scale", "Seed", "NonActorMagicCaster", "NonActorMagicTarget", "Unknown34",
	"PlayerCrimeList", "Unknown36", "EnableStateParent", "EnableStateChildren", "ItemDropper", "DroppedItemList", "RandomTeleportMarker",
	"MerchantContainer", "SavedHavokData", "CannotWear", "Poison", "Unknown40", "LastFinishedSequence", "SavedAnimation",
	"NorthRotation", "XTarget", "FriendHits", "HeadingTarget", "Unknown47", "RefractionProperty", "StartingWorldOrCell", "Hotkey",
	"Unknown4B", "EditorRefMovedData", "InfoGeneralTopic", "HasNoRumors", "Sound", "TerminalState", "LinkedRef", "LinkedRefChildren",
	"ActivateRef", "ActivateRefChildren", "TalkingActor", "ObjectHealth", "DecalRefs", "Unknown58", "CellImageSpace", "NavMeshPortal",
	"ModelSwap", "Radius", "Radiation", "FactionChanges", "DismemberedLimbs", "ActorCause", "MultiBound", "MultiBoundData",
	"MultiBoundRef", "Unknown64", "ReflectedRefs", "ReflectorRefs", "EmittanceSource", "RadioData", "CombatStyle", "Unknown6A",
	"Primitive", "OpenCloseActivateRef", "AnimNoteReciever", "Ammo", "PatrolRefData", "PackageData", "OcclusionPlane", "CollisionData",
	"SayTopicInfoOnceADay", "EncounterZone", "SayToTopicInfo", "OcclusionPlaneRefData", "PortalRefData", "Portal", "Room", "HealthPerc",
	"RoomRefData", "GuardedRefData", "CreatureAwakeSound", "WaterZoneMap", "Unknown7F", "IgnoredBySandbox", "CellAcousticSpace",
	"ReservedMarkers", "WeaponIdleSound", "WaterLightRefs", "LitWaterRefs", "WeaponAttackSound", "ActivateLoopSound",
	"PatrolRefInUseData", "AshPileRef", "CreatureMovementSound", "FollowerSwimBreadcrumbs", "CellImpactSwap", "WeaponModFlags",
	"ModdingItem", "SecuritronFace", "AudioMarker", "AudioBuoyMarker", "SpecialRenderFlags"
};