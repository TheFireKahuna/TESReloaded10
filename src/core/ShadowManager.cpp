#define ShadowMapFarPlane 32768;

/*
* Initializes the Shadow Manager by grabbing the relevant settings and shaders, and setting up map sizes.
*/
void ShadowManager::Initialize() {
	
	Logger::Log("Starting the shadows manager...");
	TheShadowManager = new ShadowManager();

	// setup the shadow render passes for the shadowmaps
	TheShadowManager->geometryPass = new ShadowRenderPass();
	TheShadowManager->alphaPass = new AlphaShadowRenderPass();
	TheShadowManager->skinnedGeoPass = new SkinnedGeoShadowRenderPass();
	TheShadowManager->skinnedAlphaPass = new SkinnedAlphaGeoShadowRenderPass();
	TheShadowManager->speedTreePass = new SpeedTreeShadowRenderPass();
	TheShadowManager->interiorPass = new InteriorShadowRenderPass();

	// load the shaders
	TheShadowManager->ShadowMapVertex = (ShaderRecordVertex*)ShaderRecord::LoadShader("ShadowMap.vso", "Shadows\\");
	TheShadowManager->ShadowMapPixel = (ShaderRecordPixel*)ShaderRecord::LoadShader("ShadowMap.pso", "Shadows\\");
	TheShadowManager->ShadowCubeMapVertex = (ShaderRecordVertex*)ShaderRecord::LoadShader("ShadowCubeMap.vso", "Shadows\\");
	TheShadowManager->ShadowCubeMapPixel = (ShaderRecordPixel*)ShaderRecord::LoadShader("ShadowCubeMap.pso", "Shadows\\");

    TheShadowManager->ShadowMapBlurVertex = (ShaderRecordVertex*) ShaderRecord::LoadShader("ShadowMapBlur.vso", "Shadows\\");
    TheShadowManager->ShadowMapBlurPixel = (ShaderRecordPixel*) ShaderRecord::LoadShader("ShadowMapBlur.pso", "Shadows\\");

	// Make sure samplers are not reset on SetCT as that causes errors.
	TheShadowManager->ShadowMapVertex->ClearSamplers = false;
	TheShadowManager->ShadowMapPixel->ClearSamplers = false;
	TheShadowManager->ShadowCubeMapVertex->ClearSamplers = false;
	TheShadowManager->ShadowCubeMapPixel->ClearSamplers = false;
	TheShadowManager->ShadowMapBlurVertex->ClearSamplers = false;
	TheShadowManager->ShadowMapBlurPixel->ClearSamplers = false;

	TheShadowManager->ShadowShadersLoaded = true;
    if (TheShadowManager->ShadowMapVertex == nullptr || TheShadowManager->ShadowMapPixel == nullptr  || TheShadowManager->ShadowMapBlurVertex  == nullptr
        || TheShadowManager->ShadowCubeMapVertex == nullptr || TheShadowManager->ShadowCubeMapPixel == nullptr || TheShadowManager->ShadowMapBlurPixel  == nullptr ){
		TheShadowManager->ShadowShadersLoaded = false;
		Logger::Log("[ERROR]: Could not load one or more of the ShadowMap generation shaders. Reinstall the mod.");
    }

	UINT ShadowCubeMapSize = TheShaderManager->Effects.ShadowsExteriors->Settings.Interiors.ShadowCubeMapSize;
	TheShadowManager->ShadowCubeMapViewPort = { 0, 0, ShadowCubeMapSize, ShadowCubeMapSize, 0.0f, 1.0f };

	TheShadowManager->shadowMapsRenderTime = 0;
}


void ShadowManager::RenderShadowSpotlight(NiSpotLight** Lights, UInt32 LightIndex) {
}

void ShadowManager::RenderInterior(NiAVObject* Object, float MinRadius) {

	if (Object) {
		float Radius = Object->GetWorldBoundRadius();
		if (!(Object->m_flags & NiAVObject::APP_CULLED) && Radius >= MinRadius) {
			void* VFT = *(void**)Object;
			if (Object->GetNiNode()) {
				NiNode* Node = (NiNode*)Object;
				for (int i = 0; i < Node->m_children.numObjs; i++) {
					RenderInterior(Node->m_children.data[i], MinRadius);
				}
			}
			else if (VFT == Pointers::VirtualTables::NiTriShape || VFT == Pointers::VirtualTables::NiTriStrips) {
				RenderGeometry((NiGeometry*)Object);
			}
		}
	}

}

void ShadowManager::RenderGeometry(NiGeometry* Geo) {

	NiGeometryBufferData* GeoData = NULL;

	if (Geo->shader) {
		GeoData = Geo->geomData->bufferData;
		if (GeoData) {
			interiorPass->RenderInteriorPass(Geo);
		}
		else if (Geo->skinInstance && Geo->skinInstance->SkinPartition && Geo->skinInstance->SkinPartition->Partitions) {
			GeoData = Geo->skinInstance->SkinPartition->Partitions[0].bufferData;
			if (GeoData) interiorPass->RenderInteriorPass(Geo);
		}
	}

}


void ShadowManager::RenderShadowCubeMap(ShadowSceneLight** Lights, UInt32 LightIndex) {
	if (Lights[LightIndex] == NULL) return; // No light at current index
	
	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	ShadowsExteriorEffect::InteriorsStruct* Settings = &Shadows->Settings.Interiors;

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	float Radius = 0.0f;
	float MinRadius = Settings->Forms.MinRadius;
	NiPoint3* LightPos = NULL;
	D3DXMATRIX View, Proj;
	D3DXVECTOR3 Eye, At, Up, CameraDirection;

	NiPointLight* pNiLight = Lights[LightIndex]->sourceLight;

	LightPos = &pNiLight->m_worldTransform.translate;
	Radius = pNiLight->radius *Shadows->Settings.Interiors.LightRadiusMult;
	//if (pNiLight->CanCarry)
	//	Radius = 256.0f;
	Eye.x = LightPos->x - TheRenderManager->CameraPosition.x;
	Eye.y = LightPos->y - TheRenderManager->CameraPosition.y;
	Eye.z = LightPos->z - TheRenderManager->CameraPosition.z;
	Shadows->Constants.ShadowCubeMapLightPosition.x = Eye.x;
	Shadows->Constants.ShadowCubeMapLightPosition.y = Eye.y;
	Shadows->Constants.ShadowCubeMapLightPosition.z = Eye.z;
	Shadows->Constants.ShadowCubeMapLightPosition.w = Radius;
	Shadows->Constants.Data.z = Radius;
	D3DXMatrixPerspectiveFovRH(&Proj, D3DXToRadian(90.0f), 1.0f, 0.1f, Radius);

	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHABLENDENABLE, 0, RenderStateArgs);

	for (int Face = 0; Face < 6; Face++) {
		At = Eye;
		switch (Face) {
		case D3DCUBEMAP_FACE_POSITIVE_X:
			CameraDirection = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_X:
			CameraDirection = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Y:
			CameraDirection = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			Up = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Y:
			CameraDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
			Up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Z:
			CameraDirection = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Z:
			CameraDirection = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		}
		At += CameraDirection;
		D3DXMatrixLookAtRH(&View, &Eye, &At, &Up);
		TList<TESObjectREFR>::Entry* Entry = &Player->parentCell->objectList.First;

		for (TESObjectREFR*& iter : Player->parentCell->objectList) {
			NiNode* RefNode = GetRef(iter, &Settings->Forms);
			//if (RefNode && TheCameraManager->InFrustum(&ShadowMap->ShadowMapFrustum, RefNode))
			AccumulateGeometry(RefNode, &Settings->Forms, false);
		}
	}
}


static void FlagShaderPropertyRecurse(NiAVObject* apObject, UInt32 auiFlags, bool abSet) {
	if (!apObject)
		return;

	if (apObject->GetNiGeometry()) {
		NiGeometry* pGeometry = static_cast<NiGeometry*>(apObject);
		BSShaderProperty* shaderProperty = static_cast<BSShaderProperty*>(pGeometry->GetProperty(NiProperty::kType_Shade));
		if (shaderProperty) {
			if (abSet)
				shaderProperty->flags1 |= auiFlags;
			else
				shaderProperty->flags1 &= ~auiFlags;
		}

	}
	else if (apObject->GetNiNode()) {
		NiNode* pNiNode = static_cast<NiNode*>(apObject);
		for (UInt32 i = 0; i < pNiNode->m_children.end; i++) {
			FlagShaderPropertyRecurse(pNiNode->m_children.data[i], auiFlags, abSet);
		}
	}
}


D3DXMATRIX ShadowManager::GetViewMatrix(D3DXVECTOR3* At, D3DXVECTOR4* Dir) {
	D3DXVECTOR3 Up = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	float FarPlane = ShadowMapFarPlane;

	// calculating the projection matrix for point of view of the light
	D3DXVECTOR3 Eye;
	Eye.x = At->x - FarPlane * Dir->x * -1;
	Eye.y = At->y - FarPlane * Dir->y * -1;
	Eye.z = At->z - FarPlane * Dir->z * -1;

	D3DXMATRIX View;
	D3DXMatrixLookAtRH(&View, &Eye, At, &Up);

	// save Billboard values for speedtree leafs rendering
	BillboardRight = { View._11, View._21, View._31, 0.0f };
	BillboardUp = { View._12, View._22, View._32, 0.0f };

	return View;
}
/*
* Return true if the NiNode is outsidethe frustum of all shadowmaps
*/
bool ShadowManager::IsOutAllFrustums(NiNode* node)
{
	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	return !TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapLod].ShadowMapFrustum, node) && !TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapOrtho].ShadowMapFrustum, node);
}

bool ShadowManager::ExcludeFromAllRadius(NiAVObject* node)
{
	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	return  node->GetWorldBoundRadius() < Shadows->ShadowMaps[ShadowMapTypeEnum::MapNear].Forms.MinRadius;
}

/*
* Returns the given object ref if it passes the test for excluded form types, otherwise returns NULL.
*/
NiNode* ShadowManager::GetRef(TESObjectREFR* Ref, ShadowsExteriorEffect::FormsStruct* Forms) {
	if (!Ref) return NULL;
	NiNode* Node = Ref->GetNode();

	if (!Node) return NULL;
	//if (Ref->flags & TESForm::FormFlags::kFormFlags_NotCastShadows) return NULL;

	TESForm* Form = Ref->baseForm;
	UInt8 TypeID = Form->formType;
	switch (TypeID) {
	case TESForm::FormType::kFormType_Land:
		return NULL; // land is handled separately
		break;
	case TESForm::FormType::kFormType_Activator:
		if (!Forms->Activators) return NULL;
		break;
	case TESForm::FormType::kFormType_Apparatus:
		if (!Forms->Apparatus) return NULL;
		break;
	case TESForm::FormType::kFormType_Book:
		if (!Forms->Books) return NULL;
		break;
	case TESForm::FormType::kFormType_Container:
		if (!Forms->Containers) return NULL;
		break;
	case TESForm::FormType::kFormType_Door:
		if (!Forms->Doors) return NULL;
		break;
	case TESForm::FormType::kFormType_Misc:
		if (!Forms->Misc) return NULL;
		break;
	case TESForm::FormType::kFormType_Tree:
		if (!Forms->Trees) return NULL;
		break;
	case TESForm::FormType::kFormType_Furniture:
		if (!Forms->Furniture) return NULL;
		break;
	case TESForm::FormType::kFormType_NPC:
	case TESForm::FormType::kFormType_Creature:
	case TESForm::FormType::kFormType_LeveledCreature:
		if (!Forms->Actors) return NULL;
		break;
	case TESForm::FormType::kFormType_Stat:
	case TESForm::FormType::kFormType_StaticCollection:
	case TESForm::FormType::kFormType_MoveableStatic:
		//return NULL;
		if (!Forms->Statics) return NULL;
		break;
	default:
		break;
	}
	// disabled for now since it's an obscure functionality
	//if (ExcludedForms->size() > 0 && std::binary_search(ExcludedForms->begin(), ExcludedForms->end(), Form->refID)) return NULL;

	////ExtraRefractionProperty* RefractionExtraProperty = (ExtraRefractionProperty*)Ref->extraDataList.GetExtraData(BSExtraData::ExtraDataType::kExtraData_RefractionProperty);
	//float Refraction = RefractionExtraProperty ? (1 - RefractionExtraProperty->refractionAmount) : 0.0f;
	//if (Refraction >= 0.5) return NULL;

	return Node;

}


void ShadowManager::SelectGeometry(NiAVObject* accum) {

	NiGeometry* geo = static_cast<NiGeometry*>(accum);
	if (!geo->shader) return; // skip Geometry without a shader

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	UInt32 visibility = ShadowMapVisibility::None;
	//if (TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapNear].ShadowMapFrustum, geo)) visibility = ShadowMapVisibility::Near;
	//else if (TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapMiddle].ShadowMapFrustum, geo)) visibility = ShadowMapVisibility::Middle;
	//else if (TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapFar].ShadowMapFrustum, geo)) visibility = ShadowMapVisibility::Far;
	//else if (TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapLod].ShadowMapFrustum, geo)) visibility = ShadowMapVisibility::Lod;
	//if (TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapOrtho].ShadowMapFrustum, geo)) visibility |= ShadowMapVisibility::Ortho;

	if (skinnedGeoPass->AccumObject(geo, visibility)) {}
	else if (speedTreePass->AccumObject(geo, visibility)) {}
	else if (AlphaEnabled && skinnedAlphaPass->AccumObject(geo, visibility)) {}
	else if (AlphaEnabled && alphaPass->AccumObject(geo, visibility)) {}
	else geometryPass->AccumObject(geo, visibility);
}

void ShadowManager::AccumulateGeometry(NiAVObject* NiObject, ShadowsExteriorEffect::FormsStruct* Forms, bool isLand) {
	if (!NiObject) return;

	std::stack<NiAVObject*> containers;
	NiAVObject* child;
	NiAVObject* object;
	NiNode* Node;

	if (!NiObject->GetNiGeometry())
		containers.push(NiObject);
	else
		SelectGeometry(NiObject); //list all objects contained, or sort the object if not a container

	// Gather geometry
	while (!containers.empty()) {
		object = containers.top();
		containers.pop();

		if (!object) continue;

		Node = object->GetNiNode();
		if (!Node || Node->m_flags & NiAVObject::NiFlags::APP_CULLED) continue; // culling containers
		//if (!isLand && Node->GetWorldBoundRadius() < Forms->MinRadius) continue;

		for (int i = 0; i < Node->m_children.end; i++) {
			child = Node->m_children.data[i];
			if (!child || child->m_flags & NiAVObject::NiFlags::APP_CULLED) continue; // culling children
			//if (!isLand && child->GetWorldBoundRadius() < Forms->MinRadius) continue;

			if (child->GetFadeNode() && static_cast<BSFadeNode*>(child)->FadeAlpha < 0.75f) continue; // stop rendering fadenodes below a certain opacity
			if (!child->GetNiGeometry())
				containers.push(child);
			else
				SelectGeometry(child);
		}
	}
}


void ShadowManager::RenderShadowExteriorMaps() {
	GridCellArray* CellArray = Tes->gridCellArray;
	UInt32 CellArraySize = CellArray->gridSize * CellArray->gridSize;

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	ShadowsExteriorEffect::ExteriorsStruct* ShadowsExteriors = &Shadows->Settings.Exteriors;

	D3DXVECTOR4 OrthoDir = D3DXVECTOR4(0.05f, 0.05f, 1.0f, 0.0f);
	D3DXVECTOR4* SunDir = &TheShaderManager->ShaderConst.SunDir;

	D3DXVECTOR4* ShadowData = &TheShaderManager->Effects.ShadowsExteriors->Constants.Data;
	D3DXVECTOR4* OrthoData = &TheShaderManager->Effects.ShadowsExteriors->Constants.OrthoData;

	D3DXVECTOR3 At;
	NiNode* PlayerNode = Player->GetNode();
	At.x = PlayerNode->m_worldTransform.translate.x - TheRenderManager->CameraPosition.x;
	At.y = PlayerNode->m_worldTransform.translate.y - TheRenderManager->CameraPosition.y;
	At.z = PlayerNode->m_worldTransform.translate.z - TheRenderManager->CameraPosition.z;
	D3DMATRIX ViewSun, ViewOrtho;
	float FarPlane = ShadowMapFarPlane;
	ViewSun = GetViewMatrix(&At, SunDir);
	ViewOrtho = GetViewMatrix(&At, &OrthoDir);

	NiDX9RenderState* RenderState = TheRenderManager->renderState;

	TheShadowManager->geometryPass->GeometryList.clear();
	TheShadowManager->alphaPass->GeometryList.clear();
	TheShadowManager->speedTreePass->GeometryList.clear();
	TheShadowManager->skinnedGeoPass->GeometryList.clear();
	TheShadowManager->skinnedAlphaPass->GeometryList.clear();

	for (UInt32 i = ShadowMapTypeEnum::MapNear; i < ShadowMapTypeEnum::MapOrtho; i++) {
		D3DMATRIX ViewProj = Shadows->GetCascadeViewProj(&Shadows->ShadowMaps[i],ViewSun);
		Shadows->ShadowMaps[i].ShadowCameraToLight = TheRenderManager->InvViewProjMatrix * ViewProj;
		TheCameraManager->SetFrustum(&Shadows->ShadowMaps[i].ShadowMapFrustum, &ViewProj);
	}
	D3DMATRIX ViewProj = Shadows->GetOrthoViewProj(ViewOrtho);
	Shadows->ShadowMaps[MapOrtho].ShadowCameraToLight = TheRenderManager->InvViewProjMatrix * ViewProj;
	TheCameraManager->SetFrustum(&Shadows->ShadowMaps[MapOrtho].ShadowMapFrustum, &ViewProj);
	OrthoData->x = Shadows->ShadowMaps[MapOrtho].ShadowMapRadius * 2;

	for (UInt32 i = 0; i < CellArraySize; i++) {
		TESObjectCELL* Cell = CellArray->GetCell(i);
		if (Cell && !Cell->IsInterior()) {
			std::vector<NiNode*> TerrainNodes = Cell->GetTerrainNodes();
			//for (NiNode* node : TerrainNodes) {
			//	AccumulateGeometry(node, &Shadows->ShadowMaps[MapNear].Forms, true);
			//}
			//			if (ShadowsExteriors->Forms[ShadowMapType].Lod) RenderLod(Tes->landLOD, ShadowMapType); //Render terrain LOD
			/*for (UInt32 i = 2; i < Cell->GetNode()->m_children.numObjs; i++) {
				//For NewVegas: 0 Actor, 2 Land, 3 Static, 4 Dynamic,5 Multibound, 1 Marker
				//For Oblivion: 0 Actor, 2-5 Static Terrain is in subnode 0 of every 2-5 node. Node 0 seems to be unstable, some actors aren't rendered sometimes (not present in the node?)
				NiNode* TerrainNode = (NiNode*)Cell->GetNode()->m_children.data[i];
				AccumulateGeometry(TerrainNode);
			} */
			for (TESObjectREFR*& iter : Cell->objectList) {
				NiNode* RefNode = GetRef(iter, &Shadows->ShadowMaps[MapNear].Forms);
				//if (RefNode && TheCameraManager->InFrustum(&ShadowMap->ShadowMapFrustum, RefNode))
				AccumulateGeometry(RefNode, &Shadows->ShadowMaps[MapNear].Forms, false);
			}
		}
	}

	IDirect3DDevice9* Device = TheRenderManager->device;

	for (int i = MapNear; i <= MapOrtho; i++) {
		ShadowsExteriorEffect::ShadowMapSettings* ShadowMap = &Shadows->ShadowMaps[i];
		if (i == MapOrtho) {
			ShadowData->z = 1; // identify ortho map in shader constant
			Shadows->Constants.ShadowViewProj = Shadows->GetOrthoViewProj(ViewOrtho);
		}
		else {
			ShadowData->z = 0; // set shader constant to identify other shadow maps
			Shadows->Constants.ShadowViewProj = Shadows->GetCascadeViewProj(ShadowMap, ViewSun);
		}
		RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
		RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
		RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE, RenderStateArgs);
		RenderState->SetRenderState(D3DRS_ALPHABLENDENABLE, 0, RenderStateArgs);

		Device->SetRenderTarget(0, ShadowMap->ShadowMapSurface);
		Device->SetDepthStencilSurface(ShadowMap->ShadowMapDepthSurface);
		Device->SetViewport(&ShadowMap->ShadowMapViewPort);

		Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0L);

		Device->BeginScene();

		//if ((SunDir->z > 0.0f && i <= ShadowMapTypeEnum::MapLod) || i == ShadowMapTypeEnum::MapOrtho) {
			geometryPass->RenderNormalPass((ShadowMapTypeEnum)i);
			alphaPass->RenderAlphaPass((ShadowMapTypeEnum)i);
			skinnedGeoPass->RenderSkinnedPass((ShadowMapTypeEnum)i);
			//skinnedAlphaPass->RenderSkinnedAlphaPass((ShadowMapTypeEnum)i);
			//speedTreePass->RenderSpeedTreePass((ShadowMapTypeEnum)i);
		//}

		Device->EndScene();
		if (ShadowsExteriors->BlurShadowMaps) BlurShadowMap(ShadowMap);
	}
}

/*
* Renders the different shadow maps: Near, Far, Ortho.
*/
void ShadowManager::RenderShadowMaps() {
	if (!TheSettingManager->SettingsMain.Main.RenderEffects) return; // cancel out if rendering effects is disabled

	// track point lights for interiors and exteriors
	ShadowSceneLight* ShadowLights[ShadowCubeMapsMax] = { NULL };
	NiPointLight* Lights[TrackedLightsMax] = { NULL };
	NiSpotLight* SpotLights[SpotLightsMax] = { NULL };

	TheShaderManager->GetNearbyLights(ShadowLights, Lights, SpotLights);

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	ShadowsExteriorEffect::ExteriorsStruct* ShadowsExteriors = &Shadows->Settings.Exteriors;
	ShadowsExteriorEffect::InteriorsStruct* ShadowsInteriors = &Shadows->Settings.Interiors;

	bool isExterior = TheShaderManager->GameState.isExterior;// || currentCell->flags0 & TESObjectCELL::kFlags0_BehaveLikeExterior; // exterior flag currently broken
	bool ExteriorEnabled = isExterior && TheShaderManager->Effects.ShadowsExteriors->Enabled && ShadowsExteriors->Enabled;
	bool InteriorEnabled = !isExterior && TheShaderManager->Effects.ShadowsInteriors->Enabled;

	// early out in case shadow rendering is not required
	if (!ExteriorEnabled && !InteriorEnabled && !TheShaderManager->orthoRequired || !ShadowShadersLoaded) {
		return;
	}
	if (!Player->parentCell) return;

	auto timer = TimeLogger();

	// prepare some pointers to the device and surfaces
	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	IDirect3DSurface9* DepthSurface = NULL;
	IDirect3DSurface9* RenderSurface = NULL;
	D3DVIEWPORT9 viewport;

	D3DXVECTOR4* ShadowData = &TheShaderManager->Effects.ShadowsExteriors->Constants.Data;
	D3DXVECTOR4* OrthoData = &TheShaderManager->Effects.ShadowsExteriors->Constants.OrthoData;
	Device->GetDepthStencilSurface(&DepthSurface);
	Device->GetRenderTarget(0, &RenderSurface);
	Device->GetViewport(&viewport);	

	DWORD zfunc;
	Device->GetRenderState(D3DRS_ZFUNC, &zfunc); // backup in case of inverted depth

	if (1.0 - NiDX9Renderer::GetSingleton()->m_fZClear) // inverted depth
		RenderState->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL, RenderStateArgs);
	else
		RenderState->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL, RenderStateArgs);

	RenderState->SetRenderState(D3DRS_STENCILENABLE, 1, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_STENCILREF, 0, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHAREF, 0, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_NORMALIZENORMALS, 1, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_POINTSIZE, 810365505, RenderStateArgs);

	TheRenderManager->UpdateSceneCameraData();
	TheRenderManager->SetupSceneCamera();
	
	D3DXVECTOR4 PlayerPosition = Player->position.toD3DXVEC4();
	TESObjectCELL* currentCell = Player->parentCell;

	// Flag player geometry so we can control if it should be rendered in shadow cubemaps
	//FlagPlayerGeometry();

	// Render directional shadows for Sun/Moon
	ShadowData->w = ShadowsExteriors->ShadowMode;	// Mode (0:off, 1:VSM, 2:ESM, 3: ESSM);
	AlphaEnabled = ShadowsInteriors->Forms.AlphaEnabled;

	// Render all shadow maps
	if (isExterior && (ExteriorEnabled || TheShaderManager->orthoRequired)) {

		geometryPass->VertexShader = ShadowMapVertex;
		geometryPass->PixelShader = ShadowMapPixel;
		alphaPass->VertexShader = ShadowMapVertex;
		alphaPass->PixelShader = ShadowMapPixel;
		skinnedGeoPass->VertexShader = ShadowMapVertex;
		skinnedGeoPass->PixelShader = ShadowMapPixel;
		skinnedAlphaPass->VertexShader = ShadowMapVertex;
		skinnedAlphaPass->PixelShader = ShadowMapPixel;
		speedTreePass->VertexShader = ShadowMapVertex;
		speedTreePass->PixelShader = ShadowMapPixel;
		interiorPass->VertexShader = ShadowMapVertex;
		interiorPass->PixelShader = ShadowMapPixel;

		auto shadowMapTimer = TimeLogger();
		// render ortho map
		RenderShadowExteriorMaps();

		std::string message = "ShadowManager::RenderShadowMap";
		shadowMapTimer.LogTime(message.c_str());

	}

	// Render shadow maps for point lights
	bool usePointLights = (TheShaderManager->GameState.isDayTime > 0.5) ? ShadowsExteriors->UsePointShadowsDay : ShadowsExteriors->UsePointShadowsNight;

	geometryPass->VertexShader = ShadowCubeMapVertex;
	geometryPass->PixelShader = ShadowCubeMapPixel;
	alphaPass->VertexShader = ShadowCubeMapVertex;
	alphaPass->PixelShader = ShadowCubeMapPixel;
	skinnedGeoPass->VertexShader = ShadowCubeMapVertex;
	skinnedGeoPass->PixelShader = ShadowCubeMapPixel;
	skinnedAlphaPass->VertexShader = ShadowCubeMapVertex;
	skinnedAlphaPass->PixelShader = ShadowCubeMapPixel;
	speedTreePass->VertexShader = ShadowCubeMapVertex;
	speedTreePass->PixelShader = ShadowCubeMapPixel;

	auto shadowMapTimer = TimeLogger();
	if ((isExterior && usePointLights) || (!isExterior && InteriorEnabled)) {
		// render the cubemaps for each light
		for (int i = 0; i < ShadowsInteriors->LightPoints; i++) {

			RenderShadowCubeMap(ShadowLights, i);

			std::string message = "ShadowManager::RenderShadowCubeMap ";
			message += std::to_string(i);
			shadowMapTimer.LogTime(message.c_str());
		}
	}

	if (TheShaderManager->Effects.Flashlight->Enabled && TheShaderManager->Effects.Flashlight->spotLightActive && TheShaderManager->Effects.Flashlight->Settings.renderShadows) {
		// render shadow maps for spotlights
		
		for (int i = 0; i < SpotLightsMax; i++) {
			if (!SpotLights[i] || SpotLights[i]->radius == 0) continue; //bypass lights with no radius

			RenderShadowSpotlight(SpotLights, i);

			std::string message = "ShadowManager::RenderShadowSpotLight";
			message += std::to_string(i);
			shadowMapTimer.LogTime(message.c_str());
		}
	}

	// reset renderer to previous state
	Device->SetDepthStencilSurface(DepthSurface);
	Device->SetRenderTarget(0, RenderSurface);
	Device->SetViewport(&viewport);
	Device->SetRenderState(D3DRS_ZFUNC, zfunc);

	//release smart pointers to prevent memory leak
	if (DepthSurface) DepthSurface->Release();
	if (RenderSurface) RenderSurface->Release();

	if (TheSettingManager->SettingsMain.Develop.DebugMode && !InterfaceManager->IsActive(Menu::MenuType::kMenuType_Console)) {
		if (Global->OnKeyDown(0x17)) { // TODO: setting for debug key ?
			char Filename[MAX_PATH];

			time_t CurrentTime = time(NULL);
			GetCurrentDirectoryA(MAX_PATH, Filename);
			strcat(Filename, "\\Test");
			if (GetFileAttributesA(Filename) == INVALID_FILE_ATTRIBUTES) CreateDirectoryA(Filename, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap0.jpg", D3DXIFF_JPG, Shadows->ShadowMaps[MapNear].ShadowMapSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap1.jpg", D3DXIFF_JPG, Shadows->ShadowMaps[MapMiddle].ShadowMapSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap2.jpg", D3DXIFF_JPG, Shadows->ShadowMaps[MapFar].ShadowMapSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap3.jpg", D3DXIFF_JPG, Shadows->ShadowMaps[MapLod].ShadowMapSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA(".\\Test\\shadowmap4.jpg", D3DXIFF_JPG, Shadows->ShadowMaps[MapOrtho].ShadowMapSurface, NULL, NULL);

			InterfaceManager->ShowMessage("Textures taken!");
		}
	}

	timer.LogTime("ShadowManager::RenderShadowMaps");
}


/*
* Filters the Shadow Map of given index using a 2 pass gaussian blur
*/
void ShadowManager::BlurShadowMap(ShadowsExteriorEffect::ShadowMapSettings* ShadowMap) {
    IDirect3DDevice9* Device = TheRenderManager->device;
    NiDX9RenderState* RenderState = TheRenderManager->renderState;
    IDirect3DTexture9* SourceShadowMap = ShadowMap->ShadowMapTexture;
    IDirect3DSurface9* TargetShadowMap = ShadowMap->ShadowMapSurface;

    Device->SetDepthStencilSurface(NULL);
    RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE, RenderStateArgs);
    RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_FALSE, RenderStateArgs);
    RenderState->SetVertexShader(ShadowMapBlurVertex->ShaderHandle, false);
    RenderState->SetPixelShader(ShadowMapBlurPixel->ShaderHandle, false);
	RenderState->SetFVF(FrameFVF, false);
	Device->SetStreamSource(0, ShadowMap->BlurShadowVertexBuffer, 0, sizeof(FrameVS));
	Device->SetRenderTarget(0, TargetShadowMap);
	
	// Pass map resolution to shader as a constant
	D3DXVECTOR4 inverseRes = { ShadowMap->ShadowMapInverseResolution, ShadowMap->ShadowMapInverseResolution, 0.0f, 0.0f };
	ShadowMapBlurPixel->SetShaderConstantF(0, &inverseRes, 1);
	RenderState->SetTexture(0, SourceShadowMap);

	// blur in two passes, vertically and horizontally
	D3DXVECTOR4 Blur[2] = {
		D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.0f),
		D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.0f),
	};

	for (int i = 0; i < 2; i++) {
		// set blur direction shader constants
		ShadowMapBlurPixel->SetShaderConstantF(1, &Blur[i], 1);

		Device->BeginScene();
		Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // draw call to execute the shader
		Device->EndScene();
	}

	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
    RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
}

