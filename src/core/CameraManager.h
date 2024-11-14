#pragma once

class CameraManager { // Never disposed
public:
	static void Initialize();

	class CameraManagerCommands { // Never disposed
	public:
		CameraManagerCommands();

		bool			IsTranslating;
		bool			IsRotating;
		bool			IsLookingAt;
		bool			IsTranslatingToPosition;
		bool			IsRotatingToPosition;
		bool			IsLookingAtPosition;
		NiVector3		Position;
		NiVector3		Rotation;
		NiVector3		LookingAtOffset;
		NiVector3		LookingAtPosition;
		TESObjectREFR*	Ref;

		void			Execute(NiAVObject* CameraNode);

	};
	
	bool					IsFirstPerson();
	bool					IsVanity();
	void					SetFirstPerson(bool FirstPerson);
	void					SetSceneGraph();
	void					ResetCamera();
	void					ExecuteCommands(NiAVObject* CameraNode);
	void					GetPlane(D3DXPLANE* plane, float a, float b, float c, float d);
	void					SetFrustum(frustum* Frustum, D3DMATRIX* Matrix);
	bool					InFrustum(frustum* frustum, NiNode* Node);

	Actor*					DialogTarget;
	NiPoint3				From;
	NiPoint3				FromOffset;
	CameraManagerCommands*	CameraCommands;
	bool					TogglePOV;

private:
	bool					FirstPersonView;
};