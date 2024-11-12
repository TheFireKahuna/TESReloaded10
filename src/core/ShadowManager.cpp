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
			if (TheRenderManager->IsNode(Object)) {
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
		GeoData = Geo->geomData->BuffData;
		if (GeoData) {
			interiorPass->RenderInteriorPass(Geo);
		}
		else if (Geo->skinInstance && Geo->skinInstance->SkinPartition && Geo->skinInstance->SkinPartition->Partitions) {
			GeoData = Geo->skinInstance->SkinPartition->Partitions[0].BuffData;
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

	LightPos = &pNiLight->m_worldTransform.pos;
	Radius = pNiLight->Spec.r * Shadows->Settings.Interiors.LightRadiusMult;
	if (pNiLight->CanCarry)
		Radius = 256.0f;
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
		while (Entry) {
			if (TESObjectREFR* Ref = GetRef(Entry->item, &Settings->Forms)) {
				NiNode* RefNode = Ref->GetNode();
				if (RefNode->GetDistance(LightPos) <= Radius * 1.2f) RenderInterior(RefNode, MinRadius);
			}
			Entry = Entry->next;
		}
	}
}


static void FlagShaderPropertyRecurse(NiAVObject* apObject, UInt32 auiFlags, bool abSet) {
	if (!apObject)
		return;

	if (apObject->IsGeometry()) {
		NiGeometry* pGeometry = static_cast<NiGeometry*>(apObject);
		NiShadeProperty* shaderProperty = static_cast<NiShadeProperty*>(pGeometry->GetProperty(NiProperty::kType_Shade));
		if (shaderProperty) {
			if (abSet)
				shaderProperty->flags |= auiFlags;
			else
				shaderProperty->flags &= ~auiFlags;
		}

	}
	else if (apObject->IsNiNode()) {
		NiNode* pNiNode = static_cast<NiNode*>(apObject);
		for (UInt32 i = 0; i < pNiNode->m_children.end; i++) {
			FlagShaderPropertyRecurse(pNiNode->m_children.data[i], auiFlags, abSet);
		}
	}
}

static SInt32 frames = -1;
static void FlagPlayerGeometry() {
	frames++;

	// Run this function every 50 frames, or on launch
	if (frames > 50 || frames == -1) {
		if (Player->firstPersonNiNode)
			FlagShaderPropertyRecurse(Player->firstPersonNiNode, NiShadeProperty::kFirstPerson, true);

		NiNode* node = Player->GetNode();
		if (node)
			FlagShaderPropertyRecurse(node, NiShadeProperty::kThirdPerson, true);

		frames = 0;
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
TESObjectREFR* ShadowManager::GetRef(TESObjectREFR* Ref, ShadowsExteriorEffect::FormsStruct* Forms) {

	TESObjectREFR* R = NULL;

	if (Ref && Ref->GetNode()) {
		TESForm* Form = Ref->baseForm;
		ExtraRefractionProperty* RefractionExtraProperty = (ExtraRefractionProperty*)Ref->extraDataList.GetExtraData(BSExtraData::ExtraDataType::kExtraData_RefractionProperty);
		float Refraction = RefractionExtraProperty ? (1 - RefractionExtraProperty->refractionAmount) : 0.0f;
		if (Refraction > 0.5) return NULL;

		if (!(Ref->flags & TESForm::FormFlags::kFormFlags_NotCastShadows)) {
			UInt8 TypeID = Form->formType;
			if ((TypeID == TESForm::FormType::kFormType_Activator && Forms->Activators) ||
				(TypeID == TESForm::FormType::kFormType_Apparatus && Forms->Apparatus) ||
				(TypeID == TESForm::FormType::kFormType_Book && Forms->Books) ||
				(TypeID == TESForm::FormType::kFormType_Container && Forms->Containers) ||
				(TypeID == TESForm::FormType::kFormType_Door && Forms->Doors) ||
				(TypeID == TESForm::FormType::kFormType_Misc && Forms->Misc) ||
				(TypeID >= TESForm::FormType::kFormType_Stat && TypeID <= TESForm::FormType::kFormType_MoveableStatic && Forms->Statics) ||
				(TypeID == TESForm::FormType::kFormType_Tree && Forms->Trees) ||
				(TypeID == TESForm::FormType::kFormType_Furniture && Forms->Furniture) ||
				(TypeID == TESForm::FormType::kFormType_Land && Forms->Terrain) ||
				(TypeID >= TESForm::FormType::kFormType_NPC && TypeID <= TESForm::FormType::kFormType_LeveledCreature && Forms->Actors))
				R = Ref;
		}
	}
	return R;

}


void ShadowManager::SelectGeometry(NiGeometry* Geo) {

	if (!Geo->shader) return;
	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;
	UInt32 visibility = ShadowMapVisibility::None;
	if (TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapNear].ShadowMapFrustum, Geo)) visibility = ShadowMapVisibility::Near;
	else if (TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapMiddle].ShadowMapFrustum, Geo)) visibility = ShadowMapVisibility::Middle;
	else if (TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapFar].ShadowMapFrustum, Geo)) visibility = ShadowMapVisibility::Far;
	else if (TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapLod].ShadowMapFrustum, Geo)) visibility = ShadowMapVisibility::Lod;
	if (TheCameraManager->InFrustum(&Shadows->ShadowMaps[MapOrtho].ShadowMapFrustum, Geo)) visibility |= ShadowMapVisibility::Ortho;
	BSShaderProperty* ShaderProperty = (BSShaderProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Shade);
	bool haveLightingProperty = ShaderProperty && ShaderProperty->IsLightingProperty();

	//if (haveLightingProperty) { /*Leaves don't have a shade property or is not a LightingProperty*/
	//	BSShaderPPLightingProperty* lightProperty = (BSShaderPPLightingProperty*)ShaderProperty;

		/*Oblivion: Only seen 0.0 or 0.208. IS the structure actually corrrect?*/
		//if (lightProperty->IsRefractive()) return; //Configure,also check for actors, they are skinned
	//}

	bool alphaObject = false;
	if (AlphaEnabled) {
		NiAlphaProperty* AProp = (NiAlphaProperty*)Geo->GetProperty(NiProperty::PropertyType::kType_Alpha);
		if (AProp->flags & NiAlphaProperty::AlphaFlags::ALPHA_BLEND_MASK || AProp->flags & NiAlphaProperty::AlphaFlags::TEST_ENABLE_MASK) alphaObject = true;
	}
	//	if (alphaObject && !haveLightingProperty) return;
	if (Geo->skinInstance && !Geo->geomData->BuffData && Geo->skinInstance->SkinPartition->Partitions[0].BuffData) {
		if (alphaObject) TheShadowManager->skinnedAlphaPass->GeometryList.push_back(std::make_tuple(Geo, visibility));
		else TheShadowManager->skinnedGeoPass->GeometryList.push_back(std::make_tuple(Geo, visibility));
		return;
	}
	else if (Geo->skinInstance && !Geo->geomData->BuffData) {
		//		Logger::Log("Skinned but no partition: %s   %s", Geo->m_pcName, Geo->m_parent ? Geo->m_parent->m_pcName : "<No parent>");
		return;
	}
	//if(!Geo->geomData->BuffData) TheRenderManager->AddGeometryToUnsharedGroup(Geo->geomData);  //TODO Oblivion only for now. Find new vegas?
	if (Geo->geomData->BuffData) {
		if (Geo->m_parent->m_pcName && !memcmp(Geo->m_parent->m_pcName, "Leaves", 6)) TheShadowManager->speedTreePass->GeometryList.push_back(std::make_tuple(Geo, visibility));
		else if (alphaObject) TheShadowManager->alphaPass->GeometryList.push_back(std::make_tuple(Geo, visibility));
		else TheShadowManager->geometryPass->GeometryList.push_back(std::make_tuple(Geo, visibility));
	}
	else {
		//	Logger::Log("%s   %s", Geo->m_pcName, Geo->m_parent ? Geo->m_parent->m_pcName : "<No parent>");
	}
}

void ShadowManager::AccumulateGeometry(NiAVObject* accum) {
	if (accum) {
		if (!(accum->m_flags & NiAVObject::APP_CULLED)) {
			void* VFT = *(void**)accum;
			if (TheRenderManager->IsNode(accum)) {
				if (VFT == Pointers::VirtualTables::BSFadeNode && ((BSFadeNode*)accum)->FadeAlpha < 0.75f) return;
				NiNode* Node = (NiNode*)accum;
				if (Node) {
					for (int i = 0; i < Node->m_children.numObjs; i++) {
						AccumulateGeometry(Node->m_children.data[i]);
					}
				}
			}
			else if (VFT == Pointers::VirtualTables::NiTriShape || VFT == Pointers::VirtualTables::NiTriStrips) {
				SelectGeometry(static_cast<NiGeometry*>(accum));

			}
			//else if (VFT != Pointers::VirtualTables::NiPointLight /*TODO attenuaton map?Test for the affecedNode list*/ && VFT != Pointers::VirtualTables::NiParticleSystem) {
			//	Logger::Log("Unknown %0X", VFT);
			//}
		}
	}
}

void ShadowManager::RenderShadowExteriorMaps(D3DXVECTOR3* At) {
	GridCellArray* CellArray = Tes->gridCellArray;
	UInt32 CellArraySize = CellArray->size * CellArray->size;

	ShadowsExteriorEffect* Shadows = TheShaderManager->Effects.ShadowsExteriors;

	D3DXVECTOR4 OrthoDir = D3DXVECTOR4(0.05f, 0.05f, 1.0f, 0.0f);
	D3DXVECTOR4* SunDir = &TheShaderManager->ShaderConst.SunDir;

	D3DXVECTOR4* ShadowData = &TheShaderManager->Effects.ShadowsExteriors->Constants.Data;
	D3DXVECTOR4* OrthoData = &TheShaderManager->Effects.ShadowsExteriors->Constants.OrthoData;

	D3DXMATRIX ViewSun, ViewOrtho;
	float FarPlane = ShadowMapFarPlane;
	ViewSun = GetViewMatrix(At, SunDir);
	ViewOrtho = GetViewMatrix(At, &OrthoDir);

	NiDX9RenderState* RenderState = TheRenderManager->renderState;

	TheShadowManager->geometryPass->GeometryList.clear();
	TheShadowManager->alphaPass->GeometryList.clear();
	TheShadowManager->speedTreePass->GeometryList.clear();
	TheShadowManager->skinnedGeoPass->GeometryList.clear();
	TheShadowManager->skinnedAlphaPass->GeometryList.clear();

	for (UInt32 i = 0; i < CellArraySize; i++) {
		if (TESObjectCELL* Cell = CellArray->GetCell(i)) {
			std::vector<NiNode*> TerrainNodes = Cell->GetTerrainNodes();
			for (NiNode* node : TerrainNodes) {
				AccumulateGeometry(node);
			}
			//			if (ShadowsExteriors->Forms[ShadowMapType].Lod) RenderLod(Tes->landLOD, ShadowMapType); //Render terrain LOD
			/*for (UInt32 i = 2; i < Cell->GetNode()->m_children.numObjs; i++) {
				//For NewVegas: 0 Actor, 2 Land, 3 Static, 4 Dynamic,5 Multibound, 1 Marker
				//For Oblivion: 0 Actor, 2-5 Static Terrain is in subnode 0 of every 2-5 node. Node 0 seems to be unstable, some actors aren't rendered sometimes (not present in the node?)
				NiNode* TerrainNode = (NiNode*)Cell->GetNode()->m_children.data[i];
				AccumulateGeometry(TerrainNode);
			} */
			TList<TESObjectREFR>::Entry* Entry = &Cell->objectList.First;
			while (Entry) {
				if (TESObjectREFR* Ref = GetRef(Entry->item, &Shadows->ShadowMaps[MapNear].Forms)) {
					NiNode* RefNode = Ref->GetNode();
					AccumulateGeometry(RefNode);
				}
				Entry = Entry->next;
			}
		}
	}

	RenderState->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_TRUE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE, RenderStateArgs);
	RenderState->SetRenderState(D3DRS_ALPHABLENDENABLE, 0, RenderStateArgs);

	for (UInt32 i = ShadowMapTypeEnum::MapNear; i <= ShadowMapTypeEnum::MapOrtho; i++) {
		ShadowsExteriorEffect::ShadowMapSettings* ShadowMap = &Shadows->ShadowMaps[i];
		if (i == MapOrtho) {
			BillboardRight = { ViewOrtho._11, ViewOrtho._21, ViewOrtho._31, 0.0f };
			BillboardUp = { ViewOrtho._12, ViewOrtho._22, ViewOrtho._32, 0.0f };

			ShadowData->z = 1; // identify ortho map in shader constant
			Shadows->Constants.ShadowViewProj = Shadows->GetOrthoViewProj(ViewOrtho);
			ShadowMap->ShadowCameraToLight = TheRenderManager->InvViewProjMatrix * ViewOrtho;
			TheCameraManager->SetFrustum(&ShadowMap->ShadowMapFrustum, &ViewOrtho);
			OrthoData->x = Shadows->ShadowMaps[MapOrtho].ShadowMapRadius * 2;
		}
		else {
			BillboardRight = { ViewSun._11, ViewSun._21, ViewSun._31, 0.0f };
			BillboardUp = { ViewSun._12, ViewSun._22, ViewSun._32, 0.0f };

			ShadowData->z = 0; // set shader constant to identify other shadow maps
			Shadows->Constants.ShadowViewProj = Shadows->GetCascadeViewProj(ShadowMap, ViewSun);
			ShadowMap->ShadowCameraToLight = TheRenderManager->InvViewProjMatrix * ViewSun;
			TheCameraManager->SetFrustum(&ShadowMap->ShadowMapFrustum, &ViewSun);
		}
		IDirect3DDevice9* Device = TheRenderManager->device;

		Device->SetRenderTarget(0, ShadowMap->ShadowMapSurface);
		Device->SetDepthStencilSurface(ShadowMap->ShadowMapDepthSurface);
		Device->SetViewport(&ShadowMap->ShadowMapViewPort);

		Device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0L);

		Device->BeginScene();

		//if ((SunDir->z > 0.0f && i <= ShadowMapTypeEnum::MapLod) || i == ShadowMapTypeEnum::MapOrtho) {
			geometryPass->RenderNormalPass((ShadowMapTypeEnum)i);
			alphaPass->RenderAlphaPass((ShadowMapTypeEnum)i);
			skinnedGeoPass->RenderSkinnedPass((ShadowMapTypeEnum)i);
			speedTreePass->RenderSpeedTreePass((ShadowMapTypeEnum)i);
		//}

		Device->EndScene();
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
	
	D3DXVECTOR4 PlayerPosition = Player->pos.toD3DXVEC4();
	TESObjectCELL* currentCell = Player->parentCell;

	// Flag player geometry so we can control if it should be rendered in shadow cubemaps
	FlagPlayerGeometry();

	// Render directional shadows for Sun/Moon
	ShadowData->w = ShadowsExteriors->ShadowMode;	// Mode (0:off, 1:VSM, 2:ESM, 3: ESSM);
	NiNode* PlayerNode = Player->GetNode();
	D3DXVECTOR3 At;
	At.x = PlayerNode->m_worldTransform.pos.x - TheRenderManager->CameraPosition.x;
	At.y = PlayerNode->m_worldTransform.pos.y - TheRenderManager->CameraPosition.y;
	At.z = PlayerNode->m_worldTransform.pos.z - TheRenderManager->CameraPosition.z;

	// Render all shadow maps
	D3DXVECTOR4* SunDir = &TheShaderManager->ShaderConst.SunDir;
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
		interiorPass->VertexShader = ShadowCubeMapVertex;
		interiorPass->PixelShader = ShadowCubeMapPixel;

		auto shadowMapTimer = TimeLogger();
		// render ortho map
		RenderShadowExteriorMaps(&At);
		if (ShadowsExteriors->BlurShadowMaps) {
			for (int i = MapNear; i < MapOrtho; i++) {
				ShadowsExteriorEffect::ShadowMapSettings* ShadowMap = &Shadows->ShadowMaps[i];
				BlurShadowMap(ShadowMap);
			}
		}

		std::string message = "ShadowManager::RenderShadowMap";
		shadowMapTimer.LogTime(message.c_str());

	}

	// Render shadow maps for point lights
	/*bool usePointLights = (TheShaderManager->GameState.isDayTime > 0.5) ? ShadowsExteriors->UsePointShadowsDay : ShadowsExteriors->UsePointShadowsNight;

	AlphaEnabled = ShadowsInteriors->Forms.AlphaEnabled;
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
			if (!SpotLights[i] || SpotLights[i]->Spec.r == 0) continue; //bypass lights with no radius

			RenderShadowSpotlight(SpotLights, i);

			std::string message = "ShadowManager::RenderShadowSpotLight";
			message += std::to_string(i);
			shadowMapTimer.LogTime(message.c_str());
		}
	}*/

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

