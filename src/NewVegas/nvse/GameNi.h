#pragma once

#define MAP_DEFAULT_ALLOC			8UL
#define MAP_MIN_BUCKET_COUNT		4UL
#define MAP_MAX_BUCKET_COUNT		0x8000UL
#define MAP_DEFAULT_BUCKET_COUNT	8UL
#define VECTOR_DEFAULT_ALLOC		8UL

#include "base_defs.h"
#include "utility.h"
#include "class_vtbls.h"
#include "GameTypes.h"
#include "DataTypes.h"
#include "DataObject.h"


#if RUNTIME

const UInt32 _NiTMap_Lookup = 0x00853130;

#endif


class NiObject;
class NiAVObject;
class BSFadeNode;
class NiExtraData;
class NiTimeController;
class NiControllerManager;
class NiStringPalette;
class NiTextKeyExtraData;
class NiCamera;
class NiProperty;
class NiStream;
class TESAnimGroup;
class NiGeometry;
class ParticleShaderProperty;
class TESObjectCELL;
class TESObjectREFR;
class TESEffectShader;
class ActiveEffect;



// 8
struct NiRTTI
{
	const char* name;
	NiRTTI* parent;
};
struct NiMemObject
{
};

// 008
class NiRefObject : public NiMemObject
{
public:
	NiRefObject();
	~NiRefObject();

	virtual void		Destructor(bool freeThis);	// 00
	virtual void		Free(void);					// 01

	//	void		** _vtbl;		// 000
	UInt32		m_uiRefCount;	// 004 - name known
};


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
		ThisStdCall(0x4AD0C0, this, bucketCount);
		scale = { 1.0F, 1.0F, 1.0F };
	}
	//~NiObjectCopyInfo() {ThisStdCall(0x4AD1D0, this);}
};

// 008
class NiObject : public NiRefObject
{
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
};



enum InterpKeyType
{
	kKeyType_Linear = 1,
	kKeyType_Quadratic,
	kKeyType_TBC,
	kKeyType_XYZ,
	kKeyType_Const
};

// 2C
class NiTransformData : public NiObject
{
public:
	UInt16					numRotationKeys;	// 08
	UInt16					numTranslationKeys;	// 0A
	UInt16					numScaleKeys;		// 0C
	UInt16					pad0E;				// 0E
	UInt32					rotationKeyType;	// 10
	UInt32					translationKeyType;	// 14
	UInt32					scaleKeyType;		// 18
	UInt8					rotationKeySize;	// 1C
	UInt8					translationKeySize;	// 1D
	UInt8					scaleKeySize;		// 1E
	UInt8					pad1F;				// 1F
	InterpKey<NiQuaternion>* rotationKeys;		// 20
	InterpKey<NiVector3>* translationKeys;	// 24
	InterpKey<float>* scaleKeys;			// 28
};
static_assert(sizeof(NiTransformData) == 0x2C);

// 18
class NiFloatData : public NiObject
{
public:
	UInt32				numKeys;	// 08
	InterpKey<float>* data;		// 0C
	UInt32				keyType;	// 10
	UInt8				byte14;		// 14
	UInt8				pad15[3];	// 15
};

// 18
class NiColorData : public NiObject
{
public:
	UInt32					numKeys;	// 08
	InterpKey<NiColorAlpha>* data;		// 0C
	UInt32					keyType;	// 10
	UInt32					unk14;		// 14
};

// 18
class NiMorphData : public NiObject
{
public:
	class MorphTarget : public NiMemObject
	{
	public:
		NiPoint3* targetVerts;
		NiFixedString	name;
		float			legacyWeight;
		NiInterpolator* legacyInterp;
	};

	UInt32			numTargets;			// 08
	UInt32			numVertsPerTarget;	// 0C
	MorphTarget* morphTargets;		// 10
	bool			relativeTargets;	// 14
	UInt8			pad15[3];			// 15
};
// 1C
class NiDefaultAVObjectPalette : public NiObject
{
public:
	/*8C*/virtual NiAVObject* GetObjectByName(NiFixedString* objName);
	/*90*/virtual void		Unk_24(void);
	/*94*/virtual void		SetTarget(NiNode* _target);
	/*98*/virtual NiNode* GetTarget();

	NiTFixedStringMap<NiAVObject*>	objectsMap;		// 08
	NiNode* pTarget;		// 18
};

// 0C
class NiInterpolator : public NiObject
{
public:
	/*8C*/virtual bool		Update(float time, NiAVObject* target, const NiQuatTransform& transform);
	/*90*/virtual void		Unk_24(void);
	/*94*/virtual void		Unk_25(void);
	/*98*/virtual void		Unk_26(void);
	/*9C*/virtual void		Unk_27(void);
	/*A0*/virtual void		Unk_28(void);
	/*A4*/virtual void		Unk_29(void);
	/*A8*/virtual void		Unk_2A(void);
	/*AC*/virtual void		Unk_2B(void);
	/*B0*/virtual void		Unk_2C(void);
	/*B4*/virtual void		Unk_2D(void);
	/*B8*/virtual void		Unk_2E(void);
	/*BC*/virtual void		Unk_2F(void);
	/*C0*/virtual void		Unk_30(void);
	/*C4*/virtual void		Unk_31(float arg1, float arg2);
	/*C8*/virtual void		Unk_32(void);
	/*CC*/virtual void		Unk_33(void);
	/*D0*/virtual void		Unk_34(void);
	/*D4*/virtual void		Unk_35(void);
	/*D8*/virtual void		Unk_36(void);

	float		flt08;		// 08
};

// 0C
class NiKeyBasedInterpolator : public NiInterpolator
{
public:
	/*DC*/virtual void		Unk_37(void);
	/*E0*/virtual void		Unk_38(void);
	/*E4*/virtual void		Unk_39(void);
	/*E8*/virtual void		Unk_3A(void);
	/*EC*/virtual void		Unk_3B(void);
	/*F0*/virtual void		Unk_3C(void);
	/*F4*/virtual void		Unk_3D(void);
};

// 18
class NiFloatInterpolator : public NiKeyBasedInterpolator
{
public:
	float				value;		// 0C
	NiFloatData* data;		// 10
	UInt32				unk14;		// 14
};

// 24
class NiColorInterpolator : public NiKeyBasedInterpolator
{
public:
	float				value[4];	// 0C
	NiColorData* data;		// 1C
	UInt32				unk20;		// 20
};

// 48
class NiTransformInterpolator : public NiKeyBasedInterpolator
{
public:
	NiQuatTransform		transformValue;	// 0C
	NiTransformData* transData;		// 2C
	UInt16				lastTransIdx;	// 30
	UInt16				lastRotIdx;		// 32
	UInt16				lastScaleIdx;	// 34
	UInt8				pad36[2];		// 36
	float				flt38[3];		// 38
	UInt8				byte44;			// 44
	UInt8				pad45[3];		// 45

	__forceinline static NiTransformInterpolator* Create() { return CdeclCall<NiTransformInterpolator*>(0xA403F0); }
};
static_assert(sizeof(NiTransformInterpolator) == 0x48);

// 30
class NiBlendInterpolator : public NiInterpolator
{
public:
	/*DC*/virtual void		Unk_37(void);
	/*E0*/virtual void		Unk_38(void);
	/*E4*/virtual void		Unk_39(void);
	/*E8*/virtual void		Unk_3A(void);

	// 18
	struct InterpArrayItem
	{
		NiTransformInterpolator* interpolator;
		float					weight;
		float					normalizedWeight;
		UInt8					priority;
		UInt8					pad0D[3];
		float					easeSpinner;
		float					updateTime;
	};

	UInt8			flags;				// 0C
	UInt8			arraySize;			// 0D
	UInt8			interpCount;		// 0E
	UInt8			singleIdx;			// 0F
	UInt8			highPriority;		// 10
	UInt8			nextHighPriority;	// 11
	UInt8			pad12[2];			// 12
	InterpArrayItem* interpArray;		// 14
	NiInterpolator* singleInterp;		// 18
	float			weightThreshold;	// 1C
	float			singleTime;			// 20
	float			highSumOfWeights;	// 24
	float			nextHiSumOfWeights;	// 28
	float			highEaseSpinner;	// 2C
};
static_assert(sizeof(NiBlendInterpolator) == 0x30);

class NiBlendTransformInterpolator : public NiBlendInterpolator
{
public:
};


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

// 10
class BSMultiBound : public NiObject
{
public:
	/*8C*/virtual void	Unk_23(void);
	/*90*/virtual void	Unk_24(void);

	UInt32				unk08;		// 08
	BSMultiBoundShape* mbShape;	// 0C
};

// 018 (used to be 100, delta E8) confirmed, confirmed no virtual funcs
class NiObjectNET : public NiObject
{
public:
	NiObjectNET();
	~NiObjectNET();

	const char* GetName() const { return m_blockName.Get(); }
	void __fastcall SetName(const char* newName);
	NiExtraData* __fastcall GetExtraData(UInt32 vtbl) const;
	__forceinline bool AddExtraData(NiExtraData* xData)
	{
		return ThisStdCall<bool>(0xA5BA40, this, xData);
	}
	void DumpExtraData();

	NiFixedString		m_blockName;				// 08
	NiTimeController* m_controller;				// 0C
	NiExtraData** m_extraDataList;			// 10
	UInt16				m_extraDataListLen;			// 14
	UInt16				m_extraDataListCapacity;	// 16
};
static_assert(sizeof(NiObjectNET) == 0x18);

// 18
class NiProperty : public NiObjectNET
{
public:
	/*8C*/virtual UInt32	GetPropertyType();
	/*90*/virtual void		UpdateController(const NiUpdateData& updParams);

	enum PropertyType
	{
		kPropertyType_Alpha = 0,
		kPropertyType_Culling = 1,
		kPropertyType_Material = 2,
		kPropertyType_Shader = 3,
		kPropertyType_Stencil = 4,
		kPropertyType_Texturing = 5,
		kPropertyType_Dither = 8,
		kPropertyType_Specular = 9,
		kPropertyType_VertexColor = 10,
		kPropertyType_ZBuffer = 11,
		kPropertyType_Fog = 13,
	};
};




// 0C
struct NiComponentSpec
{
	int			m_eComponent;
	int			m_eRepresentation;
	UInt8		m_ucBitsPerComponent;
	bool		m_bSigned;
	UInt8		pad0A[2];
};
// 44
struct NiPixelFormat
{
	UInt8			m_ucBitsPerPixel;
	bool			m_bSRGBSpace;
	UInt8			pad02[2];
	int				m_eFormat;
	int				m_eTiling;
	UInt32			m_uiRendererHint;
	UInt32			m_uiExtraData;
	NiComponentSpec	m_akComponents[4];
};
static_assert(sizeof(NiPixelFormat) == 0x44);

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

// 158
class BSFile : public NiFile
{
public:
	virtual bool	Reset(bool arg1, bool arg2);	// 20
	virtual bool	Unk_09(UInt32 arg1);	// 24
	virtual UInt32	Unk_0A();	// 28
	virtual UInt32	Unk_0B(String* string, UInt32 arg2);	// 2C
	virtual UInt32	Unk_0C(void* ptr, UInt32 arg2);	// 30
	virtual UInt32	ReadBufDelim(void* bufferPtr, UInt32 bufferSize, short delim);		// 34
	virtual UInt32	Unk_0E(void* ptr, UInt8 arg2);	// 38
	virtual UInt32	Unk_0F(void* ptr, UInt8 arg2);	// 3C
	virtual bool	IsReadable();	// 40
	virtual UInt32	ReadBuf(void* bufferPtr, UInt32 numBytes);	// 44
	virtual UInt32	WriteBuf(void* bufferPtr, UInt32 numBytes);	// 48

	UInt32		m_modeReadWriteAppend;	// 028
	UInt8		m_good;					// 02C
	UInt8		pad02D[3];				// 02D
	UInt8		m_unk030;				// 030
	UInt8		pad031[3];				// 031
	UInt32		m_unk034;				// 034
	UInt32		m_unk038;				// 038 - init'd to FFFFFFFF
	UInt32		m_unk03C;				// 038
	UInt32		m_unk040;				// 038
	char		m_path[0x104];			// 044
	UInt32		m_unk148;				// 148
	UInt32		m_unk14C;				// 14C
	UInt32		m_fileSize;				// 150
	UInt32		m_unk154;				// 154
};

enum TexturePixelLayout : UInt32
{
	kPixelLayout_Palettized8,
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
	kPixelLayout_SingleColor4
};

enum TextureAlphaFormat : UInt32
{
	kAlphaFormat_None,
	kAlphaFormat_Binary,
	kAlphaFormat_Smooth,
	kAlphaFormat_Default
};

enum TextureMipMapFlag : UInt32
{
	kMipMapFlag_Disabled,
	kMipMapFlag_Enabled,
	kMipMapFlag_Default
};

struct TextureFormatPrefs
{
	TexturePixelLayout	pixelLayout;
	TextureAlphaFormat	alpha;
	TextureMipMapFlag	mipMaps;
};

// 70
class NiDX9TextureData : public NiObject
{
public:
	/*8C*/virtual void		Unk_23(void);
	/*90*/virtual void		Unk_24(void);
	/*94*/virtual void		Unk_25(void);
	/*98*/virtual void		Unk_26(void);
	/*9C*/virtual IDirect3DBaseTexture9* GetD3DBaseTexture();
	/*A0*/virtual void		SetD3DBaseTexture(IDirect3DBaseTexture9* baseTexture);
	/*A4*/virtual void		Unk_29(void);
	/*A8*/virtual void		Unk_2A(void);
	/*AC*/virtual void		Unk_2B(void);
	/*B0*/virtual void		Unk_2C(void);

	NiTexture* parent;			// 08
	UInt32					width;			// 0C
	UInt32					height;			// 10
	NiPixelFormat			PixelFormat;	// 14
	UInt8					Unk58;			// 58
	UInt8					Unk59;			// 59
	UInt8					Unk5A;			// 5A
	UInt8					Unk5B;			// 5B
	UInt32					Unk5C;			// 5C
	NiDX9Renderer* pRenderer;		// 60
	IDirect3DBaseTexture9* dTexture;		// 64
	UInt32					Levels;			// 68
};
assert(sizeof(NiDX9TextureData) == 0x6C);

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

// 030
class NiTexture : public NiObjectNET
{
public:
	/*8C*/virtual void		Unk_23(void);
	/*90*/virtual void		Unk_24(void);
	/*94*/virtual UInt32	GetRenderedWidth();
	/*98*/virtual UInt32	GetRenderedHeight();
	/*9C*/virtual NiFixedString* GetPath();
	/*A0*/virtual UInt32	GetPixelCount();

	TextureFormatPrefs	formatPrefs;	// 18
	NiDX9TextureData* textureData;	// 24
	NiTexture* prev;			// 28
	NiTexture* next;			// 2C
};
static_assert(sizeof(NiTexture) == 0x30);

// NiDX9Direct3DTexture - not referenced

// 048
class NiSourceTexture : public NiTexture
{
public:
	/*A4*/virtual void		LoadPixelDataFromFile();
	/*A8*/virtual void		DestroyAppPixelData();
	/*AC*/virtual void		CreateRendererData();

	NiFixedString	ddsPath1;		// 30
	NiFixedString	ddsPath2;		// 34
	void* ptr38;			// 38
	NiPixelData* pixelData;		// 3C
	bool			loadDirectToRender;	// 40
	bool			persistRenderData;	// 41
	bool			srcRendererDataIsPersistent;			// 42
	UInt8			byte43;			// 43
	NiFile* pFile;			// 44

	static NiSourceTexture* __fastcall Create(const char* ddsPath);
};

// 04C
class NiSourceCubeMap : public NiSourceTexture
{
public:
	NiSourceCubeMap();
	~NiSourceCubeMap();

	UInt32			unk48;	// 48
};

// 040
class NiRenderedTexture : public NiTexture
{
public:
	/*A4*/virtual Ni2DBuffer* Get2DBuffer();

	Ni2DBuffer* buffer;		// 30
	UInt32				cellRefID;		// 34	Used by MiniMap; otherwise appears unused
	UInt32				unk38;			// 38
	UInt32				multiSampleLvl;	// 3C
	bool				isRenderTarget;	// 40
	bool				useCustomFormat;// 41
	UInt8				pad42[2];		// 42
	D3DFORMAT			customFormat;	// 44

	void __fastcall SaveToFile(UInt32 fileFmt, char* filePath);
};

// 05C
class NiRenderedCubeMap : public NiRenderedTexture
{
public:
	NiRenderedCubeMap();
	~NiRenderedCubeMap();

	UInt32		unk040;		// 040
	NiObject* faces[6];	// 044
};

// 018
class NiSequenceStreamHelper : public NiObjectNET
{
public:
	NiSequenceStreamHelper();
	~NiSequenceStreamHelper();
};

//	name			d3dfmt   00 01 04       08       0C       10       14       18       1C 1D 20       24       28 29 2C       30       34 35 38       3C       40 41
//	R8G8B8			00000014 01 18 00000000 00000000 00000014 00000000 00000002 00000000 08 01 00000001 00000000 08 01 00000000 00000000 08 01 00000013 00000005 00 01
//	A8R8G8B8		00000015 01 20 00000001 00000000 00000015 00000000 00000002 00000000 08 01 00000001 00000000 08 01 00000000 00000000 08 01 00000003 00000000 08 01
//	X8R8G8B8		00000016 01 20 00000000 00000000 00000016 00000000 00000002 00000000 08 01 00000001 00000000 08 01 00000000 00000000 08 01 0000000E 00000005 08 01
//	R5G6B5			00000017 01 10 00000000 00000000 00000017 00000000 00000002 00000000 05 01 00000001 00000000 06 01 00000000 00000000 05 01 00000013 00000005 00 01
//	X1R5G5B5		00000018 01 10 00000000 00000000 00000018 00000000 00000002 00000000 05 01 00000001 00000000 05 01 00000000 00000000 05 01 0000000E 00000005 01 01
//	A1R5G5B5		00000019 01 10 00000001 00000000 00000019 00000000 00000002 00000000 05 01 00000001 00000000 05 01 00000000 00000000 05 01 00000003 00000000 01 01
//	A4R4G4B4		0000001A 01 10 00000001 00000000 0000001A 00000000 00000002 00000000 04 01 00000001 00000000 04 01 00000000 00000000 04 01 00000003 00000000 04 01
//	R3G3B2			0000001B 01 0A 00000000 00000000 0000001B 00000000 00000002 00000000 02 01 00000001 00000000 03 01 00000000 00000000 03 01 0000000E 00000005 02 01
//	A8				0000001C 01 08 0000000B 00000000 0000001C 00000000 00000003 00000000 08 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	A8R3G3B2		0000001D 01 10 00000001 00000000 0000001D 00000000 00000002 00000000 02 01 00000001 00000000 03 01 00000000 00000000 03 01 00000003 00000000 08 01
//	X4R4G4B4		0000001E 01 10 00000000 00000000 0000001E 00000000 00000002 00000000 04 01 00000001 00000000 04 01 00000000 00000000 04 01 0000000E 00000000 04 01
//	A2B10G10R10		0000001F 01 20 00000001 00000000 0000001F 00000000 00000000 00000000 0A 01 00000001 00000000 0A 01 00000002 00000000 0A 01 00000003 00000000 02 01
//	A8B8G8R8		00000020 01 20 00000001 00000000 00000020 00000000 00000000 00000000 08 01 00000001 00000000 08 01 00000002 00000000 08 01 00000003 00000000 08 01
//	X8B8G8R8		00000021 01 20 00000000 00000000 00000021 00000000 00000000 00000000 08 01 00000001 00000000 08 01 00000002 00000000 08 01 0000000E 00000005 08 01
//	G16R16			00000022 01 20 0000000C 00000000 00000022 00000000 00000001 00000000 10 01 00000000 00000000 10 01 00000013 00000005 00 01 00000013 00000005 00 01
//	A2R10G10B10		00000023 01 20 00000001 00000000 00000023 00000000 00000002 00000000 0A 01 00000001 00000000 0A 01 00000000 00000000 0A 01 00000003 00000000 02 01
//	A16B16G16R16	00000024 01 40 00000001 00000000 00000024 00000000 00000000 00000001 10 01 00000001 00000001 10 01 00000002 00000001 10 01 00000003 00000001 10 01
//	A8P8			00000028 01 10 0000000C 00000000 00000028 00000000 00000010 00000003 08 01 00000003 00000000 08 01 00000013 00000005 00 01 00000013 00000005 00 01
//	P8				00000029 01 08 00000002 00000000 00000029 00000000 00000010 00000003 08 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	L8				00000032 01 08 0000000B 00000000 00000032 00000000 00000009 00000000 08 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	A8L8			00000033 01 10 0000000C 00000000 00000033 00000000 00000009 00000000 08 01 00000003 00000000 08 01 00000013 00000005 00 01 00000013 00000005 00 01
//	A4L4			00000034 01 08 0000000C 00000000 00000034 00000000 00000009 00000000 04 01 00000003 00000000 04 01 00000013 00000005 00 01 00000013 00000005 00 01
//	V8U8			0000003C 01 10 00000008 00000000 0000003C 00000000 00000005 00000000 08 01 00000006 00000000 08 01 00000013 00000005 00 01 00000013 00000005 00 01
//	L6V5U5			0000003D 01 10 00000009 00000000 0000003D 00000000 00000005 00000000 05 01 00000006 00000000 05 01 00000009 00000000 06 00 00000013 00000005 00 00
//	X8L8V8U8		0000003E 01 20 00000009 00000000 0000003E 00000000 00000005 00000000 08 01 00000006 00000000 08 01 00000009 00000000 08 00 0000000E 00000005 08 00
//	Q8W8V8U8		0000003F 01 20 00000008 00000000 0000003F 00000000 00000005 00000000 08 01 00000006 00000000 08 01 00000007 00000000 08 01 00000008 00000000 08 01
//	V16U16			00000040 01 20 00000008 00000000 00000040 00000000 00000005 00000000 10 01 00000006 00000000 10 01 00000013 00000005 00 01 00000013 00000005 00 01
//	A2W10V10U10		00000043 01 20 00000008 00000000 00000043 00000000 00000005 00000000 0A 01 00000006 00000000 0B 01 00000007 00000000 0B 01 00000013 00000005 00 01
//	D16_LOCKABLE	00000046 01 10 0000000F 00000000 00000046 00000000 00000011 00000000 10 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	D32				00000047 01 20 0000000F 00000000 00000047 00000000 00000011 00000000 20 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	D15S1			00000049 01 10 0000000F 00000000 00000049 00000000 00000012 00000000 01 01 00000011 00000000 0F 01 00000013 00000005 00 01 00000013 00000005 00 01
//	D24S8			0000004B 01 20 0000000F 00000000 0000004B 00000000 00000012 00000000 08 01 00000011 00000000 18 01 00000013 00000005 00 01 00000013 00000005 00 01
//	D24X8			0000004D 01 20 0000000F 00000000 0000004D 00000000 0000000E 00000000 08 01 00000011 00000000 18 01 00000013 00000005 00 01 00000013 00000005 00 01
//	D24X4S4			0000004F 01 20 0000000F 00000000 0000004F 00000000 00000012 00000000 04 01 0000000E 00000000 04 01 00000011 00000000 18 01 00000013 00000005 00 01
//	D16				00000050 01 10 0000000F 00000000 00000050 00000000 00000011 00000000 10 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	L16				00000051 01 10 0000000B 00000000 00000051 00000000 00000009 00000000 10 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	D32F_LOCKABLE	00000052 01 20 0000000B 00000000 00000052 00000000 0000000E 00000005 20 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	D24FS8			00000053 01 20 0000000B 00000000 00000053 00000000 0000000E 00000005 20 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	Q16W16V16U16	0000006E 01 40 0000000B 00000000 0000006E 00000000 0000000E 00000005 40 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	R16F			0000006F 01 10 0000000B 00000000 0000006F 00000000 00000000 00000001 10 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	G16R16F			00000070 01 20 0000000C 00000000 00000070 00000000 00000000 00000001 10 01 00000001 00000001 10 01 00000013 00000005 00 01 00000013 00000005 00 01
//	A16B16G16R16F	00000071 01 40 00000001 00000000 00000071 00000000 00000000 00000001 10 01 00000001 00000001 10 01 00000002 00000001 10 01 00000003 00000001 10 01
//	R32F			00000072 01 20 0000000B 00000000 00000072 00000000 00000000 00000002 20 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	G32R32F			00000073 01 40 0000000C 00000000 00000073 00000000 00000000 00000002 20 01 00000001 00000002 20 01 00000013 00000005 00 01 00000013 00000005 00 01
//	A32B32G32R32F	00000074 01 80 00000001 00000000 00000074 00000000 00000000 00000002 20 01 00000001 00000002 20 01 00000002 00000002 20 01 00000003 00000002 20 01
//	CxV8U8			00000075 01 10 0000000B 00000000 00000075 00000000 0000000E 00000005 10 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	DXT1			xxxxxxxx 01 00 00000004 00000000 xxxxxxxx 00000000 00000004 00000004 00 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	DXT3			xxxxxxxx 01 00 00000005 00000000 xxxxxxxx 00000000 00000004 00000004 00 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	DXT5			xxxxxxxx 01 00 00000006 00000000 xxxxxxxx 00000000 00000004 00000004 00 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01

//	invalid			xxxxxxxx 01 00 0000000B 00000000 xxxxxxxx 00000000 0000000E 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	D32_LOCKABLE	00000054 01 00 0000000B 00000000 00000054 00000000 0000000E 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	S8_LOCKABLE		00000055 01 00 0000000B 00000000 00000055 00000000 0000000E 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	VERTEXDATA		00000064 01 00 0000000B 00000000 00000064 00000000 0000000E 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	INDEX16			00000065 01 00 0000000B 00000000 00000065 00000000 0000000E 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01
//	INDEX32			00000066 01 00 0000000B 00000000 00000066 00000000 0000000E 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01 00000013 00000005 00 01

// 44
struct TextureFormat
{
	enum
	{
		kFormat_RGB = 0,		// 0
		kFormat_RGBA,			// 1
		kFormat_A,				// 2
		kFormat_Unk3,			// 3
		kFormat_DXT1,			// 4
		kFormat_DXT3,			// 5
		kFormat_DXT5,			// 6
		kFormat_Unk7,			// 7
		kFormat_Bump,			// 8
		kFormat_BumpLuminance,	// 9
		kFormat_UnkA,			// A
		kFormat_Other,			// B - A8 L8 L16 D32F_LOCKABLE D24FS8 Q16W16V16U16 R16F R32F CxV8U8
		kFormat_Other2,			// C - G16R16 A8P8 A8L8 A4L4 G16R16F G32R32F
		kFormat_UnkD,			// D
		kFormat_UnkE,			// E
		kFormat_Depth,			// F
	};

	enum
	{
		kType_Blue,			// 00
		kType_Green,		// 01
		kType_Red,			// 02
		kType_Alpha,		// 03
		kType_Unk04,		// 04
		kType_BumpU,		// 05
		kType_BumpV,		// 06
		kType_Unk07,		// 07
		kType_Unk08,		// 08
		kType_Luminance,	// 09
		kType_Unk0A,		// 0A
		kType_Unk0B,		// 0B
		kType_Unk0C,		// 0C
		kType_Unk0D,		// 0D
		kType_Unused,		// 0E
		kType_Unk0F,		// 0F
		kType_PalIdx,		// 10
		kType_Depth,		// 11
		kType_Stencil,		// 12
		kType_None,			// 13
	};

	enum
	{
		kType2_Default,		// 00
		kType2_16Bit,		// 01
		kType2_32Bit,		// 02
		kType2_Palettized,	// 03
		kType2_Compressed,	// 04
		kType2_None,		// 05
	};

	UInt8	unk00;		// 00 - always 01? (checked all D3DFMT)
	UInt8	bpp;		// 01 - zero for dxt
	UInt8	pad02[2];	// 02
	UInt32	format;		// 04 - default kFormat_A (really)
	UInt32	unk08;		// 08 - always 00000000? (checked all D3DFMT)
	UInt32	d3dfmt;		// 0C
	UInt32	unk10;		// 10 - always 00000000? (checked all D3DFMT)

	struct Channel
	{
		UInt32	type;		// 0
		UInt32	type2;		// 4
		UInt8	bits;		// 8
		UInt8	unk9;		// 9 - only seen non-01 when unused (L6V5U5 X8L8V8U8)
		UInt8	padA[2];	// A
	};

	Channel	channels[4];	// 14

	void InitFromD3DFMT(UInt32 fmt);
};

// 070
class NiPixelData : public NiObject
{
public:
	NiPixelData();
	~NiPixelData();

	// face size = unk05C[mipmapLevels]
	// total size = face size * numFaces

	TextureFormat	format;		// 008
	NiRefObject* unk04C;	// 04C
	UInt32	unk050;			// 050
	UInt32* width;		// 054 - array for mipmaps?
	UInt32* height;		// 058
	UInt32* unk05C;		// 05C - sizes?
	UInt32	mipmapLevels;	// 060
	UInt32	unk064;			// 064
	UInt32	unk068;			// 068
	UInt32	numFaces;		// 06C
};

// 068
class NiControllerSequence : public NiObject
{
public:
	NiControllerSequence();
	~NiControllerSequence();

	enum
	{
		kState_Inactive = 0,
		kState_Animating,
		kState_EaseIn,
		kState_EaseOut,
		kState_TransSource,
		kState_TransDest,
		kState_MorphSource
	};

	enum
	{
		kCycle_Loop = 0,
		kCycle_Reverse,
		kCycle_Clamp,
	};

	// 10
	struct Unk014
	{
		NiRefObject* unk00;	// 00
		NiRefObject* unk04;	// 04
		UInt32		unk08;		// 08
		UInt8		unk0C;		// 0C
		UInt8		unk0D;		// 0D
		UInt8		pad0E[2];	// 0E
	};

	// 10
	struct Unk018
	{
		NiRefObject* unk00;	// 00
		UInt16		unk04;		// 04
		UInt16		unk06;		// 06
		UInt16		unk08;		// 08
		UInt16		unk0A;		// 0A
		UInt16		unk0C;		// 0C
		UInt8		pad0E[2];	// 0E
	};

	char* filePath;		// 008
	UInt32				arraySize;		// 00C
	UInt32				unk010;			// 010
	Unk014* unk014;		// 014
	Unk018* unk018;		// 018
	float				weight;			// 01C
	NiTextKeyExtraData* unk020;		// 020
	UInt32				cycleType;		// 024
	float				freq;			// 028
	float				begin;			// 02C
	float				end;			// 030
	float				last;			// 034
	float				weightLast;		// 038
	float				lastScaled;		// 03C
	NiControllerManager* controllerMgr;	// 040
	UInt32				state;			// 044
	float				offset;			// 048
	float				start;			// 04C - offset * -1?
	float				end2;			// 050
	UInt32				unk054;			// 054
	UInt32				unk058;			// 058
	char* accumRoot;	// 05C - bone? (seen "Bip01")
	NiNode* niNode060;	// 060
	NiStringPalette* unk064;		// 064
};

// 06C
class BSAnimGroupSequence : public NiControllerSequence
{
public:
	BSAnimGroupSequence();
	~BSAnimGroupSequence();

	TESAnimGroup* animGroup;	//068
};

// 02C+
class TESAnimGroup
{
public:
	// derived from NiRefObject
	TESAnimGroup();
	~TESAnimGroup();

	virtual void Destructor(bool arg0);

	enum {
		kAnimGroup_Idle = 0x0,
		kAnimGroup_DynamicIdle,
		kAnimGroup_SpecialIdle,
		kAnimGroup_Forward,
		kAnimGroup_Backward,
		kAnimGroup_Left,
		kAnimGroup_Right,
		kAnimGroup_FastForward,
		kAnimGroup_FastBackward,
		kAnimGroup_FastLeft,
		kAnimGroup_FastRight,
		kAnimGroup_DodgeForward,
		kAnimGroup_DodgeBack,
		kAnimGroup_DodgeLeft,
		kAnimGroup_DodgeRight,
		kAnimGroup_TurnLeft,
		kAnimGroup_TurnRight,
		kAnimGroup_Aim,
		kAnimGroup_AimUp,
		kAnimGroup_AimDown,
		kAnimGroup_AimIS,
		kAnimGroup_AimISUp,
		kAnimGroup_AimISDown,
		kAnimGroup_Holster,
		kAnimGroup_Equip,
		kAnimGroup_Unequip,
		kAnimGroup_AttackLeft,
		kAnimGroup_AttackLeftUp,
		kAnimGroup_AttackLeftDown,
		kAnimGroup_AttackLeftIS,
		kAnimGroup_AttackLeftISUp,
		kAnimGroup_AttackLeftISDown,
		kAnimGroup_AttackRight,
		kAnimGroup_AttackRightUp,
		kAnimGroup_AttackRightDown,
		kAnimGroup_AttackRightIS,
		kAnimGroup_AttackRightISUp,
		kAnimGroup_AttackRightISDown,
		kAnimGroup_Attack3,
		kAnimGroup_Attack3Up,
		kAnimGroup_Attack3Down,
		kAnimGroup_Attack3IS,
		kAnimGroup_Attack3ISUp,
		kAnimGroup_Attack3ISDown,
		kAnimGroup_Attack4,
		kAnimGroup_Attack4Up,
		kAnimGroup_Attack4Down,
		kAnimGroup_Attack4IS,
		kAnimGroup_Attack4ISUp,
		kAnimGroup_Attack4ISDown,
		kAnimGroup_Attack5,
		kAnimGroup_Attack5Up,
		kAnimGroup_Attack5Down,
		kAnimGroup_Attack5IS,
		kAnimGroup_Attack5ISUp,
		kAnimGroup_Attack5ISDown,
		kAnimGroup_Attack6,
		kAnimGroup_Attack6Up,
		kAnimGroup_Attack6Down,
		kAnimGroup_Attack6IS,
		kAnimGroup_Attack6ISUp,
		kAnimGroup_Attack6ISDown,
		kAnimGroup_Attack7,
		kAnimGroup_Attack7Up,
		kAnimGroup_Attack7Down,
		kAnimGroup_Attack7IS,
		kAnimGroup_Attack7ISUp,
		kAnimGroup_Attack7ISDown,
		kAnimGroup_Attack8,
		kAnimGroup_Attack8Up,
		kAnimGroup_Attack8Down,
		kAnimGroup_Attack8IS,
		kAnimGroup_Attack8ISUp,
		kAnimGroup_Attack8ISDown,
		kAnimGroup_AttackLoop,
		kAnimGroup_AttackLoopUp,
		kAnimGroup_AttackLoopDown,
		kAnimGroup_AttackLoopIS,
		kAnimGroup_AttackLoopISUp,
		kAnimGroup_AttackLoopISDown,
		kAnimGroup_AttackSpin,
		kAnimGroup_AttackSpinUp,
		kAnimGroup_AttackSpinDown,
		kAnimGroup_AttackSpinIS,
		kAnimGroup_AttackSpinISUp,
		kAnimGroup_AttackSpinISDown,
		kAnimGroup_AttackSpin2,
		kAnimGroup_AttackSpin2Up,
		kAnimGroup_AttackSpin2Down,
		kAnimGroup_AttackSpin2IS,
		kAnimGroup_AttackSpin2ISUp,
		kAnimGroup_AttackSpin2ISDown,
		kAnimGroup_AttackPower,
		kAnimGroup_AttackForwardPower,
		kAnimGroup_AttackBackPower,
		kAnimGroup_AttackLeftPower,
		kAnimGroup_AttackRightPower,
		kAnimGroup_AttackCustom1Power,
		kAnimGroup_AttackCustom2Power,
		kAnimGroup_AttackCustom3Power,
		kAnimGroup_AttackCustom4Power,
		kAnimGroup_AttackCustom5Power,
		kAnimGroup_PlaceMine,
		kAnimGroup_PlaceMineUp,
		kAnimGroup_PlaceMineDown,
		kAnimGroup_PlaceMineIS,
		kAnimGroup_PlaceMineISUp,
		kAnimGroup_PlaceMineISDown,
		kAnimGroup_PlaceMine2,
		kAnimGroup_PlaceMine2Up,
		kAnimGroup_PlaceMine2Down,
		kAnimGroup_PlaceMine2IS,
		kAnimGroup_PlaceMine2ISUp,
		kAnimGroup_PlaceMine2ISDown,
		kAnimGroup_AttackThrow,
		kAnimGroup_AttackThrowUp,
		kAnimGroup_AttackThrowDown,
		kAnimGroup_AttackThrowIS,
		kAnimGroup_AttackThrowISUp,
		kAnimGroup_AttackThrowISDown,
		kAnimGroup_AttackThrow2,
		kAnimGroup_AttackThrow2Up,
		kAnimGroup_AttackThrow2Down,
		kAnimGroup_AttackThrow2IS,
		kAnimGroup_AttackThrow2ISUp,
		kAnimGroup_AttackThrow2ISDown,
		kAnimGroup_AttackThrow3,
		kAnimGroup_AttackThrow3Up,
		kAnimGroup_AttackThrow3Down,
		kAnimGroup_AttackThrow3IS,
		kAnimGroup_AttackThrow3ISUp,
		kAnimGroup_AttackThrow3ISDown,
		kAnimGroup_AttackThrow4,
		kAnimGroup_AttackThrow4Up,
		kAnimGroup_AttackThrow4Down,
		kAnimGroup_AttackThrow4IS,
		kAnimGroup_AttackThrow4ISUp,
		kAnimGroup_AttackThrow4ISDown,
		kAnimGroup_AttackThrow5,
		kAnimGroup_AttackThrow5Up,
		kAnimGroup_AttackThrow5Down,
		kAnimGroup_AttackThrow5IS,
		kAnimGroup_AttackThrow5ISUp,
		kAnimGroup_AttackThrow5ISDown,
		kAnimGroup_Attack9,
		kAnimGroup_Attack9Up,
		kAnimGroup_Attack9Down,
		kAnimGroup_Attack9IS,
		kAnimGroup_Attack9ISUp,
		kAnimGroup_Attack9ISDown,
		kAnimGroup_AttackThrow6,
		kAnimGroup_AttackThrow6Up,
		kAnimGroup_AttackThrow6Down,
		kAnimGroup_AttackThrow6IS,
		kAnimGroup_AttackThrow6ISUp,
		kAnimGroup_AttackThrow6ISDown,
		kAnimGroup_AttackThrow7,
		kAnimGroup_AttackThrow7Up,
		kAnimGroup_AttackThrow7Down,
		kAnimGroup_AttackThrow7IS,
		kAnimGroup_AttackThrow7ISUp,
		kAnimGroup_AttackThrow7ISDown,
		kAnimGroup_AttackThrow8,
		kAnimGroup_AttackThrow8Up,
		kAnimGroup_AttackThrow8Down,
		kAnimGroup_AttackThrow8IS,
		kAnimGroup_AttackThrow8ISUp,
		kAnimGroup_AttackThrow8ISDown,
		kAnimGroup_Counter,
		kAnimGroup_stomp,
		kAnimGroup_BlockIdle,
		kAnimGroup_BlockHit,
		kAnimGroup_Recoil,
		kAnimGroup_ReloadWStart,
		kAnimGroup_ReloadXStart,
		kAnimGroup_ReloadYStart,
		kAnimGroup_ReloadZStart,
		kAnimGroup_ReloadA,
		kAnimGroup_ReloadB,
		kAnimGroup_ReloadC,
		kAnimGroup_ReloadD,
		kAnimGroup_ReloadE,
		kAnimGroup_ReloadF,
		kAnimGroup_ReloadG,
		kAnimGroup_ReloadH,
		kAnimGroup_ReloadI,
		kAnimGroup_ReloadJ,
		kAnimGroup_ReloadK,
		kAnimGroup_ReloadL,
		kAnimGroup_ReloadM,
		kAnimGroup_ReloadN,
		kAnimGroup_ReloadO,
		kAnimGroup_ReloadP,
		kAnimGroup_ReloadQ,
		kAnimGroup_ReloadR,
		kAnimGroup_ReloadS,
		kAnimGroup_ReloadW,
		kAnimGroup_ReloadX,
		kAnimGroup_ReloadY,
		kAnimGroup_ReloadZ,
		kAnimGroup_JamA,
		kAnimGroup_JamB,
		kAnimGroup_JamC,
		kAnimGroup_JamD,
		kAnimGroup_JamE,
		kAnimGroup_JamF,
		kAnimGroup_JamG,
		kAnimGroup_JamH,
		kAnimGroup_JamI,
		kAnimGroup_JamJ,
		kAnimGroup_JamK,
		kAnimGroup_JamL,
		kAnimGroup_JamM,
		kAnimGroup_JamN,
		kAnimGroup_JamO,
		kAnimGroup_JamP,
		kAnimGroup_JamQ,
		kAnimGroup_JamR,
		kAnimGroup_JamS,
		kAnimGroup_JamW,
		kAnimGroup_JamX,
		kAnimGroup_JamY,
		kAnimGroup_JamZ,
		kAnimGroup_Stagger,
		kAnimGroup_Death,
		kAnimGroup_Talking,
		kAnimGroup_PipBoy,
		kAnimGroup_JumpStart,
		kAnimGroup_JumpLoop,
		kAnimGroup_JumpLand,
		kAnimGroup_HandGrip1,
		kAnimGroup_HandGrip2,
		kAnimGroup_HandGrip3,
		kAnimGroup_HandGrip4,
		kAnimGroup_HandGrip5,
		kAnimGroup_HandGrip6,
		kAnimGroup_JumpLoopForward,
		kAnimGroup_JumpLoopBackward,
		kAnimGroup_JumpLoopLeft,
		kAnimGroup_JumpLoopRight,
		kAnimGroup_PipBoyChild,
		kAnimGroup_JumpLandForward,
		kAnimGroup_JumpLandBackward,
		kAnimGroup_JumpLandLeft,
		kAnimGroup_JumpLandRight,

		kAnimGroup_Max						// = 0x0FFF,	// Temporary until known
	};

	// 24
	struct AnimGroupInfo {
		const char* name;				// 00
		UInt8		sequenceType;		// 04
		UInt8		pad[3];
		UInt32		unk08[7];			// 08
	};

	//void**	vtbl			//000
	UInt8		unk004;			//004
	UInt8		unk005[3];
	UInt8		animGroup;		//008 init'ed to word arg in c'tor
	UInt8		unk009;			//009 does what?
	UInt16		unk00A;
	UInt32		numFrames;		//00C count of group frames (Start, Detach, Attack, End, etc)
	float** frameData;	//010 pointer to float array of group frame times (size numFrames)
	UInt32		unk014;			//014
	UInt32		unk018;			//018
	UInt32		unk01C;			//01C
	UInt8		unk020;			//020
	UInt8		unk021;
	UInt8		pad022[2];
	UInt32		unk024;			//024
	void* unk028;		//028

	static const char* StringForAnimGroupCode(UInt32 groupCode);
	static UInt32 AnimGroupForString(const char* groupName);
};

extern std::span<TESAnimGroup::AnimGroupInfo> g_animGroups;

//// derived from NiFile, which derives from NiBinaryStream
//// 154
//class BSFile
//{
//public:
//	BSFile();
//	~BSFile();
//
//	virtual void	Destructor(bool freeMemory);				// 00
//	virtual void	Unk_01(void);								// 04
//	virtual void	Unk_02(void);								// 08
//	virtual void	Unk_03(void);								// 0C
//	virtual void	Unk_04(void);								// 10
//	virtual void	DumpAttributes(NiTArray <char *> * dst);	// 14
//	virtual UInt32	GetSize(void);								// 18
//	virtual void	Unk_07(void);								// 1C
//	virtual void	Unk_08(void);								// 20
//	virtual void	Unk_09(void);								// 24
//	virtual void	Unk_0A(void);								// 28
//	virtual void	Unk_0B(void);								// 2C
//	virtual void	Unk_0C(void);								// 30
//	virtual void	Unk_Read(void);								// 34
//	virtual void	Unk_Write(void);							// 38
//
////	void	** m_vtbl;		// 000
//	void	* m_readProc;	// 004 - function pointer
//	void	* m_writeProc;	// 008 - function pointer
//	UInt32	m_bufSize;		// 00C
//	UInt32	m_unk010;		// 010 - init'd to m_bufSize
//	UInt32	m_unk014;		// 014
//	void	* m_buf;		// 018
//	FILE	* m_file;		// 01C
//	UInt32	m_writeAccess;	// 020
//	UInt8	m_good;			// 024
//	UInt8	m_pad025[3];	// 025
//	UInt8	m_unk028;		// 028
//	UInt8	m_pad029[3];	// 029
//	UInt32	m_unk02C;		// 02C
//	UInt32	m_pos;			// 030
//	UInt32	m_unk034;		// 034
//	UInt32	m_unk038;		// 038
//	char	m_path[0x104];	// 03C
//	UInt32	m_unk140;		// 140
//	UInt32	m_unk144;		// 144
//	UInt32	m_pos2;			// 148 - used if m_pos is 0xFFFFFFFF
//	UInt32	m_unk14C;		// 14C
//	UInt32	m_fileSize;		// 150
//};

/**** misc non-NiObjects ****/

// 30
class NiPropertyState : public NiRefObject
{
public:
	NiPropertyState();
	~NiPropertyState();

	UInt32	unk008[(0x30 - 0x08) >> 2];	// 008
};

// 20
class NiDynamicEffectState : public NiRefObject
{
public:
	NiDynamicEffectState();
	~NiDynamicEffectState();

	UInt8	unk008;		// 008
	UInt8	pad009[3];	// 009
	UInt32	unk00C;		// 00C
	UInt32	unk010;		// 010
	UInt32	unk014;		// 014
	UInt32	unk018;		// 018
	UInt32	unk01C;		// 01C
};

// name is a guess
class NiCulledGeoList
{
public:
	NiCulledGeoList();
	~NiCulledGeoList();

	NiGeometry** m_geo;		// 00
	UInt32		m_numItems;		// 04
	UInt32		m_bufLen;		// 08
	UInt32		m_bufGrowSize;	// 0C
};

// 90
class NiCullingProcess
{
public:
	/*00*/virtual NiRTTI* GetRTTI();
	/*04*/virtual void		Unk_01(void);	//	Unk_01 through Unk_0F are unused
	/*08*/virtual void		Unk_02(void);
	/*0C*/virtual void		Unk_03(void);
	/*10*/virtual void		Unk_04(void);
	/*14*/virtual void		Unk_05(void);
	/*18*/virtual void		Unk_06(void);
	/*1C*/virtual void		Unk_07(void);
	/*20*/virtual void		Unk_08(void);
	/*24*/virtual void		Unk_09(void);
	/*28*/virtual void		Unk_0A(void);
	/*2C*/virtual void		Unk_0B(void);
	/*30*/virtual void		Unk_0C(void);
	/*34*/virtual void		Unk_0D(void);
	/*38*/virtual void		Unk_0E(void);
	/*3C*/virtual void		Unk_0F(void);
	/*40*/virtual void		Destructor(bool doFree);
	/*44*/virtual void		Process(NiAVObject* pObject);
	/*48*/virtual void		ProcessAlt(NiCamera* camera, NiAVObject* pScene, NiVisibleArray* pVisibleArr);
	/*4C*/virtual void		Append(NiGeometry* arg);

	UInt8				useAppendFn;	// 04 - call AddGeo when true, else just add to the list
	UInt8				pad05[3];		// 05
	NiVisibleArray* visibleArr;	// 08
	NiCamera* camera;		// 0C
	NiFrustum			frustum;		// 10
	NiFrustumPlanes		planes;			// 2C
};
static_assert(sizeof(NiCullingProcess) == 0x90);

enum CullingMode : UInt32
{
	kCull_Normal = 0,
	kCull_AllPass = 1,
	kCull_AllFail = 2,
	kCull_IgnoreMultiBounds = 3,
	kCull_ForceMultiBoundsNoUpdate = 4
};

// C8	vtbl @ 0x101E2EC
class BSCullingProcess : public NiCullingProcess
{
public:
	CullingMode				topCullMode;		// 90
	CullingMode				cullModeStack[10];	// 94
	UInt32					stackSize;			// BC
	BSCompoundFrustum* compFrustum;		// C0
	BSShaderAccumulator* shaderAccum;		// C4
};
static_assert(sizeof(BSCullingProcess) == 0xC8);

// 90
class BSFadeNodeCuller : public NiCullingProcess
{
public:
};

/**** BSTempEffects ****/

// 18
class BSTempEffect : public NiObject
{
public:
	BSTempEffect();
	~BSTempEffect();

	float			duration;		// 08
	TESObjectCELL* cell;			// 0C
	float			unk10;			// 10
	UInt8			unk14;			// 14
	UInt8			pad15[3];
};

// 28
class MagicHitEffect : public BSTempEffect
{
public:
	MagicHitEffect();
	~MagicHitEffect();

	ActiveEffect* activeEffect;	// 18	
	TESObjectREFR* target;		// 1C
	float			unk20;			// 20	Init'd from ActiveEffect.timeElapsed
	UInt8			unk24;			// 24	from ActiveEffect.EffectFlag
	UInt8			pad25[3];
};

// 6C
class MagicShaderHitEffect : public MagicHitEffect
{
public:
	MagicShaderHitEffect();
	~MagicShaderHitEffect();

	UInt8					unk28;						// 28	Init'd to byte, OK for first offset.
	UInt8					pad29[3];
	UInt32					unk2C;						// 2C	Init'd to DWord
	TESEffectShader* effectShader;				// 30	Init'd to *effectShader
	float					unk34;						// 34	Init'd to float
	BSSimpleArray<NiPointer<ParticleShaderProperty>>	unk38;	// 38	Init'd to BSSimpleArray<NiPointer<ParticleShaderProperty>>
	// the remainder is not validated..
	void* textureEffectData;		// 48 seen TextureEffectData< BSSahderLightingProperty >, init'd to RefNiObject
};	// Alloc'd to 6C, 68 is RefNiObject, 60 is Init'd to 1.0, 64 also
// 4C is byte, Init'd to 0 for non player, otherwize = Player.1stPersonSkeleton.Flags0030.Bit0 is null


class hkpSimpleConstraintContactMgr;
class hkStatisticsCollector;
class hkpContinuousSimulation;
class hkpDefaultWorldMaintenanceMgr;
class hkpBroadPhase;
class hkpBroadPhaseListener;
class hkpPhantomBroadPhaseListener;
class hkpEntityEntityBroadPhaseListener;
class hkpBroadPhaseBorderListener;
class hkpCollisionDispatcher;
class bhkCollisionFilter;
class hkpDefaultConvexListFilter;
class hkpEntityListener;
class hkpContactListener;
class bhkShape;
class hkpShape;
class hkpCachingShapePhantom;
class hkpAllCdPointCollector;
class TESTrapListener;
class TESWaterListener;


class NiCamera;
class NiLODData;
class ShadowSceneLight;	// 220, derives from NiRefObject
class BSImageSpaceShader;
class NiScreenPolygon;
class NiScreenTexture;
class NiPSysModifier;
class NiRenderer;
class NiGeometryData;

// 0AC
class NiAVObject : public NiObjectNET
{
public:
	NiAVObject();
	~NiAVObject();

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

	enum NiFlags
	{
		kNiFlag_Hidden = 0x00000001,
		kNiFlag_SelectiveUpdate = 0x00000002,
		kNiFlag_SelUpdTransforms = 0x00000004,
		kNiFlag_SelUpdController = 0x00000008,
		kNiFlag_SelUpdRigid = 0x00000010,
		kNiFlag_DisplayObject = 0x00000020,
		kNiFlag_DisableSorting = 0x00000040,
		kNiFlag_SelUpdTransformsOverride = 0x00000080,
		kNiFlag_UnkBit8 = 0x00000100,
		kNiFlag_SaveExternalGeomData = 0x00000200,
		kNiFlag_NoDecals = 0x00000400,
		kNiFlag_AlwaysDraw = 0x00000800,
		kNiFlag_MeshLOD = 0x00001000,
		kNiFlag_FixedBound = 0x00002000,
		kNiFlag_TopFadeNode = 0x00004000,
		kNiFlag_IgnoreFade = 0x00008000,
		kNiFlag_NoAnimSyncX = 0x00010000,
		kNiFlag_NoAnimSyncY = 0x00020000,
		kNiFlag_NoAnimSyncZ = 0x00040000,
		kNiFlag_NoAnimSyncS = 0x00080000,
		kNiFlag_Culled = 0x00100000,
		kNiFlag_NoDismemberValidity = 0x00200000,
		kNiFlag_RenderUse = 0x00400000,
		kNiFlag_MaterialsApplied = 0x00800000,
		kNiFlag_HighDetail = 0x01000000,
		kNiFlag_ForceUpdate = 0x02000000,
		kNiFlag_PreProcessedNode = 0x04000000,
		kNiFlag_UnkBit27 = 0x08000000,
		kNiFlag_UnkBit28 = 0x10000000,
		kNiFlag_IsPointLight = 0x20000000,	//	JIP only
		kNiFlag_DoneInitLights = 0x40000000,	//	JIP only
		kNiFlag_IsInserted = 0x80000000	//	JIP only
	};

	NiNode* m_parent;				// 18
	bhkNiCollisionObject* m_collisionObject;		// 1C
	NiBound* m_kWorldBound;			// 20
	DList<NiProperty>		m_propertyList;			// 24
	UInt32					m_flags;				// 30
	NiTransform				m_transformLocal;		// 34
	NiTransform				m_transformWorld;		// 68

	inline NiMatrix33& LocalRotate() { return m_transformLocal.rotate; }
	inline NiMatrix33& WorldRotate() { return m_transformWorld.rotate; }
	inline NiVector3& LocalTranslate() { return m_transformLocal.translate; }
	inline NiVector3& WorldTranslate() { return m_transformWorld.translate; }

	inline void SetHidden(bool hide)
	{
		UBYT(m_flags) &= ~1;
		UBYT(m_flags) |= (UInt8)hide;
	}
	inline void Hide() { UBYT(m_flags) |= 1; }
	inline void Show() { UBYT(m_flags) &= ~1; }

	NiAVObject* CreateCopy();
	void Update();
	UInt32 GetIndex() const;
	bool ReplaceObject(NiAVObject* object);
	TESObjectREFR* GetParentRef() const;
	void AssignGeometryProps();

	void ExportToFile(const char* filePath);

	void Dump(UInt8 dumpFlags = 0xF);
	void DumpParents();
};
static_assert(sizeof(NiAVObject) == 0x9C);


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
typedef FixedTypeArray<hkpWorldObject*, 0x40> ContactObjects;
// AC
class NiNode : public NiAVObject
{
public:
	/*DC*/virtual void		AddObject(NiAVObject* object, bool firstFree);
	/*E0*/virtual void		AddObjectAt(UInt32 index, NiAVObject* object);
	/*E4*/virtual void		RemoveObject2(NiAVObject* toRemove, NiAVObject** arg2);
	/*E8*/virtual void		RemoveObject(NiAVObject* toRemove);		//	Calls RemoveObject2 with arg2 as ptr to NULL
	/*EC*/virtual void		RemoveNthObject2(UInt32 index, NiAVObject** arg2);
	/*F0*/virtual void		RemoveNthObject(UInt32 index);			//	Calls RemoveNthObject2 with arg2 as ptr to NULL
	/*F4*/virtual void		ReplaceNthObject2(UInt32 index, NiAVObject* replaceWith, NiAVObject** arg3);
	/*F8*/virtual void		ReplaceNthObject(UInt32 index, NiAVObject* replaceWith);	//	Calls ReplaceNthObject2 with arg3 as ptr to NULL
	/*FC*/virtual void		UpdateUpwardPass();

	NiTArray<NiAVObject*>	m_children;		// 9C

	static NiNode* __stdcall Create(const char* nameStr);		//	str of NiFixedString
	NiAVObject* __fastcall GetBlockByName(const char* nameStr) const;	//	str of NiFixedString
	NiAVObject* __fastcall GetBlock(const char* blockName) const;
	NiNode* __fastcall GetNode(const char* nodeName) const;
	NiAVObject* __fastcall FindBlockOfType(UInt32 typeVtbl) const;
	bool IsMovable() const;
	void __fastcall ToggleCollision(UInt8 flag);
	void ResetCollision();
	void RemoveCollision();
	void __vectorcall SetAlphaRecurse(float alpha);
	void ResetShaderRenderPass();
	UInt32 GetBSXFlags() const;
	void AddPropertyRecurse(NiProperty* niProperty);
	void AddNoLightingPropertyRecurse();
	void __vectorcall SetMaterialPropValueRecurse(UInt32 traitID, float value);
	void __fastcall SetCollisionPropRecurse(UInt32 propID, FltAndInt value);
	void __fastcall SetCollisionLayer(UInt32 layerType);
	void GetContactObjects(ContactObjects& contactObjs);
	bool HasPhantom();
	float __vectorcall GetBodyMass(float totalMass) const;
	void ApplyForce(const NiVector4& forceVector);
};
static_assert(sizeof(NiNode) == 0xAC);

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

class RefNiObject
{
	NiObject* object;	// 00
};
// E4
class BSFadeNode : public NiNode
{
public:
	enum FadeType
	{
		kFade_Object = 1,
		kFade_Item = 2,
		kFade_Actor = 3,
		kFade_Unknown6 = 6,
		kFade_Unknown7 = 7,
		kFade_Unknown8 = 8,
		kFade_LODFadeOutMax = 0xA,
	};

	float			nearDistSqr;	// AC
	float			farDistSqr;		// B0
	float			lastFade;		// B4
	float			currentFade;	// B8	[0.0, 1.0]; Used for fade-in/out
	float			boundRadius;	// BC
	float			timeSinceUpdate;// C0
	UInt32			fadeType;		// C4
	UInt32			frameCounter;	// C8
	TESObjectREFR* linkedObj;		// CC
	UInt32			unkD0[5];		// D0

	__forceinline static BSFadeNode* Create() { return ThisStdCall<BSFadeNode*>(0xB4E150, CdeclCall<void*>(0xAA13E0, sizeof(BSFadeNode))); }

	void __fastcall SetVisible(bool visible);
};
static_assert(sizeof(BSFadeNode) == 0xE4);

// 114
class NiCamera : public NiAVObject
{
public:
	float			worldToCam[4][4];	// 09C
	NiFrustum		frustum;			// 0DC
	float			minNearPlaneDist;	// 0F8
	float			maxFarNearRatio;	// 0FC
	NiViewport		viewPort;			// 100
	float			LODAdjust;			// 110

	__forceinline static NiCamera* Create() { return CdeclCall<NiCamera*>(0xA71430); }

	bool __fastcall WorldToScreen(const NiVector3& worldPos, NiPoint2& scrPos);
};
static_assert(sizeof(NiCamera) == 0x114);

extern NiCamera* g_mainCamera;

// 34
class NiPick
{
public:
	NiPick(NiAVObject* origin)
	{
		ThisStdCall(0xE98F20, this, 0, 8);
		InterlockedIncrement(&origin->m_uiRefCount);
		originObj = origin;
	}
	~NiPick() { ThisStdCall(0xE98FA0, this); }

	// 44
	struct Result
	{
		NiAVObject* result;	// 00
		NiObject* proxyParent;	// 04
		NiVector3		intersect;	// 08
		float			distance;	// 14
		UInt16			triangleIdx;// 18
		UInt16			vertexIdx;	// 1E
		NiPoint2		uvCoords;	// 20
		NiVector3		normal;		// 28
		NiColorAlpha	colour;		// 34
	};

	enum PickType : UInt32
	{
		PICK_ALL,
		PICK_FIRST
	};
	enum SortType : UInt32
	{
		SORT_YES,
		SORT_NO
	};
	enum IntersectType : UInt32
	{
		INTERSECT_BOUND,
		INTERSECT_TRIANGLE
	};
	enum CoordinateType : UInt32
	{
		COORDS_MODEL,
		COORDS_WORLD
	};

	PickType			pickType;		// 00
	SortType			sortType;		// 04
	IntersectType		intersectType;	// 08
	CoordinateType		coordType;		// 0C
	UInt8				bFrontOnly;		// 10
	UInt8				observeCull;	// 11
	UInt8				pad12[2];		// 12
	NiAVObject* originObj;		// 14
	NiTArray<Result*>	results;		// 18
	UInt32				numResults;		// 28
	UInt32				lastAddedRecord;// 2C
	UInt8				retnTexture;	// 30
	UInt8				retnNormal;		// 31
	UInt8				retnSmoothNormal;	// 32
	UInt8				retnColour;		// 33

	bool __fastcall GetResults(NiCamera* camera);
};


class NiDX9ShaderConstantManager;
class NiDX9VertexBufferManager;

class LightingData;

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

enum RenderTargetTypes : UInt32
{
	/*00*/eRTT_HDR_DOWNSAMPLE,
	/*01*/eRTT_HDR_DOWNSAMPLE_ANISOINTERMEDIATE,
	/*02*/eRTT_HDR_BLURSWAP,
	/*03*/eRTT_IMAGESPACE_SWAP,
	/*04*/eRTT_MAIN_IMAGESPACE,
	/*05*/eRTT_MAIN_FIRSTPERSON,
	/*06*/eRTT_MAIN_MENUBG,
	/*07*/eRTT_TESWATER_FFT,
	/*08*/eRTT_TESWATER_GREY,
	/*09*/eRTT_TESWATER_REFLECTION,
	/*0A*/eRTT_TESWATER_LOD_REFLECTION,
	/*0B*/eRTT_TESWATER_REFRACTION,
	/*0C*/eRTT_TESWATER_RAIN,
	/*0D*/eRTT_TESWATER_SCRAMBLEMAP,
	/*0E*/eRTT_TESWATER_IFFT_BUTTERFLYMAP,
	/*0F*/eRTT_TESWATER_HIGH_W,
	/*10*/eRTT_TESWATER_HIGH_AMPLITUDE,
	/*11*/eRTT_TESWATER_DEPTH,
	/*12*/eRTT_TESWATER_NOISE_HEIGHTMAP,
	/*13*/eRTT_TESWATER_NOISE_NORMALMAP,
	/*14*/eRTT_TESWATER_DISPLACEMENT,
	/*15*/eRTT_TESWATER_DEBUG_DISPLAY,
	/*16*/eRTT_TESWATER_BLUR,
	/*17*/eRTT_TREES_CANOPYSHADOWMASK,
	/*18*/eRTT_BLURFULL_BUFFER,
	/*19*/eRTT_BLURFULL_SWAP,
	/*1A*/eRTT_BLUR20_BUFFER,
	/*1B*/eRTT_GETHIT_BUFFER,
	/*1C*/eRTT_LOCALMAP_FINAL,
	/*1D*/eRTT_LOCALMAP_COLOR,
	/*1E*/eRTT_LOCALMAP_NORMALS,
	/*1F*/eRTT_REFRACTION_NORMALS,
	/*20*/eRTT_VFOG,
	/*21*/eRTT_SCENE_DEPTH_BUFFER,
	/*22*/eRTT_VFOG_SWAP,
	/*23*/eRTT_VELOCITY,
	/*24*/eRTT_VELOCITY_SWAP,
	/*25*/eRTT_DOF_BLUR,
	/*26*/eRTT_DOF_BLUR_SWAP,
	/*27*/eRTT_DOF_MASK,
	/*28*/eRTT_DEPTH_RADIALBLUR_BLUR,
	/*29*/eRTT_CUBEMAP_CAMERA,
	/*2A*/eRTT_SHADOWS_SHADOWMAP,
	/*2B*/eRTT_SHADOWS_SHADOWMAPFRUSTUM,
	/*2C*/eRTT_ENVMAP_WINDOWS,
	/*2D*/eRTT_INTERFACE_RENDEREDMENU,
	/*2E*/eRTT_INTERFACE_RENDEREDMENU_SWAP,
	/*2F*/eRTT_INTERFACE_VATS,
	/*30*/eRTT_INTERFACE_TEMP,
	/*31*/eRTT_NOISE_BUFFER,
	/*32*/eRTT_UNK_32,
	/*33*/eRTT_DECAL_OCCLUSION
};

// 40
class BSRenderedTexture : public NiObject	//	0x10AE500
{
public:
	NiRenderTargetGroup* targetGroups[6];	// 08
	NiRenderTargetGroup* targetGroup20;		// 20
	NiObject* object24;			// 24
	NiObject* object28;			// 28
	RenderTargetTypes		rtType;				// 2C
	NiRenderedTexture* textures[4];		// 30
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

// 60
class BSShaderProperty : public NiProperty
{
public:
	/*94*/virtual void		Unk_25(void);
	/*98*/virtual void		Unk_26(UInt32 arg1);
	/*9C*/virtual void		Unk_27(UInt32 arg1);
	/*A0*/virtual void		Unk_28(UInt32 arg1, UInt32 arg2, UInt32 arg3, UInt32 arg4, UInt32 arg5, UInt32 arg6);
	/*A4*/virtual void		Unk_29(UInt32 arg1);
	/*A8*/virtual void		Unk_2A(void);
	/*AC*/virtual void		Unk_2B(UInt32 arg1);
	/*B0*/virtual void		Unk_2C(UInt32 arg1, UInt32 arg2, UInt32 arg3);
	/*B4*/virtual void		Unk_2D(void);
	/*B8*/virtual void		Unk_2E(UInt32 arg1);
	/*BC*/virtual void		Unk_2F(UInt32 arg1, UInt32 arg2);
	/*C0*/virtual void		Unk_30(void);

	enum ShaderType
	{
		kType_Lighting = 1,
		kType_DistantLOD = 2,
		kType_GeometryDecal = 3,
		kType_TallGrass = 4,
		kType_SpeedTreeLeaf = 6,
		kType_PPLighting = 8,
		kType_Hair = 9,
		kType_SpeedTreeBranch = 0xA,
		kType_SpeedTreeBillboard = 0xB,
		kType_Lighting30 = 0xC,
		kType_Sky = 0xD,
		kType_Water = 0xE,
		kType_Bolt = 0xF,
		kType_Particle = 0x11,
		kType_Precipitation = 0x12,
		kType_Tile = 0x13,
		kType_NoLighting = 0x15,
		kType_VolumetricFog = 0x16,
		kType_BloodSplatter = 0x17,
		kType_DistantTree = 0x18
	};

	enum ShaderFlag
	{
		kFlag1_Specular = 1,
		kFlag1_Skinned = 2,
		kFlag1_LowDetail = 4,
		kFlag1_VertexAlpha = 8,
		kFlag1_MotionBlur = 0x10,
		kFlag1_SinglePass = 0x20,
		kFlag1_Empty = 0x40,
		kFlag1_EnvironmentMapping = 0x80,
		kFlag1_AlphaTexture = 0x100,
		kFlag1_ZPrepass = 0x200,
		kFlag1_Facegen = 0x400,
		kFlag1_Parallax = 0x800,
		kFlag1_ModelSpaceNormals = 0x1000,
		kFlag1_NonProjectiveShadows = 0x2000,
		kFlag1_Landscape = 0x4000,
		kFlag1_Refraction = 0x8000,
		kFlag1_FireRefraction = 0x10000,
		kFlag1_EyeEnvironmentMapping = 0x20000,
		kFlag1_Hair = 0x40000,
		kFlag1_DynamicAlpha = 0x80000,
		kFlag1_LocalMapHideSecret = 0x100000,
		kFlag1_WindowEnvironmentMapping = 0x200000,
		kFlag1_TreeBillboard = 0x400000,
		kFlag1_ShadowFrustum = 0x800000,
		kFlag1_MultipleTextures = 0x1000000,
		kFlag1_RemappableTextures = 0x2000000,
		kFlag1_Decal = 0x4000000,
		kFlag1_DynamicDecal = 0x8000000,
		kFlag1_ParallaxOcclusion = 0x10000000,
		kFlag1_ExternalEmittance = 0x20000000,
		kFlag1_Shadowmap = 0x40000000,
		kFlag1_ZbufferTest = 0x80000000,

		kFlag2_ZbufferWrite = 1,
		kFlag2_LODLandscape = 2,
		kFlag2_LODBuilding = 4,
		kFlag2_NoFade = 8,
		kFlag2_RefractionTint = 0x10,
		kFlag2_VertexColors = 0x20,
		kFlag2_1stPerson = 0x40,
		kFlag2_1stLightIsPointLight = 0x80,
		kFlag2_2ndLight = 0x100,
		kFlag2_3rdLight = 0x200,
		kFlag2_VertexLighting = 0x400,
		kFlag2_UniformScale = 0x800,
		kFlag2_FitSlope = 0x1000,
		kFlag2_Billboard = 0x2000,
		kFlag2_NoLODLandBlend = 0x4000,
		kFlag2_EnvmapLightFade = 0x8000,
		kFlag2_Wireframe = 0x10000,
		kFlag2_VatsSelection = 0x20000,
		kFlag2_ShowInLocalMap = 0x40000,
		kFlag2_PremultAlpha = 0x80000,
		kFlag2_SkipNormalMaps = 0x100000,
		kFlag2_AlphaDecal = 0x200000,
		kFlag2_NoTransparencyMultisampling = 0x400000,
		kFlag2_StingerProp = 0x800000
	};

	UInt16				unk18;				// 18
	UInt16				unk1A;				// 1A
	UInt32				shaderType;			// 1C
	UInt32				flags1;				// 20
	UInt32				flags2;				// 24
	float				alpha;				// 28
	float				fadeAlpha;			// 2C
	float				envMapScale;		// 30
	float				LODFade;			// 34
	UInt32				lastRenderPassState;// 38
	BSRenderPassList* renderPassLists[7];// 3C
	UInt32				shaderIndex;		// 58
	float				depthBias;			// 5C
};
static_assert(sizeof(BSShaderProperty) == 0x60);

// 80
class BSShaderNoLightingProperty : public BSShaderProperty
{
public:
	/*C4*/virtual void		Unk_31(void);
	/*C8*/virtual void		Unk_32(void);
	/*CC*/virtual void		Unk_33(void);
	/*D0*/virtual void		Unk_34(void);

	NiTexture* srcTexture;			// 60
	const char* texturePath;			// 64
	UInt16			word68;					// 68
	UInt16			word6A;					// 6A
	UInt32			unk6C;					// 6C
	float			falloffStartAngle;		// 70
	float			falloffStopAngle;		// 74
	float			falloffStartOpacity;	// 78
	float			falloffStopOpacity;		// 7C

	__forceinline static BSShaderNoLightingProperty* Create() { return ThisStdCall<BSShaderNoLightingProperty*>(0xB6FC90, CdeclCall<void*>(0xAA13E0, sizeof(BSShaderNoLightingProperty))); }
};
static_assert(sizeof(BSShaderNoLightingProperty) == 0x80);

// 7C
class BSShaderLightingProperty : public BSShaderProperty
{
public:
	/*C4*/virtual void		Unk_31(void);
	/*C8*/virtual void		Unk_32(void);

	DList<LightingData>			illuminatingLights;	// 60
	float						flt6C;				// 6C
	UInt32						unk70;				// 70
	bool						lightListChanged;	// 74
	UInt8						pad75[3];			// 75
	DList<LightingData>::Node* lastLight;			// 78
};
static_assert(sizeof(BSShaderLightingProperty) == 0x7C);



// 34
class NiTimeController : public NiObject
{
public:
	/*8C*/virtual void		Start(float fTime);
	/*90*/virtual void		Stop();
	/*94*/virtual void		Update(const NiUpdateData& updParams);
	/*98*/virtual void		SetTarget(NiObjectNET* pTarget);
	/*9C*/virtual bool		Unk_27(void);
	/*A0*/virtual void		Unk_28(void);
	/*A4*/virtual float		ComputeScaledTime(float fTime);
	/*A8*/virtual void		Unk_2A(void);
	/*AC*/virtual void		Unk_2B(void);
	/*B0*/virtual void		Unk_2C(void);

	enum TimeCtrlFlags
	{
		kCtrlFlag_AnimType_Pos = 0,
		kCtrlFlag_AnimType_Mask = 1,
		kCtrlFlag_CycleType_Pos = 1,
		kCtrlFlag_CycleType_Mask = 6,
		kCtrlFlag_Active_Mask = 8,
		kCtrlFlag_Direction_Mask = 0x10,
		kCtrlFlag_ManagerControlled_Mask = 0x20,
		kCtrlFlag_ComputeScaledTime_Mask = 0x40,
		kCtrlFlag_ForceUdpate_Mask = 0x80
	};

	UInt16				flags;				// 08
	UInt16				unk0A;				// 0A
	float				frequency;			// 0C
	float				phaseTime;			// 10
	float				lowKeyTime;			// 14
	float				highKeyTime;		// 18
	float				startTime;			// 1C
	float				lastTime;			// 20
	float				weightedLastTime;	// 24
	float				scaledTime;			// 28
	NiObjectNET* target;			// 2C
	NiTimeController* nextCtrl;			// 30
};

template <typename T_Data>
class NiSimpleArray
{
public:
	T_Data* data;
	UInt32		capacity;
	UInt32		numItems;

	class Iterator
	{
		T_Data* pData;
		UInt32		count;

	public:
		explicit operator bool() const { return count != 0; }
		void operator++()
		{
			pData++;
			count--;
		}

		T_Data& operator*() const { return *pData; }
		T_Data& operator->() const { return *pData; }
		T_Data& Get() const { return *pData; }

		Iterator(NiSimpleArray& source) : pData(source.data), count(source.numItems) {}
	};

	Iterator Begin() { return Iterator(*this); }
};

// 7C
class NiControllerManager : public NiTimeController
{
public:
	/*B4*/virtual void		Unk_2D(void);

	NiTArray<NiControllerSequence*>				sequences;		// 34
	NiSimpleArray<NiControllerSequence*>		seqArray44;		// 44
	NiTStringPointerMap<NiControllerSequence*>	seqStrMap;		// 50	Case-sensitive!
	NiTArray<void*>* arr64;			// 64	NiTPrimitiveArray<BSAnimNoteListener::BSAnimReceiverType*>
	UInt32										unk68;			// 68
	UInt32										unk6C;			// 6C
	UInt32										unk70;			// 70
	UInt32										unk74;			// 74
	NiDefaultAVObjectPalette* defObjPlt;		// 78

	NiControllerSequence* FindSequence(const char* seqName);
};
static_assert(sizeof(NiControllerManager) == 0x7C);

// 34
class NiInterpController : public NiTimeController
{
public:
	/*B4*/virtual void		Unk_2D(void);
	/*B8*/virtual void		Unk_2E(void);
	/*BC*/virtual void		Unk_2F(void);
	/*C0*/virtual void		Unk_30(void);
	/*C4*/virtual NiInterpolator* GetInterpolator(UInt32 arg1);
	/*C8*/virtual void		SetInterpolator(NiInterpolator* pInterpolator, UInt32 arg2);
	/*CC*/virtual void		Unk_33(void);
	/*D0*/virtual void		Unk_34(void);
	/*D4*/virtual void		Unk_35(void);
	/*D8*/virtual void		Unk_36(void);
	/*DC*/virtual void		Unk_37(void);
	/*E0*/virtual void		Unk_38(float arg1, float arg2);
	/*E4*/virtual void		Unk_39(void);
};

// 40
class NiMultiTargetTransformController : public NiInterpController
{
public:
	NiBlendTransformInterpolator* blendInterp;	// 34
	NiAVObject** targets;		// 38
	UInt16							numInterps;		// 3C
	UInt8							pad3E[2];		// 3E
};

// 38
class NiSingleInterpController : public NiInterpController
{
public:
	/*E8*/virtual void		Unk_3A(void);

	NiInterpolator* interpolator;		// 34
};

// 38
class NiBoolInterpController : public NiSingleInterpController
{
public:
	/*EC*/virtual void		Unk_3B(void);
};

// 38
class NiVisController : public NiBoolInterpController
{
public:
};

// 38
class NiTransformController : public NiSingleInterpController
{
public:
	static NiTransformController* __stdcall Create(NiObjectNET* pTarget, NiTransformInterpolator* pInterpolator);
};
static_assert(sizeof(NiTransformController) == 0x38);

// 38
class NiFloatInterpController : public NiSingleInterpController
{
public:
	/*EC*/virtual void		Unk_3B(void);
};

// 38
class NiPoint3InterpController : public NiSingleInterpController
{
public:
	/*EC*/virtual void		Unk_3B(void);
};

// 38
class NiLightDimmerController : public NiFloatInterpController
{
public:
};

// 3C
class NiLightColorController : public NiPoint3InterpController
{
public:
	enum TargetColor
	{
		kColor_Diffuse,
		kColor_Ambient
	};

	UInt16			targetColor;	// 38
	UInt8			pad3A[2];		// 3A
};

// 50
class NiTextureTransformController : public NiFloatInterpController
{
public:
	/*F0*/virtual void		Unk_3C(void);

	enum TextureSlot
	{
		kTexSlot_Diffuse,
		kTexSlot_Dark,
		kTexSlot_Detail,
		kTexSlot_Gloss,
		kTexSlot_Glow,
		kTexSlot_Bump,
		kTexSlot_Normal,
		kTexSlot_Parallax,
		kTexSlot_Decal
	};

	enum OperationType
	{
		kOperation_Translate_U,
		kOperation_Translate_V,
		kOperation_Rotate,
		kOperation_Scale_U,
		kOperation_Scale_V
	};

	UInt32			unk38;			// 38
	UInt32			unk3C;			// 3C
	bool			shaderMap;		// 40
	UInt8			pad41[3];		// 41
	UInt32			textureSlot;	// 44
	UInt32			operationType;	// 48
	UInt32			unk4C;			// 4C
};

// 44
class NiGeomMorpherController : public NiInterpController
{
public:
	/*EC*/virtual void		Unk_3B(void);
	/*E8*/virtual void		Unk_3A(void);

	class InterpItem : public NiMemObject
	{
	public:
		NiInterpolator* interp;
		float			weight;
	};

	UInt16			gmcFlags;			// 34
	UInt8			pad36[2];			// 36
	NiMorphData* morphData;			// 38
	InterpItem* interpItems;		// 3C
	bool			needPreDisplay;		// 40
	bool			ignoreBaseTarget;	// 41
	bool			alwaysUpdate;		// 42
	bool			boundCalculated;	// 43
};

// 5C
class bhkBlendController : public NiTimeController
{
public:
	UInt32						unk34;		// 34
	NiTLargeArray<NiVector3>	blendKeys;	// 38
	float						flt50;		// 50
	float						flt54;		// 54
	UInt32						unk5C;		// 58
};
// 0C
class NiExtraData : public NiObject
{
public:
	/*8C*/virtual void		Unk_23(void);
	/*90*/virtual void		Unk_24(void);

	NiFixedString	name;		// 08
};

// 10
class BSXFlags : public NiExtraData
{
public:
	enum BSXFlag
	{
		kBSXFlag_Animated = 1,
		kBSXFlag_Havok = 2,
		kBSXFlag_Ragdoll = 4,
		kBSXFlag_Complex = 8,
		kBSXFlag_Addon = 0x10,
		kBSXFlag_EditorMarker = 0x20,
		kBSXFlag_Dynamic = 0x40,
		kBSXFlag_Articulated = 0x80,
		kBSXFlag_NeedsTransformUpdates = 0x100,
		kBSXFlag_ExternalEmit = 0x200
	};

	UInt32			flags;		// 0C

	__forceinline static BSXFlags* Create() { return CdeclCall<BSXFlags*>(0xC43190); }
};

// 24
class BSBound : public NiExtraData
{
public:
	NiVector3		centre;			// 0C
	NiVector3		dimensions;		// 18
};

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

// 14
class NiTextKeyExtraData : public NiExtraData
{
public:
	struct TextKey
	{
		float			m_fTime;
		NiFixedString	m_kText;
	};

	UInt32		m_uiNumKeys;	// 0C
	TextKey* m_pKeys;		// 10
};

// 1C0
class BSFaceGenAnimationData : public NiExtraData
{
public:
};

// 08
class BSTextureSet : public NiObject
{
public:
	/*8C*/virtual void		Unk_23(void);
	/*90*/virtual void		Unk_24(void);
	/*94*/virtual void		Unk_25(void);
	/*98*/virtual void		Unk_26(void);

	enum TextureSlots
	{
		kSlot_Diffuse,
		kSlot_Normal,
		kSlot_Glow,
		kSlot_Parallax,
		kSlot_CubeMap,
		kSlot_EnvMask
	};
};

// 38
class BSShaderTextureSet : public BSTextureSet
{
public:
	String		textures[6];	// 08
};
// 104
class BSShaderPPLightingProperty : public BSShaderLightingProperty
{
public:
	/*0CC*/virtual void		SetupGeometry(void*);
	/*0D0*/virtual void		CopyProperties(BSShaderPPLightingProperty* from);
	/*0D4*/virtual void		Unk_35(void);
	/*0D8*/virtual void		Unk_36(void);
	/*0DC*/virtual void		PickClampMode(UInt32 mode);
	/*0E0*/virtual UInt32	GetClampMode();
	/*0E4*/virtual void		SetClampMode(UInt32 mode);
	/*0E8*/virtual void		Unk_3A(void);
	/*0EC*/virtual void		Unk_3B(void);
	/*0F0*/virtual void		Unk_3C(void);
	/*0F4*/virtual void		Unk_3D(void);
	/*0F8*/virtual void		Unk_3E(void);
	/*0FC*/virtual void		Unk_3F(void);
	/*100*/virtual void		Unk_40(void);
	/*104*/virtual void		Unk_41(void);
	/*108*/virtual void		Unk_42(void);
	/*10C*/virtual void		Unk_43(void);
	/*110*/virtual void		Unk_44(void);
	/*114*/virtual void		Unk_45(void);
	/*118*/virtual void		Unk_46(void);
	/*11C*/virtual void		Unk_47(void);

	UInt32				unk07C;					// 07C
	UInt32				unk080;					// 080
	NiColorAlpha		hairTint;				// 084
	NiColorAlpha		color094;				// 094
	BSShaderTextureSet* textureSet;			// 0A4
	UInt16				word0A8;				// 0A8
	UInt16				word0AA;				// 0AA
	NiTexture** srcTextures[6];		// 0AC
	void* ptr0C4;				// 0C4
	UInt32				unk0C8;					// 0C8
	void* ptr0CC;				// 0CC
	NiObject* object0D0;				// 0D0
	UInt32				unk0D4;					// 0D4
	UInt32				clampMode;				// 0D8
	NiObject* object0DC;				// 0DC
	float				refractionStrength;		// 0E0
	SInt32				refractionFirePeriod;	// 0E4
	float				parallaxMaxPasses;		// 0E8
	float				parallaxScale;			// 0EC
	NiColorAlpha		emmisiveColor;			// 0F0
	BSRenderPass* depthPass;				// 100
};
static_assert(sizeof(BSShaderPPLightingProperty) == 0x104);

// 150
class WaterShaderProperty : public BSShaderProperty
{
public:
	bool				isWading;			// 060
	UInt8				byte061;			// 061
	bool				renderAtWaterLevel;	// 062
	bool				bDepth;				// 063
	UInt32				unk064;				// 064
	UInt32				unk068;				// 068
	NiPoint2			blendRadius;		// 06C
	float				fogDistanceFar;		// 074
	float				fogDistanceNear;	// 078
	UInt8				byte07C;			// 07C
	UInt8				byte07D;			// 07D
	bool				isUnderwater;		// 07E
	UInt8				useDefaultWater;	// 07F
	UInt8				bReflections;		// 080
	UInt8				bRefractions;		// 081
	UInt8				bTextureCoords;		// 082
	UInt8				byte083;			// 083
	UInt32				stencilMask;		// 084
	NiColorAlpha		shallowColor;		// 088
	NiColorAlpha		deepColor;			// 098
	NiColorAlpha		reflectionColor;	// 0A8
	float				sunPower;			// 0B8
	float				reflectivity;		// 0BC
	float				opacity;			// 0C0
	float				distortion;			// 0C4
	float				flt0C8;				// 0C8
	float				flt0CC;				// 0CC
	float				lightRadius;		// 0D0
	float				lightBrightness;	// 0D4
	float				depthFalloffStart;	// 0D8
	float				depthFalloffEnd;	// 0DC
	float				flt0E0;				// 0E0
	float				flt0E4;				// 0E4
	float				flt0E8;				// 0E8
	float				flt0EC;				// 0EC
	float				flt0F0;				// 0F0
	float				flt0F4;				// 0F4
	float				flt0F8;				// 0F8
	float				flt0FC;				// 0FC
	float				shininess;			// 100
	float				reflectionHDRMult;	// 104
	float				flt108;				// 108
	float				flt10C;				// 10C
	float				flt110;				// 110
	float				flt114;				// 114
	float				waterFresnelTerm;	// 118
	float				noiseScale;			// 11C
	float				fog;				// 120
	float				texScale;			// 124
	UInt32				unk128[3];			// 128
	NiSourceTexture* noiseTexture;		// 134
	BSRenderedTexture* noDepth;			// 138
	BSRenderedTexture* reflections;		// 13C
	BSRenderedTexture* refractions;		// 140
	BSRenderedTexture* depth;				// 144
	BSRenderedTexture* wading;			// 148
	BSRenderPass* stencilPass;		// 14C
};
static_assert(sizeof(WaterShaderProperty) == 0x150);

// 90
class SkyShaderProperty : public BSShaderProperty
{
public:
	/*C4*/virtual void		Unk_31(void);
	/*C8*/virtual void		Unk_32(void);
	/*CC*/virtual void		Unk_33(void);
	/*D0*/virtual void		Unk_34(void);

	NiColorAlpha		color;			// 60
	NiSourceTexture* srcTexture70;	// 70
	String				filePath;		// 74
	UInt32				unk7C;			// 7C
	NiSourceTexture* srcTexture80;	// 80
	float				flt84;			// 84	Maybe texture % for animation?
	UInt32				unk88;			// 88
	UInt32				unk8C;			// 8C
};
static_assert(sizeof(SkyShaderProperty) == 0x90);

// B0
class TileShaderProperty : public BSShaderProperty
{
public:
	NiTexture* srcTexture;	// 60
	NiTexture* alphaTexture;	// 64
	NiColorAlpha		overlayColor;	// 68
	float				alpha;			// 78
	NiPoint2			textureOrigin;	// 7C	X/Y: [0.0, 1.0)
	NiPoint2			texturePercent;	// 84
	UInt32				unk8C;			// 8C
	UInt8				byte90;			// 90
	bool				textureMorph;	// 91	Applies textureOrigin & texturePercent
	bool				hasVtxColors;	// 92
	UInt8				byte93;			// 93
	void* ptr94;			// 94
	UInt16				word98;			// 98
	UInt16				word9A;			// 9A
	int					minX;			// 9C	min/max values used for clipping
	int					minY;			// A0
	int					maxX;			// A4
	int					maxY;			// A8
	UInt8				byteAC;			// AC
	UInt8				padAD[3];		// AD
};
static_assert(sizeof(TileShaderProperty) == 0xB0);

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

// 0C
class NiAccumulator : public NiObject
{
public:
	/*8C*/virtual void		SetCamera(NiCamera* _camera);
	/*90*/virtual void		Unk_24(void);
	/*94*/virtual void		Unk_25(void);
	/*98*/virtual void		Unk_26(UInt32 arg);
	/*9C*/virtual void		Unk_27(void);
	/*A0*/virtual void		Unk_28(void);

	NiCamera* camera;		// 08
};

// 2C
class NiBackToFrontAccumulator : public NiAccumulator
{
public:
	/*A4*/virtual void		Unk_29(void);

	DList<void>		list0C;			// 0C
	UInt32			unk18[5];		// 18
};

// 34
class NiAlphaAccumulator : public NiBackToFrontAccumulator
{
public:
	UInt32			unk2C;			// 2C
	UInt8			byte30;			// 30
	UInt8			byte31;			// 31
	UInt8			byte32;			// 32
	UInt8			pad33;			// 33
};

// 98
class BSBatchRenderer : public NiObject
{
public:
	/*8C*/virtual void		Unk_23(void);
	/*90*/virtual void		Unk_24(void);

	UInt32			unk08[36];		// 08
};
static_assert(sizeof(BSBatchRenderer) == 0x98);

enum RenderMode : UInt32
{
	kRndrMode_Normal,		//	Renders everything
	kRndrMode_Unk1,
	kRndrMode_Unk2,
	kRndrMode_Unk3,
	kRndrMode_Unk4,
	kRndrMode_Unk5,
	kRndrMode_Unk6,
	kRndrMode_DepthMap,
	kRndrMode_ConstAlpha,	//	Everything is green
	kRndrMode_LocalMap,		//	Renders vertex normals, skips landscape
	kRndrMode_Tiles,		//	Renders only UI tiles
	kRndrMode_UnkB,
	kRndrMode_NoWaterPass,	//	Skips water rendering
	kRndrMode_UnkD,
	kRndrMode_WaterDepth,
	kRndrMode_Silhouette	//	Everything is black except for grass, sky and emissives
};

// 280	vtbl @ 0x10ADFF8
class BSShaderAccumulator : public NiAlphaAccumulator
{
public:
	/*A8*/virtual void		Unk_2A(void);
	/*AC*/virtual void		Unk_2B(void);
	/*B0*/virtual void		Unk_2C(UInt32 arg1, UInt32 arg2);

	// 14
	struct Struct48
	{
		UInt32		unk00;
		UInt8		byte04;
		UInt8		pad05[3];
		float		flt08;
		UInt32		unk0C;
		UInt32		unk10;
	};

	struct AccumStruct	//	Temp name
	{
		void* _vtbl;	// 0x10B7DC0
		UInt32		unk04;
		UInt32		unk08;
		UInt32		unk0C;
		UInt32		unk10;
	};

	UInt32					unk034;				// 034
	bool					wireFrameRelated;	// 038
	UInt8					byte039;			// 039
	bool					renderDOF;			// 03A
	UInt8					byte03B;			// 03B
	UInt32					sunPixelCount;		// 03C
	bool					waitingForSunQuery;	// 040
	UInt8					pad041[3];			// 041
	float					percentSunOccludedStored;	// 044
	Struct48				unk048;				// 048
	Struct48				unk05C;				// 05C
	Struct48				unk070;				// 070
	bool					processLights;		// 084
	UInt8					byte085;			// 085
	UInt8					byte086;			// 086
	UInt8					byte087;			// 087
	UInt16					word088;			// 088
	UInt16					word08A;			// 08A
	void* ptr08C;			// 08C	Array of 200 DWORDS, or pointers
	UInt32					unk090[3];			// 090
	AccumStruct				accum09C;			// 09C
	AccumStruct				accum0B0;			// 0B0
	AccumStruct				accum0C4;			// 0C4
	AccumStruct				accum0D8;			// 0D8
	AccumStruct				accum0EC;			// 0EC
	AccumStruct				accum100;			// 100
	AccumStruct				accum114;			// 114
	AccumStruct				accum128;			// 128
	UInt32					unk13C[5];			// 13C
	UInt8					byte150;			// 150
	UInt8					pad151[3];			// 151
	NiColorAlpha			ambientColor;		// 154
	bool					ignoreWater;		// 164
	bool					renderWaterDepth;	// 165
	UInt8					byte166;			// 166
	bool					isUnderwater;		// 167
	UInt32					waterHeight;		// 168
	bool					renderDecals;		// 16C
	UInt8					pad16D[3];			// 16D
	void* ptr170;			// 170
	BSBatchRenderer* renderer174;		// 174
	BSBatchRenderer* renderer178;		// 178
	UInt32* passes;			// 17C
	void* ptr180;			// 180
	void* ptr184;			// 184
	void* ptr188;			// 188
	UInt32					batchRendererCount;	// 18C
	UInt32					maxPassCount;		// 190
	ShadowSceneNode* shadowScene;		// 194
	UInt32					unk198;				// 198
	RenderMode				renderMode;			// 19C	Index of a callback in 2 arrays @ 0x11F9F40 & 0x11F9F80
	LightingData* depthLight;		// 1A0
	BSRenderPass* tileRenderPass;	// 1A4
	UInt16					renderPassType;		// 1A8
	UInt8					pad1AA[2];			// 1AA
	UInt32					unk1A0[2];			// 1AC
	NiTPtrMap<void*>		map1B4;				// 1B4
	UInt32					unk1C4[47];			// 1C4
};
static_assert(sizeof(BSShaderAccumulator) == 0x280);

// 24
class NiDX92DBufferData : public NiRefObject
{
public:
	Ni2DBuffer* buffer;		// 08
	void* ptr0C;			// 0C
	UInt32					unk10;			// 10
	void* ptr14;			// 14
	UInt32					unk18;			// 18
	UInt32					unk1C;			// 1C
	IDirect3DBaseTexture9* d3dBaseTexture;// 20
};

// 14
class Ni2DBuffer : public NiObject
{
public:
	/*8C*/virtual void		Unk_23(void);
	/*90*/virtual void		Unk_24(void);
	/*94*/virtual void		Unk_25(void);

	UInt32				width;			// 08
	UInt32				height;			// 0C
	NiDX92DBufferData* bufferData;	// 10
};

class NiDepthStencilBuffer : public Ni2DBuffer {};
assert(sizeof(NiDepthStencilBuffer) == 0x014);

// 28
class NiRenderTargetGroup : public NiObject
{
public:
	/*8C*/virtual void		Unk_23(void);
	/*90*/virtual void		Unk_24(void);
	/*94*/virtual void		Unk_25(void);
	/*98*/virtual void		Unk_26(void);
	/*9C*/virtual void		Unk_27(void);
	/*A0*/virtual void		Unk_28(void);
	/*A4*/virtual void		Unk_29(void);
	/*A8*/virtual void		Unk_2A(void);
	/*AC*/virtual void		Unk_2B(void);
	/*B0*/virtual void		Unk_2C(void);
	/*B4*/virtual void		Unk_2D(void);
	/*B8*/virtual void		Unk_2E(void);
	/*BC*/virtual void		Unk_2F(void);
	/*C0*/virtual void		Unk_30(void);
	/*C4*/virtual void		Unk_31(void);
	/*C8*/virtual void		Unk_32(void);
	/*CC*/virtual void		Unk_33(void);

	class RendererData;

	UInt8					byte08;			// 08
	UInt8					pad09[3];		// 09
	Ni2DBuffer* buffers[4];	// 0C
	UInt32					numBuffers;		// 1C
	NiDepthStencilBuffer* stencilBuffer;	// 20
	RendererData* rendererData;	// 24
};


struct NiVBDynamicSet;
struct NiVBChip;

// 0C
class NiGeometryGroup
{
public:
	/*00*/virtual void	Purge();
	/*04*/virtual void	Unk_01(void* arg);
	/*08*/virtual void	Unk_02(UInt32 arg1, UInt32 arg2, UInt32 arg3);
	/*0C*/virtual void	Unk_03(UInt32 arg);
	/*10*/virtual void	Unk_04(UInt32 arg);
	/*14*/virtual void	Unk_05(UInt32 arg);
	/*18*/virtual void	Unk_06(UInt32 arg1, UInt32 arg2);
	/*1C*/virtual void	Unk_07(UInt32 arg1, UInt32 arg2);
	/*20*/virtual bool	IsDynamic();
	/*24*/virtual void	Destroy(bool doFree);

	UInt32			m_uiRefCount;		// 04
	void* pD3DDevice;		// 08
};

// 1C
class NiStaticGeometryGroup : public NiGeometryGroup
{
public:
	struct NiVBSet;

	NiTPtrMap<NiVBSet>		map0C;		// 0C
};

// 0C
class NiUnsharedGeometryGroup : public NiGeometryGroup
{
public:
};

// 50
class NiDynamicGeometryGroup : public NiGeometryGroup
{
public:
	NiTPtrMap<NiVBDynamicSet>		map0C;		// 0C
	NiTPtrMap<NiVBChip>				map1C;		// 1C
	NiTArray<NiVBDynamicSet*>		array2C;	// 2C
	NiTArray<NiVBChip*>				array3C;	// 3C
	UInt32							unk4C;		// 4C
};
static_assert(sizeof(NiDynamicGeometryGroup) == 0x50);

// 2C
class NiAdditionalGeometryData : public NiObject
{
public:
	// 10
	class NiAGDDataBlock
	{
	public:
		virtual void	Destructor(bool doFree);
		virtual void* AllocBlock(UInt32 size);
		virtual void	Unk_02(UInt32, UInt32);
		virtual void	Unk_03(UInt32, UInt32);
		virtual void	FreeBlock(void* pBlock);

		UInt32			unk04;		// 04
		void* block08;	// 08
		UInt8			byte0C;		// 0C
		UInt8			pad0D[3];	// 0D
	};

	/*8C*/virtual bool		IsPacked();
	/*90*/virtual NiAGDDataBlock* AllocDataBlock();

	UInt32						unk08;		// 08
	UInt16						word0C;		// 0C
	UInt8						pad0E[2];	// 0E
	UInt32						unk10;		// 10
	void* ptr14;		// 14
	UInt32						unk18;		// 18
	NiTArray<NiAGDDataBlock*>	dataBlocks;	// 1C
};
static_assert(sizeof(NiAdditionalGeometryData) == 0x2C);


// 54	vtbl @ 0x010F017C
class NiGeometryBufferData
{
public:
	/*00*/virtual void		Destructor(bool doFree);
	/*04*/virtual bool		Unk_01(UInt32 arg1);

	UInt32					flags;			// 04
	NiGeometryGroup* geomGroup;		// 08
	UInt32					fvf;			// 0C
	void* declaration;	// 10
	bool					softwareVP;		// 14
	UInt8					pad15[3];		// 15
	UInt32					vertexCount;	// 18
	UInt32					maxVertCount;	// 1C
	UInt32					streamCount;	// 20
	void* vertexStride;	// 24
	NiVBChip** pVBChip;		// 28
	UInt32					trianglePoints;	// 2C
	UInt32					trigPtSize;		// 30	Byte size of triangles array
	void* ptr34;			// 34
	UInt32					baseVertexIdx;	// 38
	D3DPRIMITIVETYPE		primitiveType;	// 3C
	UInt32					triangleCount;	// 40
	UInt32					maxTriCount;	// 44
	UInt32					numArrays;		// 48
	UInt16* indexArray;	// 4C
	NiTriangle* triangles;		// 50	Same ptr as in NiTriShapeData
};
static_assert(sizeof(NiGeometryBufferData) == 0x54);

// 40
class NiGeometryData : public NiObject
{
public:
	/*8C*/virtual void		SetVertexCount(UInt16 vtxCount);
	/*90*/virtual UInt16	GetVertexCount();
	/*94*/virtual NiTriStripsData* GetStripsData();
	/*98*/virtual NiTriShapeData* GetShapeData();
	/*9C*/virtual bool		ContainsDataType(UInt32 dataType);
	/*A0*/virtual void		CalculateNormals();

	enum KeepFlag
	{
		eKeep_Vertices = 1,
		eKeep_Normals = 2,
		eKeep_VertexColor = 4,
		eKeep_UVCoords = 8,
		eKeep_Indices = 0x10,
		eKeep_BoneData = 0x20,
		eKeep_All = eKeep_Vertices | eKeep_Normals | eKeep_VertexColor | eKeep_UVCoords | eKeep_Indices | eKeep_BoneData
	};

	enum CompressFlag
	{
		eCompress_Normals = 1,
		eCompress_Color = 2,
		eCompress_UV = 4,
		eCompress_Weight = 8,
		eCompress_Position = 0x10,
		eCompress_All = 0x1F
	};

	UInt16						numVertices;	// 08
	UInt16						id;				// 0A
	UInt16						dataFlags;		// 0C	NormalBinormalTangent
	UInt16						dirtyFlags;		// 0E
	NiBound						bounds;			// 10
	NiVector3* vertices;		// 20
	NiVector3* normals;		// 24
	NiColorAlpha* vertexColors;	// 28
	NiPoint2* uvCoords;		// 2C
	NiAdditionalGeometryData* additionalData;// 30
	NiGeometryBufferData* bufferData;	// 34
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

// 06
struct NiTriangle
{
	UInt16		point1;
	UInt16		point2;
	UInt16		point3;

	NiTriangle(UInt16 pt1, UInt16 pt2, UInt16 pt3) : point1(pt1), point2(pt2), point3(pt3) {}

	void Dump() const;
};

template <typename T> struct NiRect
{
	T	left, right, top, bottom;

	NiRect() {}
	NiRect(T _left, T _right, T _top, T _bottom) : left(_left), right(_right), top(_top), bottom(_bottom) {}
};
// C4
class NiGeometry : public NiAVObject
{
public:
	/*DC*/virtual void		RenderImmediate(NiRenderer* pRenderer);
	/*E0*/virtual void		RenderImmediateAlt(NiRenderer* pRenderer);
	/*E4*/virtual void		SetGeometryData(NiGeometryData* pkModelData);
	/*E8*/virtual void		CalculateNormals();
	/*EC*/virtual void		CalculateConsistency(UInt8 arg1);

	NiAlphaProperty* alphaProp;		// 9C
	NiCullingProperty* cullingProp;	// A0
	NiMaterialProperty* materialProp;	// A4
	BSShaderProperty* shaderProp;	// A8
	NiStencilProperty* stencilProp;	// AC
	NiTexturingProperty* texturingProp;	// B0
	UInt32					unkB4;			// B4
	NiGeometryData* geometryData;	// B8
	NiSkinInstance* skinInstance;	// BC
	NiShader* shader;		// C0

	GeometryProperties* GetProperties() const { return (GeometryProperties*)&alphaProp; }

	NiProperty* GetProperty(UInt32 propID) const
	{
		return ((NiProperty**)&alphaProp)[propID];
	}

	void __fastcall AddProperty(NiProperty* niProperty);
};
static_assert(sizeof(NiGeometry) == 0xC4);

// C4
class NiTriBasedGeom : public NiGeometry
{
public:
	/*F0*/virtual void		Unk_3C(UInt32 arg1, UInt32 arg2, UInt32 arg3, UInt32 arg4);
};

// C4
class NiTriShape : public NiTriBasedGeom
{
public:
	static NiTriShape* Create(NiTriShapeData* pShapeData) { return ThisStdCall<NiTriShape*>(0xA74480, Ni_Alloc<NiTriShape>(), pShapeData); }
};

// C4
class NiTriStrips : public NiTriBasedGeom
{
public:
	static NiTriStrips* Create(NiTriStripsData* pShapeData) { return ThisStdCall<NiTriStrips*>(0xA71CC0, Ni_Alloc<NiTriStrips>(), pShapeData); }
};

// D8
class BSResizableTriShape : public NiTriShape
{
public:
	/*F4*/virtual void		Unk_3D();
	/*F8*/virtual void		Unk_3E();

	UInt32		unkC4[5];		// C4
};

// D0
class BSSegmentedTriShape : public NiTriShape
{
public:
	// 14
	struct Segment
	{
		UInt32		startIndex;
		UInt32		numPrimitives;
		UInt32		unk08;
		UInt32		unk0C;
		UInt32		unk10;
	};

	Segment* segments;		// C4
	UInt32			numSegments;	// C8
	UInt8			byteCC;			// CC
	UInt8			padCD[3];		// CD
};


// D4
class BSScissorTriShape : public NiTriShape
{
public:
	RECT		scissorRect;	// C4
};
static_assert(sizeof(BSScissorTriShape) == 0xD4);

// C4
class NiScreenElements : public NiTriShape
{
public:
};

// C4
class NiLines : public NiGeometry
{
public:
	static NiLines* __stdcall Create(float length, const NiColorAlpha& color, const char* objName);
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
		return ThisStdCall<NiTriShapeData*>(0xA7B630, Ni_Alloc<NiTriShapeData>(), numVertices, vertices, normals, vertexColor, uvCoords, 0, 0, numTriangles, triangles);
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
		return ThisStdCall<NiTriStripsData*>(0xA75DC0, Ni_Alloc<NiTriStripsData>(), numVertices, vertices, normals, vertexColor,
			uvCoords, 0, 0, numTriangles, numStrips, stripLengths, strips);
	}

	static NiTriStripsData* __vectorcall DrawConvex(float radius, UInt32 numEdges, const NiColorAlpha& color);
	static NiTriStripsData* __vectorcall DrawCylinder(float radius, float height, UInt32 numEdges, const NiColorAlpha& color);
	static NiTriStripsData* __vectorcall DrawPrism(float radius, float height, UInt32 numEdges, const NiColorAlpha& color);
};

// 44
class NiLinesData : public NiGeometryData
{
public:
	UInt8* vertexConnectFlags;	// 40
};

class NiSkinPartition : public NiObject {
public:
	class Partition {
	public:
		void** _vtbl;			// 00
		UInt16* pBones;			// 04
		float* Weights;		// 08
		UInt32					Unk04[4];		// 0C
		UInt16					Vertices;		// 1C
		UInt16					Triangles;		// 1E
		UInt16					Bones;			// 20
		UInt16					Strips;			// 22
		UInt16					BonesPerVertex; // 24
		UInt16					pad28;
		NiGeometryBufferData* BuffData;		// 28
	};

	UInt32		PartitionsCount;		// 08
	Partition* Partitions;				// 0C
};
assert(sizeof(NiSkinPartition) == 0x10);

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
		BoneVertData* BoneVertData;	// 44
		UInt16			Verts;			// 48
		UInt16			pad44;
	};

	NiSkinPartition* SkinPartition;		// 08
	NiTransform			RootParentToSkin;	// 0C
	BoneData* BoneData;			// 40
	UInt32				Bones;				// 44
};
assert(sizeof(NiSkinData) == 0x48);

class NiSkinInstance : public NiObject {
public:
	NiSkinData* SkinData;				// 08
	NiSkinPartition* SkinPartition;			// 0C
	NiAVObject* RootParent;				// 10
	NiAVObject** BoneObjects;			// 14
	UInt32				FrameID;				// 18
	UInt32				Bones;					// 1C
	UInt32				BoneRegisters;			// 20
	UInt32				BoneSize;				// 24
	void* BoneMatrixes;			// 28 Matrixes array for the bones D3DMATRIX
	void* SkinToWorldWorldToSkin;	// 2C D3DMATRIX
	UInt32				Unk30;					// 30

	bool IsPartitionEnabled(UInt32 partitionIndex);

};
assert(sizeof(NiSkinInstance) == 0x34);

class DismemberPartition {
public:
	UInt8 Enabled;
	UInt8 StartCap;  //Questionable, but it's the only way I can make sense of that code
	UInt16 bodyPart;
};
assert(sizeof(DismemberPartition) == 4);

class BSDismemberSkinInstance : public NiSkinInstance {
public:
	UInt32 partitionNumber;
	DismemberPartition* partitions;
	UInt8  IsRenderable;  //In Load this is made in OR with every partition->Enabled flag
	UInt8  pad[3];
};
assert(sizeof(BSDismemberSkinInstance) == 0x40);


extern const NiUpdateData kNiUpdateData;

// 4C
class NiMaterialProperty : public NiProperty
{
public:
	UInt32				m_iIndex;		// 18
	NiColor				specularRGB;	// 1C
	NiColor				emissiveRGB;	// 28
	UInt32				isExternalEmit;	// 34
	float				glossiness;		// 38
	float				alpha;			// 3C
	float				emitMult;		// 40
	UInt32				m_uiRevID;		// 44
	UInt32				unk48;			// 48

	__forceinline static NiMaterialProperty* Create() { return CdeclCall<NiMaterialProperty*>(0xA756D0); }
	void __vectorcall SetTraitValue(UInt32 traitID, float value);
};

// 1C
class NiAlphaProperty : public NiProperty
{
public:
	enum AlphaFlag
	{
		kFlag_EnableBlending = 1 << 0,
		kFlag_SourceInverse = 1 << 1,
		kFlag_SourceSrcColor = 1 << 2,
		kFlag_SourceDestColor = 1 << 3,
		kFlag_SourceSrcAlpha = kFlag_SourceSrcColor | kFlag_SourceDestColor,
		kFlag_SourceDestAlpha = 1 << 4,
		kFlag_SourceAlphaSaturate = kFlag_SourceSrcColor | kFlag_SourceDestAlpha,
		kFlag_DestinInverse = 1 << 5,
		kFlag_DestinSrcColor = 1 << 6,
		kFlag_DestinDestColor = 1 << 7,
		kFlag_DestinSrcAlpha = kFlag_DestinSrcColor | kFlag_DestinDestColor,
		kFlag_DestinDestAlpha = 1 << 8,
		kFlag_DestinAlphaSaturate = kFlag_DestinSrcColor | kFlag_DestinDestAlpha,
		kFlag_EnableTesting = 1 << 9,
		kFlag_TestFuncLess = 1 << 10,
		kFlag_TestFuncEqual = 1 << 11,
		kFlag_TestFuncLessOrEq = kFlag_TestFuncLess | kFlag_TestFuncEqual,
		kFlag_TestFuncGreater = 1 << 12,
		kFlag_TestFuncNotEqual = kFlag_TestFuncLess | kFlag_TestFuncGreater,
		kFlag_TestFuncGrtOrEq = kFlag_TestFuncEqual | kFlag_TestFuncGreater,
		kFlag_TestFuncNever = kFlag_TestFuncLess | kFlag_TestFuncEqual | kFlag_TestFuncGreater,
		kFlag_NoSorter = 1 << 13,
	};

	UInt16				flags;		// 18
	UInt8				threshold;	// 1A
	UInt8				byte1B;		// 1B

	__forceinline static NiAlphaProperty* Create() { return CdeclCall<NiAlphaProperty*>(0xA5CEB0); }
};
extern NiAlphaProperty* s_alphaProperty;

// 48
struct TextureTransform
{
	NiPoint2		translation;	// 00
	float			rotation;		// 08
	NiPoint2		scale;			// 0C
	NiPoint2		centre;			// 14
	UInt8			byte1C;			// 1C
	UInt8			pad1D[3];		// 1D
	NiMatrix33		matrix20;		// 20
	UInt32			unk44;			// 44
};
static_assert(sizeof(TextureTransform) == 0x48);

// 30
class NiTexturingProperty : public NiProperty
{
public:
	class Map
	{
	public:
		virtual void	Destroy(bool doFree);
		virtual void	Unk_01(void);
		virtual void	Unk_02(void);
		virtual void	Unk_03(void);

		Map* baseMap;		// 04
		NiSourceTexture* srcTexture;	// 08
		TextureTransform* transform;		// 0C
	};

	class ShaderMap : public Map
	{
	public:
		UInt32				mapID;		// 10
	};

	UInt16					flags;				// 18
	UInt8					pad1A[2];			// 1A
	NiTArray<Map*>			textures;			// 1C
	NiTArray<ShaderMap*>* shaderTextures;	// 2C
};
static_assert(sizeof(NiTexturingProperty) == 0x30);

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

enum RendererClearFlag
{
	kClrFlag_BackBuffer = 1,
	kClrFlag_Stencil = 2,
	kClrFlag_ZBuffer = 4,
	kClrFlag_All = kClrFlag_BackBuffer | kClrFlag_Stencil | kClrFlag_ZBuffer
};

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
assert(sizeof(NiRenderer) == 0x280);

// 18
class NiPSysModifier : public NiObject
{
public:
	/*8C*/virtual void		Unk_23(void);
	/*90*/virtual void		Unk_24(void);
	/*94*/virtual void		Unk_25(void);
	/*98*/virtual void		Unk_26(void);
	/*9C*/virtual void		Unk_27(void);
	/*A0*/virtual void		Unk_28(void);

	NiFixedString		name;			// 08
	UInt32				unk0C;			// 0C
	NiParticleSystem* particleSys;	// 10
	UInt8				byte14;			// 14
	UInt8				pad15[3];		// 15
};
static_assert(sizeof(NiPSysModifier) == 0x18);

// 54
class NiPSysEmitter : public NiPSysModifier
{
public:
	/*A4*/virtual void		Unk_29(void);
	/*A8*/virtual void		Unk_2A(void);

	float			speed;					// 18
	float			speedVariation;			// 1C
	float			declination;			// 20
	float			declinationVariation;	// 24
	float			planarAngle;			// 28
	float			planarAngleVariation;	// 2C
	NiColorAlpha	initialColor;			// 30
	float			initialRadius;			// 40
	float			radiusVariation;		// 44
	float			lifespan;				// 48
	float			lifespanVariation;		// 4C
	float			flt50;					// 50
};
static_assert(sizeof(NiPSysEmitter) == 0x54);

// 58
class NiPSysVolumeEmitter : public NiPSysEmitter
{
public:
	/*AC*/virtual void		Unk_2B(void);
	/*B0*/virtual void		Unk_2C(void);

	NiNode* emitterObj;	// 54
};

// 64
class NiPSysBoxEmitter : public NiPSysVolumeEmitter
{
public:
	NiVector3		dimensions;	// 58	width, height, depth
};

// 5C
class NiPSysSphereEmitter : public NiPSysVolumeEmitter
{
public:
	float			radius;		// 58
};

// 60
class NiPSysCylinderEmitter : public NiPSysVolumeEmitter
{
public:
	float		flt58;		// 58
	float		flt5C;		// 5C
};

// 88
class NiPSysMeshEmitter : public NiPSysEmitter
{
public:
	// 0C
	class NiSkinnedEmitterData : public NiRefObject
	{
	public:
		UInt32			unk08;
	};

	NiTArray<NiGeometry*>			geometryArr;	// 54
	NiTArray<NiSkinnedEmitterData*>	skinnedDataArr;	// 64
	UInt32							unk74;			// 74
	UInt32							unk78;			// 78
	NiVector3						vec7C;			// 7C
};

// C4
class NiParticles : public NiGeometry
{
public:
	/*F0*/virtual void		Unk_3C(void);
};

// 110
class NiParticleSystem : public NiParticles
{
public:
	/*F4*/virtual void		Unk_3D(void);

	UInt8					byte0C4;	// 0C4
	UInt8					pad0C5[3];	// 0C5
	DList<NiPSysModifier>	modifiers;	// 0C8
	float					flt0D4;		// 0D4
	UInt8					byte0D8;	// 0D8
	UInt8					byte0D9;	// 0D9
	UInt8					pad0DA[2];	// 0DA
	UInt32					unk0DC[13];	// 0DC

	__forceinline static NiParticleSystem* Create() { return CdeclCall<NiParticleSystem*>(0xC1B7F0); }

	NiPSysModifier* __fastcall FindModifier(UInt32 typeVtbl) const;
};
static_assert(sizeof(NiParticleSystem) == 0x110);

// 14C
class ParticleShaderProperty : public BSShaderProperty
{
public:
	void* ptr060;	// 060
	UInt32					unk064[2];	// 064
	UInt8					byte06C;	// 06C
	UInt8					pad06D[3];	// 06D
	UInt32					unk070;		// 070
	float					flt074[7];	// 074
	UInt32					unk090[6];	// 090
	float					flt0A8[4];	// 0A8
	UInt32					unk0B8[3];	// 0B8
	float					flt0C4[12];	// 0C4
	UInt32					unk0F4[4];	// 0F4
	float					flt104;		// 104
	UInt32					unk108[5];	// 108
	NiTArray<NiAVObject*>	array11C;	// 11C
	UInt32					unk12C;		// 12C
	float					flt130[3];	// 130
	UInt32					unk13C[3];	// 13C
	float					flt148;		// 148
};
static_assert(sizeof(ParticleShaderProperty) == 0x14C);


// 2C
class NiFogProperty : public NiProperty
{
public:
	UInt16			flags;			// 18
	UInt16			unk1A;			// 1A
	float			depth;			// 1C
	NiColor			color;			// 20
};

// 64
class BSFogProperty : public NiFogProperty
{
public:
	float			startDistance;	// 2C
	float			endDistance;	// 30
	float			waterStartDist;	// 34
	float			waterEndDist;	// 38
	NiPlane			plane3C;		// 3C
	float			falloff;		// 4C
	float			height;			// 50
	NiColor			waterColor;		// 54
	float			power;			// 60
};
static_assert(sizeof(BSFogProperty) == 0x64);

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
	virtual IDirect3DPixelShader9* GetPixelShader();																				// 20
	virtual	void							RestorePixelShader();																			// 21
	virtual void							RemovePixelShader(IDirect3DPixelShader9* PixelShader);											// 22
	virtual void							SetVertexShader(IDirect3DVertexShader9* pShader, UInt8 BackUp);									// 23
	virtual IDirect3DVertexShader9* GetVertexShader();																				// 24
	virtual void							RestoreVertexShader();																			// 25
	virtual void							RemoveVertexShader(IDirect3DVertexShader9* pShader);											// 26
	virtual void							SetFVF(UInt32 FVF, UInt8 BackUp);																// 27
	virtual UInt32							GetFVF();																						// 28
	virtual void							RestoreFVF();																					// 29
	virtual void							ClearFVF(UInt32 FVF);																			// 2A
	virtual void							SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl, UInt8 BackUp);							// 2B
	virtual IDirect3DVertexDeclaration9* GetVertexDeclaration();																			// 2C
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
	virtual IDirect3DBaseTexture9* GetTexture(UInt32 Sampler);																		// 38
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
	NiAlphaProperty* DisabledAlphaProperty;			// 0074
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
	NiDX9ShaderConstantManager* ShaderConstantManager;			// 10F0
	UInt8							ForceNormalizeNormals;			// 10F4
	UInt8							InternalNormalizeNormals;		// 10F5
	UInt8							UsingSoftwareVP;				// 10F6
	UInt8							Declaration;					// 10F7
	IDirect3DDevice9* Device;							// 10F8
	NiDX9Renderer* Renderer;						// 10FC
	UInt32							unk1000[(0x1018 - 0x1000) >> 2];// 1100
	D3DCAPS9						Caps;							// 1118
};
assert(offsetof(NiDX9RenderState, Device) == 0x10F8);
assert(sizeof(NiDX9RenderState) == 0x1248);
class NiDX9AdapterDesc;
class NiDX9DeviceDesc;



class NiD3DShaderDeclaration : public NiObject {
public:
	NiDX9Renderer* Renderer;		// 008
	NiDX9VertexBufferManager* BufferManager;	// 00C
	IDirect3DDevice9* Device;			// 010
	UInt32						Unk014;			// 014
	UInt32						Unk018;			// 018
	UInt32						Unk01C;			// 01C
	UInt32						StreamCount;	// 020
	UInt32						Unk024;			// 024
	UInt8						Unk028;			// 028
	UInt8						Unk029[3];		// 029

};
assert(sizeof(NiD3DShaderDeclaration) == 0x02C);

class NiDX9ShaderDeclaration : public NiD3DShaderDeclaration {
public:
	UInt32		Unk02C;			// 02C
	UInt32		Unk030;			// 030
	UInt8		Unk034;			// 034
	UInt8		Unk035[3];		// 035

};
assert(sizeof(NiDX9ShaderDeclaration) == 0x038);


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
	const char* const		Name;				// 0C
	ShaderRecord* ShaderProg[3];
	IUnknown* ShaderHandleBackup;
	IDirect3DDevice9* Device;				// 20
	NiDX9Renderer* Renderer;			// 24
	NiDX9RenderState* RenderState;		// 28

	ShaderRecord* GetShaderRecord(int Type) {
		return ShaderProg[Type];
	};

	void SetName(const char* name) {
		ThisStdCall(0xBE0920, this, name);
	}
};
assert(sizeof(NiD3DShaderProgram) == 0x2C);

class NiD3DVertexShader : public NiD3DShaderProgram {
public:
	UInt8							SoftwareVP;		// 2C
	UInt8							pad[3];			// 2D
	UInt32							Usage;			// 30
	IDirect3DVertexShader9* ShaderHandle;	// 34
	IDirect3DVertexDeclaration9* Declaration;	// 38
};
assert(sizeof(NiD3DVertexShader) == 0x3C);

class NiD3DPixelShader : public NiD3DShaderProgram {
public:
	IDirect3DPixelShader9* ShaderHandle;	// 2C
};
assert(sizeof(NiD3DPixelShader) == 0x30);

class NiD3DTextureStage;
class NiD3DShaderConstantMap;
class NiD3DRenderStateGroup;

class NiD3DPass {
public:
	void** _vtbl;						// 00
	char							Name[16];					// 04
	UInt32							CurrentStage;				// 14
	UInt32							StageCount;					// 18
	UInt32							TexturesPerPass;			// 1C
	NiTArray<NiD3DTextureStage*>	Stages;						// 20
	NiD3DRenderStateGroup* RenderStateGroup;			// 30
	NiD3DShaderConstantMap* PixelConstantMap;			// 34
	char* PixelShaderProgramFile;		// 38
	char* PixelShaderEntryPoint;		// 3C
	char* PixelShaderTarget;			// 40
	NiD3DPixelShader* PixelShader;				// 44
	NiD3DShaderConstantMap* VertexConstantMap;			// 48
	UInt8							Unk04C;						// 4C
	UInt8							pad04C[3];
	char* VertexShaderProgramFile;	// 50
	char* VertexShaderEntryPoint;		// 54
	char* VertexShaderTarget;			// 58
	NiD3DVertexShader* VertexShader;				// 5C
	UInt8							SoftwareVP;					// 60
	UInt8							RendererOwned;				// 61
	UInt8							pad[2];
	UInt32							RefCount;					// 64
};
assert(sizeof(NiD3DPass) == 0x68);

class NiShader : public NiRefObject {
public:
	char* Name;					// 008
	UInt32		Unk00C;					// 00C
	UInt8		Unk010;					// 010
	UInt8		pad010[3];
};
assert(sizeof(NiShader) == 0x14);

class NiD3DShaderInterface : public NiShader {
public:
	IDirect3DDevice9* D3DDevice;		// 014
	NiDX9Renderer* D3DRenderer;	// 018
	NiDX9RenderState* D3DRenderState;	// 01C
	UInt8				Unk020;			// 020
};
assert(sizeof(NiD3DShaderInterface) == 0x24);

class NiD3DShader : public NiD3DShaderInterface {
public:
	UInt8					IsInitialized;		// 024
	UInt8					Unk021;				// 025
	UInt8					pad021[2];
	NiD3DShaderDeclaration* ShaderDeclaration;	// 028
	NiD3DRenderStateGroup* RenderStateGroup;	// 02C
	NiD3DShaderConstantMap* PixelConstantMap;	// 030
	NiD3DShaderConstantMap* VertexConstantMap;	// 034
	UInt32					Unk038;				// 038
};
assert(sizeof(NiD3DShader) == 0x3C);

class NiD3DDefaultShader : public NiD3DShader {
public:
	UInt32					Unk03C[8];			// 03C	
};
assert(sizeof(NiD3DDefaultShader) == 0x5C);

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
assert(sizeof(BSShader) == 0x6C);

class NiDX9Renderer : public NiRenderer
{
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
	IDirect3DDevice9* device;							// 288
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
	const NiDX9AdapterDesc* m_pkAdapterDesc;
	const NiDX9DeviceDesc* m_pkDeviceDesc;
	UInt32							m_uiBackground;
	float							m_fZClear;
	UInt32							m_uiStencilClear;
	UInt32							m_uiRendFlags;
	char							m_acBehavior[32];
	NiTMap<void*, void*>			PrePackObjects;					// 610 - NiTPointerMap <NiVBBlock *, NiDX9Renderer::PrePackObject *>
	UInt32							Unk620[153];					// 620
	NiRenderTargetGroup* defaultRTGroup;					// 884 - back buffer
	NiRenderTargetGroup* currentRTGroup;					// 888
	NiRenderTargetGroup* currentscreenRTGroup;			// 88C
	NiTMap<HWND*, NiRenderTargetGroup*>	screenRTGroups;					// 890 - NiTPointerMap <HWND *, NiPointer <NiRenderTargetGroup> >
	UInt32							Unk8A0[6];						// 8A0
	NiDX9RenderState* renderState;					// 8B8
	UInt32							Unk8BC[33];						// 8BC
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
	RefreshRate						refreshRate;				// ACC
	UInt32							UnkAD0[44];					// AD0

	__forceinline static NiDX9Renderer* GetSingleton() { return *(NiDX9Renderer**)0x11C73B4; }
};


// 200
class ShadowSceneNode : public NiNode
{
public:
	UInt32								unk0AC[2];			// 0AC
	DList<LightingData>					sceneLights;		// 0B4
	DList<LightingData>					actorShadowCasters;	// 0C0
	DList<LightingData>::Node* node0CC;			// 0CC
	DList<LightingData>::Node* node0D0;			// 0D0
	DList<LightingData>::Node* node0D4;			// 0D4
	LightingData* sceneLight1;		// 0D8
	LightingData* sceneLight2;		// 0DC
	LightingData* pSunLight;			// 0E0
	DList<LightingData>					lgtList0E4;			// 0E4
	DList<LightingData>					list0F0;			// 0F0
	DList<LightingData>					list0FC;			// 0FC
	DList<void>							list108;			// 108
	DList<void>							list114;			// 114
	UInt32								unk120;				// 120
	UInt32								unk124;				// 124
	BSCubeMapCamera* cubeMapCam;		// 128
	UInt32								unk12C;				// 12C
	bool								noLightProcessing;	// 130
	bool								isWireFrameMode;	// 131
	UInt8								pad132[2];			// 132
	BSFogProperty* fogProperty;		// 134
	BSCompoundFrustum					compoundFrustum;	// 138
	UInt8								byte1DC;			// 1DC
	UInt8								pad1DD[3];			// 1DD
	BSPortalGraph* portalGraph;		// 1E0
	NiPoint3							lightingOffset;		// 1E4
	NiPoint3							cameraPos;			// 1F0
	bool								allowLightRemoveQueues;	// 1FC
	UInt8								pad1FD[3];			// 1FD
};
static_assert(sizeof(ShadowSceneNode) == 0x200);

extern ShadowSceneNode* g_shadowSceneNode;

class ImageSpaceEffect
{
public:
	virtual void Destroy(bool doFree);
	virtual void RenderShader(NiGeometry* apScreenShape, NiDX9Renderer* pkRenderer, ImageSpaceEffectParam* apParam, bool abEndFrame);
	virtual void Setup(ImageSpaceManager* pISManager, ImageSpaceEffectParam* apParam);
	virtual void Shutdown();
	virtual void BorrowTextures(ImageSpaceEffectParam* apParam);
	virtual void ReturnTextures();
	virtual bool IsActive();
	virtual bool UpdateParams(ImageSpaceEffectParam* apParam);

	bool							bIsActive;
	bool							bParamsChanged;
	UInt8							pad02[2];
	NiTArray<ImageSpaceEffect>		pEffectList;
	NiTArray<ImageSpaceShaderParam>	pEffectParamList;
	NiTArray<ImageSpaceTexture>		pTextures;
	NiTArray<int>					pEffectInputs;
	NiTArray<int>					pEffectOutput;
};

enum ImageSpaceEffectEffectID : SInt32
{
	IS_EFFECT_NONE = -1,
	/*00*/IS_EFFECT_BLOOM,
	/*01*/IS_EFFECT_HDR,
	/*02*/IS_EFFECT_REFRACTION,
	/*03*/IS_EFFECT_CINEMATIC,
	/*04*/IS_EFFECT_DEPTH_OF_FIELD,
	/*05*/IS_EFFECT_DEPTH_RADIAL_BLUR,
	/*06*/IS_EFFECT_RADIAL_BLUR,
	/*07*/IS_EFFECT_FULLSCREEN_BLUR,
	/*08*/IS_EFFECT_GET_HIT,
	/*09*/IS_EFFECT_SUNBEAMS,
	/*0A*/IS_EFFECT_INTERFACE,
	/*0B*/IS_EFFECT_VATS_SCAN,
	/*0C*/IS_EFFECT_MOTION_BLUR,
	/*0D*/IS_EFFECT_VOLUMETRIC_FOG,
	/*0E*/IS_EFFECT_MAP,
	/*0F*/IS_EFFECT_MENU_BG,
	/*10*/IS_EFFECT_BLUR_3,
	/*11*/IS_EFFECT_BLUR_5,
	/*12*/IS_EFFECT_BLUR_7,
	/*13*/IS_EFFECT_BLUR_9,
	/*14*/IS_EFFECT_BLUR_11,
	/*15*/IS_EFFECT_BLUR_13,
	/*16*/IS_EFFECT_BLUR_15,
	/*17*/IS_EFFECT_BRIGHTPASS_BLUR_3,
	/*18*/IS_EFFECT_BRIGHTPASS_BLUR_5,
	/*19*/IS_EFFECT_BRIGHTPASS_BLUR_7,
	/*1A*/IS_EFFECT_BRIGHTPASS_BLUR_9,
	/*1B*/IS_EFFECT_BRIGHTPASS_BLUR_11,
	/*1C*/IS_EFFECT_BRIGHTPASS_BLUR_13,
	/*1D*/IS_EFFECT_BRIGHTPASS_BLUR_15,
	/*1E*/IS_EFFECT_TV,
	/*1F*/IS_EFFECT_WATER_FFT,
	/*20*/IS_EFFECT_WATER_DISPLACEMENT,
	/*21*/IS_EFFECT_NOISE,
	/*22*/IS_SHADER_COPY,
	/*23*/IS_SHADER_COPY_RENDERTARGET1,
	/*24*/IS_SHADER_RESTORE_EDRAM,
	/*25*/IS_SHADER_NULL,
	/*26*/IS_SHADER_COPY_ALPHA,
	/*27*/IS_SHADER_COPY_STENCIL,
	/*28*/IS_SHADER_COPY_STENCIL_NO_TEXTURE_NO_DEPTH,
	/*29*/IS_SHADER_REFRACTION,
	/*2A*/IS_SHADER_VOLUMETRIC_FOG,
	/*2B*/IS_SHADER_BLUR,
	/*2C*/IS_SHADER_DOUBLE_VISION,
	/*2D*/IS_SHADER_ALPHA_MULT,
	/*2E*/IS_SHADER_BLEND,
	/*2F*/IS_SHADER_TEXTURE_MASK,
	/*30*/IS_SHADER_MAP,
	/*31*/IS_SHADER_MENU_BG,
	/*32*/IS_SHADER_CINEMATIC,
	/*33*/IS_SHADER_DEPTH_OF_FIELD,
	/*34*/IS_SHADER_DEPTH_OF_FIELD_MASK,
	/*35*/IS_SHADER_MOTION_BLUR,
	/*36*/IS_SHADER_RADIAL_BLUR,
	/*37*/IS_SHADER_RADIAL_BLUR_MEDIUM,
	/*38*/IS_SHADER_RADIAL_BLUR_HIGH,
	/*39*/IS_SHADER_SUNBEAMS,
	/*3A*/IS_SHADER_HDR_BLEND,
	/*3B*/IS_SHADER_HDR_BLEND_CINEMATIC,
	/*3C*/IS_SHADER_HDR_BLEND_CINEMATIC_ALPHA_MASK,
	/*3D*/IS_SHADER_HDR_BLUR,
	/*3E*/IS_SHADER_BRIGHTPASS_FILTER,
	/*3F*/IS_SHADER_DOWNSAMPLE_16,
	/*40*/IS_SHADER_DOWNSAMPLE_9,
	/*41*/IS_SHADER_DOWNSAMPLE_4,
	/*42*/IS_SHADER_DOWNSAMPLE_4_LUM_CLAMP,
	/*43*/IS_SHADER_DOWNSAMPLE_4_LIGHT_ADAPT,
	/*44*/IS_SHADER_DOWNSAMPLE_16_LUM_CLAMP,
	/*45*/IS_SHADER_DOWNSAMPLE_16_LIGHT_ADAPT,
	/*46*/IS_SHADER_LIGHT_ADAPT,
	/*47*/IS_SHADER_LUM_CLAMP,
	/*48*/IS_SHADER_SCANLINES_BLEND,
	/*49*/IS_SHADER_SCANLINES_BLEND_1,
	/*4A*/IS_SHADER_UNK_4A,
	/*4B*/IS_SHADER_VATS_SCAN,
	/*4C*/IS_SHADER_MENU_UNPACK,
	/*4D*/IS_SHADER_TV,
	/*4E*/IS_SHADER_BLUR_3,
	/*4F*/IS_SHADER_BLUR_5,
	/*50*/IS_SHADER_BLUR_7,
	/*51*/IS_SHADER_BLUR_9,
	/*52*/IS_SHADER_BLUR_11,
	/*53*/IS_SHADER_BLUR_13,
	/*54*/IS_SHADER_BLUR_15,
	/*55*/IS_SHADER_BRIGHTPASS_BLUR_3,
	/*56*/IS_SHADER_BRIGHTPASS_BLUR_5,
	/*57*/IS_SHADER_BRIGHTPASS_BLUR_7,
	/*58*/IS_SHADER_BRIGHTPASS_BLUR_9,
	/*59*/IS_SHADER_BRIGHTPASS_BLUR_11,
	/*5A*/IS_SHADER_BRIGHTPASS_BLUR_13,
	/*5B*/IS_SHADER_BRIGHTPASS_BLUR_15,
	/*5C*/IS_SHADER_UNK_5C,
	/*5D*/IS_SHADER_WATER_FFT_WATER_SPECTRUM,
	/*5E*/IS_SHADER_WATER_FFT_HORIZONTAL_BUTTERFLY,
	/*5F*/IS_SHADER_WATER_FFT_VERTICAL_BUTTERFLY,
	/*60*/IS_SHADER_WATER_FFT_HORIZONTAL_SCRAMBLE,
	/*61*/IS_SHADER_WATER_FFT_VERTICAL_SCRAMBLE,
	/*62*/IS_SHADER_WATER_FFT_NORMALS,
	/*63*/IS_SHADER_WATER_FFT_FILTER,
	/*64*/IS_SHADER_WATER_FFT_DISPLAY_NORMALS,
	/*65*/IS_SHADER_WATER_FFT_DISPLAY_HIGHW,
	/*66*/IS_SHADER_WATER_FFT_DISPLAY_AMPLITUTE,
	/*67*/IS_SHADER_WATER_FFT_WATER_SPECTRUM_1,
	/*68*/IS_SHADER_WATER_FFT_DISPLAY_WATER_HEIGHT,
	/*69*/kWaterDisplaceClearSimulation,
	/*6A*/kWaterDisplaceTexOffset,
	/*6B*/kWaterDisplacementWadingRipple,
	/*6C*/kWaterDisplacementRainRipple,
	/*6D*/kWaterWadingHeightmap,
	/*6E*/kWaterRainHeightmap,
	/*6F*/kWaterBlendHeightmaps,
	/*70*/kWaterSmoothHeightmap,
	/*71*/kWaterDisplacementNormals,
	/*72*/IS_SHADER_NOISE_SCROLL_AND_BLEND,
	/*73*/IS_SHADER_NOISE_NORMAL_MAP
};

// 31C
class ImageSpaceManager
{
public:
	BSRenderedTexture* pMainTarget;
	NiTArray<ImageSpaceEffect*>	effectList;
	UInt32						unk14[194];

	__forceinline static ImageSpaceManager* GetSingleton() { return *(ImageSpaceManager**)0x11F91AC; }
};
static_assert(sizeof(ImageSpaceManager) == 0x31C);

#define IMAGESPACEMANAGER ds:0x11F91AC


struct TextureParams
{
	UInt32						width;		// 00
	UInt32						height;		// 04
	D3DFORMAT					d3dFormat;	// 08
	RenderMode					renderMode;	// 0C
	UInt32						bgColorMask;// 10
	ImageSpaceEffectEffectID	isEffect;	// 14

	TextureParams(UInt32 _width, UInt32 _height, D3DFORMAT _d3dFmt, RenderMode _renderMode = kRndrMode_Normal, UInt32 _bgColorMask = 0xFFFFFFFF, ImageSpaceEffectEffectID _isEffect = IS_EFFECT_NONE) :
		width(_width), height(_height), d3dFormat(_d3dFmt), renderMode(_renderMode), bgColorMask(_bgColorMask), isEffect(_isEffect) {}
};
// B0
class BSTextureManager
{
public:
	struct TextureHandle
	{
		BSRenderedTexture* texture;
		D3DFORMAT			d3dFormat;
		TextureHandle* next;
		UInt32				uiFlags;
		bool				byte10;
		UInt8				pad11[3];
		RenderTargetTypes	renderTargetType;
	};

	DList<void>					unkLists[5];			// 00
	DList<BSRenderedTexture>	shadowFrustumTextures;	// 3C
	DList<BSRenderedTexture>	waterDepthTextures;		// 48
	DList<BSRenderedTexture>	waterReflectionTextures;// 54
	DList<BSRenderedTexture>	waterBlurTextures;		// 60
	DList<BSRenderedTexture>	waterNoiseTextures;		// 6C
	DList<TextureHandle>		textureLists[2];		// 78
	DList<BSRenderedTexture>	shadowMapLists[2];		// 90
	NiDepthStencilBuffer* depthStencilBuffer;	// A8
	NiObject* objAC;					// AC

	__forceinline static BSTextureManager* GetSingleton() { return *(BSTextureManager**)0x11F91A8; }

	static void __stdcall GenerateRenderedTexture(NiCamera* camera, const TextureParams& texParams, NiTexture** pTexture);
	static void __stdcall GenerateRenderedUITexture(NiNode* tileNode, const NiVector4& scrArea, NiTexture** pTexture);
};
static_assert(sizeof(BSTextureManager) == 0xB0);

#define BSTEXTUREMANAGER ds:0x11F91A8


// C4
class NiDynamicEffect : public NiAVObject
{
public:
	enum EffectType
	{
		kEffect_AmbientLight = 0,
		kEffect_PointLight = 2,
		kEffect_DirectionalLight = 3,
		kEffect_SpotLight = 4,
		kEffect_TextureEffect = 5
	};

	bool			switchState;	// 9C
	UInt8			effectType;		// 9D
	bool			resetTraits;	// 9E	JIP only
	UInt8			extraFlags;		// 9F	JIP only
	SInt32			iIndex;			// A0
	UInt32			pushCount;		// A4
	UInt32			revID;			// A8
	DList<NiNode>	shadowGenerator;// AC
	DList<NiNode>	unaffectedNodes;// B8
};

// F0
class NiLight : public NiDynamicEffect
{
public:
	float			fadeValue;			// C4
	NiColor			ambientColor;		// C8
	NiColor			diffuseColor;		// D4
};

// FC (JIP: 110)
class NiPointLight : public NiLight
{
public:
	float			radius;			// 0E0
	float			radius0E4;		// 0E4
	TESObjectLIGH* baseLight;		// 0E8	JIP only
	NiObject* obj0EC;		// 0EC
	NiVector3		vector0F0;		// 0F0	Used for animated lights
	//	JIP Only
	float			flt0FC;			// 0FC
	NiVector4		vector100;		// 100

	__forceinline static NiPointLight* Create() { return CdeclCall<NiPointLight*>(0xA7D6E0); }
};
static_assert(sizeof(NiPointLight) == 0x110);

// 114
class NiSpotLight : public NiLight
{
public:
	float			radius;			// 0E0
	float			radius0E4;		// 0E4
	TESObjectLIGH* baseLight;		// 0E8	JIP only
	NiObject* obj0EC;		// 0EC
	NiVector3		vector0F0;		// 0F0	Used for animated lights
	NiVector3		direction;		// 0FC
	float			outerSpotAngle;	// 108
	float			innerSpotAngle;	// 10C
	float			spotExponent;	// 110
};
static_assert(sizeof(NiSpotLight) == 0x114);

// FC
class NiDirectionalLight : public NiLight
{
public:
	NiColor			fogColor;		// E0
	NiObject* objEC;			// EC
	NiVector3		direction;		// F0

	__forceinline static NiDirectionalLight* Create() { return CdeclCall<NiDirectionalLight*>(0xA75D50); }
};

// 15C
class NiTextureEffect : public NiDynamicEffect
{
public:
	NiMatrix33		modelProjMat;	// 0C4
	NiVector3		modelProjTrans;	// 0E8
	NiMatrix33		worldProjMat;	// 0F4
	NiVector3		worldProjTrans;	// 118
	NiTexture* texture;		// 124
	UInt32			filterMode;		// 128
	UInt32			clampMode;		// 12C
	UInt32			textureMode;	// 130
	UInt32			coordMode;		// 134
	bool			planesEnable;	// 138
	UInt8			pad139[3];		// 139
	NiPlane			modelPlane;		// 13C
	NiPlane			worldPlane;		// 14C
};

// B4
class NiBillboardNode : public NiNode
{
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

	__forceinline void RotateToCamera(NiCamera* camera) { ThisStdCall(0xA7DE40, this, camera); }
};

// B0
class BSTempNode : public NiNode
{
public:
	float			secondsLeft;// AC
};

// B0
class BSTempNodeManager : public NiNode
{
public:
	float			lastTime;	// AC
};
