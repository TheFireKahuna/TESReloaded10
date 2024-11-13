#include "GameNi.h"


__declspec(naked) NiObject* __fastcall NiObject::HasBaseType(const NiRTTI* baseType)
{
	__asm
	{
		mov		eax, [ecx]
		call	dword ptr[eax + 8]
		ALIGN 16
		iterHead:
		test	eax, eax
			jz		done
			cmp		eax, edx
			mov		eax, [eax + 4]
			jnz		iterHead
			mov		eax, ecx
			done :
		retn
	}
}
template <typename T>
UInt16	NiTArray<T>::Add(T* Item) {
    return (UInt16)ThisStdCall(0x00A5EB20, this, Item);
}
template class NiTArray<NiAVObject*>; //CURSED

float NiAVObject::GetDistance(NiPoint3* Point) {

    NiPoint3 v;

    v.x = this->m_worldTransform.translate.x - Point->x;
    v.y = this->m_worldTransform.translate.y - Point->y;
    v.z = this->m_worldTransform.translate.z - Point->z;
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

NiBound* NiAVObject::GetWorldBound() { 
    return m_kWorldBound;
}

float	NiAVObject::GetWorldBoundRadius() {
    return (m_kWorldBound ? m_kWorldBound->Radius : 0.0f); 
}

void NiNode::New(UInt16 Children) { 
    ThisStdCall(0x00A5ECB0, this, Children);
}

void	SceneGraph::UpdateParticleShaderFoV(float FoV) {
    void (__cdecl* UpdateParticleShaderFoVData)(float) = (void (__cdecl*)(float))0x00B54000;
    ThisStdCall(0x00C52020, this, FoV, 0, NULL, 0);
    UpdateParticleShaderFoVData(FoV);
}

void	SceneGraph::SetNearDistance(float Distance) { 
    float* SettingNearDistance = (float*)0x01203148;
    *SettingNearDistance = Distance;
}

NiProperty*	 NiGeometry::GetProperty(NiProperty::PropertyType Type) {
    return propertyState.prop[Type];
}


bool NiSkinInstance::IsPartitionEnabled(UInt32 partitionIndex) {
    void* VFT = *(void**)this;
    if (VFT == Pointers::VirtualTables::BSDismemberSkinInstance){
        BSDismemberSkinInstance* t = (BSDismemberSkinInstance*) this;
        if (t->IsRenderable == 0) return false;
        if (t->partitions && t->partitionNumber < partitionIndex){
            DismemberPartition p = t->partitions[partitionIndex];
            if (!p.Enabled) return false;
        }
    }
    return true;
} 

void NiObject::LogObjectAttributes(){
    NiTArray<char*>* debug = (NiTArray<char*>*) Pointers::Functions::FormMemoryAlloc(sizeof(NiTArray<char*>));
    memset(debug, 0, sizeof(NiTArray<char*>));
    *(void**)debug = (void*)0x0102102C;  //NiTArray<char*> vtbl;
    debug->growSize = 16; //Must be initialized > 0    
    this->GetViewerStrings(debug);
    for (int i = 0; i < debug->numObjs; i++){Logger::Log("%s", debug->data[i]); }
    Pointers::Functions::FormMemoryDeAlloc(debug);
}

void NiDX9Renderer::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE State, DWORD Value) {
    renderState->SetSamplerState(Sampler, State, Value, false);
}

void NiDX9Renderer::PackGeometryBuffer(NiGeometryBufferData* GeoData, NiGeometryData* ModelData, NiSkinInstance* SkinInstance, NiD3DShaderDeclaration* ShaderDeclaration) {
    
}

void NiDX9Renderer::PackSkinnedGeometryBuffer(NiGeometryBufferData* GeoData, NiGeometryData* ModelData, NiSkinInstance* SkinInstance, NiSkinPartition::Partition* Partition, NiD3DShaderDeclaration* ShaderDeclaration){
    
}

void NiDX9Renderer::CalculateBoneMatrixes(NiSkinInstance* SkinInstance, NiTransform* WorldTrasform) {
    ThisStdCall(0x00E6FE30, this, SkinInstance, WorldTrasform, false, 3, true); 
}

bool BSShaderProperty::IsLightingProperty() {
    return (shaderType == ShaderDefinitionEnum::kShaderDefinition_ShadowLightShader || shaderType == ShaderDefinitionEnum::kShaderDefinition_Lighting30Shader);
}

__declspec(naked) NiNode* __stdcall NiNode::Create(const char* nameStr)	//	str of NiFixedString
{
	__asm
	{
		CALL_EAX(0xA5F030)
		mov		ecx, [esp + 4]
		test	ecx, ecx
		jz		done
		lock inc dword ptr[ecx - 8]
		mov[eax + 8], ecx
		done :
		retn	4
	}
}

template <typename T_Key, typename T_Data>
__declspec(naked) T_Data __fastcall NiTMap<T_Key, T_Data>::Lookup(T_Key key) const
{
	__asm
	{
		push	ebx
		push	esi
		push	edi
		mov		ebx, ecx
		mov		edi, edx
		push	edx
		mov		esi, [ecx]
		call	dword ptr[esi + 4]
		mov		edx, [ebx + 8]
		mov		ebx, [esi + 8]
		mov		esi, [edx + eax * 4]
		ALIGN 16
		iterHead:
		test	esi, esi
			jz		retnNull
			push	dword ptr[esi + 4]
			push	edi
			call	ebx
			mov		ecx, esi
			mov		esi, [esi]
			test	al, al
			jz		iterHead
			mov		eax, [ecx + 8]
			pop		edi
			pop		esi
			pop		ebx
			retn
			retnNull :
		xor eax, eax
			pop		edi
			pop		esi
			pop		ebx
			retn
	}
}

template <typename T_Key, typename T_Data>
__declspec(naked) void _fastcall NiTMap<T_Key, T_Data>::FreeBuckets()
{
	__asm
	{
		cmp		dword ptr[ecx + 0xC], 0
		jz		done
		push	ebx
		mov		ebx, ecx
		mov		ecx, 0x11C5F80
		call	LightCS::Enter
		push	esi
		push	edi
		mov		esi, [ebx + 8]
		mov		edi, [ebx + 4]
		and dword ptr[ebx + 0xC], 0
		mov		ebx, 0x11C5F58
		xorps	xmm0, xmm0
		ALIGN 16
		bucketIter:
		dec		edi
			js		bucketEnd
			mov		ecx, [esi]
			add		esi, 4
			test	ecx, ecx
			jz		bucketIter
			ALIGN 16
			entryIter:
		mov		eax, ecx
			mov		ecx, [ecx]
			movlps[eax + 4], xmm0
			mov		edx, [ebx]
			mov[eax], edx
			mov[ebx], eax
			test	ecx, ecx
			jnz		entryIter
			mov[esi - 4], ecx
			jmp		bucketIter
			ALIGN 16
			bucketEnd:
		mov		ecx, 0x11C5F80
			dec		dword ptr[ecx + 4]
			jnz		inUse
			and dword ptr[ecx], 0
			inUse :
			pop		edi
			pop		esi
			pop		ebx
			done :
		retn
	}
}

unsigned long NiTMap<unsigned long, unsigned long>::Lookup(unsigned long key) const;
void NiTMap<unsigned long, unsigned long>::FreeBuckets();

__declspec(noinline) NiObjectCopyInfo* GetNiObjectCopyInfo()
{
	thread_local static NiObjectCopyInfo s_NiObjectCopyInfo(0x97);
	return &s_NiObjectCopyInfo;
}

const NiUpdateData kNiUpdateData;

__declspec(naked) void NiAVObject::Update()
{
	__asm
	{
		push	ecx
		push	0
		push	offset kNiUpdateData
		mov		eax, [ecx]
		call	dword ptr[eax + 0xA4]
		pop		ecx
		mov		ecx, [ecx + 0x18]
		test	ecx, ecx
		jz		done
		mov		eax, [ecx]
		call	dword ptr[eax + 0xFC]
		done:
		retn
	}
}

__declspec(naked) UInt32 NiAVObject::GetIndex() const
{
	__asm
	{
		mov		eax, [ecx + 0x18]
		test	eax, eax
		jz		done
		mov		edx, [eax + 0xA0]
		movzx	eax, word ptr[eax + 0xA6]
		test	eax, eax
		jz		done
		sub		edx, 4
		ALIGN 16
		iterHead:
		cmp[edx + eax * 4], ecx
			jz		done
			dec		eax
			jnz		iterHead
			done :
		retn
	}
}

__declspec(naked) bool NiAVObject::ReplaceObject(NiAVObject* object)
{
	__asm
	{
		mov		edx, [ecx + 0x18]
		test	edx, edx
		jz		done
		mov		eax, [edx + 0xA0]
		movzx	edx, word ptr[edx + 0xA6]
		ALIGN 16
		iterHead:
		dec		edx
			js		done
			cmp[eax + edx * 4], ecx
			jnz		iterHead
			lea		eax, [eax + edx * 4]
			mov		edx, [esp + 4]
			push	edx
			push	eax
			mov		eax, [ecx + 0x18]
			and dword ptr[ecx + 0x18], 0
			mov[edx + 0x18], eax
			call	NiReplaceObject
			mov		al, 1
			retn	4
			ALIGN 16
			done:
		xor al, al
			retn	4
	}
}

__declspec(naked) void __fastcall NiGeometry::AddProperty(NiProperty* niProperty)
{
	__asm
	{
		lock inc dword ptr[edx + 4]
		push	ecx
		push	edx
		CALL_EAX(0x43A010)
		pop		dword ptr[eax + 8]
		pop		ecx
		inc		dword ptr[ecx + 0x2C]
		mov		edx, [ecx + 0x24]
		mov[ecx + 0x24], eax
		test	edx, edx
		jz		emptyList
		mov[eax], edx
		mov[edx + 4], eax
		retn
		emptyList :
		mov[ecx + 0x28], eax
			retn
	}
}

__declspec(naked) TESObjectREFR* NiAVObject::GetParentRef() const
{
	__asm
	{
	iterHead:
		test	ecx, ecx
			jz		retnNULL
			mov		eax, ecx
			mov		ecx, [ecx + 0x18]
			cmp		dword ptr[eax], kVtbl_BSFadeNode
			jnz		iterHead
			mov		eax, [eax + 0xCC]
			test	eax, eax
			jz		iterHead
			retn
			ALIGN 16
			retnNULL:
		xor eax, eax
			retn
	}
}

__declspec(naked) void NiAVObject::AssignGeometryProps()
{
	__asm
	{
		push	0
		push	0
		push	0
		push	ecx
		CALL_EAX(0xA5A040)
		CALL_EAX(0xB57BD0)
		add		esp, 0x10
		retn
	}
}

__declspec(naked) NiAVObject* __fastcall NiNode::GetBlockByName(const char* nameStr) const	//	str of NiFixedString
{
	__asm
	{
		movzx	eax, word ptr[ecx + 0xA6]
		test	eax, eax
		jz		done
		push	esi
		push	edi
		mov		esi, [ecx + 0xA0]
		mov		edi, eax
		ALIGN 16
		iterHead:
		dec		edi
			js		iterEnd
			mov		eax, [esi]
			add		esi, 4
			test	eax, eax
			jz		iterHead
			cmp[eax + 8], edx
			jz		found
			mov		ecx, [eax]
			cmp		dword ptr[ecx + 0xC], ADDR_ReturnThis
			jnz		iterHead
			mov		ecx, eax
			call	NiNode::GetBlockByName
			test	eax, eax
			jz		iterHead
			found :
		pop		edi
			pop		esi
			retn
			ALIGN 16
			iterEnd :
			xor eax, eax
			pop		edi
			pop		esi
			done :
		retn
	}
}

__declspec(naked) NiAVObject* __fastcall NiNode::GetBlock(const char* blockName) const
{
	__asm
	{
		cmp[edx], 0
		jz		retnNULL
		push	ecx
		push	edx
		call	GetNiFixedString
		pop		ecx
		pop		ecx
		lock dec dword ptr[eax - 8]
		jz		retnNULL
		cmp[ecx + 8], eax
		jz		found
		mov		edx, eax
		jmp		NiNode::GetBlockByName
		found :
		mov		eax, ecx
			retn
			retnNULL :
		xor eax, eax
			retn
	}
}

__declspec(naked) NiNode* __fastcall NiNode::GetNode(const char* nodeName) const
{
	__asm
	{
		call	NiNode::GetBlock
		test	eax, eax
		jz		done
		xor edx, edx
		mov		ecx, [eax]
		cmp		dword ptr[ecx + 0xC], ADDR_ReturnThis
		cmovnz	eax, edx
		done :
		retn
	}
}

__declspec(naked) NiAVObject* __fastcall NiNode::FindBlockOfType(UInt32 typeVtbl) const
{
	__asm
	{
		mov		eax, ecx
		cmp[eax], edx
		jz		done
		movzx	eax, word ptr[ecx + 0xA6]
		test	eax, eax
		jz		done
		push	esi
		push	edi
		mov		esi, [ecx + 0xA0]
		mov		edi, eax
		ALIGN 16
		iterHead:
		dec		edi
			js		iterEnd
			mov		ecx, [esi]
			add		esi, 4
			test	ecx, ecx
			jz		iterHead
			mov		eax, [ecx]
			cmp		dword ptr[eax + 0xC], ADDR_ReturnThis
			jnz		notNode
			call	NiNode::FindBlockOfType
			test	eax, eax
			jz		iterHead
			pop		edi
			pop		esi
			retn
			ALIGN 16
			notNode:
		cmp		eax, edx
			jnz		iterHead
			mov		eax, ecx
			pop		edi
			pop		esi
			retn
			ALIGN 16
			iterEnd:
		xor eax, eax
			pop		edi
			pop		esi
			done :
		retn
	}
}

__declspec(naked) bool NiNode::IsMovable() const
{
	__asm
	{
		mov		eax, [ecx + 0x1C]
		test	eax, eax
		jz		immovable
		mov		eax, [eax + 0x10]
		test	eax, eax
		jz		immovable
		mov		eax, [eax + 8]
		test	byte ptr[eax + 0xE8], 2
		setnz	al
		jz		immovable
		retn
		ALIGN 16
		immovable:
		push	esi
			push	edi
			mov		esi, [ecx + 0xA0]
			movzx	edi, word ptr[ecx + 0xA6]
			ALIGN 16
			iterHead :
			dec		edi
			js		done
			mov		ecx, [esi]
			add		esi, 4
			test	ecx, ecx
			jz		iterHead
			mov		edx, [ecx]
			cmp		dword ptr[edx + 0xC], ADDR_ReturnThis
			jnz		iterHead
			call	NiNode::IsMovable
			test	al, al
			jz		iterHead
			done :
		pop		edi
			pop		esi
			retn
	}
}

__declspec(naked) void __vectorcall NiNode::SetAlphaRecurse(float alpha)
{
	__asm
	{
		movzx	eax, word ptr[ecx + 0xA6]
		test	eax, eax
		jz		done
		mov		edx, [ecx + 0x20]
		test	edx, edx
		jz		done
		cmp		dword ptr[edx + 0xC], 0
		jz		done
		push	esi
		push	edi
		mov		esi, [ecx + 0xA0]
		mov		edi, eax
		ALIGN 16
		iterHead:
		dec		edi
			js		iterEnd
			mov		ecx, [esi]
			add		esi, 4
			test	ecx, ecx
			jz		iterHead
			mov		eax, [ecx]
			cmp		dword ptr[eax + 0xC], ADDR_ReturnThis
			jnz		notNode
			call	NiNode::SetAlphaRecurse
			jmp		iterHead
			ALIGN 16
			notNode:
		cmp		dword ptr[eax + 0x18], ADDR_ReturnThis2
			jnz		iterHead
			mov		eax, [ecx + 0xA8]
			test	eax, eax
			jz		iterHead
			movss[eax + 0x2C], xmm0
			jmp		iterHead
			ALIGN 16
			iterEnd:
		pop		edi
			pop		esi
			done :
		retn
	}
}

__declspec(naked) void NiNode::ResetShaderRenderPass()
{
	__asm
	{
		movzx	eax, word ptr[ecx + 0xA6]
		test	eax, eax
		jz		done
		mov		edx, [ecx + 0x20]
		test	edx, edx
		jz		done
		cmp		dword ptr[edx + 0xC], 0
		jz		done
		push	esi
		push	edi
		mov		esi, [ecx + 0xA0]
		mov		edi, eax
		ALIGN 16
		iterHead:
		dec		edi
			js		iterEnd
			mov		ecx, [esi]
			add		esi, 4
			test	ecx, ecx
			jz		iterHead
			mov		eax, [ecx]
			cmp		dword ptr[eax + 0xC], ADDR_ReturnThis
			jnz		notNode
			call	NiNode::ResetShaderRenderPass
			jmp		iterHead
			ALIGN 16
			notNode:
		cmp		dword ptr[eax + 0x18], ADDR_ReturnThis2
			jnz		iterHead
			mov		eax, [ecx + 0xA8]
			test	eax, eax
			jz		iterHead
			and dword ptr[eax + 0x38], 0
			jmp		iterHead
			ALIGN 16
			iterEnd:
		pop		edi
			pop		esi
			done :
		retn
	}
}

__declspec(naked) UInt32 NiNode::GetBSXFlags() const
{
	__asm
	{
		push	esi
		movzx	edx, word ptr[ecx + 0x14]
		mov		ecx, [ecx + 0x10]
		mov		esi, kVtbl_BSXFlags
		ALIGN 16
		iterHead:
		dec		edx
			js		done
			mov		eax, [ecx + edx * 4]
			cmp[eax], esi
			jnz		iterHead
			mov		eax, [eax + 0xC]
			pop		esi
			retn
			done :
		xor eax, eax
			pop		esi
			retn
	}
}

__declspec(naked) void __fastcall BSFadeNode::SetVisible(bool visible)
{
	__asm
	{
		movzx	eax, dl
		neg		eax
		and eax, 0x3F800000
		movd	xmm0, eax
		unpcklps	xmm0, xmm0
		shl		dl, 6
		and byte ptr[ecx + 0x31], 0xBF
		or [ecx + 0x31], dl
		movlps[ecx + 0xB4], xmm0
		jmp		NiNode::SetAlphaRecurse
	}
}

__declspec(naked) void NiNode::AddPropertyRecurse(NiProperty* niProperty)
{
	__asm
	{
		movzx	eax, word ptr[ecx + 0xA6]
		test	eax, eax
		jz		done
		push	esi
		push	edi
		mov		esi, [ecx + 0xA0]
		mov		edi, eax
		ALIGN 16
		iterHead:
		dec		edi
			js		iterEnd
			mov		ecx, [esi]
			add		esi, 4
			test	ecx, ecx
			jz		iterHead
			mov		eax, [ecx]
			cmp		dword ptr[eax + 0xC], ADDR_ReturnThis
			jnz		notNode
			push	dword ptr[esp + 0xC]
			call	NiNode::AddPropertyRecurse
			jmp		iterHead
			ALIGN 16
			notNode:
		cmp		dword ptr[eax + 0x18], ADDR_ReturnThis2
			jnz		iterHead
			mov		edx, [esp + 0xC]
			call	NiGeometry::AddProperty
			jmp		iterHead
			iterEnd :
		pop		edi
			pop		esi
			done :
		retn	4
	}
}

__declspec(naked) void NiNode::AddNoLightingPropertyRecurse()
{
	__asm
	{
		movzx	eax, word ptr[ecx + 0xA6]
		test	eax, eax
		jz		done
		push	esi
		push	edi
		mov		esi, [ecx + 0xA0]
		mov		edi, eax
		ALIGN 16
		iterHead:
		dec		edi
			js		iterEnd
			mov		ecx, [esi]
			add		esi, 4
			test	ecx, ecx
			jz		iterHead
			mov		eax, [ecx]
			cmp		dword ptr[eax + 0xC], ADDR_ReturnThis
			jnz		notNode
			call	NiNode::AddNoLightingPropertyRecurse
			jmp		iterHead
			ALIGN 16
			notNode:
		cmp		dword ptr[eax + 0x18], ADDR_ReturnThis2
			jnz		iterHead
			push	ecx
			push	0x80
			CALL_EAX(0xAA13E0)
			pop		ecx
			mov		ecx, eax
			CALL_EAX(0xB6FC90)
			mov		edx, eax
			pop		ecx
			call	NiGeometry::AddProperty
			jmp		iterHead
			iterEnd :
		pop		edi
			pop		esi
			done :
		retn
	}
}

__declspec(naked) void __vectorcall NiNode::SetMaterialPropValueRecurse(UInt32 traitID, float value)
{
	__asm
	{
		movzx	eax, word ptr[ecx + 0xA6]
		test	eax, eax
		jz		done
		push	esi
		push	edi
		mov		esi, [ecx + 0xA0]
		mov		edi, eax
		ALIGN 16
		iterHead:
		dec		edi
			js		iterEnd
			mov		ecx, [esi]
			add		esi, 4
			test	ecx, ecx
			jz		iterHead
			mov		eax, [ecx]
			cmp		dword ptr[eax + 0xC], ADDR_ReturnThis
			jnz		notNode
			call	NiNode::SetMaterialPropValueRecurse
			jmp		iterHead
			ALIGN 16
			notNode:
		cmp		dword ptr[eax + 0x18], ADDR_ReturnThis2
			jnz		iterHead
			mov		ecx, [ecx + 0xA4]
			test	ecx, ecx
			jz		iterHead
			push	edx
			call	NiMaterialProperty::SetTraitValue
			pop		edx
			jmp		iterHead
			ALIGN 16
			iterEnd:
		pop		edi
			pop		esi
			done :
		retn
	}
}

__declspec(naked) bool __fastcall NiCamera::WorldToScreen(const NiVector3& worldPos, NiPoint2& scrPos)
{
	__asm
	{
		add		ecx, 0x9C
		movups	xmm0, [ecx]
		movups	xmm1, [ecx + 0x10]
		movups	xmm2, [ecx + 0x30]
		movups	xmm3, [edx]
		andps	xmm3, PS_XYZ0Mask
		pshufd	xmm4, PS_V3_One, 0x3F
		orps	xmm3, xmm4
		xorps	xmm4, xmm4
		mulps	xmm0, xmm3
		haddps	xmm0, xmm4
		haddps	xmm0, xmm4
		mulps	xmm1, xmm3
		haddps	xmm1, xmm4
		haddps	xmm1, xmm4
		unpcklps	xmm0, xmm1
		mulps	xmm2, xmm3
		haddps	xmm2, xmm4
		haddps	xmm2, xmm4
		comiss	xmm2, xmm4
		setnz	al
		jz		done
		movss	xmm1, SS_1d1K
		cmpnltss	xmm1, xmm2
		movmskps	eax, xmm1
		movss	xmm3, PS_FlipSignMask0
		andps	xmm1, xmm3
		xorps	xmm2, xmm1
		unpcklps	xmm2, xmm2
		divps	xmm0, xmm2
		movups	xmm1, [ecx + 0x64]
		shufps	xmm1, xmm1, 0xE1
		pshufd	xmm2, PS_V3_Half, 0
		mulps	xmm1, xmm2
		movaps	xmm2, xmm1
		hsubps	xmm2, xmm4
		mulps	xmm0, xmm2
		shufps	xmm3, xmm3, 0x45
		xorps	xmm1, xmm3
		haddps	xmm1, xmm4
		addps	xmm0, xmm1
		shufps	xmm3, xmm3, 8
		xorps	xmm0, xmm3
		xor al, 1
		jz		done
		unpcklpd	xmm0, xmm0
		movq	xmm1, qword ptr PS_V3_One
		pshufd	xmm2, xmm1, 0
		shufps	xmm3, xmm3, 0x50
		xorps	xmm0, xmm3
		xorps	xmm2, xmm3
		cmpltps	xmm1, xmm0
		movmskps	edx, xmm1
		test	dl, dl
		jz		done
		andps	xmm1, xmm2
		shufps	xmm1, xmm1, 0xD8
		haddps	xmm1, xmm4
		subps	xmm0, xmm1
		xor al, al
		done :
		mov		edx, [esp + 4]
			movlps[edx], xmm0
			retn	4
	}
}

__declspec(naked) void NiGeometryData::FlipNormals()
{
	__asm
	{
		mov		eax, [ecx + 0x24]
		test	eax, eax
		jz		done
		movzx	edx, word ptr[ecx + 8]
		test	edx, edx
		jz		done
		lea		ecx, [edx + edx * 2]
		shl		ecx, 2
		mov		edx, 0x80000000
		movaps	xmm1, PS_FlipSignMask
		Iter4 :
		test	cl, 0xF
			jz		iter10
			xor [eax + ecx - 4], edx
			sub		ecx, 4
			jnz		Iter4
			retn
			ALIGN 16
			iter10:
		sub		ecx, 0x10
			movups	xmm0, [eax + ecx]
			xorps	xmm0, xmm1
			movups[eax + ecx], xmm0
			jnz		Iter10
			done :
		retn
	}
}

__declspec(naked) NiTriStripsData* __vectorcall NiTriStripsData::DrawConvex(float radius, UInt32 numEdges, const NiColorAlpha& color)
{
	__asm
	{
		push	ebx
		mov		ebx, ecx
		movq	xmm7, xmm0
		movups	xmm6, [edx]
		lea		eax, [ecx + ecx + 2]
		push	eax
		call	Game_DoHeapAlloc
		push	eax
		lea		ecx, [eax + ebx * 2]
		mov[ecx], bx
		push	ecx
		xor ecx, ecx
		ALIGN 16
		pntIter:
		mov[eax + ecx * 2], cx
			inc		ecx
			cmp		ecx, ebx
			jb		pntIter
			push	1
			lea		eax, [ebx - 2]
			push	eax
			push	0
			push	0
			push	0
			mov		eax, ebx
			shl		eax, 4
			push	eax
			call	Game_DoHeapAlloc
			push	eax
			mov		edx, ebx
			ALIGN 16
			colIter:
		movups[eax], xmm6
			add		eax, 0x10
			dec		edx
			jnz		colIter
			push	0
			cvtsi2ss	xmm1, ebx
			movss	xmm0, PS_V3_PIx2
			divss	xmm0, xmm1
			call	GetSinCos
			movss	xmm5, PS_FlipSignMask0
			movq	xmm1, xmm0
			xorps	xmm0, xmm5
			shufps	xmm0, xmm1, 0x41
			lea		eax, [ebx + ebx * 2]
			shl		eax, 2
			push	eax
			push	eax
			call	Game_DoHeapAlloc
			pop		ecx
			push	eax
			push	ebx
			pshufd	xmm1, xmm7, 0x51
			movups[eax], xmm1
			xor edx, edx
			test	bl, 1
			jnz		skipLast
			lea		ecx, [eax + ecx - 0xC]
			movlps[ecx], xmm1
			mov[ecx + 8], edx
			xor byte ptr[ecx + 7], 0x80
			skipLast:
		add		eax, 0xC
			dec		ebx
			shr		ebx, 1
			ALIGN 16
			vtxIter :
			pshufd	xmm2, xmm1, 0x44
			mulps	xmm2, xmm0
			haddps	xmm2, xmm2
			movq	xmm1, xmm2
			xorps	xmm2, xmm5
			movlps[eax], xmm2
			mov[eax + 8], edx
			movlps[eax + 0xC], xmm1
			mov[eax + 0x14], edx
			add		eax, 0x18
			dec		ebx
			jnz		vtxIter
			push	0x50
			CALL_EAX(0xAA13E0)
			pop		ecx
			mov		ecx, eax
			CALL_EAX(0xA75DC0)
			mov		ebx, eax
			mov		ecx, eax
			CALL_EAX(0xA8AB40)
			mov		eax, ebx
			pop		ebx
			retn
	}
}

__declspec(naked) NiTriStripsData* __vectorcall NiTriStripsData::DrawCylinder(float radius, float height, UInt32 numEdges, const NiColorAlpha& color)
{
	__asm
	{
		push	ebx
		push	esi
		lea		ebx, [ecx + ecx + 2]
		movups	xmm5, [edx]
		pshufd	xmm6, xmm0, 0x51
		movq	xmm7, xmm1
		lea		eax, [ebx + ebx + 4]
		push	eax
		call	Game_DoHeapAlloc
		push	eax
		mov		esi, eax
		lea		ecx, [eax + ebx * 2]
		push	ecx
		mov[ecx], bx
		mov		dword ptr[ecx - 4], 0x10000
		sub		ebx, 2
		push	1
		push	ebx
		push	0
		push	0
		push	0
		mov		eax, ebx
		shl		eax, 4
		push	eax
		push	eax
		call	Game_DoHeapAlloc
		pop		edx
		push	eax
		push	0
		xor ecx, ecx
		ALIGN 16
		pntIter:
		mov[esi + ecx * 2], cx
			inc		ecx
			sub		edx, 0x10
			movups[eax + edx], xmm5
			jnz		pntIter
			mov		eax, ebx
			shr		eax, 1
			cvtsi2ss	xmm1, eax
			movss	xmm0, PS_V3_PIx2
			divss	xmm0, xmm1
			call	GetSinCos
			movss	xmm5, PS_FlipSignMask0
			movq	xmm1, xmm0
			xorps	xmm0, xmm5
			shufps	xmm0, xmm1, 0x41
			lea		esi, [ebx + ebx * 2]
			shl		esi, 2
			push	esi
			call	Game_DoHeapAlloc
			push	eax
			push	ebx
			movq	xmm1, xmm6
			movups[eax], xmm1
			movlps[eax + 0xC], xmm1
			movss[eax + 0x14], xmm7
			lea		ecx, [eax + esi - 0x18]
			xor edx, edx
			test	bl, 2
			jnz		skipLast
			shr		esi, 1
			add		esi, eax
			movlps[esi], xmm1
			mov[esi + 8], edx
			xor byte ptr[esi + 7], 0x80
			movlps[esi + 0xC], xmm1
			movss[esi + 0x14], xmm7
			xor byte ptr[esi + 0x13], 0x80
			skipLast:
		add		eax, 0x18
			dec		ebx
			shr		ebx, 2
			ALIGN 16
			vtxIter :
			pshufd	xmm2, xmm1, 0x44
			mulps	xmm2, xmm0
			haddps	xmm2, xmm2
			movq	xmm1, xmm2
			xorps	xmm2, xmm5
			movlps[eax], xmm2
			mov[eax + 8], edx
			movlps[eax + 0xC], xmm2
			movss[eax + 0x14], xmm7
			movlps[ecx], xmm1
			mov[ecx + 8], edx
			movlps[ecx + 0xC], xmm1
			movss[ecx + 0x14], xmm7
			add		eax, 0x18
			sub		ecx, 0x18
			dec		ebx
			jnz		vtxIter
			push	0x50
			CALL_EAX(0xAA13E0)
			pop		ecx
			mov		ecx, eax
			CALL_EAX(0xA75DC0)
			mov		ebx, eax
			mov		ecx, eax
			CALL_EAX(0xA8AB40)
			mov		eax, ebx
			pop		esi
			pop		ebx
			retn
	}
}

__declspec(naked) NiTriStripsData* __vectorcall NiTriStripsData::DrawPrism(float radius, float height, UInt32 numEdges, const NiColorAlpha& color)
{
	__asm
	{
		push	ebx
		mov		ebx, ecx
		push	esi
		lea		esi, [ecx * 4]
		push	edi
		movups	xmm5, [edx]
		pshufd	xmm6, xmm0, 0x51
		movq	xmm7, xmm1
		lea		eax, [ecx * 8 + 0xC]
		push	eax
		call	Game_DoHeapAlloc
		push	eax
		mov		esi, eax
		lea		ecx, [eax + ebx * 8 + 4]
		push	ecx
		lea		edx, [ebx + ebx + 2]
		mov[ecx], bx
		mov[ecx + 2], bx
		mov[ecx + 4], dx
		push	3
		lea		edi, [ebx * 4]
		lea		ecx, [edi - 4]
		push	ecx
		push	0
		push	0
		push	0
		shl		edi, 4
		push	edi
		call	Game_DoHeapAlloc
		push	eax
		push	0
		xor ecx, ecx
		ALIGN 16
		pntIter:
		mov[esi + ecx * 2], cx
			inc		ecx
			sub		edi, 0x10
			movups[eax + edi], xmm5
			jnz		pntIter
			mov		edx, [esi + ebx * 4]
			mov[esi + ecx * 2], edx
			cvtsi2ss	xmm1, ebx
			movss	xmm0, PS_V3_PIx2
			divss	xmm0, xmm1
			call	GetSinCos
			movss	xmm5, PS_FlipSignMask0
			movq	xmm1, xmm0
			xorps	xmm0, xmm5
			shufps	xmm0, xmm1, 0x41
			lea		edi, [ebx + ebx * 2]
			shl		edi, 4
			push	edi
			call	Game_DoHeapAlloc
			push	eax
			lea		ecx, [ebx * 4]
			push	ecx
			shr		edi, 2
			lea		edx, [eax + edi]
			lea		esi, [edx + edi]
			movq	xmm1, xmm6
			movq	xmm2, xmm1
			unpcklpd	xmm2, xmm7
			movups[eax], xmm1
			movups[edx], xmm2
			movups[esi], xmm1
			movups[esi + 0xC], xmm2
			xor ecx, ecx
			test	bl, 1
			jnz		skipLast
			pshufd	xmm3, xmm5, 0x51
			xorps	xmm2, xmm3
			movlps[edx - 0xC], xmm2
			mov[edx - 4], ecx
			movlps[esi - 0xC], xmm2
			movss[esi - 4], xmm7
			movlps[esi + edi], xmm2
			mov[esi + edi + 8], ecx
			movlps[esi + edi + 0xC], xmm2
			movss[esi + edi + 0x14], xmm7
			skipLast :
		add		eax, 0xC
			add		edx, 0xC
			lea		edi, [esi + edi * 2 - 0x18]
			add		esi, 0x18
			dec		ebx
			shr		ebx, 1
			xorps	xmm3, xmm3
			ALIGN 16
			vtxIter:
		pshufd	xmm2, xmm1, 0x44
			mulps	xmm2, xmm0
			haddps	xmm2, xmm3
			movq	xmm1, xmm2
			xorps	xmm2, xmm5
			movlps[eax], xmm2
			mov[eax + 8], ecx
			movlps[edx + 0xC], xmm2
			movss[edx + 0x14], xmm7
			movlps[esi], xmm2
			mov[esi + 8], ecx
			movlps[esi + 0xC], xmm2
			movss[esi + 0x14], xmm7
			movlps[eax + 0xC], xmm1
			mov[eax + 0x14], ecx
			movlps[edx], xmm1
			movss[edx + 8], xmm7
			movlps[edi], xmm1
			mov[edi + 8], ecx
			movlps[edi + 0xC], xmm1
			movss[edi + 0x14], xmm7
			add		eax, 0x18
			add		edx, 0x18
			add		esi, 0x18
			sub		edi, 0x18
			dec		ebx
			jnz		vtxIter
			push	0x50
			CALL_EAX(0xAA13E0)
			pop		ecx
			mov		ecx, eax
			CALL_EAX(0xA75DC0)
			mov		ebx, eax
			mov		ecx, eax
			CALL_EAX(0xA8AB40)
			mov		eax, ebx
			pop		edi
			pop		esi
			pop		ebx
			retn
	}
}

__declspec(naked) void __fastcall NiObjectNET::SetName(const char* newName)
{
	__asm
	{
		push	ecx
		push	edx
		call	GetNiFixedString
		pop		ecx
		pop		ecx
		mov		edx, [ecx + 8]
		cmp		eax, edx
		jz		decCount
		test	edx, edx
		jz		noCurrName
		lock dec dword ptr[edx - 8]
		noCurrName:
		mov[ecx + 8], eax
			retn
			decCount :
		test	eax, eax
			jz		done
			lock dec dword ptr[eax - 8]
			done :
			retn
	}
}

__declspec(naked) NiExtraData* __fastcall NiObjectNET::GetExtraData(UInt32 vtbl) const
{
	__asm
	{
		push	esi
		mov		esi, [ecx + 0x10]
		movzx	ecx, word ptr[ecx + 0x14]
		ALIGN 16
		iterHead:
		dec		ecx
			js		retnNULL
			mov		eax, [esi + ecx * 4]
			test	eax, eax
			jz		iterHead
			cmp[eax], edx
			jnz		iterHead
			pop		esi
			retn
			retnNULL :
		xor eax, eax
			pop		esi
			retn
	}
}

__declspec(noinline) void __vectorcall NiMaterialProperty::SetTraitValue(UInt32 traitID, float value)
{
	switch (traitID)
	{
	case 0:
	case 1:
	case 2:
		spec[traitID] = value;
		break;
	case 3:
	case 4:
	case 5:
		emit[traitID - 3] = value;
		break;
	case 6:
		fShine = value;
		break;
	case 7:
		fAlpha = value;
		break;
	default:
		fEmitMult = value;
	}
}

bool BaseExtraList::HasType(UInt32 type) const
{
	return (m_presenceBitfield[type >> 3] & (1 << (type & 7))) != 0;
}

__declspec(naked) void __fastcall BaseExtraList::SetTypePresent(UInt32 type, bool present)
{
	__asm
	{
		mov		eax, edx
		shr		eax, 5
		lea		ecx, [ecx + eax * 4 + 8]
		mov		eax, [ecx]
		cmp[esp + 4], 0
		jz		unset
		bts		eax, edx
		mov[ecx], eax
		retn	4
		unset:
		btr		eax, edx
			mov[ecx], eax
			retn	4
	}
}

__declspec(naked) BSExtraData* BaseExtraList::GetByType(UInt32 xType) const
{
	__asm
	{
		cmp		dword ptr[ecx + 4], 0
		jz		retnNULL
		mov		edx, [esp + 4]
		shr		edx, 5
		mov		eax, [ecx + edx * 4 + 8]
		mov		edx, [esp + 4]
		bt		eax, edx
		jnc		retnNULL
		push	ecx
		mov		ecx, EXTRA_DATA_CS
		call	LightCS::Enter
		pop		ecx
		mov		eax, [ecx + 4]
		mov		edx, [esp + 4]
		ALIGN 16
		iterHead:
		cmp[eax + 4], dl
			jz		lockLeave
			mov		eax, [eax + 8]
			test	eax, eax
			jnz		iterHead
			lockLeave :
		mov		edx, EXTRA_DATA_CS
			dec		dword ptr[edx + 4]
			jnz		done
			and dword ptr[edx], 0
			done :
			retn	4
			retnNULL :
			xor eax, eax
			retn	4
	}
}

__declspec(naked) ExtraDataList* ExtraDataList::Create()
{
	__asm
	{
		push	0x20
		call	Game_DoHeapAlloc
		xorps	xmm0, xmm0
		movups[eax], xmm0
		movups[eax + 0x10], xmm0
		mov		dword ptr[eax], kVtbl_ExtraDataList
		retn
	}
}

__declspec(naked) char __fastcall BaseExtraList::GetExtraFactionRank(TESFaction* faction) const
{
	__asm
	{
		push	edx
		push	kXData_ExtraFactionChanges
		call	BaseExtraList::GetByType
		pop		edx
		test	eax, eax
		jz		noRank
		mov		eax, [eax + 0xC]
		ALIGN 16
		iterHead:
		test	eax, eax
			jz		noRank
			mov		ecx, [eax]
			mov		eax, [eax + 4]
			test	ecx, ecx
			jz		iterHead
			cmp[ecx], edx
			jnz		iterHead
			mov		al, [ecx + 4]
			retn
			noRank :
		dec		al
			retn
	}
}

SInt32 BaseExtraList::GetCount() const
{
	ExtraCount* xCount = GetExtraType(this, ExtraCount);
	return xCount ? xCount->count : 1;
}