#include "Types.h"


__declspec(naked) void* __fastcall MemCopy(void* dst, const void* src, size_t size)
{
	__asm
	{
		push	esi
		push	edi
		push	ecx
		mov		esi, edx
		mov		edi, ecx
		mov		eax, ecx
		sub		eax, edx
		jz		CopyUpReturn
		mov		ecx, [esp + 0x10]
		js		CopyUp
		cmp		eax, ecx
		jb		CopyDown
		CopyUp :
		cmp		ecx, 0x20
			jb		CopyUpByteMov
			cmp		ecx, 0x80
			jnb		CopyUpByteMov
			ALIGN 16
			XmmCopySmallLoop :
			movdqu	xmm0, xmmword ptr[esi]
			movdqu	xmm1, xmmword ptr[esi + 0x10]
			movdqu	xmmword ptr[edi], xmm0
			movdqu	xmmword ptr[edi + 0x10], xmm1
			lea		esi, [esi + 0x20]
			lea		edi, [edi + 0x20]
			sub		ecx, 0x20
			cmp		ecx, 0x20
			jnb		XmmCopySmallLoop
			test	ecx, ecx
			jz		CopyUpReturn
			CopyUpByteMov :
		rep movsb
			CopyUpReturn :
		pop		eax
			pop		edi
			pop		esi
			retn	4
			ALIGN 16
			CopyDown :
			lea		esi, [esi + ecx]
			lea		edi, [edi + ecx]
			cmp		ecx, 0x20
			jb		CopyDownDwordLoop
			test	edi, 0xF
			jz		XmmMovLargeLoop
			ALIGN 16
			XmmMovAlignLoop:
		dec		ecx
			dec		esi
			dec		edi
			mov		al, [esi]
			mov[edi], al
			test	edi, 0xF
			jnz		XmmMovAlignLoop
			ALIGN 16
			XmmMovLargeLoop:
		cmp		ecx, 0x80
			jb		XmmMovSmallLoop
			sub		esi, 0x80
			sub		edi, 0x80
			movdqu	xmm0, xmmword ptr[esi]
			movdqu	xmm1, xmmword ptr[esi + 0x10]
			movdqu	xmm2, xmmword ptr[esi + 0x20]
			movdqu	xmm3, xmmword ptr[esi + 0x30]
			movdqu	xmm4, xmmword ptr[esi + 0x40]
			movdqu	xmm5, xmmword ptr[esi + 0x50]
			movdqu	xmm6, xmmword ptr[esi + 0x60]
			movdqu	xmm7, xmmword ptr[esi + 0x70]
			movdqa	xmmword ptr[edi], xmm0
			movdqa	xmmword ptr[edi + 0x10], xmm1
			movdqa	xmmword ptr[edi + 0x20], xmm2
			movdqa	xmmword ptr[edi + 0x30], xmm3
			movdqa	xmmword ptr[edi + 0x40], xmm4
			movdqa	xmmword ptr[edi + 0x50], xmm5
			movdqa	xmmword ptr[edi + 0x60], xmm6
			movdqa	xmmword ptr[edi + 0x70], xmm7
			sub		ecx, 0x80
			jmp		XmmMovLargeLoop
			ALIGN 16
			XmmMovSmallLoop:
		cmp		ecx, 0x20
			jb		CopyDownDwordLoop
			sub		esi, 0x20
			sub		edi, 0x20
			movdqu	xmm0, xmmword ptr[esi]
			movdqu	xmm1, xmmword ptr[esi + 0x10]
			movdqa	xmmword ptr[edi], xmm0
			movdqa	xmmword ptr[edi + 0x10], xmm1
			sub		ecx, 0x20
			jmp		XmmMovSmallLoop
			ALIGN 16
			CopyDownDwordLoop:
		cmp		ecx, 4
			jb		CopyDownByte
			sub		edi, 4
			sub		esi, 4
			mov		eax, [esi]
			mov[edi], eax
			sub		ecx, 4
			jmp		CopyDownDwordLoop
			CopyDownByte :
		test	ecx, ecx
			jz		CopyDownReturn
			ALIGN 16
			CopyDownByteLoop :
			dec		edi
			dec		esi
			mov		al, [esi]
			mov[edi], al
			dec		ecx
			jnz		CopyDownByteLoop
			CopyDownReturn :
		pop		eax
			pop		edi
			pop		esi
			retn	4
	}
}

__declspec(naked) void* __stdcall Game_DoHeapAlloc(size_t size)
{
	__asm
	{
		mov		ecx, GAME_HEAP
		JMP_EAX(0xAA3E40)
	}
}

__declspec(naked) void __stdcall Game_HeapFree(void* ptr)
{
	__asm
	{
		mov		ecx, GAME_HEAP
		JMP_EAX(0xAA4060)
	}
}

__declspec(naked) PrimitiveCS* PrimitiveCS::Enter()
{
	__asm
	{
		push	esi
		push	edi
		mov		esi, ecx
		mov		edi, Sleep
		ALIGN 16
		spinHead:
		xor eax, eax
			lock cmpxchg[ecx], esi
			jz		done
			push	0
			call	edi
			mov		ecx, esi
			jmp		spinHead
			ALIGN 16
			done:
		mov		eax, esi
			pop		edi
			pop		esi
			retn
	}
}

__declspec(naked) void LightCS::Enter()
{
	__asm
	{
		push	ebx
		mov		ebx, ecx
		call	GetCurrentThreadId
		cmp[ebx], eax
		jz		incRefCnt
		push	esi
		push	edi
		mov		esi, eax
		mov		edi, Sleep
		ALIGN 16
		spinHead:
		xor eax, eax
			lock cmpxchg[ebx], esi
			jz		done
			push	0
			call	edi
			jmp		spinHead
			ALIGN 16
			done:
		mov		dword ptr[ebx + 4], 1
			pop		edi
			pop		esi
			pop		ebx
			retn
			ALIGN 16
			incRefCnt:
		inc		dword ptr[ebx + 4]
			pop		ebx
			retn
	}
}

alignas(16) const char
kLwrCaseConverter[] =
{
	'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F',
	'\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',
	'\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F',
	'\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F',
	'\x40', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
	'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F',
	'\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
	'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F',
	'\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
	'\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
	'\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
	'\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
	'\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
	'\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
	'\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
	'\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'
};

const __m128i kStringMasks[] =
{
	_MM_SET_EPI32_4(0xFFFFFFFF),
	_MM_SET_EPI32_4(0x00000000),
	_MM_SET_EPI32_4(0x41414141),
	_MM_SET_EPI32_4(0x61616161),
	_MM_SET_EPI32_4(0x7F7F7F7F),
	_MM_SET_EPI32_4(0x19191919),
	_MM_SET_EPI32_4(0x20202020)
};
__declspec(naked) UInt32 __vectorcall cvtd2ul(double value)
{
	__asm
	{
		lea		eax, [esp - 8]
		movlpd[eax], xmm0
		fld		qword ptr[eax]
		fisttp	qword ptr[eax]
		mov		eax, [eax]
		retn
	}
}

__declspec(naked) double __vectorcall cvtul2d(UInt32 value)
{
	__asm
	{
		push	0
		push	ecx
		fild	qword ptr[esp]
		fstp	qword ptr[esp]
		movq	xmm0, qword ptr[esp]
		add		esp, 8
		retn
	}
}

__declspec(naked) void __fastcall cvtul2d(UInt32 value, double* result)
{
	__asm
	{
		mov[edx], ecx
		and dword ptr[edx + 4], 0
		fild	qword ptr[edx]
		fstp	qword ptr[edx]
		retn
	}
}

UInt32 s_CPUFeatures = 0;

__declspec(naked) UInt32 GetCPUFeatures()
{
	__asm
	{
		push	ebx
		xor ecx, ecx
		mov		eax, 1
		cpuid
		xor eax, eax
		bt		edx, 0x19
		setc	al
		bt		edx, 0x1A
		setc	dl
		shl		dl, 1
		or al, dl
		xor edx, edx
		test	ecx, 1
		setnz	dl
		shl		dl, 2
		or eax, edx
		bt		ecx, 9
		setc	dl
		shl		dl, 3
		or eax, edx
		bt		ecx, 0x13
		setc	dl
		shl		dl, 4
		or eax, edx
		bt		ecx, 0x14
		setc	dl
		shl		dl, 5
		or eax, edx
		bt		ecx, 0x1C
		setc	dl
		shl		dl, 6
		or eax, edx
		push	eax
		xor ecx, ecx
		mov		eax, 7
		cpuid
		xor edx, edx
		bt		ebx, 5
		setc	dl
		shl		edx, 7
		mov		ecx, edx
		bt		ebx, 0x10
		setc	dl
		shl		edx, 8
		or ecx, edx
		pop		eax
		or eax, ecx
		pop		ebx
		retn
	}
}

__declspec(naked) int __vectorcall ifloor(float value)
{
	__asm
	{
		test	s_CPUFeatures, 0x10
		jz		noSSE4
		roundss	xmm0, xmm0, 1
		cvttss2si	eax, xmm0
		retn
		noSSE4 :
		push	0x3FA0
			ldmxcsr[esp]
			cvtss2si	eax, xmm0
			mov		byte ptr[esp + 1], 0x1F
			ldmxcsr[esp]
			pop		ecx
			retn
	}
}

__declspec(naked) int __vectorcall iceil(float value)
{
	__asm
	{
		test	s_CPUFeatures, 0x10
		jz		noSSE4
		roundss	xmm0, xmm0, 2
		cvttss2si	eax, xmm0
		retn
		noSSE4 :
		push	0x5FA0
			ldmxcsr[esp]
			cvtss2si	eax, xmm0
			mov		byte ptr[esp + 1], 0x1F
			ldmxcsr[esp]
			pop		ecx
			retn
	}
}

__declspec(naked) float __vectorcall fMod(float numer, float denom)
{
	__asm
	{
		movq	xmm2, xmm0
		divss	xmm2, xmm1
		cvttps2dq	xmm2, xmm2
		cvtdq2ps	xmm2, xmm2
		mulss	xmm2, xmm1
		subss	xmm0, xmm2
		retn
	}
}

__declspec(naked) float __vectorcall Cos(float angle)
{
	__asm
	{
		andps	xmm0, PS_AbsMask0
		movss	xmm2, PS_V3_PIx2
		movss	xmm1, kFlt2dPI
		mulss	xmm1, xmm0
		cvttss2si	eax, xmm1
		cmp		eax, 3
		jle		perdOK
		mov		edx, eax
		and eax, 3
		shr		edx, 2
		cvtsi2ss	xmm1, edx
		mulss	xmm1, xmm2
		subss	xmm0, xmm1
		perdOK :
		xorps	xmm3, xmm3
			comiss	xmm0, xmm3
			jbe		retn1
			test	al, al
			jz		doCalc
			jp		fourthQ
			movss	xmm2, PS_V3_PI
			movss	xmm3, PS_FlipSignMask0
			fourthQ :
		subss	xmm0, xmm2
			doCalc :
		mulss	xmm0, xmm0
			movq	xmm1, xmm0
			mulss	xmm1, kCosConsts
			subss	xmm1, kCosConsts + 4
			mulss	xmm1, xmm0
			addss	xmm1, kCosConsts + 8
			mulss	xmm1, xmm0
			subss	xmm1, kCosConsts + 0xC
			mulss	xmm0, xmm1
			addss	xmm0, kCosConsts + 0x10
			xorps	xmm0, xmm3
			retn
			retn1 :
		movss	xmm0, PS_V3_One
			retn
			ALIGN 16
			kFlt2dPI :
			EMIT_DW(0x3F22F983)
			kCosConsts :
			EMIT_DW(0x37C23AB1)
			EMIT_DW(0x3AB59551)
			EMIT_DW(0x3D2AA76F)
			EMIT_DW(0x3EFFFFE0)
			EMIT_DW(0x3F7FFFFF)
	}
}

__declspec(naked) __m128 __vectorcall Cos_V3(__m128 angles)
{
	__asm
	{
		andps	xmm0, PS_AbsMask
		movaps	xmm1, xmm0
		movaps	xmm2, PS_V3_PIx2
		mulps	xmm1, kPS2dPI
		cvttps2dq	xmm1, xmm1
		movaps	xmm3, xmm1
		pcmpgtd	xmm3, kQuadTest
		movmskps	edx, xmm3
		test	dl, dl
		jz		perdOK
		movaps	xmm3, xmm1
		andps	xmm1, kQuadTest
		psrld	xmm3, 2
		cvtdq2ps	xmm3, xmm3
		mulps	xmm3, xmm2
		subps	xmm0, xmm3
		perdOK :
		movaps	xmm3, xmm1
			pcmpgtd	xmm1, kQuadTest + 0x10
			andps	xmm2, xmm1
			andnps	xmm1, xmm3
			xorps	xmm3, xmm3
			pcmpgtd	xmm1, xmm3
			movaps	xmm3, xmm1
			pslld	xmm3, 0x1F
			andps	xmm1, PS_V3_PI
			orps	xmm1, xmm2
			subps	xmm0, xmm1
			mulps	xmm0, xmm0
			movaps	xmm1, kCosConsts
			mulps	xmm1, xmm0
			subps	xmm1, kCosConsts + 0x10
			mulps	xmm1, xmm0
			addps	xmm1, kCosConsts + 0x20
			mulps	xmm1, xmm0
			subps	xmm1, kCosConsts + 0x30
			mulps	xmm0, xmm1
			addps	xmm0, kCosConsts + 0x40
			xorps	xmm0, xmm3
			retn
			ALIGN 16
			kPS2dPI:
		EMIT_PS_3(0x3F22F983)
			kQuadTest :
			EMIT_PS_3(0x00000003)
			EMIT_PS_3(0x00000002)
			kCosConsts :
			EMIT_PS_3(0x37C23AB1)
			EMIT_PS_3(0x3AB59551)
			EMIT_PS_3(0x3D2AA76F)
			EMIT_PS_3(0x3EFFFFE0)
			EMIT_PS_3(0x3F7FFFFF)
	}
}

__declspec(naked) __m128 __vectorcall GetSinCos(float angle)
{
	__asm
	{
		movq	xmm1, xmm0
		movss	xmm0, PS_V3_PId2
		subss	xmm0, xmm1
		unpcklps	xmm0, xmm1
		call	Cos_V3
		movshdup	xmm1, xmm0
		retn
	}
}

__declspec(naked) __m128 __vectorcall GetSinCos_V3(__m128 angles)
{
	__asm
	{
		movaps	xmm5, xmm0
		call	Cos_V3
		movaps	xmm4, xmm0
		movaps	xmm0, PS_V3_PId2
		subps	xmm0, xmm5
		call	Cos_V3
		movaps	xmm1, xmm4
		retn
	}
}

__declspec(naked) float __vectorcall Tan(float angle)
{
	_asm
	{
		call	GetSinCos
		movq	xmm2, xmm1
		andps	xmm2, PS_AbsMask0
		comiss	xmm2, PS_Epsilon
		jb		ooRange
		divss	xmm0, xmm1
		retn
		ooRange :
		xorps	xmm0, xmm0
			retn
	}
}

alignas(16) const UInt32 kASinConsts[] =
{
	0xBC996E30, 0x3D981627, 0x3E593484, 0x3FC90FDB, 0x34000000, 0x3F800000, 0x40490FDB, 0x03800000,
	0x37202A00, 0x37E24000, 0x38333600, 0x386E4C00, 0x38913200, 0x38A84000, 0x38BC5200, 0x38CDAC00,
	0x38DC5000, 0x38E8C000, 0x38F2C800, 0x38FAA800, 0x39005600, 0x39029000, 0x3903C800, 0x39046400,
	0x39042C00, 0x39036800, 0x39022800, 0x39003400, 0x38FB7000, 0x38F60800, 0x38EFD800, 0x38E8B800,
	0x38E15800, 0x38D96000, 0x38D0C000, 0x38C83000, 0x38BF2000, 0x38B59800, 0x38AC5800, 0x38A2E000,
	0x38998000, 0x388FD000, 0x38866000, 0x387AA000, 0x38678000, 0x3855E000, 0x3844A000, 0x38338000,
	0x38234000, 0x3812E000, 0x38042000, 0x37EB4000, 0x37CF4000, 0x37B64000, 0x379D4000, 0x37874000,
	0x37658000, 0x373F8000, 0x371A8000, 0x36F90000, 0x36C30000, 0x36900000, 0x364E0000, 0x360E0000,
	0x35B40000, 0x35500000, 0x34D00000, 0x33C00000, 0x34000000, 0x34800000, 0x35400000, 0x35A80000,
	0x36040000, 0x36400000, 0x367C0000, 0x36A60000, 0x36D60000, 0x37040000, 0x371D0000, 0x37390000,
	0x37560000, 0x37750000, 0x378B0000, 0x379C0000, 0x37AC8000, 0x37BE8000, 0x37D08000, 0x37E30000,
	0x37F58000, 0x3803C000, 0x380D0000, 0x3815C000, 0x381EC000, 0x38274000, 0x382F8000, 0x38380000,
	0x383F8000, 0x38470000, 0x384E8000, 0x3854C000, 0x385B0000, 0x38604000, 0x38658000, 0x3869C000,
	0x386D4000, 0x38708000, 0x3872C000, 0x38744000, 0x38750000, 0x38750000, 0x38740000, 0x38724000,
	0x38700000, 0x386C0000, 0x3867C000, 0x3862C000, 0x385C8000, 0x38558000, 0x384D8000, 0x38450000,
	0x383B0000, 0x38318000, 0x38260000, 0x381A0000, 0x380D8000, 0x38010000, 0x37E70000, 0x37CB0000,
	0x37AF0000, 0x37930000, 0x376C0000, 0x37340000, 0x37020000, 0x36A00000, 0x36180000, 0x35000000
};

__declspec(naked) float __vectorcall ASin(float x)
{
	__asm
	{
		xorps	xmm4, xmm4
		comiss	xmm0, xmm4
		jz		done
		movq	xmm4, xmm0
		andps	xmm4, PS_FlipSignMask0
		xorps	xmm0, xmm4
		movss	xmm3, kASinConsts + 0x14
		comiss	xmm0, xmm3
		jnb		ooRange
		movss	xmm1, kASinConsts + 0x1C
		paddd	xmm1, xmm0
		movaps	xmm2, kASinConsts
		cvttss2si	eax, xmm1
		movq	xmm1, xmm2
		mulss	xmm1, xmm0
		psrldq	xmm2, 4
		addss	xmm1, xmm2
		mulss	xmm1, xmm0
		psrldq	xmm2, 4
		subss	xmm1, xmm2
		mulss	xmm1, xmm0
		subss	xmm3, xmm0
		movshdup	xmm0, xmm2
		addss	xmm1, xmm0
		sqrtss	xmm2, xmm3
		mulss	xmm2, xmm1
		subss	xmm0, xmm2
		addss	xmm0, kASinConsts[eax * 4 + 0x20]
		xorps	xmm0, xmm4
		done :
		retn
			ooRange :
		movss	xmm0, kASinConsts + 0xC
			xorps	xmm0, xmm4
			retn
	}
}

__declspec(naked) float __vectorcall ACos(float x)
{
	_asm
	{
		xorps	xmm4, xmm4
		comiss	xmm0, xmm4
		jz		isZero
		movq	xmm4, xmm0
		andps	xmm4, PS_FlipSignMask0
		xorps	xmm0, xmm4
		movss	xmm3, kASinConsts + 0x14
		comiss	xmm0, xmm3
		jnb		ooRange
		movss	xmm1, kASinConsts + 0x1C
		paddd	xmm1, xmm0
		movaps	xmm2, kASinConsts
		cvttss2si	eax, xmm1
		movq	xmm1, xmm2
		mulss	xmm1, xmm0
		psrldq	xmm2, 4
		addss	xmm1, xmm2
		mulss	xmm1, xmm0
		psrldq	xmm2, 4
		subss	xmm1, xmm2
		mulss	xmm1, xmm0
		psrldq	xmm2, 4
		addss	xmm1, xmm2
		subss	xmm3, xmm0
		sqrtss	xmm0, xmm3
		mulss	xmm0, xmm1
		subss	xmm0, kASinConsts[eax * 4 + 0x20]
		movmskps	eax, xmm4
		test	al, al
		jz		done
		xorps	xmm0, xmm4
		addss	xmm0, kASinConsts + 0x18
		done:
		retn
			isZero :
		movss	xmm0, kASinConsts + 0xC
			retn
			ooRange :
		movss	xmm0, kASinConsts + 0x18
			psrad	xmm4, 0x1F
			andps	xmm0, xmm4
			retn
	}
}

__declspec(naked) float __vectorcall ATan2(float y, float x)
{
	__asm
	{
		xorps	xmm2, xmm2
		comiss	xmm0, xmm2
		jz		zeroY
		comiss	xmm1, xmm2
		jz		zeroX
		movq	xmm2, xmm0
		unpcklpd	xmm2, xmm1
		andps	xmm2, PS_AbsMask
		movaps	xmm3, xmm2
		pshufd	xmm4, xmm2, 0xFE
		maxss	xmm3, xmm4
		minss	xmm4, xmm2
		divss	xmm4, xmm3
		movq	xmm3, xmm4
		mulss	xmm3, xmm4
		movq	xmm5, xmm3
		mulss	xmm5, kATanConsts
		addss	xmm5, kATanConsts + 4
		mulss	xmm5, xmm3
		subss	xmm5, kATanConsts + 8
		mulss	xmm5, xmm3
		addss	xmm5, kATanConsts + 0xC
		mulss	xmm5, xmm3
		subss	xmm5, kATanConsts + 0x10
		mulss	xmm5, xmm3
		addss	xmm5, kATanConsts + 0x14
		mulss	xmm4, xmm5
		movss	xmm3, PS_FlipSignMask0
		pshufd	xmm5, xmm2, 0xFE
		comiss	xmm2, xmm5
		jbe		doneCmp1
		xorps	xmm4, xmm3
		addss	xmm4, PS_V3_PId2
		doneCmp1 :
		xorps	xmm2, xmm2
			comiss	xmm1, xmm2
			jnb		doneCmp2
			xorps	xmm4, xmm3
			addss	xmm4, PS_V3_PI
			doneCmp2 :
		andps	xmm0, xmm3
			xorps	xmm0, xmm4
			retn
			zeroY :
		comiss	xmm1, xmm2
			jnb		done
			movss	xmm0, PS_V3_PI
			retn
			zeroX :
		movss	xmm1, PS_V3_PId2
			andps	xmm0, PS_FlipSignMask0
			orps	xmm0, xmm1
			done :
		retn
			ALIGN 16
			kATanConsts :
			EMIT_DW(0xBC5CDD30)
			EMIT_DW(0x3D6B6D55)
			EMIT_DW(0x3DF84C31)
			EMIT_DW(0x3E4854C9)
			EMIT_DW(0x3EAA7E45)
			EMIT_DW(0x3F7FFFB7)
	}
}

__declspec(naked) float __vectorcall Length_V4(__m128 inPS)
{
	__asm
	{
		xorps	xmm1, xmm1
		mulps	xmm0, xmm0
		haddps	xmm0, xmm1
		haddps	xmm0, xmm1
		comiss	xmm0, xmm1
		jz		done
		movq	xmm1, xmm0
		rsqrtss	xmm2, xmm0
		mulss	xmm1, xmm2
		mulss	xmm1, xmm2
		movss	xmm3, SS_3
		subss	xmm3, xmm1
		mulss	xmm3, xmm2
		mulss	xmm3, PS_V3_Half
		mulss	xmm0, xmm3
		done :
		retn
	}
}

__declspec(naked) __m128 __vectorcall Normalize_V4(__m128 inPS)
{
	__asm
	{
		movaps	xmm1, xmm0
		movaps	xmm2, xmm1
		mulps	xmm2, xmm2
		xorps	xmm0, xmm0
		haddps	xmm2, xmm0
		haddps	xmm2, xmm0
		comiss	xmm2, PS_Epsilon
		jb		zeroLen
		rsqrtss	xmm3, xmm2
		movss	xmm0, SS_3
		mulss	xmm2, xmm3
		mulss	xmm2, xmm3
		subss	xmm0, xmm2
		mulss	xmm0, xmm3
		mulss	xmm0, PS_V3_Half
		shufps	xmm0, xmm0, 0
		mulps	xmm0, xmm1
		zeroLen :
		retn
	}
}

__declspec(naked) bool __vectorcall Equal_V3(__m128 v1, __m128 v2)
{
	__asm
	{
		subps	xmm0, xmm1
		pshufd	xmm1, PS_AbsMask0, 0x40
		andps	xmm0, xmm1
		cmpltps	xmm0, PS_Epsilon
		movmskps	eax, xmm0
		cmp		al, 0xF
		setz	al
		retn
	}
}

__declspec(naked) bool __vectorcall Equal_V4(__m128 v1, __m128 v2)
{
	__asm
	{
		subps	xmm0, xmm1
		andps	xmm0, PS_AbsMask
		cmpltps	xmm0, PS_Epsilon
		movmskps	eax, xmm0
		cmp		al, 0xF
		setz	al
		retn
	}
}

__declspec(naked) char* __fastcall CopyStringKey(const char* key)
{
	__asm
	{
		push	esi
		push	edi
		mov		esi, ecx
		call	StrLen
		lea		edi, [eax + 1]
		lea		ecx, [edi + 0x14]
		and cl, 0xF0
		push	ecx
		call	MemoryPool::Alloc
		pop		dword ptr[eax]
		add		eax, 4
		mov		ecx, edi
		mov		edi, eax
		rep movsb
		pop		edi
		pop		esi
		retn
	}
}

__declspec(naked) UInt32 __fastcall AlignBucketCount(UInt32 count)
{
	__asm
	{
		mov		eax, MAP_MIN_BUCKET_COUNT
		cmp		ecx, eax
		cmovb	ecx, eax
		mov		eax, MAP_MAX_BUCKET_COUNT
		cmp		ecx, eax
		cmova	ecx, eax
		bsf		eax, ecx
		bsr		ecx, ecx
		cmp		al, cl
		setnz	al
		inc		eax
		shl		eax, cl
		retn
	}
}

#define STR_HASH_SEED 0xD39CA509UL

__declspec(naked) UInt32 __fastcall StrHashCS(const char* inKey)
{
	__asm
	{
		test	ecx, ecx
		jz		nullStr
		push	ebx
		mov		eax, STR_HASH_SEED
		ALIGN 16
		iterHead:
		mov		ebx, [ecx]
			test	bl, bl
			jz		done
			movzx	edx, bl
			shl		edx, 4
			sub		eax, edx
			mov		edx, eax
			shl		eax, 5
			sub		eax, edx
			test	bh, bh
			jz		done
			movzx	edx, bh
			shl		edx, 0xC
			sub		eax, edx
			mov		edx, eax
			shl		eax, 5
			sub		eax, edx
			shr		ebx, 0x10
			test	bl, bl
			jz		done
			movzx	edx, bl
			shl		edx, 0x14
			sub		eax, edx
			mov		edx, eax
			shl		eax, 5
			sub		eax, edx
			test	bh, bh
			jz		done
			movzx	edx, bh
			sub		eax, edx
			mov		edx, eax
			shl		eax, 5
			sub		eax, edx
			add		ecx, 4
			jmp		iterHead
			nullStr :
		xor eax, eax
			retn
			ALIGN 16
			done :
			pop		ebx
			retn
	}
}

__declspec(naked) UInt32 __fastcall StrHashCI(const char* inKey)
{
	__asm
	{
		test	ecx, ecx
		jz		nullStr
		push	ebx
		push	esi
		mov		eax, STR_HASH_SEED
		mov		esi, ecx
		xor ecx, ecx
		ALIGN 16
		iterHead:
		mov		ebx, [esi]
			test	bl, bl
			jz		done
			mov		cl, bl
			movzx	edx, kLwrCaseConverter[ecx]
			shl		edx, 4
			sub		eax, edx
			mov		edx, eax
			shl		eax, 5
			sub		eax, edx
			test	bh, bh
			jz		done
			mov		cl, bh
			movzx	edx, kLwrCaseConverter[ecx]
			shl		edx, 0xC
			sub		eax, edx
			mov		edx, eax
			shl		eax, 5
			sub		eax, edx
			shr		ebx, 0x10
			test	bl, bl
			jz		done
			mov		cl, bl
			movzx	edx, kLwrCaseConverter[ecx]
			shl		edx, 0x14
			sub		eax, edx
			mov		edx, eax
			shl		eax, 5
			sub		eax, edx
			test	bh, bh
			jz		done
			mov		cl, bh
			movzx	edx, kLwrCaseConverter[ecx]
			sub		eax, edx
			mov		edx, eax
			shl		eax, 5
			sub		eax, edx
			add		esi, 4
			jmp		iterHead
			nullStr :
		xor eax, eax
			retn
			ALIGN 16
			done :
			pop		esi
			pop		ebx
			retn
	}
}

__declspec(naked) char __fastcall StrCompareCS(const char* lstr, const char* rstr)
{
	__asm
	{
		test	ecx, ecx
		jz		retnLT
		push	ebx
		push	esi
		push	edi
		lea		esi, [ecx - 0x10]
		lea		edi, [edx - 0x10]
		and ecx, 0xF
		sub		esi, ecx
		sub		edi, ecx
		or ebx, 0xFFFFFFFF
		shl		ebx, cl
		ALIGN 16
		iterHead:
		add		esi, 0x10
			add		edi, 0x10
			movaps	xmm0, [esi]
			movups	xmm1, [edi]
			xorps	xmm2, xmm2
			pcmpeqb	xmm2, xmm0
			pmovmskb	ecx, xmm2
			xorps	xmm2, xmm2
			pcmpeqb	xmm2, xmm1
			pmovmskb	edx, xmm2
			movaps	xmm2, xmm0
			pcmpeqb	xmm2, xmm1
			pmovmskb	eax, xmm2
			not ax
			and eax, ebx
			or edx, ecx
			and edx, ebx
			jnz		foundEnd
			mov		edx, ebx
			or ebx, 0xFFFFFFFF
			test	eax, eax
			jz		iterHead
			bsf		ecx, eax
			pcmpgtb	xmm0, xmm1
			pmovmskb	eax, xmm0
			and eax, edx
			bsf		eax, eax
			cmp		al, cl
			setz	al
			cmovnz	eax, ebx
			add		ecx, edi
			pop		edi
			pop		esi
			pop		ebx
			retn
			retnLT :
		mov		al, 0xFF
			retn
			ALIGN 16
			foundEnd :
			test	eax, eax
			jz		done
			bsf		ecx, eax
			bsf		edx, edx
			xor eax, eax
			cmp		cl, dl
			ja		done
			pcmpgtb	xmm0, xmm1
			pmovmskb	edx, xmm0
			and edx, ebx
			bsf		edx, edx
			cmp		cl, dl
			setz	al
			lea		eax, [eax + eax - 1]
			add		ecx, edi
			done :
		pop		edi
			pop		esi
			pop		ebx
			retn
	}
}

__declspec(naked) char __fastcall StrCompareCI(const char* lstr, const char* rstr)
{
	__asm
	{
		test	ecx, ecx
		jz		retnLT
		push	ebx
		push	esi
		push	edi
		lea		esi, [ecx - 0x10]
		lea		edi, [edx - 0x10]
		and ecx, 0xF
		sub		esi, ecx
		sub		edi, ecx
		or ebx, 0xFFFFFFFF
		shl		ebx, cl
		movaps	xmm3, kStringMasks + 0x20
		movaps	xmm4, kStringMasks + 0x40
		movaps	xmm5, kStringMasks + 0x50
		movaps	xmm6, kStringMasks + 0x60
		ALIGN 16
		iterHead:
		add		esi, 0x10
			add		edi, 0x10
			movaps	xmm0, [esi]
			movups	xmm1, [edi]
			xorps	xmm2, xmm2
			pcmpeqb	xmm2, xmm0
			pmovmskb	ecx, xmm2
			xorps	xmm2, xmm2
			pcmpeqb	xmm2, xmm1
			pmovmskb	edx, xmm2
			movaps	xmm2, xmm0
			psubb	xmm2, xmm3
			pand	xmm2, xmm4
			pcmpgtb	xmm2, xmm5
			pandn	xmm2, xmm6
			pxor	xmm0, xmm2
			movaps	xmm2, xmm1
			psubb	xmm2, xmm3
			pand	xmm2, xmm4
			pcmpgtb	xmm2, xmm5
			pandn	xmm2, xmm6
			pxor	xmm1, xmm2
			movaps	xmm2, xmm0
			pcmpeqb	xmm2, xmm1
			pmovmskb	eax, xmm2
			not ax
			and eax, ebx
			or edx, ecx
			and edx, ebx
			jnz		foundEnd
			mov		edx, ebx
			or ebx, 0xFFFFFFFF
			test	eax, eax
			jz		iterHead
			bsf		ecx, eax
			pcmpgtb	xmm0, xmm1
			pmovmskb	eax, xmm0
			and eax, edx
			bsf		eax, eax
			cmp		al, cl
			setz	al
			cmovnz	eax, ebx
			add		ecx, edi
			pop		edi
			pop		esi
			pop		ebx
			retn
			retnLT :
		mov		al, 0xFF
			retn
			ALIGN 16
			foundEnd :
			test	eax, eax
			jz		done
			bsf		ecx, eax
			bsf		edx, edx
			xor eax, eax
			cmp		cl, dl
			ja		done
			pcmpgtb	xmm0, xmm1
			pmovmskb	edx, xmm0
			and edx, ebx
			bsf		edx, edx
			cmp		cl, dl
			setz	al
			lea		eax, [eax + eax - 1]
			add		ecx, edi
			done :
		pop		edi
			pop		esi
			pop		ebx
			retn
	}
}

void String::Init(UInt32 bufSize)
{
	if (m_data) Game_HeapFree(m_data);
	m_bufLen = bufSize;
	m_data = Game_HeapAlloc(m_bufLen + 1);
	*m_data = 0;
	m_dataLen = 0;
}

void String::Set(const char* src)
{
	m_dataLen = StrLen(src);
	if (!m_dataLen)
	{
		if (!src)
		{
			if (m_data)
			{
				Game_HeapFree(m_data);
				m_data = NULL;
			}
			m_bufLen = 0;
		}
		else if (m_data) *m_data = 0;
		return;
	}
	if (m_bufLen < m_dataLen)
	{
		m_bufLen = m_dataLen;
		if (m_data) Game_HeapFree(m_data);
		m_data = Game_HeapAlloc(m_dataLen + 1);
	}
	COPY_BYTES(m_data, src, m_dataLen + 1);
}

void String::Append(const char* toAppend)
{
	UInt16 length = StrLen(toAppend);
	if (!length) return;
	UInt16 newLen = m_dataLen + length;
	if (m_bufLen < newLen)
	{
		m_bufLen = newLen;
		char* newStr = Game_HeapAlloc(m_bufLen + 1);
		if (m_data)
		{
			COPY_BYTES(newStr, m_data, m_dataLen);
			Game_HeapFree(m_data);
		}
		m_data = newStr;
	}
	COPY_BYTES(m_data + m_dataLen, toAppend, length + 1);
	m_dataLen = newLen;
}

void String::AppendChar(char toAppend)
{
	if (m_bufLen == m_dataLen)
	{
		m_bufLen++;
		char* newStr = Game_HeapAlloc(m_bufLen + 1);
		if (m_data)
		{
			COPY_BYTES(newStr, m_data, m_dataLen);
			Game_HeapFree(m_data);
		}
		m_data = newStr;
	}
	m_data[m_dataLen++] = toAppend;
	m_data[m_dataLen] = 0;
}

void String::InsertChar(char toInsert, UInt32 index)
{
	if (index > m_dataLen) return;
	if (m_bufLen == m_dataLen)
	{
		m_bufLen++;
		char* newStr = Game_HeapAlloc(m_bufLen + 1);
		if (m_data)
		{
			COPY_BYTES(newStr, m_data, m_dataLen);
			Game_HeapFree(m_data);
		}
		m_data = newStr;
	}
	if (index < m_dataLen)
		MemCopy(m_data + index + 1, m_data + index, m_dataLen - index);
	m_dataLen++;
	m_data[index] = toInsert;
	m_data[m_dataLen] = 0;
}

void String::EraseAt(UInt32 index)
{
	if (!m_data || (index >= m_dataLen)) return;
	m_dataLen--;
	if (m_dataLen)
	{
		UInt32 length = m_dataLen - index;
		if (!length)
			m_data[m_dataLen] = 0;
		else
			COPY_BYTES(m_data + index, m_data + index + 1, length + 1);
	}
	else *m_data = 0;
}

const char* String::CStr()
{
	return (m_data && m_dataLen) ? m_data : "";
}

__declspec(naked) UInt32 __fastcall StrLen(const char* str)
{
	__asm
	{
		test	ecx, ecx
		jz		nullPtr
		push	ecx
		xorps	xmm7, xmm7
		mov		edx, ecx
		and ecx, 0xF
		sub		edx, ecx
		or eax, 0xFFFFFFFF
		shl		eax, cl
		ALIGN 16
		iterHead:
		movaps	xmm6, [edx]
			pcmpeqb	xmm6, xmm7
			pmovmskb	ecx, xmm6
			and ecx, eax
			jnz		foundEnd
			or eax, 0xFFFFFFFF
			add		edx, 0x10
			jmp		iterHead
			foundEnd :
		bsf		eax, ecx
			add		eax, edx
			mov		edx, eax
			pop		ecx
			sub		eax, ecx
			retn
			nullPtr :
		xor edx, edx
			xor eax, eax
			retn
	}
}

MemoryPool s_memoryPool;

size_t MemoryPool::GetTotalAllocSize() { return s_memoryPool.m_allocPoolCount * MEMORY_POOL_SIZE; }

__declspec(naked) void* __fastcall MemoryPool::Alloc(size_t size)
{
	__asm
	{
		cmp		ecx, MAX_BLOCK_SIZE
		ja		doMalloc
		push	esi
		mov		esi, ecx
		mov		ecx, offset s_memoryPool.m_cs
		call	PrimitiveCS::Enter
		mov		edx, esi
		shr		edx, 2
		lea		edx, [eax + edx + 4]
		mov		eax, [edx]
		test	eax, eax
		jz		newSection
		mov		ecx, [eax]
		mov[edx], ecx
		and s_memoryPool.m_cs.selfPtr, 0
		pop		esi
		retn
		ALIGN 16
		doMalloc:
		push	0x10
			push	ecx
			call	_aligned_malloc
			add		esp, 8
			retn
			newSection :
		push	edi
			push	edx
			mov		edi, s_memoryPool.m_freeSections
			test	edi, edi
			jz		newPool
			mov		eax, [edi]
			mov		s_memoryPool.m_freeSections, eax
			jmp		gotSection
			newPool :
		push	MEMORY_POOL_SIZE + 0x40
			call	malloc
			pop		ecx
			add		eax, 0x20
			and eax, 0xFFFFFFF0
			mov		edi, eax
			mov		ecx, offset s_memoryPool.m_freeSections
			mov		edx, MEMORY_POOL_SIZE / POOL_SECTION_SIZE - 1
			sectLinker:
		add		eax, POOL_SECTION_SIZE
			mov[ecx], eax
			mov		ecx, eax
			dec		edx
			jnz		sectLinker
			mov[eax], edx
			inc		s_memoryPool.m_allocPoolCount
			gotSection :
		pop		ecx
			lea		eax, [esi + edi]
			lea		edx, [edi + POOL_SECTION_SIZE]
			sub		edx, esi
			ALIGN 16
			blockLinker :
			mov[ecx], eax
			mov		ecx, eax
			add		eax, esi
			cmp		eax, edx
			jbe		blockLinker
			add		edx, esi
			xor esi, esi
			mov[ecx], esi
			sub		edx, eax
			jbe		done
			push	eax
			test	edx, 0xFFFFFF00
			setnz	cl
			or cl, 4
			shr		edx, cl
			or esi, 1
			shl		esi, cl
			ALIGN 16
			surplIter:
		mov		ecx, eax
			add		eax, esi
			mov[ecx], eax
			dec		edx
			ja		surplIter
			shr		esi, 2
			lea		eax, s_memoryPool.m_freeSections[esi]
			mov		edx, [eax]
			mov[ecx], edx
			pop		dword ptr[eax]
			done:
		and s_memoryPool.m_cs.selfPtr, 0
			mov		eax, edi
			pop		edi
			pop		esi
			retn
	}
}

__declspec(naked) void __fastcall MemoryPool::Free(void* pBlock, size_t size)
{
	__asm
	{
		test	ecx, ecx
		jz		nullPtr
		cmp		edx, MAX_BLOCK_SIZE
		ja		doFree
		push	edx
		push	ecx
		mov		ecx, offset s_memoryPool.m_cs
		call	PrimitiveCS::Enter
		pop		ecx
		pop		edx
		shr		edx, 2
		lea		edx, [eax + edx + 4]
		mov		eax, [edx]
		mov[ecx], eax
		mov[edx], ecx
		and s_memoryPool.m_cs.selfPtr, 0
		nullPtr:
		retn
			ALIGN 16
			doFree :
			push	ecx
			call	_aligned_free
			pop		ecx
			retn
	}
}

__declspec(naked) void* __fastcall MemoryPool::Realloc(void* pBlock, size_t curSize, size_t reqSize)
{
	__asm
	{
		mov		eax, ecx
		mov		ecx, [esp + 4]
		test	eax, eax
		jz		nullPtr
		cmp		ecx, edx
		jbe		done
		cmp		edx, MAX_BLOCK_SIZE
		ja		doRealloc
		push	esi
		push	edi
		mov		esi, eax
		mov		edi, edx
		call	MemoryPool::Alloc
		mov		ecx, edi
		mov		edx, ecx
		shr		ecx, 2
		mov		edi, eax
		rep movsd
		mov		edi, eax
		mov		ecx, esi
		sub		ecx, edx
		call	MemoryPool::Free
		mov		eax, edi
		pop		edi
		pop		esi
		retn	4
		ALIGN 16
		doRealloc:
		push	0x10
			push	ecx
			push	eax
			call	_aligned_realloc
			add		esp, 0xC
			retn	4
			ALIGN 16
			nullPtr:
		call	MemoryPool::Alloc
			done :
		retn	4
	}
}