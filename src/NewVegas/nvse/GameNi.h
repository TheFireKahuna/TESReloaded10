#pragma once

#define CREATE_OBJECT(CLASS, ADDRESS) static CLASS* CreateObject() { return StdCall<CLASS*>(ADDRESS); };

extern UInt32 g_TLSIndex;

struct NiRTTI {
	const char* name;
	NiRTTI* parent;
};
static_assert(sizeof(NiRTTI) == 0x008);

template <typename T>
class NiTList {
public:
	struct Entry {
		Entry* next;
		Entry* prev;
		T* data;
	};

	Entry*	start;		// 000
	Entry*	end;		// 004
	UInt32	numItems;	// 008
};
static_assert(sizeof(NiTList<void>) == 0x00C);

template <typename T>
class NiTArray {
public:
	UInt16			Add(T* Item);

	virtual ~NiTArray();	// 00
	T*		data;			// 04
	UInt16	capacity;		// 08 - init'd to size of preallocation
	UInt16	end;			// 0A - index of the first free entry in the block of free entries at the end of the array (or numObjs if full)
	UInt16	numObjs;		// 0C - init'd to 0
	UInt16	growSize;		// 0E - init'd to size of preallocation
};
static_assert(sizeof(NiTArray<void>) == 0x010);

class NiPixelFormat {
public:
	enum Format {
		kFormat_RGB = 0,
		kFormat_RGBA,
		kFormat_PAL,
		kFormat_PALALPHA,
		kFormat_DXT1,
		kFormat_DXT3,
		kFormat_DXT5,
		kFormat_RGB24NONINTERLEAVED,
		kFormat_BUMP,
		kFormat_BUMPLUMA,
		kFormat_RENDERERSPECIFIC,
		kFormat_ONE_CHANNEL,
		kFormat_TWO_CHANNEL,
		kFormat_THREE_CHANNEL,
		kFormat_FOUR_CHANNEL,
		kFormat_DEPTH_STENCIL,
		kFormat_UNKNOWN,
		kFormat_MAX
	};

	enum Component {
		kComp_RED = 0,
		kComp_GREEN,
		kComp_BLUE,
		kComp_ALPHA,
		kComp_COMPRESSED,
		kComp_OFFSET_U,
		kComp_OFFSET_V,
		kComp_OFFSET_W,
		kComp_OFFSET_Q,
		kComp_LUMA,
		kComp_HEIGHT,
		kComp_VECTOR_X,
		kComp_VECTOR_Y,
		kComp_VECTOR_Z,
		kComp_PADDING,
		kComp_INTENSITY,
		kComp_INDEX,
		kComp_DEPTH,
		kComp_STENCIL,
		kComp_EMPTY,
		kComp_MAX
	};

	enum Representation {
		kRep_NORM_INT = 0,
		kRep_HALF,
		kRep_FLOAT,
		kRep_INDEX,
		kRep_COMPRESSED,
		kRep_UNKNOWN,
		kRep_INT,
		kRep_MAX
	};

	enum Tiling {
		kTile_NONE = 0,
		kTile_XENON,
		kTile_MAX
	};

	struct NiComponentSpec {
		Component		eComponent;			// 0
		Representation	eRepresentation;	// 4
		UInt8			BitsPerComponent;	// 8
		UInt8			Signed;				// 9
		UInt8			padA[2];			// A
	};

	UInt8			BitsPerPixel;	// 00
	UInt8			SRGBSpace;		// 01
	UInt8			pad02[2];
	Format			eFormat;		// 04
	Tiling			eTiling;		// 08
	UInt32			RendererHint;	// 0C
	UInt32			ExtraData;		// 10
	NiComponentSpec	Components[4];	// 14
};

template <typename T_Key, typename T_Data> class NiTMap
{
	Use_HashMapUtils(NiTMap)

		struct Entry
	{
		Entry* next;
		T_Key		key;
		T_Data		data;
	};

	struct Bucket
	{
		Entry* entries;

		UInt32 Size() const
		{
			UInt32 size = 0;
			for (Entry* pEntry = entries; pEntry; pEntry = pEntry->next, size++);
			return size;
		}
	};

	UInt32		numBuckets;	// 04
	Bucket* buckets;	// 08
	UInt32		numItems;	// 0C

	Bucket* GetBuckets() const { return buckets; }
	Bucket* End() const { return buckets + numBuckets; }

public:
	/*00*/virtual void		Destroy(bool doFree);
	/*04*/virtual UInt32	CalculateBucket(T_Key key);
	/*08*/virtual bool		Equal(T_Key key1, T_Key key2);
	/*0C*/virtual void		FillEntry(Entry* entry, T_Key key, T_Data data);
	/*10*/virtual void		FreeKey(Entry* entry);
	/*14*/virtual Entry* AllocNewEntry();
	/*18*/virtual void		FreeEntry(Entry* entry);

	UInt32 Size() const { return numItems; }
	bool Empty() const { return !numItems; }
	UInt32 BucketCount() const { return numBuckets; }

	T_Data __fastcall Lookup(T_Key key) const;

	void _fastcall FreeBuckets() ;

	class Iterator
	{
		NiTMap* table;
		Bucket* bucket;
		Entry* entry;

		void FindNonEmpty()
		{
			for (Bucket* end = table->End(); bucket != end; bucket++)
				if (entry = bucket->entries) break;
		}

	public:
		Iterator(NiTMap& _table) : table(&_table), bucket(table->buckets), entry(nullptr) { FindNonEmpty(); }

		explicit operator bool() const { return entry != nullptr; }
		void operator++()
		{
			entry = entry->next;
			if (!entry)
			{
				bucket++;
				FindNonEmpty();
			}
		}
		T_Data Get() const { return entry->data; }
		T_Key Key() const { return entry->key; }
		Entry* GetEntry() { return entry; }
	};

	Iterator Begin() { return Iterator(*this); }
};

// 14
template <typename T_Data>
class NiTStringPointerMap : public NiTMap<const char*, T_Data*>
{
public:
	UInt8		byte10;
	UInt8		pad11[3];
};

static_assert(sizeof(NiPixelFormat) == 0x044);

class NiRefObject {
public:
	virtual void		Destructor(bool freeThis);	// 00
	virtual void		Free(void);					// 01

	UInt32				m_uiRefCount;	// 004
};
static_assert(sizeof(NiRefObject) == 0x008);

class NiObject : public NiRefObject {
public:
	/*08*/virtual NiRTTI* GetType();
	/*0C*/virtual NiNode* GetNiNode();	// Returns this
	/*10*/virtual BSFadeNode* GetFadeNode();	// Returns this
	/*14*/virtual BSMultiBoundNode* GetMultiBoundNode();	// Returns this
	/*18*/virtual NiGeometry* GetNiGeometry();	// Returns this
	/*1C*/virtual NiTriBasedGeom* GetTriBasedGeom();	// Returns this
	/*20*/virtual NiTriStrips* GetTriStrips();	// Returns this
	/*24*/virtual NiTriShape* GetTriShape();	// Returns this
	/*28*/virtual BSSegmentedTriShape* GetSegmentedTriShape();	// Returns this
	/*2C*/virtual BSResizableTriShape* GetResizableTriShape();	// Returns this
	/*30*/virtual NiParticles* GetNiParticles();	// Returns this
	/*34*/virtual NiLines* GetNiLines();	// Returns this
	/*38*/virtual bhkNiCollisionObject* GetCollisionObject();	// Returns this
	/*3C*/virtual bhkBlendCollisionObject* GetBlendCollisionObject();	// Returns this
	/*40*/virtual bhkRigidBody* GetRigidBody();	// Returns this
	/*44*/virtual bhkLimitedHingeConstraint* GetLimitedHingeConstraint();	// Returns this
	/*48*/virtual NiObject* Clone(NiObjectCopyInfo* copyInfo);
	/*4C*/virtual void		LoadBinary(NiStream* stream);
	/*50*/virtual void		LinkObject(NiStream* stream);
	/*54*/virtual void		RegisterStreamables(NiStream* stream);
	/*58*/virtual void		SaveBinary(NiStream* stream);
	/*5C*/virtual bool		Compare(NiObject* to);
	/*60*/virtual void		GetViewerStrings(NiTArray<char*>* strings);
	/*64*/virtual void		AddViewerStrings(NiTArray<char*>* strings);
	/*68*/virtual void		ProcessClone(NiObjectCopyInfo* copyInfo);
	/*6C*/virtual void		PostLinkObject(NiStream* stream);
	/*70*/virtual bool		StreamCanSkip();
	/*74*/virtual NiRTTI* GetStreamableRTTI();
	/*78*/virtual void		SetWorldBound(NiBound* worldBound);
	/*7C*/virtual UInt32	GetBlockAllocationSize();
	/*80*/virtual void		Unk_20(void);
	/*84*/virtual void		Unk_21(UInt32 arg);
	/*88*/virtual NiControllerManager* GetControllerManager();	// Returns this

	NiObject* __fastcall HasBaseType(const NiRTTI* baseType);
    
    
    void LogObjectAttributes();
};

enum InterpKeyType
{
	kKeyType_Linear = 1,
	kKeyType_Quadratic,
	kKeyType_TBC,
	kKeyType_XYZ,
	kKeyType_Const
};

template <typename T_Data> struct InterpKey
{
	float			time;
	T_Data			value;
};

template <typename T_Data> struct QuadraticKey : public InterpKey<T_Data>
{
	T_Data			forward;
	T_Data			backward;
};

template <typename T_Data> struct TBCKey : public InterpKey<T_Data>
{
	NiVector3		TBC;
};

static_assert(sizeof(NiObject) == 0x008);

class NiExtraData : public NiObject {
public:
	virtual	bool	Unk_23();
	virtual bool	Unk_24();

	NiFixedString	name;		// 08
};
static_assert(sizeof(NiExtraData) == 0x00C);

// 14
class NiBinaryExtraData : public NiExtraData
{
public:
	UInt32		unk0C;		// 0C
	UInt32		unk10;		// 10
};

// 10
class NiIntegerExtraData : public NiExtraData
{
public:
	SInt32		intData;	// 0C
};

// 10
class NiFloatExtraData : public NiExtraData
{
public:
	float		fltData;	// 0C
};

// 14
class NiFloatsExtraData : public NiExtraData
{
public:
	UInt32		count;		// 0C
	float* data;		// 10
};

// 10
class NiStringExtraData : public NiExtraData
{
public:
	NiFixedString	strData;	// 0C
};

// 1C
class NiVectorExtraData : public NiExtraData
{
public:
	NiVector4		vec4;		// 0C

	__forceinline static NiVectorExtraData* Create() { return CdeclCall<NiVectorExtraData*>(0xA91FE0); }
};

class NiObjectNET : public NiObject {
public:

	NiFixedString		m_blockName;						// 008
	NiTimeController*	m_controller;					// 00C
	NiExtraData**		m_extraDataList;				// 010
	UInt16				m_extraDataListLen;				// 014
	UInt16				m_extraDataListCapacity;		// 016

	const char* GetName() const { return m_blockName.Get(); }
	void __fastcall SetName(const char* newName);
	NiExtraData* __fastcall GetExtraData(UInt32 vtbl) const;
	__forceinline bool AddExtraData(NiExtraData* xData)
	{
		return ThisCall<bool>(0xA5BA40, this, xData);
	}
};
static_assert(sizeof(NiObjectNET) == 0x018);

struct PropertyNode{
    PropertyNode* next;
    PropertyNode* prev;
    NiProperty* data;
};
struct PropertyList {
    PropertyNode* first;
    PropertyNode* last;
    UInt32 count;
};

// 0C	c'tor @ 0x43D410
struct NiUpdateData
{
	float		timePassed;			// 00
	bool		updateControllers;	// 04
	bool		isMultiThreaded;	// 05
	UInt8		byte06;				// 06
	bool		updateGeomorphs;	// 07
	bool		updateShadowScene;	// 08
	UInt8		pad09[3];			// 09

	NiUpdateData() { ZeroMemory(this, sizeof(NiUpdateData)); }
};

extern const NiUpdateData kNiUpdateData;


struct NiObjectCopyInfo
{
	NiTMap<NiObject*, NiObject*>* map00;		// 00
	NiTMap<NiObject*, bool>* map04;		// 04
	UInt32							unk08;		// 08	Init'd to 1
	UInt8							byte0C;		// 0C	Init'd to 0x24
	UInt8							pad0D[3];	// 0D
	NiVector3						scale;		// 10	Appears to be unused

	NiObjectCopyInfo(UInt32 bucketCount)
	{
		ThisCall(0x4AD0C0, this, bucketCount);
		scale = { 1.0F, 1.0F, 1.0F };
	}
	//~NiObjectCopyInfo() {ThisCall(0x4AD1D0, this);}
};
// 1C
struct GeometryProperties
{
	NiAlphaProperty* alphaProp;		// 00
	NiCullingProperty* cullingProp;	// 04
	NiMaterialProperty* materialProp;	// 08
	BSShaderProperty* shaderProp;	// 0C
	NiStencilProperty* stencilProp;	// 10
	NiTexturingProperty* texturingProp;	// 14
	UInt32					unk18;			// 18
};


template <std::size_t N> struct Debug; // No definition

template <> struct Debug<12> {}; // Definition for "old" sizeof;

template struct Debug<sizeof(PropertyNode)>; // Issue error if definition is missing

class NiAVObject : public NiObjectNET {
public:
	/*8C*/virtual void		UpdateControllers(const NiUpdateData& updParams);
	/*90*/virtual void		ApplyTransform(NiMatrix33* arg1, NiVector3* arg2, bool arg3);
	/*94*/virtual void		Unk_25(UInt32 arg);
	/*98*/virtual void		Unk_26(UInt32 arg);
	/*9C*/virtual NiAVObject* GetObjectByName(NiFixedString* objName);
	/*A0*/virtual void		SetSelectiveUpdateFlags(UInt8* bSelectiveUpdate, UInt32 bSelectiveUpdateTransform, UInt8* bRigid);
	/*A4*/virtual void		UpdateDownwardPass(const NiUpdateData& updParams, UInt32 flags);
	/*A8*/virtual void		UpdateSelectedDownwardPass(const NiUpdateData& updParams, UInt32 flags);
	/*AC*/virtual void		UpdateRigidDownwardPass(const NiUpdateData& updParams, UInt32 flags);
	/*B0*/virtual void		Unk_2C(const GeometryProperties& properties);
	/*B4*/virtual void		Unk_2D(UInt32 arg);
	/*B8*/virtual void		UpdateWorldData(const NiUpdateData& updParams);
	/*BC*/virtual void		UpdateWorldBound();
	/*C0*/virtual void		UpdateTransformAndBounds(const NiUpdateData& updParams);
	/*C4*/virtual void		PreAttachUpdate(NiNode* newParent, const NiUpdateData& updParams);
	/*C8*/virtual void		PreAttachUpdateProperties(NiNode* newParent);
	/*CC*/virtual void		DetachParent(UInt32 arg);
	/*D0*/virtual void		UpdateUpwardPassParent();
	/*D4*/virtual void		OnVisible(NiCullingProcess* culling);
	/*D8*/virtual void		PurgeRendererData(NiDX9Renderer* renderer);

	enum NiFlags : UInt32 {
		APP_CULLED = 0x1,
		SELECTIVE_UPDATE = 0x2,
		SELECTIVE_UPDATE_TRANSFORMS = 0x4,
		SELECTIVE_UPDATE_CONTROLLER = 0x8,
		SELECTIVE_UPDATE_RIGID = 0x10,
		DISPLAY_OBJECT = 0x20,
		DISABLE_SORTING = 0x40,
		SELECTIVE_UPDATE_TRANSFORMS_OVERRIDE = 0x80,
		IS_NODE = 0x100,
		SAVE_EXTERNAL_GEOM_DATA = 0x200,
		NO_DECALS = 0x400,
		ALWAYS_DRAW = 0x800,
		MESH_LOD = 0x1000,
		FIXED_BOUND = 0x2000,
		TOP_FADE_NODE = 0x4000,
		IGNORE_FADE = 0x8000,
		NO_ANIM_SYNC_X = 0x10000,
		NO_ANIM_SYNC_Y = 0x20000,
		NO_ANIM_SYNC_Z = 0x40000,
		NO_ANIM_SYNC_S = 0x80000,
		NO_DISMEMBER = 0x100000,
		NO_DISMEMBER_VALIDITY = 0x200000,
		RENDER_USE = 0x400000,
		MATERIALS_APPLIED = 0x800000,
		HIGH_DETAIL = 0x1000000,
		FORCE_UPDATE = 0x2000000,
		PREPROCESSED_NODE = 0x4000000,
		UNK_27 = 0x8000000,
		UNK_28 = 0x10000000,
		IS_POINTLIGHT = 0x20000000,
		DONE_INIT_LIGHTS = 0x40000000,
		IS_INSERTED = 0x80000000,
	};
    
	NiNode*					m_parent;				// 018
	bhkNiCollisionObject*	m_collisionObject;		// 1C
	NiBound*				m_kWorldBound;			// 020
	DList<NiProperty>		propertyList;			// 024
	UInt32					m_flags;				// 030
	NiTransform				m_localTransform;		// 034
	NiTransform				m_worldTransform;		// 068

	inline NiMatrix33& LocalRotate() { return m_localTransform.rotate; }
	inline NiMatrix33& WorldRotate() { return m_worldTransform.rotate; }
	inline NiVector3& LocalTranslate() { return m_localTransform.translate; }
	inline NiVector3& WorldTranslate() { return m_worldTransform.translate; }

	inline void SetHidden(bool hide)
	{
		UBYT(m_flags) &= ~1;
		UBYT(m_flags) |= (UInt8)hide;
	}
	inline void Hide() { UBYT(m_flags) |= 1; }
	inline void Show() { UBYT(m_flags) &= ~1; }

	void Update();
	UInt32 GetIndex() const;
	bool ReplaceObject(NiAVObject* object);
	TESObjectREFR* GetParentRef() const;
	void AssignGeometryProps();

	float		GetDistance(NiPoint3* Point);
	NiBound*	GetWorldBound();
	float		GetWorldBoundRadius();
};
static_assert(sizeof(NiAVObject) == 0x9C);

class NiNode : public NiAVObject {
public:
	/*DC*/virtual void		AddObject(NiAVObject *object, bool firstFree);
	/*E0*/virtual void		AddObjectAt(UInt32 index, NiAVObject *object);
	/*E4*/virtual void		RemoveObject2(NiAVObject *toRemove, NiAVObject **arg2);
	/*E8*/virtual void		RemoveObject(NiAVObject *toRemove);		//	Calls RemoveObject2 with arg2 as ptr to NULL
	/*EC*/virtual void		RemoveNthObject2(UInt32 index, NiAVObject **arg2);
	/*F0*/virtual void		RemoveNthObject(UInt32 index);			//	Calls RemoveNthObject2 with arg2 as ptr to NULL
	/*F4*/virtual void		ReplaceNthObject2(UInt32 index, NiAVObject *replaceWith, NiAVObject **arg3);
	/*F8*/virtual void		ReplaceNthObject(UInt32 index, NiAVObject *replaceWith);	//	Calls ReplaceNthObject2 with arg3 as ptr to NULL
	/*FC*/virtual void		UpdateUpwardPass();

	NiTArray<NiAVObject*>	m_children;		// 
	static NiNode* __stdcall Create(const char* nameStr);		//	str of NiFixedString
	NiAVObject* __fastcall GetBlockByName(const char* nameStr) const;	//	str of NiFixedString
	NiAVObject* __fastcall GetBlock(const char* blockName) const;
	NiNode* __fastcall GetNode(const char* nodeName) const;
	NiAVObject* __fastcall FindBlockOfType(UInt32 typeVtbl) const;
	bool IsMovable() const;
	void __vectorcall SetAlphaRecurse(float alpha);
	void ResetShaderRenderPass();
	UInt32 GetBSXFlags() const;
	void AddPropertyRecurse(NiProperty* niProperty);
	void AddNoLightingPropertyRecurse();
	void __vectorcall SetMaterialPropValueRecurse(UInt32 traitID, float value);
	void New(UInt16 Children);
};
static_assert(sizeof(NiNode) == 0xAC);


// 0C
class BSMultiBoundShape : public NiObject
{
public:
	/*8C*/virtual UInt32	GetBoundType();
	/*90*/virtual float	GetRadius();
	/*94*/virtual int	CheckBSBound(BSMultiBound* targetBound);
	/*98*/virtual int	CheckBound(NiBound* targetBound);
	/*9C*/virtual bool	WithinFrustum(NiFrustumPlanes* planes);
	/*A0*/virtual bool	CompletelyWithinFrustum(NiFrustumPlanes* planes);
	/*A4*/virtual void	Unk_29(void);
	/*A8*/virtual void	Unk_2A(void);
	/*AC*/virtual void	Unk_2B(void);
	/*B0*/virtual void	Unk_2C(void);
	/*B4*/virtual bool	IsPointWithinRadius(const NiVector3& inPos);
	/*B8*/virtual NiVector3* GetShapeOrigin(NiVector3* out);

	UInt32		cullResult;	// 08
};

// 1C
class BSMultiBoundSphere : public BSMultiBoundShape
{
public:
	NiBound		sphere;		// 0C
};

// 24
class BSMultiBoundAABB : public BSMultiBoundShape
{
public:
	NiVector3		diagonalPt0;// 0C
	NiVector3		diagonalPt1;// 18
};

// 10
class BSMultiBound : public NiObject
{
public:
	/*8C*/virtual void	Unk_23(void);
	/*90*/virtual void	Unk_24(void);

	UInt32				unk08;		// 08
	BSMultiBoundShape* mbShape;	// 0C
};
// B4
class BSMultiBoundNode : public NiNode
{
public:
	/*100*/virtual void		Unk_40(UInt32 arg1, UInt32 arg2);
	/*104*/virtual void		Unk_41(void);
	/*108*/virtual void		Unk_42(UInt32 arg1);
	/*10C*/virtual void		Unk_43(UInt32 arg1);
	/*110*/virtual void		Unk_44(UInt32 arg1);

	BSMultiBound* multiBound;	// AC
	UInt32			cullingMode;	// B0
};
class NiBillboardNode : public NiNode {
public:
	/*100*/virtual void		Unk_40(void);

	enum BillboardMode
	{
		ALWAYS_FACE_CAMERA,
		ROTATE_ABOUT_UP,
		RIGID_FACE_CAMERA,
		ALWAYS_FACE_CENTER,
		RIGID_FACE_CENTER,
		BSROTATE_ABOUT_UP
	};

	UInt16			billboardMode;	// AC
	UInt16			wordAE;			// AE
	float			savedTime;		// B0

	__forceinline void RotateToCamera(NiCamera* camera) { ThisCall(0xA7DE40, this, camera); }
};
static_assert(sizeof(NiBillboardNode) == 0xB4);

// 1C0
class BSFaceGenAnimationData : public NiExtraData
{
public:
};

// F0
class BSFaceGenNiNode : public NiNode
{
public:
	/*100*/virtual BSFaceGenAnimationData* GetAnimData();
	/*104*/virtual void		SetAnimData(BSFaceGenAnimationData* inAnimData);
	/*108*/virtual NiMatrix33* GetRotation(NiMatrix33* outMat);
	/*10C*/virtual void		SetRotation(NiMatrix33* inMat);
	/*110*/virtual bool		GetAnimationUpdate();
	/*114*/virtual void		SetAnimationUpdate(bool arg);
	/*118*/virtual bool		GetApplyRotToParent();
	/*11C*/virtual void		SetApplyRotToParent(bool doSet);
	/*120*/virtual bool		GetFixedNormals();
	/*124*/virtual void		SetFixedNormals(bool doSet);
	/*128*/virtual void		Unk_4A(void* arg);

	BSFaceGenAnimationData* animData;			// AC
	NiMatrix33				rotation;			// B0
	UInt8					forceBaseMorph;		// D4
	UInt8					fixedNormals;		// D5
	UInt8					animationUpdate;	// D6
	UInt8					rotatedLastUpdate;	// D7
	UInt8					applyRotToParent;	// D8
	UInt8					padD9[3];			// D9
	float					lastTime;			// DC
	UInt8					usingLoResHead;		// E0
	UInt8					isThePlayer;		// E1
	UInt8					isInDialouge;		// E2
	UInt8					padE3;				// E3
	UInt32					unkE4;				// E4
	UInt32					unkE8;				// E8
	UInt8					byteEC;				// EC
	UInt8					padED[3];			// ED
};
static_assert(sizeof(BSFaceGenNiNode) == 0xF0);

class NiCamera : public NiAVObject {
public:
	D3DMATRIX		worldToCam;	// 09C
	NiFrustum		Frustum;			// D4
	float			MinNearPlaneDist;	// F8
	float			MaxFarNearRatio;	// FC
	NiViewport		ViewPort;			// 100
	float			LODAdjust;			// 110
	__forceinline static NiCamera* Create() { return CdeclCall<NiCamera*>(0xA71430); }

	bool __fastcall WorldToScreen(const NiVector3& worldPos, NiPoint2& scrPos);
};
static_assert(sizeof(NiCamera) == 0x114);

class SceneGraph : public NiNode {
public:
	void				UpdateParticleShaderFoV(float FoV);
	void				SetNearDistance(float Distance);
    
	NiCamera*			camera;					// AC
	UInt32				unkB0;					// B0
	NiCullingProcess*	cullingProcess;			// B4
	UInt8				IsMinFarPlaneDistance;	// B8 The farplane is set to 20480.0 when the flag is true. Probably used for interiors.
	UInt8				padB8[3];
	float				cameraFOV;				// BC
};
static_assert(sizeof(SceneGraph) == 0xC0);

class NiDynamicEffect : public NiAVObject {
public:
	enum EffectTypes {
		AMBIENT_LIGHT = 0,
		POINT_LIGHT = 2,
		DIR_LIGHT = 3,
		SPOT_LIGHT = 4,
		TEXTURE_EFFECT = 5,
		MAX_TYPES = TEXTURE_EFFECT + 1,
	};

	UInt8			switchState;	// 9C
	UInt8			EffectType;		// 9D
	UInt8			CastShadows;	// 9E	JIP only
	UInt8			CanCarry;		// 9F	JIP only
	SInt32			iIndex;			// A0
	UInt32			pushCount;		// A4
	UInt32			revID;			// A8
	DList<NiNode>	shadowGenerator;// AC
	DList<NiNode>	unaffectedNodes;// B8

};

class NiLight : public NiDynamicEffect {
public:
	float			fadeValue;			// C4
	NiColor			ambientColor;		// C8
	NiColor			diffuseColor;		// D4
};

class NiPointLight : public NiLight {
public:
	float			radius;			// 0E0
	float			radius0E4;		// 0E4
	TESObjectLIGH*	baseLight;		// 0E8	JIP only
	NiObject*		obj0EC;		// 0EC
	NiVector3		Atten;		// 0F0	Used for animated lights
	//	JIP Only
	float			flt0FC;			// 0FC
	NiVector4		vector100;		// 100
	__forceinline static NiPointLight* Create() { return CdeclCall<NiPointLight*>(0xA7D6E0); }
};
static_assert(sizeof(NiPointLight) == 0x110);

class NiDirectionalLight : public NiLight {
public:
	NiColor			fogColor;		// E0
	NiObject* objEC;			// EC
	NiVector3		direction;		// F0
};
static_assert(sizeof(NiDirectionalLight) == 0xFC);

class NiSpotLight : public NiLight {
public:
	
	float			radius;			// 0E0
	float			radius0E4;		// 0E4
	TESObjectLIGH	*baseLight;		// 0E8	JIP only
	NiObject		*obj0EC;		// 0EC
	NiVector3		vector0F0;		// 0F0	Used for animated lights
	NiPoint3		direction;
	float			OuterSpotAngle;
	float			InnerSpotAngle;
	float			SpotExponent;

	CREATE_OBJECT(NiSpotLight, 0xA94E20);
};
static_assert(sizeof(NiSpotLight) == 0x114);

class NiVBChip {
public:
	UInt32					Index;      // 00
	UInt32					Unk04;		// 04 union { NiVBBlock* m_pkBlock; NiVBDynamicSet* m_pkDynamicSet; };
	IDirect3DVertexBuffer9* VB;			// 08
	UInt32					Offset;		// 0C
	UInt32					LockFlags;	// 10
	UInt32					Size;		// 14
	NiVBChip*				Next;		// 18
	NiVBChip*				Prev;		// 1C
};
static_assert(sizeof(NiVBChip) == 0x020);

class NiGeometryBufferData {
public:
	virtual void		Destructor(bool freeThis);			// 00
	virtual void		Free();								// 01

	UInt32							Flags;					// 04
	NiGeometryGroup*				GeometryGroup;			// 08
	UInt32							FVF;					// 0C
	IDirect3DVertexDeclaration9*	VertexDeclaration;		// 10
	UInt32							unk10;					// 14
	UInt32							VertCount;				// 18
	UInt32							MaxVertCount;			// 1C
	UInt32							StreamCount;			// 20
	UInt32*							VertexStride;			// 24
	NiVBChip**						VBChip;					// 28
	UInt32							IndexCount;				// 2C
	UInt32							IBSize;					// 30
	IDirect3DIndexBuffer9*			IB;						// 34
	UInt32							BaseVertexIndex;		// 38
	D3DPRIMITIVETYPE				PrimitiveType;			// 3C
	UInt32							TriCount;				// 40
	UInt32							MaxTriCount;			// 44
	UInt32							NumArrays;				// 48
	UInt16*							ArrayLengths;			// 4C
	UInt16*							IndexArray;				// 50
};
static_assert(sizeof(NiGeometryBufferData) == 0x54);

class NiGeometryData : public NiObject
{
public:
	/*8C*/virtual void		SetVertexCount(UInt16 vtxCount);
	/*90*/virtual UInt16	GetVertexCount();
	/*94*/virtual NiTriStripsData	*GetStripsData();
	/*98*/virtual NiTriShapeData	*GetShapeData();
	/*9C*/virtual bool		ContainsDataType(UInt32 dataType);
	/*A0*/virtual void		CalculateNormals();

	enum Consistency {
		MUTABLE = 0x0000,
		STATIC = 0x4000,
		VOLATILE = 0x8000,
		CONSISTENCY_MASK = 0xF000,
	};
	enum KeepFlag
	{
		eKeep_Vertices =	1,
        eKeep_Normals =		2,
        eKeep_VertexColor =	4,
        eKeep_UVCoords =	8,
        eKeep_Indices =		0x10,
        eKeep_BoneData =	0x20,
		eKeep_All =			eKeep_Vertices | eKeep_Normals | eKeep_VertexColor | eKeep_UVCoords | eKeep_Indices | eKeep_BoneData
	};

	enum CompressFlag
	{
		eCompress_Normals =		1,
		eCompress_Color =		2,
		eCompress_UV =			4,
		eCompress_Weight =		8,
		eCompress_Position =	0x10,
		eCompress_All =			0x1F
	};

	UInt16						numVertices;	// 08
	UInt16						id;				// 0A
	UInt16						dataFlags;		// 0C	NormalBinormalTangent
	UInt16						dirtyFlags;		// 0E
	NiBound						bounds;			// 10
	NiPoint3*					vertices;		// 20
	NiPoint3*					normals;		// 24
	NiColorAlpha				*vertexColors;	// 28
	NiPoint2					*uvCoords;		// 2C
	NiAdditionalGeometryData	*additionalData;// 30
	NiGeometryBufferData		*bufferData;	// 34
	UInt8						keepFlags;		// 38
	UInt8						compressFlags;	// 39
	UInt8						byte3A;			// 3A
	UInt8						byte3B;			// 3B
	bool						canSave;		// 3C
	UInt8						pad3D[3];		// 3D

	void FlipNormals();
};
static_assert(sizeof(NiGeometryData) == 0x40);

// 44
class NiTriBasedGeomData : public NiGeometryData
{
public:
	/*A4*/virtual void		Unk_29(UInt32 arg);
	/*A8*/virtual UInt16	GetNumTriangles();
	/*AC*/virtual void		Unk_2B(UInt32 arg1, UInt32 arg2, UInt32 arg3, UInt32 arg4);
	/*B0*/virtual void		Unk_2C(UInt32 arg1, UInt32 arg2, UInt32 arg3, UInt32 arg4);

	UInt16			numTriangles;		// 40
	UInt16			activeTriangles;	// 42
};

// 58
class NiTriShapeData : public NiTriBasedGeomData
{
public:
	struct SharedNormalArray
	{
		UInt16		numSharedNormals;
		UInt16		pad02;
		UInt16* sharedNormalIndexArray;
	};

	struct SharedNormArrBlock
	{
		UInt16* pBlock;
		UInt16* pFreeBlock;
		UInt32				blockSize;
		UInt32				freeBlockSize;
		SharedNormArrBlock* pNext;
	};

	UInt32				trianglePoints;		// 44
	NiTriangle* triangles;			// 48
	SharedNormalArray* sharedNormals;		// 4C
	UInt16				sharedNormArrSize;	// 50
	UInt16				pad52;				// 52
	SharedNormArrBlock* snArrBlock;		// 54

	static NiTriShapeData* Create(UInt32 numVertices, NiPoint3* vertices, NiPoint3* normals, NiColorAlpha* vertexColor,
		NiPoint2* uvCoords, UInt32 numTriangles, NiTriangle* triangles)
	{
		return ThisCall<NiTriShapeData*>(0xA7B630, Ni_Alloc<NiTriShapeData>(), numVertices, vertices, normals, vertexColor, uvCoords, 0, 0, numTriangles, triangles);
	}
};
static_assert(sizeof(NiTriShapeData) == 0x58);

// 50
class NiTriStripsData : public NiTriBasedGeomData
{
public:
	UInt16			numStrips;		// 44
	UInt16			unk46;			// 46
	UInt16* stripLengths;	// 48
	UInt16* strips;		// 4C

	static NiTriStripsData* Create(UInt32 numVertices, NiPoint3* vertices, NiPoint3* normals, NiColorAlpha* vertexColor,
		NiPoint2* uvCoords, UInt32 numTriangles, UInt32 numStrips, UInt16* stripLengths, UInt16* strips)
	{
		return ThisCall<NiTriStripsData*>(0xA75DC0, Ni_Alloc<NiTriStripsData>(), numVertices, vertices, normals, vertexColor,
			uvCoords, 0, 0, numTriangles, numStrips, stripLengths, strips);
	}

	static NiTriStripsData* __vectorcall DrawConvex(float radius, UInt32 numEdges, const NiColorAlpha& color);
	static NiTriStripsData* __vectorcall DrawCylinder(float radius, float height, UInt32 numEdges, const NiColorAlpha& color);
	static NiTriStripsData* __vectorcall DrawPrism(float radius, float height, UInt32 numEdges, const NiColorAlpha& color);
};

class NiSkinPartition : public NiObject {
public:
	class Partition {
	public:
		void**					_vtbl;			// 00
		UInt16*					pBones;			// 04
		float*					Weights;		// 08
		UInt32					Unk04[4];		// 0C
		UInt16					Vertices;		// 1C
		UInt16					Triangles;		// 1E
		UInt16					Bones;			// 20
		UInt16					Strips;			// 22
		UInt16					BonesPerVertex; // 24
		UInt16					pad28;
		NiGeometryBufferData*	bufferData;		// 28
	};

	UInt32		PartitionsCount;		// 08
	Partition*	Partitions;				// 0C
};
static_assert(sizeof(NiSkinPartition) == 0x10);

class NiSkinData : public NiObject {
public:
	class BoneVertData {
	public:
		UInt16	Verts;		// 00
		float	Weight;		// 04
	};

	class BoneData {
	public:
		NiTransform		SkinToBone;		// 00
		NiBound			Bound;			// 34
		BoneVertData*	BoneVertData;	// 44
		UInt16			Verts;			// 48
		UInt16			pad44;
	};

	NiSkinPartition*	SkinPartition;		// 08
	NiTransform			RootParentToSkin;	// 0C
	BoneData*			BoneData;			// 40
	UInt32				Bones;				// 44
};
static_assert(sizeof(NiSkinData) == 0x48);

class NiSkinInstance : public NiObject {
public:
	NiSkinData		* SkinData;		// 08
	NiSkinPartition	* SkinPartition;	// 0C
	NiNode			* RootParent;		// 10
	NiAVObject		** BoneObjects;		// 14
	UInt32			FrameID;		// 18
	UInt32			numMatrices;	// 1C
	UInt32			numRegisters;	// 20
	UInt32			allocatedSize;	// 24
	void*			BoneMatrixes;	// 28
	void*			SkinToWorldWorldToSkin;			// 2C
	void*			Unk30;	// 30
	
	bool IsPartitionEnabled(UInt32 partitionIndex);
    
};
static_assert(sizeof(NiSkinInstance) == 0x34);

class DismemberPartition {
public:
    UInt8 Enabled;
    UInt8 StartCap;  //Questionable, but it's the only way I can make sense of that code
    UInt16 bodyPart;
}; 
static_assert(sizeof(DismemberPartition) == 4);

class BSDismemberSkinInstance : public NiSkinInstance {
public:
    UInt32 partitionNumber;
    DismemberPartition* partitions;
    UInt8  IsRenderable;  //In Load this is made in OR with every partition->Enabled flag
    UInt8  pad[3];
};
static_assert(sizeof(BSDismemberSkinInstance) == 0x40);

class NiProperty : public NiObjectNET {
public:
	enum PropertyType {
		kType_Alpha,
		kType_Culling,
		kType_Material,
		kType_Shade,
		kType_Stencil,
		kType_Texturing,
		kType_Dither = 8,
		kType_Specular = 9,
		kType_VertexColor = 10,
		kType_ZBuffer = 11,
		kType_Fog = 13,
	};
    virtual PropertyType	GetPropertyType();
	/*90*/virtual void		UpdateController(const NiUpdateData& updParams);
};
static_assert(sizeof(NiProperty) == 0x18);

class NiPropertyState {
public:
	NiProperty* prop[6];
	// 0 00
	// 1 04
	// 2 08
	// 3 0C
	// 4 10
	// 5 14
};

class NiGeometry : public NiAVObject {
public:

	virtual void	RenderImmediate(NiRenderer* pkRenderer);
	virtual void	RenderImmediateAlt(NiRenderer* pkRenderer);
	virtual void	SetModelData(NiGeometryData* pkModelData);
	virtual void	CalculateNormals();
	virtual void	CalculateConsistency(bool bTool);

	NiProperty*			GetProperty(NiProperty::PropertyType Type);

	NiPropertyState		propertyState;	// 9C
	UInt32				unkB4;			// B4
	NiGeometryData*		geomData;		// B8
	NiSkinInstance*		skinInstance;	// BC This seems to be a BSDismemberSkinInstance (old NiSkinInstance constructor is never used)
	NiD3DShader*		shader;			// C0

	void __fastcall AddProperty(NiProperty* niProperty);
};
static_assert(sizeof(NiGeometry) == 0xC4);

class NiBinaryStream
{
public:
	virtual void	Destructor(bool freeMemory);		// 00
	virtual void	Unk_01(void);						// 04
	virtual void	SeekCur(SInt32 delta);				// 08
	virtual void	GetBufferSize(void);				// 0C
	virtual void	InitReadWriteProcs(bool useAlt);	// 10

	//	void	** m_vtbl;		// 000
	UInt32	m_offset;		// 004
	void* m_readProc;	// 008 - function pointer
	void* m_writeProc;	// 00C - function pointer
};
class NiFile : public NiBinaryStream
{
public:
	virtual UInt32	SetOffset(UInt32 newOffset, UInt32 arg2);	// 14
	virtual UInt32	GetFilename(void);	// 18
	virtual UInt32	GetSize();			// 1C

	UInt32	m_bufSize;	// 010
	UInt32	m_unk014;	// 014 - Total read in buffer
	UInt32	m_unk018;	// 018 - Consumed from buffer
	UInt32	m_unk01C;	// 01C
	void* m_buffer;	// 020
	FILE* m_File;		// 024
};

class NiDX9TextureData : public NiObject {
public:
	NiTexture*				parent;			// 08
	UInt32					width;			// 0C
	UInt32					height;			// 10
	NiPixelFormat			PixelFormat;	// 14
	UInt8					Unk58;			// 58
	UInt8					Unk59;			// 59
	UInt8					Unk5A;			// 5A
	UInt8					Unk5B;			// 5B
	UInt32					Unk5C;			// 5C
	NiDX9Renderer*			pRenderer;		// 60
	IDirect3DBaseTexture9*	dTexture;		// 64
	UInt32					Levels;			// 68
};
static_assert(sizeof(NiDX9TextureData) == 0x6C);

class NiDX9SourceTextureData : public NiDX9TextureData {
public:
	UInt8					ReplacementData;	// 6C
	UInt8					Mipmap;				// 6D
	UInt8					pad64[2];
	UInt32					FormattedSize;		// 70
	UInt32					Palette;			// 74 NiPalette
	UInt32					LevelsSkipped;		// 78
	UInt32					SourceRevID;		// 7C
	UInt32					PalRevID;			// 80
};
static_assert(sizeof(NiDX9SourceTextureData) == 0x84);

class NiTexture : public NiObjectNET {
public:
	/*8C*/virtual void		Unk_23(void);
	/*90*/virtual void		Unk_24(void);
	/*94*/virtual UInt32	GetWidth();
	/*98*/virtual UInt32	GetHeight();
	/*9C*/virtual NiFixedString* GetPath();
	/*A0*/virtual UInt32	GetPixelCount();

	enum PixelLayout {
		kPixelLayout_Palettized8 = 0,
		kPixelLayout_HighColor16,
		kPixelLayout_TrueColor32,
		kPixelLayout_Compressed,
		kPixelLayout_Bumpmap,
		kPixelLayout_Palettized4,
		kPixelLayout_PixDefault,
		kPixelLayout_SingleColor8,
		kPixelLayout_SingleColor16,
		kPixelLayout_SingleColor32,
		kPixelLayout_DoubleColor32,
		kPixelLayout_DoubleColor64,
		kPixelLayout_FloatColor32,
		kPixelLayout_FloatColor64,
		kPixelLayout_FloatColor128,
		kPixelLayout_SingleColor4,
	};

	enum AlphaFormat {
		kAlpha_None = 0,
		kAlpha_Binary,	// 1bpp
		kAlpha_Smooth,	// 8bpp
		kAlpha_Default,
	};

	enum MipMapFlag {
		kMipMap_Disabled = 0,
		kMipMap_Enabled,
		kMipMap_Default,
	};

	struct FormatPrefs {
		PixelLayout pixelLayout;
		AlphaFormat alphaFormat;
		MipMapFlag	mipmapFormat;
	};

	FormatPrefs			formatPrefs;	// 018
	NiDX9TextureData*	rendererData;	// 024
	NiTexture*			nextTex;		// 028 - linked list updated in ctor/dtor
	NiTexture*			prevTex;		// 02C


	NiDX9TextureData* GetDX9RendererData() {
		return reinterpret_cast<NiDX9TextureData*>(rendererData);
	};
};
static_assert(sizeof(NiTexture) == 0x30);

class NiSourceTexture : public NiTexture {
public:
	/*A4*/virtual void		LoadPixelDataFromFile();
	/*A8*/virtual void		DestroyAppPixelData();
	/*AC*/virtual void		CreateRendererData();

	NiFixedString	ddsPath1;		// 30
	NiFixedString	ddsPath2;		// 34
	void			*ptr38;			// 38
	NiPixelData		*pixelData;		// 3C
	bool			loadDirectToRender;	// 40
	bool			persistRenderData;	// 41
	bool			srcRendererDataIsPersistent;			// 42
	UInt8			byte43;			// 43
	NiFile			*pFile;			// 44
};
static_assert(sizeof(NiSourceTexture) == 0x48);

class NiRenderedTexture : public NiTexture {
public:
	virtual Ni2DBuffer* GetBuffer();

	Ni2DBuffer*			buffer;		// 30
	UInt32				cellRefID;		// 34	Used by MiniMap; otherwise appears unused
	UInt32				unk38;			// 38
	UInt32				multiSampleLvl;	// 3C
};
static_assert(sizeof(NiRenderedTexture) == 0x040);

class NiD3DTextureStage;
class NiD3DShaderConstantMap;

enum ShaderDefinitionEnum
{
  kShaderDefinition_ShadowLightShader = 0x1,
  kShaderDefinition_TallGrassShader = 0x2,
  kShaderDefinition_DistantLODShader = 0x3,
  kShaderDefinition_SpeedTreeBranchShader = 0x4,
  kShaderDefinition_SpeedTreeFrondShader = 0x5,
  kShaderDefinition_SpeedTreeLeafShader = 0x6,
  kShaderDefinition_BSShaderBloodSplatter = 0x7,
  kShaderDefinition_BSDistantTreeShader = 0x8,
  kShaderDefinition_NiD3DDefaultShader = 0x9,
  kShaderDefinition_SkyShader = 0xA,
  kShaderDefinition_HairShader = 0xD,
  kShaderDefinition_SkinShader = 0xE,
  kShaderDefinition_ParallaxShader = 0xF,
  kShaderDefinition_NiDX9ShaderDeclaration = 0x10,
  kShaderDefinition_WaterShader = 0x11,
  kShaderDefinition_ParticleShader = 0x18,
  kShaderDefinition_BoltShader = 0x19,
  kShaderDefinition_BeamShader = 0x1A,
  kShaderDefinition_Lighting30Shader = 0x1D,
  kShaderDefinition_PrecipitationShader = 0x1E,
  kShaderDefinition_TileShader = 0x20,
  kShaderDefinition_BSShaderNoLighting = 0x21,
  kShaderDefinition_VolumetricFogShader = 0x22,
};


class NiD3DShaderDeclaration : public NiObject {
public:
	NiDX9Renderer*				Renderer;		// 008
	NiDX9VertexBufferManager*	BufferManager;	// 00C
	IDirect3DDevice9*			Device;			// 010
	UInt32						Unk014;			// 014
	UInt32						Unk018;			// 018
	UInt32						Unk01C;			// 01C
	UInt32						StreamCount;	// 020
	UInt32						Unk024;			// 024
	UInt8						Unk028;			// 028
	UInt8						Unk029[3];		// 029

};
static_assert(sizeof(NiD3DShaderDeclaration) == 0x02C);

class NiDX9ShaderDeclaration : public NiD3DShaderDeclaration {
public:
	UInt32		Unk02C;			// 02C
	UInt32		Unk030;			// 030
	UInt8		Unk034;			// 034
	UInt8		Unk035[3];		// 035

};
static_assert(sizeof(NiDX9ShaderDeclaration) == 0x038);

class ShaderRecord;

class NiD3DShaderProgram : public NiRefObject {
public:
	enum ProgramType {
		PROGRAM_VERTEX = 0,
		PROGRAM_PIXEL,
		PROGRAM_GEOMETRY,
		PROGRAM_MAX
	};

	union {
		ProgramType				eProgramType;		// 08

		struct {
			UInt8				ucProgramType;				// 08
			UInt8				Unk09;					// 09
			UInt8				Unk0A;					// 0A
			bool				Enabled;				// 0B
		};
	};
	const char*	const		Name;				// 0C
	ShaderRecord*			ShaderProg[3];
	IUnknown*				ShaderHandleBackup;
	IDirect3DDevice9*		Device;				// 20
	NiDX9Renderer*			Renderer;			// 24
	NiDX9RenderState*		RenderState;		// 28

	ShaderRecord* GetShaderRecord(int Type) {
		return ShaderProg[Type];
	};

	void SetName(const char* name) {
		ThisStdCall(0xBE0920, this, name);
	}
};
static_assert(sizeof(NiD3DShaderProgram) == 0x2C);

class NiD3DVertexShader : public NiD3DShaderProgram {
public:
	UInt8							SoftwareVP;		// 2C
	UInt8							pad[3];			// 2D
	UInt32							Usage;			// 30
	IDirect3DVertexShader9*			ShaderHandle;	// 34
	IDirect3DVertexDeclaration9*	Declaration;	// 38
};
static_assert(sizeof(NiD3DVertexShader) == 0x3C);

class NiD3DPixelShader : public NiD3DShaderProgram {
public:
	IDirect3DPixelShader9* ShaderHandle;	// 2C
};
static_assert(sizeof(NiD3DPixelShader) == 0x30);

class NiD3DPass {
public:
	void**							_vtbl;						// 00
	char							Name[16];					// 04
	UInt32							CurrentStage;				// 14
	UInt32							StageCount;					// 18
	UInt32							TexturesPerPass;			// 1C
	NiTArray<NiD3DTextureStage*>	Stages;						// 20
	NiD3DRenderStateGroup*			RenderStateGroup;			// 30
	NiD3DShaderConstantMap*			PixelConstantMap;			// 34
	char*							PixelShaderProgramFile;		// 38
	char*							PixelShaderEntryPoint;		// 3C
	char*							PixelShaderTarget;			// 40
	NiD3DPixelShader*				PixelShader;				// 44
	NiD3DShaderConstantMap*			VertexConstantMap;			// 48
	UInt8							Unk04C;						// 4C
	UInt8							pad04C[3];
	char*							VertexShaderProgramFile;	// 50
	char*							VertexShaderEntryPoint;		// 54
	char*							VertexShaderTarget;			// 58
	NiD3DVertexShader*				VertexShader;				// 5C
	UInt8							SoftwareVP;					// 60
	UInt8							RendererOwned;				// 61
	UInt8							pad[2];
	UInt32							RefCount;					// 64
};
static_assert(sizeof(NiD3DPass) == 0x68);

class NiShader : public NiRefObject {
public:
	char*		Name;					// 008
	UInt32		Unk00C;					// 00C
	UInt8		Unk010;					// 010
	UInt8		pad010[3];
};
static_assert(sizeof(NiShader) == 0x14);

class NiD3DShaderInterface : public NiShader {
public:
	IDirect3DDevice9*	D3DDevice;		// 014
	NiDX9Renderer*		D3DRenderer;	// 018
	NiDX9RenderState*	D3DRenderState;	// 01C
	UInt8				Unk020;			// 020
};
static_assert(sizeof(NiD3DShaderInterface) == 0x24);

class NiD3DShader : public NiD3DShaderInterface {
public:
	UInt8					IsInitialized;		// 024
	UInt8					Unk021;				// 025
	UInt8					pad021[2];
	NiD3DShaderDeclaration* ShaderDeclaration;	// 028
	NiD3DRenderStateGroup*  RenderStateGroup;	// 02C
	NiD3DShaderConstantMap* PixelConstantMap;	// 030
	NiD3DShaderConstantMap* VertexConstantMap;	// 034
	UInt32					Unk038;				// 038
};
static_assert(sizeof(NiD3DShader) == 0x3C);

class NiD3DDefaultShader : public NiD3DShader {
public:
	UInt32					Unk03C[8];			// 03C	
};
static_assert(sizeof(NiD3DDefaultShader) == 0x5C);

class BSShader : public NiD3DDefaultShader {
public:
	enum ShaderType {
		TYPE_ShadowLightShader = 0x1,
		TYPE_HairShader = 0x2,
		TYPE_ParallaxShader = 0x3,
		TYPE_SkinShader = 0x4,
		TYPE_SpeedTreeBranchShader = 0x5,
		TYPE_TallGrassShader = 0x6,
		TYPE_DistantLODShader = 0x7,
		TYPE_SpeedTreeFrondShader = 0x8,
		TYPE_SpeedTreeLeafShader = 0x9,
		TYPE_NiD3DDefaultShader = 0xA,
		TYPE_SkyShader = 0xB,
		TYPE_GeometryDecalShader = 0xC,
		TYPE_WaterShader = 0xD,
		TYPE_ParticleShader = 0xE,
		TYPE_BoltShader = 0xF,
		TYPE_BeamShader = 0x10,
		TYPE_Lighting30Shader = 0x11,
		TYPE_PrecipitationShader = 0x12,
		TYPE_VolumetricFogShader = 0x13,
		TYPE_TileShader = 0x14,
		TYPE_BSShaderNoLighting = 0x16,
		TYPE_BSShaderBloodSplatter = 0x17,
		TYPE_BSImagespaceShader = 0x18,
		TYPE_BSDistantTreeShader = 0x19,
	};

	NiD3DPass* pass;
	void* Unk60;
	bool Unk64;
	UInt32 iShaderType;
};
static_assert(sizeof(BSShader) == 0x6C);

class WaterShader : public BSShader {
public:
	NiD3DPass*				pPasses[74];
	NiD3DVertexShader*		pVertexShaders[3];
	NiD3DPixelShader*		pPixelShaders[38];
	NiRefObject*			spObject238;
	NiRefObject*			spObject23C;
	int						dword240;
	NiRefObject*			spObject244;
	NiRefObject*			spObject248;
	int						dword24C;
};
static_assert(sizeof(WaterShader) == 0x250);


class ShadowLightShader : public BSShader{
public:
	NiDX9ShaderDeclaration* pShaderDeclarationGlowParallaxSkin;
	NiDX9ShaderDeclaration* pShaderDeclaration70;
	NiDX9ShaderDeclaration* pShaderDeclarationModelSpaceNormals;
	NiDX9ShaderDeclaration* pShaderDeclarationLandscape;
	NiD3DShaderConstantMap* spPixelConstantMap;
	NiD3DShaderConstantMap* spVertexConstantMap;
	NiD3DShaderConstantMap* spPixelConstantMap2;
	NiD3DShaderConstantMap* spVertexConstantMap2;
};
static_assert(sizeof(ShadowLightShader) == 0x8C);


class ParallaxShader : public ShadowLightShader{
public:
	NiD3DVertexShader*	pVertexShaders[20];
	NiD3DPixelShader*	pPixelShaders[33];
};
static_assert(sizeof(ParallaxShader) == 0x160);

class ImageSpaceEffect {
public:
	virtual void Destroy(bool doFree);
	virtual void RenderShader(NiGeometry* apScreenShape, NiDX9Renderer* pkRenderer, ImageSpaceEffectParam* apParam, bool abEndFrame);
	virtual void Setup(ImageSpaceManager* pISManager, ImageSpaceEffectParam* apParam);
	virtual void Shutdown(void);
	virtual void BorrowTextures(ImageSpaceEffectParam* apParam);
	virtual void ReturnTextures(void);
	virtual bool IsActive(void);
	virtual bool UpdateParams(ImageSpaceEffectParam* apParam);

	bool							bIsActive;
	bool							bParamsChanged;
	NiTArray<ImageSpaceEffect>		EffectList;
	NiTArray<int>					EffectParamList;
	NiTArray<int>					Textures;
	NiTArray<int>					EffectInputs;
	NiTArray<int>					EffectOutput;
};
static_assert(sizeof(ImageSpaceEffect) == 0x58);

class BSImageSpaceShader : public BSShader {
public:
	ImageSpaceEffect		Effect;
	NiD3DVertexShader*		pVertexShader;
	NiD3DPixelShader*		pPixelShader;
	ImageSpaceShaderParam*	pParam;
};
static_assert(sizeof(BSImageSpaceShader) == 0xD0);

class WaterShaderHeightMap : public BSImageSpaceShader {
public:
};
static_assert(sizeof(WaterShaderHeightMap) == 0xD0);

class Ni2DBuffer : public NiObject {
public:
	UInt32				width;	// 008
	UInt32				height;	// 00C
	NiDX92DBufferData*	data;	// 010
};
static_assert(sizeof(Ni2DBuffer) == 0x014);

class NiDepthStencilBuffer : public Ni2DBuffer {};
static_assert(sizeof(NiDepthStencilBuffer) == 0x014);

class NiDX92DBufferData : public NiRefObject {
public:
	virtual void		func02();												// 02
	virtual void		func03();												// 03
	virtual void		func04();												// 04
	virtual	void		func05();												// 05
	virtual	void		func06();												// 06
	virtual void		func07();												// 07
	virtual void		func08();												// 08
	virtual void		func09();												// 09
	virtual void		func0A();												// 0A
	virtual void		func0B();												// 0B
	virtual void		func0C();												// 0C
	virtual void		func0D();												// 0D
	virtual void		func0E();												// 0E
	virtual void		func0F();												// 0F
	virtual void		func10();												// 10
	virtual void		func11();												// 11
	virtual void		func12();												// 12
	virtual void		func13();												// 13
	virtual void		func14();												// 14
	virtual void		func15();												// 15
	virtual void		func16();												// 16
	virtual void		func17();												// 17
	virtual void		func18();												// 18
	virtual void		func19();												// 19
	virtual void		func1A();												// 1A

	Ni2DBuffer*			ParentData;												// 08
	UInt32				Unk0C;													// 0C
	UInt32				Unk10;													// 10
	IDirect3DSurface9*	Surface;												// 14
};

class NiRenderTargetGroup : public NiObject {
public:
	virtual UInt32					GetWidth(UInt32 Index);											// 23
	virtual UInt32					GetHeight(UInt32 Index);										// 24
	virtual UInt32					GetDepthStencilWidth();											// 25
	virtual UInt32					GetDepthStencilHeight();										// 26
	virtual const NiPixelFormat*	GetPixelFormat(UInt32 Index);									// 27
	virtual const NiPixelFormat*	GetDepthStencilPixelFormat();									// 28
	virtual UInt32					GetBufferCount();											// 29
	virtual bool					AttachBuffer(Ni2DBuffer* Buffer, UInt32 Index);					// 2A
	virtual bool					AttachDepthStencilBuffer(NiDepthStencilBuffer* DepthBuffer);	// 2B
	virtual Ni2DBuffer*				GetBuffer(UInt32 Index);										// 2C
	virtual NiDepthStencilBuffer*	GetDepthStencilBuffer();										// 2D
	virtual void*					GetRendererData();												// 2E
	virtual void					SetRendererData(void* RendererData);							// 2F
	virtual void*					GetRenderTargetData(UInt32 RenderTargetIndex);					// 30
	virtual void*					GetDepthStencilBufferRendererData();							// 31
	virtual void					func32();													// 32
	virtual void					func33();													// 33

	UInt32							Unk08;						// 08
	Ni2DBuffer*						RenderTargets[4];			// 0C
	UInt32							numRenderTargets;			// 1C
	NiDepthStencilBuffer*			DepthStencilBuffer;			// 20
	void*							RenderData;					// 24

	NiDX9TextureData* GetDX9RendererData() {
		return reinterpret_cast<NiDX9TextureData*>(RenderData);
	}

};
static_assert(sizeof(NiRenderTargetGroup) == 0x28);

class NiDX9RenderState : public NiRefObject {
public:
	virtual void							UpdateRenderState(const NiPropertyState* pkNew);// 02
	virtual void							SetAlpha(void* AlphaProperty);					// 03
	virtual void							SetDither(void* DitherProperty);				// 04
	virtual void							SetFog(NiFogProperty* FogProperty);				// 05
	virtual void							SetMaterial(void* u1);							// 06
	virtual void							SetShadeMode(void* ShadeProperty);				// 07
	virtual void							SetSpecular(void* SpecularProperty);			// 08
	virtual void							SetStencil(void* StencilProperty);				// 09
	virtual void							SetWireframe(void* WireframeProperty);			// 0A
	virtual void 							SetZBuffer(void* ZBufferProperty);				// 0B
	virtual void							RestoreAlpha();									// 0C
	virtual void							SetVertexBlending(UInt16 u1);					// 0D
	virtual void							SetNormalization(void* u1);						// 0E
	virtual void							func_0F(UInt32 u1);								// 0F
	virtual float							GetVar088();									// 10
	virtual void							func_11(float u1);								// 11
	virtual void							func_12(float u1, float u2);					// 12
	virtual BOOL							func_13();										// 13
	virtual void							func_14(BOOL u1);								// 14
	virtual void							func_15(UInt32 u1);								// 15
	virtual void							InitializeRenderStates();						// 16
	virtual void							BackUpAllStates();								// 17
	virtual void 							RestoreRenderState(D3DRENDERSTATETYPE state);	// 18
	virtual void							RestoreAllRenderStates();						// 19
	virtual void 							SetRenderState(D3DRENDERSTATETYPE state, UInt32 value, UInt8 Arg3, UInt8 Arg4);					// 1A
	virtual UInt32							GetRenderState(D3DRENDERSTATETYPE state);														// 1B
	virtual void							ClearPixelShaders();																			// 1C
	virtual void							BackUpPixelShader();																			// 1D
	virtual void							func_1E();																						// 1E
	virtual void							SetPixelShader(IDirect3DPixelShader9* PixelShader, UInt8 BackUp);								// 1F
	virtual IDirect3DPixelShader9*			GetPixelShader();																				// 20
	virtual	void							RestorePixelShader();																			// 21
	virtual void							RemovePixelShader(IDirect3DPixelShader9* PixelShader);											// 22
	virtual void							SetVertexShader(IDirect3DVertexShader9* pShader, UInt8 BackUp);									// 23
	virtual IDirect3DVertexShader9*			GetVertexShader();																				// 24
	virtual void							RestoreVertexShader();																			// 25
	virtual void							RemoveVertexShader(IDirect3DVertexShader9* pShader);											// 26
	virtual void							SetFVF(UInt32 FVF, UInt8 BackUp);																// 27
	virtual UInt32							GetFVF();																						// 28
	virtual void							RestoreFVF();																					// 29
	virtual void							ClearFVF(UInt32 FVF);																			// 2A
	virtual void							SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl, UInt8 BackUp);							// 2B
	virtual IDirect3DVertexDeclaration9*	GetVertexDeclaration();																			// 2C
	virtual void							RestoreVertexDeclaration();																		// 2D
	virtual void							ClearVertexDeclatation(IDirect3DVertexDeclaration9* pDecl);										// 2E
	virtual void							func_02F();																						// 2F
	virtual void 							SetTextureStageState(UInt32 Stage, D3DTEXTURESTAGESTATETYPE Type, UInt32 Value, UInt8 BackUp);	// 30
	virtual UInt32							GetTextureStageState(UInt32 Stage, D3DTEXTURESTAGESTATETYPE Type);								// 31
	virtual void							func_032();																						// 32
	virtual HRESULT							SetSamplerState(UInt32 Sampler, D3DSAMPLERSTATETYPE Type, UInt32 Value, UInt8 BackUp);			// 33
	virtual UInt32 							GetSamplerState(UInt32 Sampler, D3DSAMPLERSTATETYPE Type);										// 34
	virtual void							RestoreSamplerState(UInt32 Sampler, D3DSAMPLERSTATETYPE Type);									// 35
	virtual void							ClearTextureList();																				// 36
	virtual void							SetTexture(UInt32 Sampler, IDirect3DBaseTexture9* pTexture);									// 37
	virtual IDirect3DBaseTexture9*			GetTexture(UInt32 Sampler);																		// 38
	virtual void							RemoveTexture(IDirect3DBaseTexture9* pTexture);													// 39
	virtual void							SetSoftwareVertexProcessing(UInt8 bSoftware);													// 3A
	virtual UInt8							GetSoftwareVertexProcessing();																	// 3B
	virtual void							SetVar_0FF4(UInt8 u1);																			// 3C
	virtual UInt8							GetVar_0FF4();																					// 3D
	virtual void							SetVar_0FF5(UInt8 u1);																			// 3E
	virtual UInt8							GetVar_0FF5();																					// 3F
	virtual void							Reset();																						// 40
	virtual void							func_041();																						// 41
	
	struct NiRenderStateSetting {
		UInt32 CurrentValue;
		UInt32 PreviousValue;
	};

	UInt32							Flags;							// 0008
	UInt32							unk000C[(0x0074 - 0x000C) >> 2];// 000C
	NiAlphaProperty*				DisabledAlphaProperty;			// 0074
	float							CameraNear;						// 0078
	float							CameraFar;						// 007C
	float							CameraDepthRange;				// 0080
	float							MaxFogFactor;					// 0084
	float							MaxFogValue;					// 0088
	NiColor							CurrentFogColor;				// 008C
	UInt32                          Unk98[23];						// 0098
	UInt32							LeftHanded;						// 00F4
	UInt32							Unk0F8[10];						// 00F8
	NiRenderStateSetting			RenderStateSettings[256];		// 0120
	UInt32							Unk0920[64];					// 0920
	NiRenderStateSetting			TextureStageStateSettings[128];	// 0A20
	NiRenderStateSetting			SamplerStateSettings[80];		// 0E20
	UInt32							unk10A0[20];					// 10A0
	NiDX9ShaderConstantManager*		ShaderConstantManager;			// 10F0
	UInt8							ForceNormalizeNormals;			// 10F4
	UInt8							InternalNormalizeNormals;		// 10F5
	UInt8							UsingSoftwareVP;				// 10F6
	UInt8							Declaration;					// 10F7
	IDirect3DDevice9*				Device;							// 10F8
	NiDX9Renderer*					Renderer;						// 10FC
	UInt32							unk1000[(0x1018 - 0x1000) >> 2];// 1100
	D3DCAPS9						Caps;							// 1118
};
static_assert(offsetof(NiDX9RenderState, Device) == 0x10F8);
static_assert(sizeof(NiDX9RenderState) == 0x1248);
class PrePackObject;

class NiRenderer : public NiObject {
public:
	enum ClearFlags {
		kClear_BACKBUFFER = 0x1,
		kClear_STENCIL = 0x2,
		kClear_ZBUFFER = 0x4,
		kClear_NONE = 0,
		kClear_ALL = kClear_BACKBUFFER | kClear_STENCIL | kClear_ZBUFFER
	};

	virtual void			Unk_23();
	virtual void			Unk_24();
	virtual void			Unk_25();
	virtual void			Unk_26();
	virtual void			Unk_27();
	virtual void			Unk_28();
	virtual void			Unk_29();
	virtual void			Unk_2A();
	virtual void			Unk_2B();
	virtual void			Unk_2C();
	virtual void			Unk_2D();
	virtual void			Unk_2E();
	virtual void			Unk_2F();
	virtual void			Unk_30();
	virtual void			Unk_31();
	virtual NiRenderTargetGroup* GetDefaultRT();	// get back buffer rt
	virtual NiRenderTargetGroup* GetCurrentRT();	// get currentRTGroup
	virtual void			Unk_34();
	virtual void			Unk_35();
	virtual void			Unk_36();
	virtual void			Unk_37();
	virtual void 			Unk_38();
	virtual void 			Unk_39();
	virtual void			Unk_3A();
	virtual void			Unk_3B();
	virtual void			PurgeGeometry(NiGeometryData* geo);
	virtual void			PurgeMaterial(NiMaterialProperty* material);
	virtual void			PurgeEffect(NiDynamicEffect* effect);
	virtual void			PurgeScreenTexture();
	virtual void			PurgeSkinPartition(NiSkinPartition* skinPartition);
	virtual void			PurgeSkinInstance(NiSkinInstance* skinInstance);
	virtual void			Unk_42();
	virtual bool			Unk_43();
	virtual void			Unk_44();
	virtual bool			FastCopy(void* src, void* dst, RECT* srcRect, SInt32 xOffset, SInt32 yOffset);
	virtual bool			Copy(void* src, void* dst, RECT* srcRect, RECT* dstRect, UInt32 filterMode);
	virtual void			Unk_47();
	virtual bool			Unk_48(void* arg);
	virtual void			Unk_49();
	virtual void			Unk_4A(float arg);
	virtual void 			Unk_4B(UInt32 size);
	virtual void			Unk_4C(UInt32 arg0, UInt32 arg1);
	virtual void			Unk_4D(UInt32 arg0, UInt32 arg1);
	virtual void			Unk_4E(void* buf);
	virtual void			CreateSourceTexture(NiSourceTexture* texture);
	virtual bool			CreateRenderedTexture(NiRenderedTexture* arg);
	virtual bool			CreateSourceCubeMap(NiSourceCubeMap* arg);
	virtual bool			CreateRenderedCubeMap(NiRenderedCubeMap* arg);
	virtual bool			CreateDynamicTexture(void* arg);
	virtual void			Unk_54();
	virtual bool			CreateDepthStencil(NiDepthStencilBuffer* arg, void* textureFormat);
	virtual void			Unk_56();
	virtual void			Unk_57();
	virtual void			Unk_58();
	virtual void			Unk_59();
	virtual void			Unk_5A();
	virtual void			Unk_5B();
	virtual void			Unk_5C();
	virtual void			Unk_5D();
	virtual void			Unk_5E();
	virtual bool			BeginScene();
	virtual bool			EndScene();
	virtual void			DisplayScene();
	virtual void			Clear(float* rect, UInt32 flags);
	virtual void			SetupCamera(NiPoint3* pos, NiPoint3* at, NiPoint3* up, NiPoint3* right, NiFrustum* frustum, float* viewport);
	virtual void			SetupScreenSpaceCamera(float* viewport);
	virtual bool			BeginUsingRenderTargetGroup(NiRenderTargetGroup* renderTarget, ClearFlags clearFlags);
	virtual bool			EndUsingRenderTargetGroup();
	virtual void			BeginBatch(UInt32 arg0, UInt32 arg1);
	virtual void			EndBatch();
	virtual void			BatchRenderShape(void* arg);
	virtual void			BatchRenderStrips(void* arg);
	virtual void			RenderTriShape(NiTriShape* obj);
	virtual void			RenderTriStrips(NiTriStrips* obj);
	virtual void			RenderTriShape2(NiTriShape* obj);
	virtual void			RenderTriStrips2(NiTriStrips* obj);
	virtual void			RenderParticles(NiParticles* obj);
	virtual void			RenderLines(NiLines* obj);
	virtual void			RenderScreenTexture();

	UInt32					Unk008[126];				// 008
	UInt32					SceneState;					// 200
	UInt32					m_uiFrameID;
	bool					m_bRenderTargetGroupActive;
	bool					m_bBatchRendering;
	int						unk20C[29];
};
static_assert(sizeof(NiRenderer) == 0x280);

class NiDX9Renderer : public NiRenderer {
public:
	enum FrameBufferFormat {
		FBFMT_UNKNOWN = 0,
		FBFMT_R8G8B8,
		FBFMT_A8R8G8B8,
		FBFMT_X8R8G8B8,
		FBFMT_R5G6B5,
		FBFMT_X1R5G5B5,
		FBFMT_A1R5G5B5,
		FBFMT_A4R4G4B4,
		FBFMT_R3G3B2,
		FBFMT_A8,
		FBFMT_A8R3G3B2,
		FBFMT_X4R4G4B4,
		FBFMT_R16F,
		FBFMT_G16R16F,
		FBFMT_A16B16G16R16F,
		FBFMT_R32F,
		FBFMT_G32R32F,
		FBFMT_A32B32G32R32F,
		FBFMT_NUM
	};

	enum DepthStencilFormat {
		DSFMT_UNKNOWN = 0,
		DSFMT_D16_LOCKABLE = 70,
		DSFMT_D32 = 71,
		DSFMT_D15S1 = 73,
		DSFMT_D24S8 = 75,
		DSFMT_D16 = 80,
		DSFMT_D24X8 = 77,
		DSFMT_D24X4S4 = 79,
	};

	enum PresentationInterval {
		PRESENT_INTERVAL_IMMEDIATE = 0,
		PRESENT_INTERVAL_ONE = 1,
		PRESENT_INTERVAL_TWO = 2,
		PRESENT_INTERVAL_THREE = 3,
		PRESENT_INTERVAL_FOUR = 4,
		PRESENT_INTERVAL_NUM
	};

	enum SwapEffect {
		SWAPEFFECT_DEFAULT,
		SWAPEFFECT_DISCARD,
		SWAPEFFECT_FLIP,
		SWAPEFFECT_COPY,
		SWAPEFFECT_NUM
	};

	enum FrameBufferMode {
		FBMODE_DEFAULT,
		FBMODE_LOCKABLE,
		FBMODE_MULTISAMPLES_2 = 0x00010000,
		FBMODE_MULTISAMPLES_3 = 0x00020000,
		FBMODE_MULTISAMPLES_4 = 0x00030000,
		FBMODE_MULTISAMPLES_5 = 0x00040000,
		FBMODE_MULTISAMPLES_6 = 0x00050000,
		FBMODE_MULTISAMPLES_7 = 0x00060000,
		FBMODE_MULTISAMPLES_8 = 0x00070000,
		FBMODE_MULTISAMPLES_9 = 0x00080000,
		FBMODE_MULTISAMPLES_10 = 0x00090000,
		FBMODE_MULTISAMPLES_11 = 0x000a0000,
		FBMODE_MULTISAMPLES_12 = 0x000b0000,
		FBMODE_MULTISAMPLES_13 = 0x000c0000,
		FBMODE_MULTISAMPLES_14 = 0x000d0000,
		FBMODE_MULTISAMPLES_15 = 0x000e0000,
		FBMODE_MULTISAMPLES_16 = 0x000f0000,
		FBMODE_MULTISAMPLES_NONMASKABLE = 0x80000000,
		FBMODE_QUALITY_MASK = 0x0000FFFF,
		FBMODE_NUM = 18
	};

	enum RefreshRate {
		REFRESHRATE_DEFAULT = 0
	};

	void							SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE State, DWORD Value);
	void							PackGeometryBuffer(NiGeometryBufferData* GeoData, NiGeometryData* ModelData, NiSkinInstance* SkinInstance, NiD3DShaderDeclaration* ShaderDeclaration);
	void							PackSkinnedGeometryBuffer(NiGeometryBufferData* GeoData, NiGeometryData* ModelData, NiSkinInstance* SkinInstance, NiSkinPartition::Partition* Partition, NiD3DShaderDeclaration* ShaderDeclaration);
	void							CalculateBoneMatrixes(NiSkinInstance* SkinInstance, NiTransform* WorldTrasform);

	LPDIRECT3D9						ms_pkD3D9;
	UInt32							unk284;
	IDirect3DDevice9*				device;							// 288
	D3DCAPS9						caps;							// 28C
	HANDLE							deviceWindow;					// 3BC
	HANDLE							focusWindow;					// 3C0
	char							rendererInfo[0x200];			// 3C4
	UInt32							adapterIdx;						// 5C4
	UInt32							d3dDevType;						// 5C8 - D3DDEVTYPE
	UInt32							m_uiBehaviorFlags;
	D3DFORMAT						m_eAdapterFormat;
	bool							m_bSWVertexCapable;
	bool							m_bSWVertexSwitchable;
	const NiDX9AdapterDesc*			m_pkAdapterDesc;
	const NiDX9DeviceDesc*			m_pkDeviceDesc;
	UInt32							m_uiBackground;
	float							m_fZClear;
	UInt32							m_uiStencilClear;
	UInt32							m_uiRendFlags;
	char							m_acBehavior[32];
	NiTPtrMap<PrePackObject*>		PrePackObjects;					// 610 - NiTPointerMap <NiVBBlock *, NiDX9Renderer::PrePackObject *>
	UInt32							Unk620[153];					// 620
	NiRenderTargetGroup*			defaultRTGroup;					// 884 - back buffer
	NiRenderTargetGroup*			currentRTGroup;					// 888
	NiRenderTargetGroup*			currentscreenRTGroup;			// 88C
	NiTPtrMap<NiRenderTargetGroup*>	screenRTGroups;					// 890 - NiTPointerMap <HWND *, NiPointer <NiRenderTargetGroup> >
	UInt32							Unk8A0[6];						// 8A0
	NiDX9RenderState*				renderState;					// 8B8
	UInt32							unk8BC[3];					// 8BC
	NiTMap<NiLight*, void*>*		lightsMap;					// 8C8
	UInt32							UnkA81[28];					// A80
	D3DXMATRIXA16					worldMatrix;				// 940
	D3DXMATRIX						viewMatrix;					// 980
	D3DXMATRIX						projMatrix;					// 9C0
	D3DXMATRIX						UnkMatrix;					// A00
	D3DXMATRIX						invViewMatrix;				// A40
	UInt32							UnkA80[6];					// A80
	UInt32							width;						// A98
	UInt32							height;						// A9C
	UInt32							flags;						// AA0
	UInt32							windowDevice;				// AA4
	UInt32							windowFocus;				// AA8
	UInt32							adapterType;				// AAC
	UInt32							deviceType;					// AB0
	FrameBufferFormat				frameBufferFormat;			// AB4
	DepthStencilFormat				depthStencilFormat;			// AB8
	PresentationInterval			presentationInterval;		// ABC
	SwapEffect						swapEffect;					// AC0
	FrameBufferMode					frameBufferMode;			// AC4
	UInt32							backBufferCount;			// AC8
	UInt32								unkAD0[17];					// AD0
	NiTMap<D3DFORMAT, NiPixelFormat*>	formatMatchMao;				// B14
	UInt32								unkB24[23];					// B24

	static NiDX9Renderer* GetSingleton() { return *(NiDX9Renderer**)0x11C73B4; };
};
static_assert(sizeof(NiDX9Renderer) == 0xB80);

class NiControllerSequence : public NiObject {
public:
	virtual void	Unk_23();

	struct ControlledBlock {
		void*	unk00;		// 00 NiInterpolator*
		void*	unk04;		// 04 NiMultiTargetTransformController*
		UInt32	unk08;		// 08
		UInt8	unk0C;		// 0C
		UInt8	pad0C[3];
	};

	const char*				sequenceName;			// 08
	UInt32					numControlledBlocks;	// 0C
	UInt32					arrayGrowBy;			// 10
	ControlledBlock**		controlledBlocks;		// 14
	const char**			unkNodeName;			// 18
	float					weight;					// 1C
	NiTextKeyExtraData*		textKeyData;			// 20
	UInt32					cycleType;				// 24
	float					frequency;				// 28
	float					startTime;				// 2C
	float					stopTime;				// 30
	float					flt34;					// 34
	float					flt38;					// 38
	float					flt3C;					// 3C
	NiControllerManager*	manager;				// 40
	UInt32					unk44;					// 44
	UInt32					unk48;					// 48
	float					flt4C;					// 4C
	float					flt50;					// 50
	float					flt54;					// 54
	UInt32					unk58;					// 58
	const char*				rootNodeName;			// 5C
	UInt32					unk60[5];				// 60
};
static_assert(sizeof(NiControllerSequence) == 0x74);

class BSRenderedTexture : public NiObject {
public:
	NiRenderTargetGroup*	RenderTargetGroups[6];	// 008
	UInt32					unk020;				// 020
	UInt32					unk024;				// 024
	UInt32					unk028;				// 028
	UInt32					eType;				// 02C // BSTextureManager::RenderTargetTypes
	NiRenderedTexture*		RenderedTextures[4];// 030

	IDirect3DTexture9* GetD3DTexture(const UINT32 auIndex) {
		return (IDirect3DTexture9*)GetTexture(auIndex)->GetDX9RendererData()->dTexture;
	}

	NiRenderedTexture* GetTexture(const UInt32 uiIndex) {
		return RenderedTextures[uiIndex];
	}
};
static_assert(sizeof(BSRenderedTexture) == 0x40);

class NiAlphaProperty : public NiProperty {
public:
	enum AlphaFlags {
        ALPHA_BLEND_MASK    = 0x0001,
        SRC_BLEND_MASK      = 0x001e,
        SRC_BLEND_POS       = 1,
        DEST_BLEND_MASK     = 0x01e0,
        DEST_BLEND_POS      = 5,
        TEST_ENABLE_MASK    = 0x0200,
        TEST_FUNC_MASK      = 0x1c00,
        TEST_FUNC_POS       = 10,
        ALPHA_NOSORTER_MASK = 0x2000
    };

	UInt16	flags;			// 018
	UInt8	alphaTestRef;	// 01A
	UInt8	unk01B;			// 01B
};
static_assert(sizeof(NiAlphaProperty) == 0x01C);

// 24
class NiStencilProperty : public NiProperty
{
public:
	UInt16				flags;		// 18
	UInt16				word1A;		// 1A
	UInt32				unk1C;		// 1C
	UInt32				mask;		// 20

	__forceinline static NiStencilProperty* Create() { return CdeclCall<NiStencilProperty*>(0xA6F410); }
};
static_assert(sizeof(NiStencilProperty) == 0x24);

// 1C
class NiCullingProperty : public NiProperty
{
public:
	UInt32				unk18;		// 18
};

class NiMaterialProperty : public NiProperty {
public:
	SInt32		iIndex;
	NiColor		spec;
	NiColor		emit;
	NiColor*	pExternalEmittance;
	float		fShine;
	float		fAlpha;
	float		fEmitMult;
	UInt32		uiRevID;
	void*		pvRendererData;
	__forceinline static NiMaterialProperty* Create() { return CdeclCall<NiMaterialProperty*>(0xA756D0); }
	void __vectorcall SetTraitValue(UInt32 traitID, float value);
};
static_assert(sizeof(NiMaterialProperty) == 0x04C);


class BSFadeNode : public NiNode {
public:
	float			nearDistSqr;	// AC
	float			farDistSqr;		// B0
	float			lastFade;		// B4
	float			FadeAlpha;	// B8	[0.0, 1.0]; Used for fade-in/out
	float			BoundRadius;	// BC
	float			timeSinceUpdate;// C0
	UInt32			MultType;		// C4
	UInt32			frameCounter;	// C8
	TESObjectREFR	*linkedObj;		// CC
	UInt32			unkD0[5];		// D0

	__forceinline static BSFadeNode* Create() { return ThisStdCall<BSFadeNode*>(0xB4E150, CdeclCall<void*>(0xAA13E0, sizeof(BSFadeNode))); }

	void __fastcall SetVisible(bool visible);
};
static_assert(sizeof(BSFadeNode) == 0xE4);

class BSTreeModel : public NiRefObject {
public:
	UInt32					Unk08;				// 08
	UInt32					Unk0C;				// 0C
	UInt32					Unk10;				// 10
	UInt32					Unk14;				// 14
	UInt32					Unk18;				// 18
	UInt32					Unk1C;				// 1C
	UInt32					Unk20;				// 20
	UInt32					Unk24;				// 24
	UInt32					Unk28;				// 28
	void*					Unk2C;				// 2C BSShaderTextureSet*
	NiSourceTexture*		LeavesTexture;		// 30
	void*					Unk34;				// 34 BSShaderTextureSet*
	UInt32					Unk38;				// 38
	float					Unk3C;				// 3C
	UInt32					Unk40;				// 40
	UInt8					Unk44;				// 44
	UInt8					pad44[3];
	float					Unk48;				// 48
	float					Unk4C;				// 4C
};
static_assert(sizeof(BSTreeModel) == 0x50);

class BSTreeNode : public BSFadeNode {
public:
	BSTreeModel*		TreeModel;		// E4
	UInt32				UnkE0;			// E8
	UInt32				UnkE4;			// EC
	UInt32				UnkE8;			// F0
	float				UnkEC;			// F4
};
static_assert(sizeof(BSTreeNode) == 0xF8);


// 10
struct BSRenderPass
{
	void* geometry;		// 00
	UInt16			passEnum;		// 04
	UInt8			accumHint;		// 06
	UInt8			bEnabled;		// 07
	UInt8			noFog;			// 08
	UInt8			numLights;		// 09
	UInt8			maxNumLights;	// 0A
	UInt8			currLandTexture;// 0B
	LightingData** sceneLights;	// 0C
};

enum LightingPassFlags
{
	kPass_Ambient = 1,
	kPass_Diffuse = 2,
	kPass_Texture = 4,
	kPass_Specular = 8,
	kPass_Shadow = 0x10,
	kPass_CanopyShadow = 0x20,
	kPass_Opt = 0x100
};
// 14	vtbl 0x10B8480
class BSRenderPassList
{
public:
	virtual void	Destroy(bool doFree);

	BSRenderPass** passes;	// 04
	UInt16			maxSize;	// 08
	UInt16			word0A;		// 0A
	UInt16			arraySize;	// 0C
	UInt16			growBy;		// 0E
	UInt32			passCount;	// 10
};


// 78
class BSPortalGraph : public NiRefObject
{
public:
	UInt32					unk08[10];	// 08
	void* ptr30;		// 30
	void* ptr34;		// 34
	UInt32					unk38;		// 38
	NiTArray<NiAVObject*>	array3C;	// 3C
	NiNode* node4C;	// 4C
	UInt32					unk50[6];	// 50
	BSSimpleArray<NiNode*>	array68;	// 68
};
static_assert(sizeof(BSPortalGraph) == 0x78);

// 250
class LightingData : public NiRefObject
{
public:
	UInt32								unk008;				// 008
	float								luminance;			// 00C
	float								matrix4x4[4][4];	// 010
	float								flt050[32];			// 050
	float								lodDimmer;			// 0D0
	float								alpha;				// 0D4
	float								shadowFadeTime0D8;	// 0D8
	float								shadowFadeTime0DC;	// 0DC
	DList<NiGeometry>					geometryList;		// 0E0
	bool								isShadowCasting;	// 0EC
	UInt8								byte0ED;			// 0ED
	UInt8								pad0EE[2];			// 0EE
	NiRefObject* obj0F0;			// 0F0
	bool								isPointLight;		// 0F4
	bool								isAmbientLight;		// 0F5
	UInt8								pad0F6[2];			// 0F6
	NiLight* light;				// 0F8
	bool								isDynamic;			// 0FC
	UInt8								pad0FD[3];			// 0FD
	NiVector3							centrePos;			// 100
	BSRenderedTexture* shadowRenderTarget;// 10C
	UInt16								frustumCull;		// 110	0xFF = disabled (distance too far), anything else = enabled
	UInt16								word112;			// 112
	NiRefObject* obj114;			// 114
	UInt8								byte118;			// 118
	UInt8								pad119[3];			// 119
	float								flt11C;				// 11C
	float								flt120;				// 120
	bool								showDebugTexture;	// 124
	UInt8								pad125[3];			// 125
	NiNode* shadowCaster;		// 128
	DList<void>							list12C;			// 12C
	DList<NiGeometry>::DNode* fenceGeomList;		// 138
	NiTriShape* fenceObject;		// 13C
	NiCamera* shadowMapCamera;	// 140
	NiFrustumPlanes						frustumPlanes;		// 144
	float								clipPlanes[24];		// 1A8
	UInt8								byte208;			// 208
	UInt8								pad209[3];			// 209
	BSShaderAccumulator* shaderAccum;		// 20C
	BSSimpleArray<BSMultiBoundNode*>	multiboundRooms;	// 210
	BSSimpleArray<BSPortal*>			array220;			// 220
	BSSimpleArray<NiNode*>				processedNodes;		// 230
	BSPortalGraph* portalGraph;		// 240
	UInt32								unk244[3];			// 244
};
static_assert(sizeof(LightingData) == 0x250); // 7C

class ShadowSceneLight : public NiRefObject {
public:

	UInt32					unk008;
	float					fLuminance;
	D3DXMATRIX				kViewProjMatrix;
	D3DXMATRIX				kViewMatrix;
	D3DXMATRIX				kProjMatrix;
	float					fLODDimmer;
	float					fFade;
	float					fShadowFadeTime0D8;
	float					fShadowFadeTime0DC;
	NiTList<NiGeometry>		kGeometryList;
	bool					bIsShadowCasting;
	UInt8					byte0ED;
	NiAVObject*				spObject0F0;
	bool					bPointLight;
	bool					bAmbientLight;
	NiPointLight*			sourceLight;
	bool					bDynamicLight;
	NiVector3				kPointPosition;
	UInt32					spShadowRenderTarget;
	UInt16					bIsEnabled;
	NiAVObject*				spObject114;
	bool					bUnk118;
	float					fUnk11C;
	float					fUnk120;
	bool					bShowDebugTexture;
	NiNode*					spShadowCasterNode;
	UInt32					kList12C[3];
	void*					pGeomListFence;
	void*					spFenceObject;
	NiCamera*				spShadowMapCamera;
	NiFrustumPlanes			kFrustumPlanes;
	float					fClipPlanes[24];
	bool					bUnk208;
	BSShaderAccumulator*	spShadowAccum;
	UInt32					kMultiBoundRooms[4];	// BSSimpleArray<BSMultiBoundRoom*>
	UInt32					kPortals[4];			// BSSimpleArray<BSPortal*>
	UInt32					kProcessedNodes[4];		// BSSimpleArray<NiNode>
	BSPortalGraph*			pPortalGraph;
	UInt32					unk244;
	UInt32					unk248;
	UInt32					unk24C;
};
static_assert(sizeof(ShadowSceneLight) == 0x250);

class ShadowSceneNode : public NiNode {
public:
	UInt32								unk0AC[2];			// 0AC
	NiTList<ShadowSceneLight>			lights;				// 0B4
	NiTList<ShadowSceneLight>			actorShadowCasters;	// 0C0
	NiTList<ShadowSceneLight>::Entry*	unk0CC;				// 0CC
	NiTList<ShadowSceneLight>::Entry*	unk0D0;				// 0D0
	NiTList<ShadowSceneLight>::Entry*	unk0D4;				// 0D4
	ShadowSceneLight*					sceneLight1;		// 0D8
	ShadowSceneLight*					sceneLight2;		// 0DC
	ShadowSceneLight*					directionalLight;	// 0E0
	UInt32								unk0E4[6];			// 0E4
	void*								ptr0FC;				// 0FC
	void*								ptr100;				// 100
	UInt32								unk104;				// 104
	UInt32								unk108[3];			// 108
	void*								ptr114;				// 114
	void*								ptr118;				// 118
	UInt32								unk11C;				// 11C
	UInt32								unk120;				// 120
	UInt32								unk124;				// 124
	NiCamera*							cubeMapCam;			// 128 BSCubeMapCamera*
	UInt32								unk12C;				// 12C
	UInt8								byte130;			// 130
	UInt8								byte131;			// 131
	UInt8								pad132[2];			// 132
	void*								fogProperty;		// 134 BSFogProperty*
	UInt32								unk138;				// 138
	UInt32								array13C[4];		// 13C BSSimpleArray<NiFrustumPlanes>
	UInt32								array14C[4];		// 14C BSSimpleArray<void>
	UInt32								unk15C[3];			// 15C
	NiVector4							unk168;				// 168
	NiVector4							unk178;				// 178
	NiVector4							unk188;				// 188
	NiVector4							unk198;				// 198
	NiVector4							unk1A8;				// 1A8
	NiVector4							unk1B8;				// 1B8
	UInt32								lightingPasses;		// 1C8
	float								flt1CC[3];			// 1CC
	UInt32								unk1D8;				// 1D8
	UInt8								byte1DC;			// 1DC
	UInt8								pad1DD[3];			// 1DD
	void*								portalGraph;		// 1E0 BSPortalGraph*
	UInt32								unk1E4[3];			// 1E4
	float								flt1F0[3];			// 1F0
	UInt8								byte1FC;			// 1FC
	UInt8								pad1FD[3];			// 1FD
};
static_assert(sizeof(ShadowSceneNode) == 0x200);