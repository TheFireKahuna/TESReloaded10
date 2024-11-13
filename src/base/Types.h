#pragma once

typedef unsigned char		UInt8;
typedef unsigned short		UInt16;
typedef unsigned long		UInt32;
typedef unsigned long long	UInt64;
typedef signed char			SInt8;
typedef signed short		SInt16;
typedef signed long			SInt32;
typedef signed long long	SInt64;
typedef float				Float32;
typedef double				Float64;


template <typename T_Ret = void, typename ...Args>
__forceinline T_Ret ThisCall(UInt32 _addr, void* _this, Args ...args)
{
	return ((T_Ret(__thiscall*)(void*, Args...))_addr)(_this, std::forward<Args>(args)...);
}

template <typename T_Ret = void, typename ...Args>
__forceinline T_Ret StdCall(UInt32 _addr, Args ...args)
{
	return ((T_Ret(__stdcall*)(Args...))_addr)(std::forward<Args>(args)...);
}
template <typename T_Ret = void, typename ...Args>
__forceinline T_Ret ThisCall(UInt32 _addr, Args ...args)
{
	return ((T_Ret(__stdcall*)(Args...))_addr)(std::forward<Args>(args)...);
}

template <typename T_Ret = UInt32, typename ...Args>
__forceinline T_Ret ThisStdCall(UInt32 _addr, const void* _this, Args ...args)
{
	return ((T_Ret(__thiscall*)(const void*, Args...))_addr)(_this, std::forward<Args>(args)...);
}
template <typename T_Ret = void, typename ...Args>
__forceinline T_Ret CdeclCall(UInt32 _addr, Args ...args)
{
	return ((T_Ret(__cdecl*)(Args...))_addr)(std::forward<Args>(args)...);
}

void* __fastcall MemCopy(void* dst, const void* src, size_t size);

#define GAME_HEAP 0x11F6238

#define MARK_MODIFIED(form, flag) __asm push 0 __asm push flag __asm push form __asm mov ecx, g_BGSSaveLoadGame CALL_EAX(0x84A690)

void* __stdcall Game_DoHeapAlloc(size_t size);
void __stdcall Game_HeapFree(void* ptr);

template <typename T = char> __forceinline T* Game_HeapAlloc(size_t count = 1)
{
	return (T*)Game_DoHeapAlloc(count * sizeof(T));
}

template <typename T = char> __forceinline T* Ni_Alloc(size_t count = 1)
{
	return CdeclCall<T*>(0xAA13E0, count * sizeof(T));
}
template <typename T = char> __forceinline void Ni_Free(T* ptr, size_t count = 1)
{
	CdeclCall(0xAA1460, ptr, count * sizeof(T));
}

#define MAX_BLOCK_SIZE		0x800UL
#define POOL_SECTION_SIZE	0x1000UL
#define MEMORY_POOL_SIZE	0x40000UL

class PrimitiveCS
{
	UInt32		selfPtr = 0;

public:
	PrimitiveCS* Enter();
	__forceinline void Leave() { selfPtr &= 0; }
};

class LightCS
{
	UInt32	owningThread = 0;
	UInt32	enterCount = 0;

public:
	void Enter();
	__forceinline void Leave()
	{
		if (!--enterCount)
			owningThread &= 0;
	}
};

class CriticalSection : public CRITICAL_SECTION
{
public:
	CriticalSection() { InitializeCriticalSection(this); }
	~CriticalSection() { DeleteCriticalSection(this); }

	void Enter() { EnterCriticalSection(this); }
	void Leave() { LeaveCriticalSection(this); }
	bool TryEnter() { return TryEnterCriticalSection(this) != 0; }
};


template <typename T_CS> class ScopedLock
{
	T_CS* cs;

public:
	ScopedLock(T_CS* _cs) : cs(_cs) { cs->Enter(); }
	~ScopedLock() { cs->Leave(); }
};

typedef ScopedLock<CriticalSection> ScopedCS;
typedef ScopedLock<PrimitiveCS> ScopedPrimitiveCS;
typedef ScopedLock<LightCS> ScopedLightCS;

struct MemoryPool
{
	struct BlockNode
	{
		BlockNode* m_next;
		//	Data
	};

	PrimitiveCS		m_cs;
	BlockNode* m_freeSections = nullptr;
	BlockNode* m_sections[MAX_BLOCK_SIZE >> 4] = { nullptr };
	size_t			m_allocPoolCount = 0;

	static void* __fastcall Alloc(size_t size);
	static void __fastcall Free(void* pBlock, size_t size);
	static void* __fastcall Realloc(void* pBlock, size_t curSize, size_t reqSize);

	static size_t GetTotalAllocSize();
};

template <typename T> consteval size_t AlignAlloc()
{
	return (sizeof(T) & 0xF) ? ((sizeof(T) & 0xFFFFFFF0) + 0x10) : sizeof(T);
}

template <typename T> __forceinline size_t AlignNumAlloc(size_t numAlloc)
{
	switch (sizeof(T) & 0xF)
	{
	case 0:
		return numAlloc;
	case 2:
	case 6:
	case 0xA:
	case 0xE:
		return (numAlloc & 7) ? ((numAlloc & 0xFFFFFFF8) + 8) : numAlloc;
	case 4:
	case 0xC:
		return (numAlloc & 3) ? ((numAlloc & 0xFFFFFFFC) + 4) : numAlloc;
	case 8:
		return (numAlloc & 1) ? (numAlloc + 1) : numAlloc;
	default:
		return (numAlloc & 0xF) ? ((numAlloc & 0xFFFFFFF0) + 0x10) : numAlloc;
	}
}

template <typename T> __forceinline T* Pool_Alloc()
{
	return (T*)MemoryPool::Alloc(AlignAlloc<T>());
}

template <typename T> __forceinline void Pool_Free(void* block)
{
	MemoryPool::Free(block, AlignAlloc<T>());
}

template <typename T = char> __forceinline T* Pool_CAlloc(size_t count = 1)
{
	return (T*)MemoryPool::Alloc(count * sizeof(T));
}

template <typename T = char> __forceinline void Pool_CFree(void* block, size_t count = 1)
{
	MemoryPool::Free(block, count * sizeof(T));
}

template <typename T = char> __forceinline T* Pool_CRealloc(void* block, size_t curCount, size_t newCount)
{
	return (T*)MemoryPool::Realloc(block, curCount * sizeof(T), newCount * sizeof(T));
}

#define CALL_EAX(addr) __asm mov eax, addr __asm call eax
#define JMP_EAX(addr)  __asm mov eax, addr __asm jmp eax
#define JMP_EDX(addr)  __asm mov edx, addr __asm jmp edx

#define DUP_2(a) a a
#define DUP_3(a) a a a
#define DUP_4(a) a a a a

// These are used for 10h aligning segments in ASM code (massive performance gain, particularly with loops).
#define EMIT(bt) __asm _emit bt

#define NOP_0x1 EMIT(0x90)
//	"\x90"
#define NOP_0x2 EMIT(0x66) NOP_0x1
//	"\x66\x90"
#define NOP_0x3 EMIT(0x0F) EMIT(0x1F) EMIT(0x00)
//	"\x0F\x1F\x00"
#define NOP_0x4 EMIT(0x0F) EMIT(0x1F) EMIT(0x40) EMIT(0x00)
//	"\x0F\x1F\x40\x00"
#define NOP_0x5 EMIT(0x0F) EMIT(0x1F) EMIT(0x44) EMIT(0x00) EMIT(0x00)
//	"\x0F\x1F\x44\x00\x00"
#define NOP_0x6 EMIT(0x66) NOP_0x5
//	"\x66\x0F\x1F\x44\x00\x00"
#define NOP_0x7 EMIT(0x0F) EMIT(0x1F) EMIT(0x80) EMIT(0x00) EMIT(0x00) EMIT(0x00) EMIT(0x00)
//	"\x0F\x1F\x80\x00\x00\x00\x00"
#define NOP_0x8 EMIT(0x0F) EMIT(0x1F) EMIT(0x84) EMIT(0x00) EMIT(0x00) EMIT(0x00) EMIT(0x00) EMIT(0x00)
//	"\x0F\x1F\x84\x00\x00\x00\x00\x00"
#define NOP_0x9 EMIT(0x66) NOP_0x8
//	"\x66\x0F\x1F\x84\x00\x00\x00\x00\x00"
#define NOP_0xA EMIT(0x66) NOP_0x9
//	"\x66\x66\x0F\x1F\x84\x00\x00\x00\x00\x00"
#define NOP_0xB EMIT(0x66) NOP_0xA
//	"\x66\x66\x66\x0F\x1F\x84\x00\x00\x00\x00\x00"
#define NOP_0xC NOP_0x8 NOP_0x4
#define NOP_0xD NOP_0x8 NOP_0x5
#define NOP_0xE NOP_0x7 NOP_0x7
#define NOP_0xF NOP_0x8 NOP_0x7

#define PS_DUP_1(a)	a, 0UL, 0UL, 0UL
#define PS_DUP_2(a)	a, a, 0UL, 0UL
#define PS_DUP_3(a)	a, a, a, 0UL
#define PS_DUP_4(a)	a, a, a, a

#define UBYT(a) *((UInt8*)&a)
#define USHT(a) *((UInt16*)&a)
#define ULNG(a) *((UInt32*)&a)

#define NBYTE(a, n) ((UInt8*)&a)[n]
#define NWORD(a, n) ((UInt16*)&a)[n]

#define GET_N_BYTE(a, n) ((a >> (n * 8)) & 0xFF)

#define EMIT_W(a) EMIT(GET_N_BYTE(a, 0)) EMIT(GET_N_BYTE(a, 1))
#define EMIT_DW(a) EMIT(GET_N_BYTE(a, 0)) EMIT(GET_N_BYTE(a, 1)) EMIT(GET_N_BYTE(a, 2)) EMIT(GET_N_BYTE(a, 3))
#define EMIT_DW_0 DUP_4(EMIT(0x00))
#define EMIT_PS_1(a) EMIT_DW(a) DUP_3(EMIT_DW_0)
#define EMIT_PS_2(a) DUP_2(EMIT_DW(a)) DUP_2(EMIT_DW_0)
#define EMIT_PS_3(a) DUP_3(EMIT_DW(a)) EMIT_DW_0
#define EMIT_PS_4(a) DUP_4(EMIT_DW(a))
#define EMIT_B_8(a0, a1, a2, a3, a4, a5, a6, a7) EMIT(a0) EMIT(a1) EMIT(a2) EMIT(a3) EMIT(a4) EMIT(a5) EMIT(a6) EMIT(a7)
#define EMIT_W_4(a0, a1, a2, a3) EMIT_W(a0) EMIT_W(a1) EMIT_W(a2) EMIT_W(a3)
#define EMIT_W_8(a0, a1, a2, a3, a4, a5, a6, a7) EMIT_W(a0) EMIT_W(a1) EMIT_W(a2) EMIT_W(a3) EMIT_W(a4) EMIT_W(a5) EMIT_W(a6) EMIT_W(a7)
#define EMIT_DW_4(a0, a1, a2, a3) EMIT_DW(a0) EMIT_DW(a1) EMIT_DW(a2) EMIT_DW(a3)

#define AS_CHAR1(a) GET_N_BYTE(a, 0)
#define AS_CHAR2(a) GET_N_BYTE(a, 0), GET_N_BYTE(a, 1)
#define AS_CHAR4(a) GET_N_BYTE(a, 0), GET_N_BYTE(a, 1), GET_N_BYTE(a, 2), GET_N_BYTE(a, 3)
#define AS_CHAR8(a) GET_N_BYTE(a, 0), GET_N_BYTE(a, 1), GET_N_BYTE(a, 2), GET_N_BYTE(a, 3), \
	GET_N_BYTE(a, 4), GET_N_BYTE(a, 5), GET_N_BYTE(a, 6), GET_N_BYTE(a, 7)

#define _MM_SET_EPI8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab, ac, ad, ae, af) \
	{AS_CHAR1(a0), AS_CHAR1(a1), AS_CHAR1(a2), AS_CHAR1(a3), \
	 AS_CHAR1(a4), AS_CHAR1(a5), AS_CHAR1(a6), AS_CHAR1(a7), \
	 AS_CHAR1(a8), AS_CHAR1(a9), AS_CHAR1(aa), AS_CHAR1(ab), \
	 AS_CHAR1(ac), AS_CHAR1(ad), AS_CHAR1(ae), AS_CHAR1(af)}

#define _MM_SET_EPI16(a0, a1, a2, a3, a4, a5, a6, a7) \
	{AS_CHAR2(a0), AS_CHAR2(a1), AS_CHAR2(a2), AS_CHAR2(a3), \
	 AS_CHAR2(a4), AS_CHAR2(a5), AS_CHAR2(a6), AS_CHAR2(a7)}

#define _MM_SET_EPI32(a0, a1, a2, a3) \
	{AS_CHAR4(a0), AS_CHAR4(a1), AS_CHAR4(a2), AS_CHAR4(a3)}

#define _MM_SET_EPI64(a0, a1) {AS_CHAR8(a0), AS_CHAR8(a1)}

#define _MM_SET_EPI32_1(a) _MM_SET_EPI32(a, 0, 0, 0)
#define _MM_SET_EPI32_2(a) _MM_SET_EPI32(a, a, 0, 0)
#define _MM_SET_EPI32_3(a) _MM_SET_EPI32(a, a, a, 0)
#define _MM_SET_EPI32_4(a) _MM_SET_EPI32(a, a, a, a)

#define _MM_SET_EPI64_1(a) _MM_SET_EPI64(a, 0)
#define _MM_SET_EPI64_2(a) _MM_SET_EPI64(a, a)

#define AS_I32(a) std::bit_cast<UInt32>(a)
#define AS_I64(a) std::bit_cast<UInt64>(a)


#define FltPId2		1.570796371F
#define FltPI		3.141592741F
#define FltPIx2		6.283185482F
#define FltPId180	0.01745329238F
#define Flt180dPI	57.29578018F
#define DblPId180	0.017453292519943295
#define Dbl180dPI	57.29577951308232

#define PS_AbsMask			kPackedValues
#define PS_AbsMask0			kPackedValues+0x10
#define PS_FlipSignMask		kPackedValues+0x20
#define PS_FlipSignMask0	kPackedValues+0x30
#define PS_XYZ0Mask			kPackedValues+0x40
#define PD_AbsMask			kPackedValues+0x50
#define PD_FlipSignMask		kPackedValues+0x60

#define PS_Epsilon			kPackedValues+0x70
#define PS_V3_PId180		kPackedValues+0x80
#define PS_V3_180dPI		kPackedValues+0x90
#define PS_V3_PId2			kPackedValues+0xA0
#define PS_V3_PI			kPackedValues+0xB0
#define PS_V3_PIx2			kPackedValues+0xC0
#define PS_V3_Half			kPackedValues+0xD0
#define PS_V3_One			kPackedValues+0xE0
#define PS_HKUnitCnvrt		kPackedValues+0xF0

#define SS_1d1K				kPackedValues+0x100
#define SS_1d100			kPackedValues+0x104
#define SS_1d10				kPackedValues+0x108
#define SS_1d4				kPackedValues+0x10C
#define SS_3				kPackedValues+0x110
#define SS_10				kPackedValues+0x114
#define SS_100				kPackedValues+0x118

#define NRGB(r, g, b) r / 255.0F, g / 255.0F, b / 255.0F
#define NRGBA(r, g, b, a) r / 255.0F, g / 255.0F, b / 255.0F, a / 255.0F

#define GAME_RNG 0x11C4180
#define COPY_BYTES(dest, src, count) __movsb((UInt8*)(dest), (const UInt8*)(src), count)
#define ZERO_BYTES(addr, size) __stosb((UInt8*)(addr), 0, size)
#define CPY_RET_END(dest, src, length) ((char*)memcpy(dest, src, length + 1) + length)

#define ADDR_AddExtraData		0x40FF60
#define ADDR_RemoveExtraType	0x410140
#define ADDR_LoadModel			0x447080
#define ADDR_ApplyAmmoEffects	0x59A030
#define ADDR_MoveToMarker		0x5CCB20
#define ADDR_ApplyPerkModifiers	0x5E58F0
#define ADDR_ReturnThis			0x6815C0
#define ADDR_PurgeTerminalModel	0x7FFE00
#define ADDR_EquipItem			0x88C650
#define ADDR_UnequipItem		0x88C790
#define ADDR_ReturnTrue			0x8D0360
#define ADDR_TileGetFloat		0xA011B0
#define ADDR_TileSetFloat		0xA012D0
#define ADDR_TileSetString		0xA01350
#define ADDR_InitFontInfo		0xA12020
#define ADDR_GetRandomInt		0xAA5230
#define ADDR_ReturnThis2		0xE68810

#define EXTRA_DATA_CS			0x11C3920
#define SCENE_LIGHTS_CS			0x11F9EA0

#define IS_REFERENCE(form) ((*(UInt32**)form)[0xF0 >> 2] == ADDR_ReturnTrue)
#define NOT_REFERENCE(form) ((*(UInt32**)form)[0xF0 >> 2] != ADDR_ReturnTrue)
#define IS_ACTOR(form) ((*(UInt32**)form)[0x100 >> 2] == ADDR_ReturnTrue)
#define NOT_ACTOR(form) ((*(UInt32**)form)[0x100 >> 2] != ADDR_ReturnTrue)
#define IS_PROJECTILE(form) ((*(UInt32**)form)[0x224 >> 2] == ADDR_ReturnTrue)
#define IS_NODE(object) ((*(UInt32**)object)[0xC >> 2] == ADDR_ReturnThis)
#define IS_GEOMETRY(object) ((*(UInt32**)object)[0x18 >> 2] == ADDR_ReturnThis2)

#define REFR_RES *(UInt32*)result

__forceinline UInt32 GetRandomUInt(UInt32 uBound)
{
	return ThisCall<UInt32, UInt32>(ADDR_GetRandomInt, (void*)GAME_RNG, uBound);
}
__forceinline SInt32 GetRandomIntInRange(SInt32 iMin, SInt32 iMax)
{
	return ThisCall<SInt32, SInt32>(ADDR_GetRandomInt, (void*)GAME_RNG, iMax - iMin) + iMin;
}
//	Swap lhs and rhs, bypassing operator=
template <typename T> __forceinline void RawSwap(T* lhs, T* rhs)
{
	alignas(T) UInt8	buffer[sizeof(T)];
	memcpy((void*)buffer, (const void*)lhs, sizeof(T));
	memcpy((void*)lhs, (const void*)rhs, sizeof(T));
	memcpy((void*)rhs, (const void*)buffer, sizeof(T));
}


__forceinline __m128 __vectorcall operator+(__m128 a, __m128 b)
{
	return _mm_add_ps(a, b);
}
__forceinline __m128 __vectorcall operator-(__m128 a, __m128 b)
{
	return _mm_sub_ps(a, b);
}
__forceinline __m128 __vectorcall operator*(__m128 a, __m128 b)
{
	return _mm_mul_ps(a, b);
}
__forceinline __m128 __vectorcall operator&(__m128 a, __m128 b)
{
	return _mm_and_ps(a, b);
}
__forceinline __m128 __vectorcall operator|(__m128 a, __m128 b)
{
	return _mm_or_ps(a, b);
}
__forceinline __m128 __vectorcall operator^(__m128 a, __m128 b)
{
	return _mm_xor_ps(a, b);
}

template <typename T1, typename T2> __forceinline T1 GetMin(T1 value1, T2 value2)
{
	return (value1 < value2) ? value1 : value2;
}

template <typename T1, typename T2> __forceinline T1 GetMax(T1 value1, T2 value2)
{
	return (value1 > value2) ? value1 : value2;
}

template <typename T> __forceinline T sqr(T value)
{
	return value * value;
}
extern UInt32 s_CPUFeatures;
UInt32 GetCPUFeatures();

UInt32 __vectorcall cvtd2ul(double value);

double __vectorcall cvtul2d(UInt32 value);
void __fastcall cvtul2d(UInt32 value, double* result);

int __vectorcall ifloor(float value);

int __vectorcall iceil(float value);

__forceinline int iround(float value)
{
	return _mm_cvt_ss2si(_mm_load_ss(&value));
}

float __vectorcall fMod(float numer, float denom);

float __vectorcall Cos(float angle);
__forceinline float Sin(float angle)
{
	return Cos(FltPId2 - angle);
}
float __vectorcall Tan(float angle);

//	Takes:   xmm0 = {a, 0, 0, 0};
//	Returns: xmm0 = {sin(a), cos(a), 0, 0},
//			 xmm1 = {cos(a), 0, 0, 0}
__m128 __vectorcall GetSinCos(float angle);

//	Takes:   xmm0 = {x, y, z, 0};
//	Returns: xmm0 = {sin(x), sin(y), sin(z), 0},
//			 xmm1 = {cos(x), cos(y), cos(z), 0}
__m128 __vectorcall GetSinCos_V3(__m128 angles);

float __vectorcall ASin(float x);
float __vectorcall ACos(float x);
float __vectorcall ATan2(float y, float x);
__forceinline float ATan(float x)
{
	return ATan2(x, 1.0F);
}

float __vectorcall Length_V4(__m128 inPS);
__m128 __vectorcall Normalize_V4(__m128 inPS);
bool __vectorcall Equal_V3(__m128 v1, __m128 v2);
bool __vectorcall Equal_V4(__m128 v1, __m128 v2);

#define MAP_DEFAULT_ALLOC			8UL
#define MAP_MIN_BUCKET_COUNT		4UL
#define MAP_MAX_BUCKET_COUNT		0x8000UL
#define MAP_DEFAULT_BUCKET_COUNT	8UL
#define VECTOR_DEFAULT_ALLOC		8UL

char* __fastcall CopyStringKey(const char* key);
__forceinline void FreeStringKey(char* key)
{
	UInt32* bgnPtr = (UInt32*)(key - 4);
	Pool_CFree(bgnPtr, *bgnPtr);
}

__forceinline UInt32* AllocBuckets(UInt32 numBuckets)
{
	UInt32* buckets = Pool_CAlloc<UInt32>(numBuckets);
	__stosd(buckets, 0, numBuckets);
	return buckets;
}
UInt32 __fastcall AlignBucketCount(UInt32 count);
UInt32 __fastcall StrHashCS(const char* inKey);
UInt32 __fastcall StrHashCI(const char* inKey);


template <typename T_Array> class ArrayUtils
{
	using T_Data = T_Array::Element;
	using Data_Arg = std::conditional_t<std::is_scalar_v<T_Data>, T_Data, const T_Data&>;
	using Data_Val = std::conditional_t<std::is_scalar_v<T_Data>, T_Data, T_Data&&>;

	static bool __vectorcall CompareLT(Data_Arg lhs, Data_Arg rhs) { return lhs < rhs; }
	static bool __vectorcall CompareGT(Data_Arg lhs, Data_Arg rhs) { return rhs < lhs; }

	typedef bool(__vectorcall* SortComperator)(Data_Arg, Data_Arg);
	static void QuickSort(T_Array& array, UInt32 l, UInt32 h, SortComperator comperator)
	{
		UInt32 i = l;
		for (UInt32 j = l + 1; j < h; j++)
			if (!comperator(array[l], array[j]))
				RawSwap<T_Data>(&array[++i], &array[j]);
		RawSwap<T_Data>(&array[l], &array[i]);
		if (l < i)
			QuickSort(array, l, i, comperator);
		if (++i < h)
			QuickSort(array, i, h, comperator);
	}

public:
	static void Sort(T_Array& array, bool descending = false)
	{
		if (array.Size() > 1)
			QuickSort(array, 0, array.Size(), descending ? CompareGT : CompareLT);
	}

	static void Sort(T_Array& array, SortComperator comperator)
	{
		if (array.Size() > 1)
			QuickSort(array, 0, array.Size(), comperator);
	}

	__declspec(noinline) static UInt32 InsertSorted(T_Array& array, Data_Val item, SortComperator comperator)
	{
		UInt32 lBound = 0, uBound = array.Size();
		while (lBound != uBound)
		{
			UInt32 index = (lBound + uBound) >> 1;
			if (comperator(item, array[index]))
				uBound = index;
			else lBound = index + 1;
		}
		uBound = array.Size() - lBound;
		T_Data* pData = array.AllocateData();
		if (uBound)
		{
			pData = array.Data() + lBound;
			MemCopy(pData + 1, pData, sizeof(T_Data) * uBound);
		}
		*pData = std::move(item);
		return lBound;
	}

	static UInt32 InsertSorted(T_Array& array, Data_Val item, bool descending = false)
	{
		return InsertSorted(array, std::forward<T_Data>(item), descending ? CompareGT : CompareLT);
	}

	static void Shuffle(T_Array& array)
	{
		T_Data* pData = array.Data();
		for (UInt32 count = array.Size(); count > 1; count--, pData++)
			if (UInt32 rand = GetRandomUInt(count))
				RawSwap<T_Data>(pData, &pData[rand]);
	}

	static void Reverse(T_Array& array)
	{
		if (array.Size() > 1)
			for (UInt32 ftIdx = 0, bkIdx = array.Size() - 1; ftIdx < bkIdx; ftIdx++, bkIdx--)
				RawSwap<T_Data>(&array[ftIdx], &array[bkIdx]);
	}
};

template <typename T_List> class LinkedListUtils
{
	using T_Data = T_List::Element;
	using Node = T_List::Node;

	static void SwapData(Node* n1, Node* n2) { RawSwap<T_Data>(&n1->data, &n2->data); }

	static bool __fastcall CompareLT(const Node* n1, const Node* n2) { return n1->data < n2->data; }
	static bool __fastcall CompareGT(const Node* n1, const Node* n2) { return n2->data < n1->data; }

	typedef bool(__fastcall* SortComperator)(const Node*, const Node*);
	static void QuickSort(Node* lNode, UInt32 l, UInt32 h, SortComperator comperator)
	{
		UInt32 i = l;
		Node* iNode = lNode, * jNode = lNode->next;
		for (UInt32 j = l + 1; j < h; j++, jNode = jNode->next)
			if (!comperator(lNode, jNode))
			{
				i++;
				iNode = iNode->next;
				SwapData(jNode, iNode);
			}
		SwapData(lNode, iNode);
		if (l < i)
			QuickSort(lNode, l, i, comperator);
		if (++i < h)
			QuickSort(iNode->next, i, h, comperator);
	}

public:
	static void Sort(T_List& list, bool descending = false)
	{
		if (UInt32 count = list.Count(); count > 1)
			QuickSort(list.Head(), 0, count, descending ? CompareGT : CompareLT);
	}

	static void Sort(T_List& list, SortComperator comperator)
	{
		if (UInt32 count = list.Count(); count > 1)
			QuickSort(list.Head(), 0, count, comperator);
	}

	static void Shuffle(T_List& list)
	{
		Node* head = list.Head();
		for (UInt32 count = list.Count(); count > 1; count--, head = head->next)
			if (UInt32 rand = GetRandomUInt(count))
				SwapData(head, head->GetNth(rand));
	}

	static void Exchange(T_List& list, UInt32 idx1, UInt32 idx2)
	{
		if (list.Empty()) return;
		if (idx1 > idx2)
		{
			UInt32 tmp = idx1;
			idx1 = idx2;
			idx2 = tmp;
		}
		Node* iter = list.Head(), * node1 = nullptr;
		UInt32 idx = 0;
		do
		{
			if (!node1)
			{
				if (idx == idx1)
					node1 = iter;
			}
			else if (idx == idx2)
			{
				SwapData(node1, iter);
				break;
			}
			idx++;
		} while (iter = iter->next);
	}
};
template <typename T_HashMap> class HashMapUtils
{
	using Entry = T_HashMap::Entry;
	using Bucket = T_HashMap::Bucket;

public:
	static void DumpLoads(const T_HashMap& map)
	{
		UInt32 loadsArray[0x40];
		ZERO_BYTES(loadsArray, sizeof(loadsArray));
		UInt32 maxLoad = 0;
		for (Bucket* pBucket = map.GetBuckets(), *pEnd = map.End(); pBucket != pEnd; pBucket++)
		{
			UInt32 entryCount = pBucket->Size();
			loadsArray[entryCount]++;
			if (maxLoad < entryCount)
				maxLoad = entryCount;
		}
		PrintDebug("Size = %d\nBuckets = %d\n----------------\n", map.Size(), map.BucketCount());
		for (UInt32 iter = 0; iter <= maxLoad; iter++)
			if (loadsArray[iter]) PrintDebug("%d:\t%05d (%.4f%%)", iter, loadsArray[iter], 100.0 * (double)loadsArray[iter] / map.Size());
	}
};


#define Use_ArrayUtils(cont_type, data_type)	\
	using Element = data_type;	\
	friend ArrayUtils<cont_type>;
#define Use_LinkedListUtils(cont_type, data_type)	\
	using Element = data_type;	\
	friend LinkedListUtils<cont_type>;
#define Use_HashMapUtils(cont_type)	\
	friend HashMapUtils<cont_type>;


//	Workaround used for:
//	* Preventing the compiler from generating _atexit d'tors for static objects.
//	* Bypassing the compiler calling the d'tor on function-scope objects.
template <typename T> class TempObject
{
	alignas(T) UInt8	objData[sizeof(T)];

public:
	TempObject() { Reset(); }
	TempObject(const T& src) { memcpy((void*)this, (const void*)&src, sizeof(T)); }

	template <typename ...Args>
	TempObject(Args&& ...args)
	{
		new (this) T(std::forward<Args>(args)...);
	}

	void Reset() { new (this) T(); }

	void Destroy() { (*this)().~T(); }

	T& operator()() { return *(reinterpret_cast<T*>(this)); }
	T* operator*() { return reinterpret_cast<T*>(this); }
	T* operator->() { return reinterpret_cast<T*>(this); }

	inline operator T& () { return *(reinterpret_cast<T*>(this)); }

	TempObject& operator=(const T& rhs)
	{
		memcpy((void*)this, (const void*)&rhs, sizeof(T));
		return *this;
	}
	TempObject& operator=(const TempObject& rhs)
	{
		memcpy((void*)this, (const void*)&rhs, sizeof(T));
		return *this;
	}
};


char __fastcall StrCompareCS(const char* lstr, const char* rstr);

char __fastcall StrCompareCI(const char* lstr, const char* rstr);

UInt32 __fastcall StrLen(const char* str);
class String
{
public:
	char* m_data;
	UInt16		m_dataLen;
	UInt16		m_bufLen;

	String() : m_data(nullptr), m_dataLen(0), m_bufLen(0) {}
	~String() { if (m_data) Game_HeapFree(m_data); }

	void Init(UInt32 bufSize);
	void Set(const char* src);
	void Append(const char* toAppend);
	void AppendChar(char toAppend);
	void InsertChar(char toInsert, UInt32 index);
	void EraseAt(UInt32 index);
	const char* CStr();
};

enum
{
	eListInvalid = -1,
	eListEnd = -2,
	eListCount = -3,
};

#define AUX_BUFFER_ARRAY_SIZE 0x10
#define AUX_BUFFER_INIT_SIZE 0x8000U

class AuxBufferArray
{
public:
	template <typename T> struct Buffer
	{
		T* ptr;
		UInt32		size;
	};

	__declspec(noinline) static Buffer<void>* __fastcall GetAvailable(UInt32 reqSize)
	{
		thread_local static AuxBufferArray* s_auxBufferArray = nullptr;
		if (!s_auxBufferArray)
			s_auxBufferArray = (AuxBufferArray*)calloc(1, sizeof(AuxBufferArray));
		Buffer<void>* buffer = nullptr, * pIter = s_auxBufferArray->buffers, * pEnd = pIter + AUX_BUFFER_ARRAY_SIZE;
		do
		{
			if (NBYTE(pIter->size, 3) & 0x80)
				continue;
			if (pIter->size >= reqSize)
			{
				NBYTE(pIter->size, 3) |= 0x80;
				return pIter;
			}
			if (!buffer)
				buffer = pIter;
			if (!pIter->ptr)
				break;
		} while (++pIter != pEnd);
		if (buffer)
		{
			buffer->size = (reqSize > AUX_BUFFER_INIT_SIZE) ? reqSize : AUX_BUFFER_INIT_SIZE;
			if (buffer->ptr)
				_aligned_free(buffer->ptr);
			buffer->ptr = _aligned_malloc(buffer->size, 0x10);
			NBYTE(buffer->size, 3) |= 0x80;
		}
		return buffer;
	}

private:
	Buffer<void>	buffers[AUX_BUFFER_ARRAY_SIZE];
};

template <typename T> class AuxBuffer
{
	using Buffer = AuxBufferArray::Buffer<T>;

	Buffer* buffer;

public:
	AuxBuffer() : buffer(nullptr) {}
	AuxBuffer(UInt32 numElements) { *this = numElements; }
	~AuxBuffer() { if (buffer) NBYTE(buffer->size, 3) &= 0x7F; }

	inline void operator=(UInt32 numElements)
	{
		UInt32 required = numElements * sizeof(T);
		if (buffer = reinterpret_cast<Buffer*>(AuxBufferArray::GetAvailable(required)))
			if (!std::is_scalar_v<T>) __stosd((UInt32*)buffer->ptr, 0, required >> 2);
	}

	inline operator T* () const { return buffer->ptr; }
	inline T* operator*() const { return buffer->ptr; }

	T* Copy(UInt32 numElements, T* data)
	{
		UInt32 required = numElements * sizeof(T);
		if (buffer = reinterpret_cast<Buffer*>(AuxBufferArray::GetAvailable(required)))
			return (T*)MemCopy(buffer->ptr, data, required);
		return nullptr;
	}
};