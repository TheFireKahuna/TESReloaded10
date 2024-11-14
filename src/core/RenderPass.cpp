#include "RenderPass.h"


bool RenderPass::IsVisible(ShadowMapTypeEnum type, UInt32 visibility) {
	if (type == ShadowMapTypeEnum::MapOrtho) return (visibility & ShadowMapVisibility::Ortho) == ShadowMapVisibility::Ortho;
	ShadowMapVisibility clean = (ShadowMapVisibility)(visibility & ~ShadowMapVisibility::Ortho);
	if (type == ShadowMapTypeEnum::MapLod) return  clean != ShadowMapVisibility::None;
	if (type == ShadowMapTypeEnum::MapFar) return  clean <= ShadowMapVisibility::Far && clean != ShadowMapVisibility::None;
	if (type == ShadowMapTypeEnum::MapMiddle) return  clean <= ShadowMapVisibility::Middle && clean != ShadowMapVisibility::None;
	if (type == ShadowMapTypeEnum::MapNear) return  clean == ShadowMapVisibility::Near;
	return false;
}
void ShadowRenderPass::RenderNormalPass(ShadowMapTypeEnum ShadowMapType) {
	//if (GeometryList.empty()) return;
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	RenderState->SetPixelShader(PixelShader->ShaderHandle, false);
	RenderState->SetVertexShader(VertexShader->ShaderHandle, false);

	for (std::tuple<NiGeometry*, UInt32>& obj : GeometryList) {
		NiGeometry* Geo = obj._Myfirst._Val;
		//if (IsVisible(ShadowMapType, obj._Get_rest()._Myfirst._Val)) {
			UpdateConstants(Geo);
			VertexShader->SetCT();
			PixelShader->SetCT();
			int StartIndex = 0;
			int PrimitiveCount = 0;
			NiGeometryData* ModelData = Geo->geomData;
			NiGeometryBufferData* GeoData = ModelData->bufferData;
			NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;

			TheRenderManager->PackGeometryBuffer(GeoData, ModelData, NULL, ShaderDeclaration);
			for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
				Device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
			}
			//			Logger::Log("%s %s  %08X", Geo->m_blockName, Geo->m_parent, GeoData->VBChip[0]->VB);
			Device->SetIndices(GeoData->IB);
			if (GeoData->FVF)
				RenderState->SetFVF(GeoData->FVF, false);
			else
				RenderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);
			for (UInt32 i = 0; i < GeoData->NumArrays; i++) {
				PrimitiveCount = GeoData->ArrayLengths ? GeoData->ArrayLengths[i] - 2 : GeoData->TriCount;
				Device->DrawIndexedPrimitive(GeoData->PrimitiveType, GeoData->BaseVertexIndex, 0, GeoData->VertCount, StartIndex, PrimitiveCount);
				StartIndex += PrimitiveCount + 2;
			}
		//}
	}
}

void SpeedTreeShadowRenderPass::RenderSpeedTreePass(ShadowMapTypeEnum ShadowMapType) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	RenderState->SetPixelShader(PixelShader->ShaderHandle, false);
	RenderState->SetVertexShader(VertexShader->ShaderHandle, false);

	for (std::tuple<NiGeometry*, UInt32>& obj : GeometryList) {
		NiGeometry* Geo = obj._Myfirst._Val;
		if (IsVisible(ShadowMapType, obj._Get_rest()._Myfirst._Val)) {
			int StartIndex = 0;
			int PrimitiveCount = 0;
			NiGeometryData* ModelData = Geo->geomData;
			NiGeometryBufferData* GeoData = ModelData->bufferData;
			NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;

			TheRenderManager->PackGeometryBuffer(GeoData, ModelData, NULL, ShaderDeclaration);
			for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
				Device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
			}
			Device->SetIndices(GeoData->IB);
			if (GeoData->FVF)
				RenderState->SetFVF(GeoData->FVF, false);
			else
				RenderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);
			UpdateConstants(Geo);
			VertexShader->SetCT();
			PixelShader->SetCT();
			for (UInt32 i = 0; i < GeoData->NumArrays; i++) {
				PrimitiveCount = GeoData->ArrayLengths ? GeoData->ArrayLengths[i] - 2 : GeoData->TriCount;
				Device->DrawIndexedPrimitive(GeoData->PrimitiveType, GeoData->BaseVertexIndex, 0, GeoData->VertCount, StartIndex, PrimitiveCount);
				StartIndex += PrimitiveCount + 2;
			}
		}
	}
}
void AlphaShadowRenderPass::RenderAlphaPass(ShadowMapTypeEnum ShadowMapType) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	RenderState->SetPixelShader(PixelShader->ShaderHandle, false);
	RenderState->SetVertexShader(VertexShader->ShaderHandle, false);

	for (std::tuple<NiGeometry*, UInt32>& obj : GeometryList) {
		NiGeometry* Geo = obj._Myfirst._Val;
		if (IsVisible(ShadowMapType, obj._Get_rest()._Myfirst._Val)) {
			int StartIndex = 0;
			int PrimitiveCount = 0;
			NiGeometryData* ModelData = Geo->geomData;
			NiGeometryBufferData* GeoData = ModelData->bufferData;
			NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;

			TheRenderManager->PackGeometryBuffer(GeoData, ModelData, NULL, ShaderDeclaration);
			for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
				Device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
			}
			Device->SetIndices(GeoData->IB);
			if (GeoData->FVF)
				RenderState->SetFVF(GeoData->FVF, false);
			else
				RenderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);
			UpdateConstants(Geo);
			VertexShader->SetCT();
			PixelShader->SetCT();

			for (UInt32 i = 0; i < GeoData->NumArrays; i++) {
				PrimitiveCount = GeoData->ArrayLengths ? GeoData->ArrayLengths[i] - 2 : GeoData->TriCount;
				Device->DrawIndexedPrimitive(GeoData->PrimitiveType, GeoData->BaseVertexIndex, 0, GeoData->VertCount, StartIndex, PrimitiveCount);
				StartIndex += PrimitiveCount + 2;
			}
		}
	}
}
void SkinnedAlphaGeoShadowRenderPass::RenderSkinnedAlphaPass(ShadowMapTypeEnum ShadowMapType) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	RenderState->SetPixelShader(PixelShader->ShaderHandle, false);
	RenderState->SetVertexShader(VertexShader->ShaderHandle, false);

	for (std::tuple<NiGeometry*, UInt32>& obj : GeometryList) {
		NiGeometry* Geo = obj._Myfirst._Val;
		if (IsVisible(ShadowMapType, obj._Get_rest()._Myfirst._Val)) {
			int StartIndex = 0;
			int PrimitiveCount = 0;
			int StartRegister = 9;
			NiGeometryData* ModelData = Geo->geomData;
			NiGeometryBufferData* GeoData = ModelData->bufferData;
			NiSkinInstance* SkinInstance = Geo->skinInstance;
			NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;
			NiSkinPartition* SkinPartition = SkinInstance->SkinPartition;
			D3DPRIMITIVETYPE PrimitiveType = (SkinPartition->Partitions[0].Strips == 0) ? D3DPT_TRIANGLELIST : D3DPT_TRIANGLESTRIP;
			TheRenderManager->CalculateBoneMatrixes(SkinInstance, &Geo->m_worldTransform);
			if (SkinInstance->SkinToWorldWorldToSkin)
				memcpy(&TheShaderManager->Effects.ShadowsExteriors->Constants.ShadowWorld, SkinInstance->SkinToWorldWorldToSkin, 0x40);

			for (UInt32 p = 0; p < SkinPartition->PartitionsCount; p++) {

				if (!SkinInstance->IsPartitionEnabled(p)) continue;

				StartIndex = 0;
				StartRegister = 9;
				NiSkinPartition::Partition* Partition = &SkinPartition->Partitions[p];
				for (int i = 0; i < Partition->Bones; i++) {
					UInt16 NewIndex = (Partition->pBones == NULL) ? i : Partition->pBones[i];
					Device->SetVertexShaderConstantF(StartRegister, ((float*)SkinInstance->BoneMatrixes) + (NewIndex * 3 * 4), 3);
					StartRegister += 3;
				}
				GeoData = Partition->bufferData;
				TheRenderManager->PackSkinnedGeometryBuffer(GeoData, ModelData, SkinInstance, Partition, ShaderDeclaration);
				for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
					Device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
				}
				Device->SetIndices(GeoData->IB);
				if (GeoData->FVF)
					RenderState->SetFVF(GeoData->FVF, false);
				else
					RenderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);
				UpdateConstants(Geo);
				VertexShader->SetCT();
				PixelShader->SetCT();
				for (UInt32 i = 0; i < GeoData->NumArrays; i++) {
					PrimitiveCount = GeoData->ArrayLengths ? GeoData->ArrayLengths[i] - 2 : GeoData->TriCount;
					Device->DrawIndexedPrimitive(PrimitiveType, GeoData->BaseVertexIndex, 0, Partition->Vertices, StartIndex, PrimitiveCount);
					StartIndex += PrimitiveCount + 2;
				}
			}
		}
	}
}

void SkinnedGeoShadowRenderPass::RenderSkinnedPass(ShadowMapTypeEnum ShadowMapType) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	RenderState->SetPixelShader(PixelShader->ShaderHandle, false);
	RenderState->SetVertexShader(VertexShader->ShaderHandle, false);

	for (std::tuple<NiGeometry*, UInt32>& obj : GeometryList) {
		NiGeometry* Geo = obj._Myfirst._Val;
		if (IsVisible(ShadowMapType, obj._Get_rest()._Myfirst._Val)) {
			int StartIndex = 0;
			int PrimitiveCount = 0;
			int StartRegister = 9;
			NiGeometryData* ModelData = Geo->geomData;
			NiGeometryBufferData* GeoData = ModelData->bufferData;
			NiSkinInstance* SkinInstance = Geo->skinInstance;
			NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;
			NiSkinPartition* SkinPartition = SkinInstance->SkinPartition;
			D3DPRIMITIVETYPE PrimitiveType = (SkinPartition->Partitions[0].Strips == 0) ? D3DPT_TRIANGLELIST : D3DPT_TRIANGLESTRIP;
			TheRenderManager->CalculateBoneMatrixes(SkinInstance, &Geo->m_worldTransform);
			if (SkinInstance->SkinToWorldWorldToSkin)
				memcpy(&TheShaderManager->Effects.ShadowsExteriors->Constants.ShadowWorld, SkinInstance->SkinToWorldWorldToSkin, 0x40);
			for (UInt32 p = 0; p < SkinPartition->PartitionsCount; p++) {

				if (!SkinInstance->IsPartitionEnabled(p)) continue;

				StartIndex = 0;
				StartRegister = 9;
				NiSkinPartition::Partition* Partition = &SkinPartition->Partitions[p];
				for (int i = 0; i < Partition->Bones; i++) {
					UInt16 NewIndex = (Partition->pBones == NULL) ? i : Partition->pBones[i];
					Device->SetVertexShaderConstantF(StartRegister, ((float*)SkinInstance->BoneMatrixes) + (NewIndex * 3 * 4), 3);
					StartRegister += 3;
				}
				GeoData = Partition->bufferData;
				TheRenderManager->PackSkinnedGeometryBuffer(GeoData, ModelData, SkinInstance, Partition, ShaderDeclaration);
				for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
					Device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
				}
				Device->SetIndices(GeoData->IB);
				if (GeoData->FVF)
					RenderState->SetFVF(GeoData->FVF, false);
				else
					RenderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);
				UpdateConstants(Geo);
				VertexShader->SetCT();
				PixelShader->SetCT();
				for (UInt32 i = 0; i < GeoData->NumArrays; i++) {
					PrimitiveCount = GeoData->ArrayLengths ? GeoData->ArrayLengths[i] - 2 : GeoData->TriCount;
					Device->DrawIndexedPrimitive(PrimitiveType, GeoData->BaseVertexIndex, 0, Partition->Vertices, StartIndex, PrimitiveCount);
					StartIndex += PrimitiveCount + 2;
				}
			}
		}
	}
}


ShadowRenderPass::ShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}


void ShadowRenderPass::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_ShadowWorldTransform", (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowWorld);
	//TheTextureManager->RegisterTexture("TESR_DiffuseMap", &Constants.DiffuseMap);
}


void ShadowRenderPass::UpdateConstants(NiGeometry* Geo) {
	ShadowsExteriorEffect::ShadowStruct* ShadowConstants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	ShadowConstants->Data.x = 0.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	ShadowConstants->Data.y = 0.0f; // Alpha Control
	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);
}


bool ShadowRenderPass::AccumObject(NiGeometry* Geo, UInt32& visibility) {
	if (!Geo->geomData || !Geo->geomData->bufferData) return false; // discard objects without buffer data

	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return false;

	GeometryList.push_back(std::make_tuple(Geo, visibility));
	return true;
}



AlphaShadowRenderPass::AlphaShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}


void AlphaShadowRenderPass::RegisterConstants() {
}


void AlphaShadowRenderPass::UpdateConstants(NiGeometry* Geo) {
	ShadowsExteriorEffect::ShadowStruct* ShadowConstants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	ShadowConstants->Data.x = 0.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	ShadowConstants->Data.y = 0.0f; // Alpha Control
	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);

	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	NiTexture* Texture = *((BSShaderPPLightingProperty*)ShaderProperty)->srcTextures[0];

	if (Texture && Texture->rendererData->dTexture) {

		ShadowConstants->Data.y = 1.0f; // Alpha Control
//			Constants.DiffuseMap = Texture->rendererData->dTexture;

		//// Set diffuse texture at register 0
		NiDX9RenderState* RenderState = TheRenderManager->renderState;
		RenderState->SetTexture(0, Texture->rendererData->dTexture);
		RenderState->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP, false);
		RenderState->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP, false);
		RenderState->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT, false);
		RenderState->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT, false);
		RenderState->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT, false);
	}
}


bool AlphaShadowRenderPass::AccumObject(NiGeometry* Geo, UInt32& visibility) {
	if (!Geo->geomData || !Geo->geomData->bufferData) return false; // discard objects without buffer data

	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	NiAlphaProperty* AProp = (NiAlphaProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Alpha);

	if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return false;
	if (!AProp) return false;
	if (!(AProp->flags & NiAlphaProperty::AlphaFlags::ALPHA_BLEND_MASK) && !(AProp->flags & NiAlphaProperty::AlphaFlags::TEST_ENABLE_MASK)) return false;

	GeometryList.push_back(std::make_tuple(Geo, visibility));
	return true;
}


SkinnedGeoShadowRenderPass::SkinnedGeoShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}

void SkinnedGeoShadowRenderPass::RegisterConstants() {
	//TheShaderManager->RegisterConstant("TESR_Bones", NULL);
}


void SkinnedGeoShadowRenderPass::UpdateConstants(NiGeometry* Geo) {
	// Render skinned geometry
	ShadowsExteriorEffect::ShadowStruct* Constants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	Constants->Data.x = 1.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	Constants->Data.y = 0.0f; // Alpha control
}


bool SkinnedGeoShadowRenderPass::AccumObject(NiGeometry* Geo, UInt32& visibility) {
	// check data for rigged geometry
	if (Geo->skinInstance &&
		Geo->skinInstance->SkinPartition &&
		Geo->skinInstance->SkinPartition->Partitions) {

		// only accum if valid data preset
		if (Geo->skinInstance->SkinPartition->Partitions[0].bufferData) {
			GeometryList.push_back(std::make_tuple(Geo, visibility));
		}

		// we return true in any case because we still found skinned geo either way
		return true;
	}

	return false;
}


SkinnedAlphaGeoShadowRenderPass::SkinnedAlphaGeoShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}

void SkinnedAlphaGeoShadowRenderPass::RegisterConstants() {
	//TheShaderManager->RegisterConstant("TESR_Bones", NULL);
}


void SkinnedAlphaGeoShadowRenderPass::UpdateConstants(NiGeometry* Geo) {
	// Render skinned geometry
	ShadowsExteriorEffect::ShadowStruct* ShadowConstants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	ShadowConstants->Data.x = 1.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	ShadowConstants->Data.y = 0.0f; // Alpha control

	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	NiTexture* Texture = *((BSShaderPPLightingProperty*)ShaderProperty)->srcTextures[0];

	if (Texture && Texture->rendererData->dTexture) {

		ShadowConstants->Data.y = 1.0f; // Alpha Control
		//			Constants.DiffuseMap = Texture->rendererData->dTexture;

				//// Set diffuse texture at register 0
		NiDX9RenderState* RenderState = TheRenderManager->renderState;
		RenderState->SetTexture(0, Texture->rendererData->dTexture);
		RenderState->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP, false);
		RenderState->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP, false);
		RenderState->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT, false);
		RenderState->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT, false);
		RenderState->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT, false);
	}
}


bool SkinnedAlphaGeoShadowRenderPass::AccumObject(NiGeometry* Geo, UInt32& visibility) {
	// check data for rigged geometry
	if (Geo->skinInstance &&
		Geo->skinInstance->SkinPartition &&
		Geo->skinInstance->SkinPartition->Partitions) {

		// only accum if valid data preset
		if (Geo->skinInstance->SkinPartition->Partitions[0].bufferData) {
			BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
			NiAlphaProperty* AProp = (NiAlphaProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Alpha);

			if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return false;
			if (!AProp) return false;
			if (!(AProp->flags & NiAlphaProperty::AlphaFlags::ALPHA_BLEND_MASK) && !(AProp->flags & NiAlphaProperty::AlphaFlags::TEST_ENABLE_MASK)) return false;
			GeometryList.push_back(std::make_tuple(Geo, visibility));
		}

		// we return true in any case because we still found skinned geo either way
		return true;
	}

	return false;
}


SpeedTreeShadowRenderPass::SpeedTreeShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	RegisterConstants();
}


void SpeedTreeShadowRenderPass::RegisterConstants() {
	//TheShaderManager->RegisterConstant("TESR_BillboardRight", Constants.BillboardRight);
	//TheShaderManager->RegisterConstant("TESR_BillboardUp", Constants.BillboardUp);
	//TheShaderManager->RegisterConstant("TESR_RockParams", (D3DXVECTOR4*)Pointers::ShaderParams::RockParams);
	//TheShaderManager->RegisterConstant("TESR_RustleParams", (D3DXVECTOR4*)Pointers::ShaderParams::RustleParams);
	//TheShaderManager->RegisterConstant("TESR_WindMatrices", (D3DXVECTOR4*)Pointers::ShaderParams::WindMatrixes);
	//TheShaderManager->RegisterConstant("TESR_LeafBase", Constants.LeafBase);
	//TheTextureManager->RegisterTexture("TESR_LeafDiffuseMap", &Constants.DiffuseMap);
}

void SpeedTreeShadowRenderPass::UpdateConstants(NiGeometry* Geo) {

	ShadowsExteriorEffect::ShadowStruct* ShadowConstants = &TheShaderManager->Effects.ShadowsExteriors->Constants;
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;

	ShadowConstants->Data.x = 2.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	ShadowConstants->Data.y = 0.0f; // Alpha control
	TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);

	//Constants.BillboardRight = (D3DXVECTOR4*)&TheShadowManager->BillboardRight;
	//Constants.BillboardUp = (D3DXVECTOR4*)&TheShadowManager->BillboardUp;

	//// Bind constant values for leaf transformation
	Device->SetVertexShaderConstantF(63, (float*)&TheShadowManager->BillboardRight, 1);
	Device->SetVertexShaderConstantF(64, (float*)&TheShadowManager->BillboardUp, 1);
	Device->SetVertexShaderConstantF(65, Pointers::ShaderParams::RockParams, 1);
	Device->SetVertexShaderConstantF(66, Pointers::ShaderParams::RustleParams, 1);
	Device->SetVertexShaderConstantF(67, Pointers::ShaderParams::WindMatrixes, 16);

	SpeedTreeLeafShaderProperty* STProp = (SpeedTreeLeafShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	BSTreeNode* Node = (BSTreeNode*)Geo->m_parent->m_parent;
	NiDX9SourceTextureData* Texture = (NiDX9SourceTextureData*)Node->TreeModel->LeavesTexture->rendererData;

	//Constants.LeafBase = (D3DXVECTOR4*)STProp->leafData->leafBase;
	if (Texture) ShadowConstants->Data.y = 1.0f;
	//Constants.DiffuseMap = Texture->dTexture;

	// Bind constant values for leaf transformation
	Device->SetVertexShaderConstantF(83, STProp->leafData->leafBase, 48);
	
	// Set diffuse texture at register 0
	RenderState->SetTexture(0, Texture->dTexture);
	RenderState->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP, false);
	RenderState->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP, false);
	RenderState->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT, false);
	RenderState->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT, false);
	RenderState->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT, false);
}


bool SpeedTreeShadowRenderPass::AccumObject(NiGeometry* Geo, UInt32& visibility) {

	BSShaderProperty* shaderProp = static_cast<BSShaderProperty*>(Geo->GetProperty(NiProperty::kType_Shade));
	if (shaderProp->shaderType != ShaderDefinitionEnum::kShaderDefinition_SpeedTreeLeafShader) return false;

	GeometryList.push_back(std::make_tuple(Geo, visibility));
	return true;
}


InteriorShadowRenderPass::InteriorShadowRenderPass() {
	PixelShader = TheShadowManager->ShadowMapPixel;
	VertexShader = TheShadowManager->ShadowMapVertex;
	//RegisterConstants();
}


void InteriorShadowRenderPass::RenderInteriorPass(NiGeometry* Geo) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	RenderState->SetPixelShader(PixelShader->ShaderHandle, false);
	RenderState->SetVertexShader(VertexShader->ShaderHandle, false);

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	ShadowsExteriorEffect::InteriorsStruct* ShadowsInteriors = &Shadows->Settings.Interiors;
	bool AlphaEnabled = ShadowsInteriors->Forms.AlphaEnabled;
	int StartIndex = 0;
	int PrimitiveCount = 0;
	int StartRegister = 9;
	NiGeometryData* ModelData = Geo->geomData;
	NiGeometryBufferData* GeoData = ModelData->bufferData;
	NiSkinInstance* SkinInstance = Geo->skinInstance;
	NiD3DShaderDeclaration* ShaderDeclaration = Geo->shader->ShaderDeclaration;

	if (Geo->m_blockName && !memcmp(Geo->m_blockName, "Torch", 5)) return; // No torch geo, it is too near the light and a bad square is rendered.

	Shadows->Constants.Data.x = 0.0f; // Type of geo (0 normal, 1 actors (skinned), 2 speedtree leaves)
	Shadows->Constants.Data.y = 0.0f; // Alpha control
	if (GeoData) {
		TheRenderManager->CreateD3DMatrix(&TheShaderManager->ShaderConst.ShadowWorld, &Geo->m_worldTransform);
		if (Geo->m_parent->m_blockName && !memcmp(Geo->m_parent->m_blockName, "Leaves", 6)) {
			SpeedTreeLeafShaderProperty* STProp = (SpeedTreeLeafShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
			BSTreeNode* Node = (BSTreeNode*)Geo->m_parent->m_parent;
			NiDX9SourceTextureData* Texture = (NiDX9SourceTextureData*)Node->TreeModel->LeavesTexture->rendererData;

			Shadows->Constants.Data.x = 2.0f;
			Device->SetVertexShaderConstantF(63, (float*)&TheShadowManager->BillboardRight, 1);
			Device->SetVertexShaderConstantF(64, (float*)&TheShadowManager->BillboardUp, 1);
			Device->SetVertexShaderConstantF(65, Pointers::ShaderParams::RockParams, 1);
			Device->SetVertexShaderConstantF(66, Pointers::ShaderParams::RustleParams, 1);
			Device->SetVertexShaderConstantF(67, Pointers::ShaderParams::WindMatrixes, 16);
			Device->SetVertexShaderConstantF(83, STProp->leafData->leafBase, 48);
			RenderState->SetTexture(0, Texture->dTexture);
			RenderState->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP, false);
			RenderState->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP, false);
			RenderState->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT, false);
			RenderState->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT, false);
			RenderState->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT, false);
		}
		else {
			BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
			if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return;
			NiTexture* Texture = *((BSShaderPPLightingProperty*)ShaderProperty)->srcTextures[0];
			if (AlphaEnabled) {
				NiAlphaProperty* AProp = (NiAlphaProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Alpha);
				if (AProp->flags & NiAlphaProperty::AlphaFlags::ALPHA_BLEND_MASK || AProp->flags & NiAlphaProperty::AlphaFlags::TEST_ENABLE_MASK) {
					if (Texture && Texture->rendererData->dTexture) {
						Shadows->Constants.Data.y = 1.0f; // Alpha Control
						RenderState->SetTexture(0, Texture->rendererData->dTexture);
						RenderState->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP, false);
						RenderState->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP, false);
						RenderState->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT, false);
						RenderState->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT, false);
						RenderState->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT, false);
					}
				}
			}
		}
		TheRenderManager->PackGeometryBuffer(GeoData, ModelData, NULL, ShaderDeclaration);
		if (GeoData->StreamCount != 1) Logger::Log("%s %s  %i", Geo->m_blockName, Geo->m_parent->m_blockName, GeoData->StreamCount);
		for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
			Device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
		}
		Device->SetIndices(GeoData->IB);
		if (GeoData->FVF)
			RenderState->SetFVF(GeoData->FVF, false);
		else
			RenderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);
		VertexShader->SetCT();
		PixelShader->SetCT();
		if (GeoData->NumArrays != 1) Logger::Log("%i   %i", GeoData->NumArrays, GeoData->ArrayLengths);
		for (UInt32 i = 0; i < GeoData->NumArrays; i++) {
			PrimitiveCount = GeoData->ArrayLengths ? GeoData->ArrayLengths[i] - 2 : GeoData->TriCount;
			Device->DrawIndexedPrimitive(GeoData->PrimitiveType, GeoData->BaseVertexIndex, 0, GeoData->VertCount, StartIndex, PrimitiveCount);
			StartIndex += PrimitiveCount + 2;
		}
	}
	else {
		Shadows->Constants.Data.x = 1.0f;

		BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
		if (!ShaderProperty || !ShaderProperty->IsLightingProperty()) return;
		NiTexture* Texture = *((BSShaderPPLightingProperty*)ShaderProperty)->srcTextures[0];

		if (AlphaEnabled) {
			NiAlphaProperty* AProp = (NiAlphaProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Alpha);
			if (AProp->flags & NiAlphaProperty::AlphaFlags::ALPHA_BLEND_MASK || AProp->flags & NiAlphaProperty::AlphaFlags::TEST_ENABLE_MASK) {
				if (Texture && Texture->rendererData->dTexture) {

					Shadows->Constants.Data.y = 1.0f; // Alpha Control
					RenderState->SetTexture(0, Texture->rendererData->dTexture);
					RenderState->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP, false);
					RenderState->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP, false);
					RenderState->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT, false);
					RenderState->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT, false);
					RenderState->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT, false);
				}
			}
		}
		NiSkinPartition* SkinPartition = SkinInstance->SkinPartition;
		D3DPRIMITIVETYPE PrimitiveType = (SkinPartition->Partitions[0].Strips == 0) ? D3DPT_TRIANGLELIST : D3DPT_TRIANGLESTRIP;
		TheRenderManager->CalculateBoneMatrixes(SkinInstance, &Geo->m_worldTransform);
		if (SkinInstance->SkinToWorldWorldToSkin) memcpy(&TheShaderManager->ShaderConst.ShadowWorld, SkinInstance->SkinToWorldWorldToSkin, 0x40);
		for (UInt32 p = 0; p < SkinPartition->PartitionsCount; p++) {
			if (!SkinInstance->IsPartitionEnabled(p)) continue;

			StartIndex = 0;
			StartRegister = 9;
			NiSkinPartition::Partition* Partition = &SkinPartition->Partitions[p];
			for (int i = 0; i < Partition->Bones; i++) {
				UInt16 NewIndex = (Partition->pBones == NULL) ? i : Partition->pBones[i];
				Device->SetVertexShaderConstantF(StartRegister, ((float*)SkinInstance->BoneMatrixes) + (NewIndex * 3 * 4), 3);
				StartRegister += 3;
			}
			GeoData = Partition->bufferData;
			TheRenderManager->PackSkinnedGeometryBuffer(GeoData, ModelData, SkinInstance, Partition, ShaderDeclaration);
			for (UInt32 i = 0; i < GeoData->StreamCount; i++) {
				Device->SetStreamSource(i, GeoData->VBChip[i]->VB, 0, GeoData->VertexStride[i]);
			}
			Device->SetIndices(GeoData->IB);
			if (GeoData->FVF)
				RenderState->SetFVF(GeoData->FVF, false);
			else
				RenderState->SetVertexDeclaration(GeoData->VertexDeclaration, false);
			VertexShader->SetCT();
			PixelShader->SetCT();
			for (UInt32 i = 0; i < GeoData->NumArrays; i++) {
				PrimitiveCount = GeoData->ArrayLengths ? GeoData->ArrayLengths[i] - 2 : GeoData->TriCount;
				Device->DrawIndexedPrimitive(PrimitiveType, GeoData->BaseVertexIndex, 0, Partition->Vertices, StartIndex, PrimitiveCount);
				StartIndex += PrimitiveCount + 2;
			}
		}
	}
}