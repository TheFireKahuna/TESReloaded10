void CameraManager::Initialize() {

	Logger::Log("Starting the camera manager...");
	TheCameraManager = new CameraManager();

	TheCameraManager->DialogTarget = NULL;
	TheCameraManager->From = { 0.0f, 0.0f, 0.0f };
	TheCameraManager->FromOffset = { 0.0f, 0.0f, 0.0f };
	TheCameraManager->CameraCommands = new CameraManagerCommands();
	TheCameraManager->FirstPersonView = false;
	TheCameraManager->TogglePOV = false;

}

void CameraManager::ResetCamera() {
	
	ThisCall(0x0066C600, this);

}

void CameraManager::SetSceneGraph() {
	
	float* SettingWorldFoV = (float*)0x00B0313C;
	float* Setting1stPersonFoV = (float*)0x00B0313C;
	float* SettingNearDistance = (float*)0x00B03134;
	float FoV = TheSettingManager->SettingsMain.CameraMode.FoV;
	
	*SettingNearDistance = 2.5f;
	if (FoV != WorldSceneGraph->cameraFOV && !Player->IsAiming() && InterfaceManager->IsActive(Menu::MenuType::kMenuType_None)) {
		WorldSceneGraph->UpdateParticleShaderFoVData(FoV);
		Player->worldFoV = *SettingWorldFoV = *Setting1stPersonFoV = FoV;
	}
	TheShaderManager->ShaderConst.ReciprocalResolution.w = Player->worldFoV;
}

bool CameraManager::IsVanityView() {
	
	return *(bool*)0x00B3BB04;

}

void CameraManager::ExecuteCommands(NiAVObject* CameraNode) {

	CameraCommands->Execute(CameraNode);

}

CameraManager::CameraManagerCommands::CameraManagerCommands() {

	memset(this, NULL, sizeof(CameraManagerCommands));

}

void CameraManager::CameraManagerCommands::Execute(NiAVObject* CameraNode) {

	if (!IsTranslating && !IsRotating && !IsLookingAt && !IsTranslatingToPosition && !IsRotatingToPosition && !IsLookingAtPosition) return;

	NiMatrix33* CameraRotationW = &CameraNode->m_worldTransform.rot;
	NiPoint3* CameraPositionW = &CameraNode->m_worldTransform.pos;
	NiMatrix33* CameraRotationL = &CameraNode->m_localTransform.rot;
	NiPoint3* CameraPositionL = &CameraNode->m_localTransform.pos;
	NiMatrix33* NodeRotationW = NULL;
	NiPoint3* NodePositionW = NULL;
	NiMatrix33 m;
	NiPoint3 v;

	if (Ref) {
		NiNode* RootNode = Ref->GetNiNode();
		if (Ref->IsActor() && (IsTranslating || IsLookingAt)) {
			NiAVObject* Head = RootNode->GetObjectByName("Bip01 Head");
			NodeRotationW = &Head->m_worldTransform.rot;
			NodePositionW = &Head->m_worldTransform.pos;
		}
		else {
			NodeRotationW = &RootNode->m_worldTransform.rot;
			NodePositionW = &RootNode->m_worldTransform.pos;
		}
	}

	if (IsTranslating) {
		v = *NodeRotationW * Position;
		v.x += NodePositionW->x;
		v.y += NodePositionW->y;
		v.z += NodePositionW->z;
		memcpy(CameraPositionW, &v, sizeof(NiPoint3));
		memcpy(CameraPositionL, &v, sizeof(NiPoint3));
	}

	if (IsRotating) {
		m.GenerateRotationMatrixZXY(&Rotation, 1);
		m = *NodeRotationW * m;
		memcpy(CameraRotationW, &m, sizeof(NiMatrix33));
		memcpy(CameraRotationL, &m, sizeof(NiMatrix33));
	}

	if (IsTranslatingToPosition) {
		memcpy(CameraPositionW, &Position, sizeof(NiPoint3));
		memcpy(CameraPositionL, &Position, sizeof(NiPoint3));
	}

	if (IsRotatingToPosition) {
		m.GenerateRotationMatrixZXY(&Rotation, 1);
		memcpy(CameraRotationW, &m, sizeof(NiMatrix33));
		memcpy(CameraRotationL, &m, sizeof(NiMatrix33));
	}

	if (IsLookingAt || IsLookingAtPosition) {
		if (!IsTranslating && !IsTranslatingToPosition) {
			Position.x = CameraPositionW->x;
			Position.y = CameraPositionW->y;
			Position.z = CameraPositionW->z;
		}
		if (IsLookingAt) {
			LookingAtPosition = *NodeRotationW * LookingAtOffset;
			LookingAtPosition.x += NodePositionW->x;
			LookingAtPosition.y += NodePositionW->y;
			LookingAtPosition.z += NodePositionW->z;
		}
		Position.GetLookAt(&LookingAtPosition, &v);
		m.GenerateRotationMatrixZXY(&v, 1);
		memcpy(CameraRotationW, &m, sizeof(NiMatrix33));
		memcpy(CameraRotationL, &m, sizeof(NiMatrix33));
	}

}