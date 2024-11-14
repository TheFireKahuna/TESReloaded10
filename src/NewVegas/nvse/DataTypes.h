#pragma once

extern const __m128i kPackedValues[];
extern const char kLwrCaseConverter[];

#define GET_PS(i)	((const __m128*)kPackedValues)[i]
#define GET_SS(i)	((const float*)kPackedValues)[i << 2]

// 08
struct NiPoint2
{
	float	x, y;

	NiPoint2() {}
	__forceinline NiPoint2(float _x, float _y) : x(_x), y(_y) {}
	__forceinline NiPoint2(const NiPoint2& rhs) { *this = rhs; }
	__forceinline explicit NiPoint2(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiPoint2&& rhs)
	{
		x = rhs.x;
		y = rhs.y;
	}
	__forceinline void operator=(const NiPoint2& rhs) { _mm_storeu_si64(this, _mm_loadu_si64(&rhs)); }

	__forceinline NiPoint2& SetPS(const __m128 rhs)
	{
		_mm_storeu_si64(this, _mm_castps_si128(rhs));
		return *this;
	}

	inline operator float* () { return &x; }

	__forceinline __m128 PS() const { return _mm_castsi128_ps(_mm_loadu_si64(this)); }
};

// 0C
struct NiVector3
{
	float	x, y, z;

	NiVector3() {}
	//__forceinline NiVector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	__forceinline NiVector3(const NiVector3& rhs) { *this = rhs; }
	__forceinline explicit NiVector3(const NiMatrix33& rhs) { *this = rhs; }
	__forceinline explicit NiVector3(const NiQuaternion& rhs) { *this = rhs; }
	__forceinline explicit NiVector3(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiVector3&& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}

	__forceinline void operator=(const NiVector3& rhs)
	{
		_mm_storeu_si64(this, _mm_loadu_si64(&rhs));
		z = rhs.z;
	}
	__forceinline void operator=(const __m128 rhs)
	{
		_mm_storeu_si64(this, _mm_castps_si128(rhs));
		_mm_store_ss(&z, _mm_unpackhi_ps(rhs, rhs));
	}
	__forceinline void operator=(const NiPoint2& rhs) { _mm_storeu_si64(this, _mm_castps_si128(rhs.PS())); }

	void operator=(const NiMatrix33& from);
	void operator=(const NiQuaternion& from);

	__forceinline NiVector3& SetPS(const __m128 rhs)
	{
		_mm_storeu_si64(this, _mm_castps_si128(rhs));
		_mm_store_ss(&z, _mm_unpackhi_ps(rhs, rhs));
		return *this;
	}

	__forceinline __m128 operator+(__m128 packedPS) const { return PS() + packedPS; }
	__forceinline __m128 operator+(NiVector3& vec) const { return PS() + vec.PS(); }
	__forceinline __m128 operator-(__m128 packedPS) const { return PS() - packedPS; }
	__forceinline __m128 operator-(NiVector3& vec) const { return PS() - vec.PS(); }
	__forceinline __m128 operator*(__m128 packedPS) const { return PS() * packedPS; }
	__forceinline __m128 operator*(NiVector3& vec) const { return PS() * vec.PS(); }
	__forceinline __m128 operator*(float s) const { return PS() * _mm_set_ps1(s); }

	__forceinline NiVector3& operator+=(const NiVector3& vec) { return SetPS(*this + vec.PS()); }
	__forceinline NiVector3& operator+=(__m128 packedPS) { return SetPS(*this + packedPS); }
	__forceinline NiVector3& operator-=(const NiVector3& vec) { return SetPS(*this - vec.PS()); }
	__forceinline NiVector3& operator-=(__m128 packedPS) { return SetPS(*this - packedPS); }
	__forceinline NiVector3& operator*=(__m128 packedPS) { return SetPS(*this * packedPS); }
	__forceinline NiVector3& operator*=(float s) { return SetPS(*this * s); }

	inline operator float* () { return &x; }
	inline operator NiPoint2& () const { return *(NiPoint2*)this; }

	__forceinline __m128 PS() const { return _mm_loadu_ps(&x); }
	__forceinline __m128 PS2() const { return _mm_castsi128_ps(_mm_loadu_si64(this)); }
	__forceinline __m128 PS3() const { return PS() & GET_PS(4); }

	inline bool operator==(const NiVector3& rhs) const { return Equal_V3(PS(), rhs.PS()); }
	inline bool operator!=(const NiVector3& rhs) const { return !(*this == rhs); }

	inline float __vectorcall DotProduct(const NiVector3& rhs) const
	{
		__m128 k = _mm_setzero_ps();
		return _mm_cvtss_f32(_mm_hadd_ps(_mm_hadd_ps(PS3() * rhs.PS(), k), k));
	}

	inline float __vectorcall Length() const { return Length_V4(PS3()); }
	NiVector3& Normalize();
	float __vectorcall CrossProductF(const NiVector3& vB) { return x * vB.x + y * vB.y + z * vB.z; };
	__m128 __vectorcall CrossProduct(const NiVector3& vB) const;
	__m128 __vectorcall Interpolate(const NiVector3& vB, float t) const;
	__m128 __vectorcall GetTranslatedPos(const NiTransform& transfrm) const;
	__m128 __vectorcall GetRotatedPos(__m128 pry, const NiVector3& origin) const;

	inline float __vectorcall GetAngleBetweenV3(const NiVector3& vB) const
	{
		return ACos(DotProduct(vB));
	}
	void GetLookAt(NiVector3* LookAt, NiVector3* Rotation);


	D3DXVECTOR3		toD3DXVEC3() {
		return D3DXVECTOR3(x, y, z);
	}
	D3DXVECTOR4		toD3DXVEC4() {
		return D3DXVECTOR4(x, y, z, 1.0);
	}
};
typedef NiVector3 NiPoint3;

float __vectorcall Point2Distance(const NiVector3& pt1, const NiVector3& pt2);
float __vectorcall Point3Distance(const NiVector3& pt1, const NiVector3& pt2);
__m128 __vectorcall GetAnglesBetweenPoints(__m128 pt1, __m128 pt2);
__m128 __vectorcall TransformWorldToLocal(const NiVector3& origin, const NiVector3& target, __m128 rotation);

struct NiVector4
{
	float	x, y, z, w;

	NiVector4() {}
	__forceinline NiVector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	__forceinline NiVector4(const NiVector4& rhs) { *this = rhs; }
	__forceinline NiVector4(const NiVector3& rhs) { *this = rhs; }
	__forceinline explicit NiVector4(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiVector4&& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
	}
	__forceinline void operator=(const NiVector4& rhs) { SetPS(rhs.PS()); }
	__forceinline void operator=(const NiVector3& rhs) { SetPS(rhs.PS3()); }

	__forceinline NiVector4& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&x, rhs);
		return *this;
	}

	__forceinline __m128 operator+(const NiVector3& rhs) const { return PS() + rhs.PS3(); }
	__forceinline __m128 operator+(const NiVector4& vec) const { return PS() + vec.PS(); }
	__forceinline __m128 operator+(__m128 packedPS) const { return PS() + packedPS; }

	__forceinline __m128 operator-(const NiVector3& rhs) const { return PS() - rhs.PS3(); }
	__forceinline __m128 operator-(const NiVector4& vec) const { return PS() - vec.PS(); }
	__forceinline __m128 operator-(__m128 packedPS) const { return PS() * packedPS; }

	__forceinline __m128 operator*(float s) const { return PS() * _mm_set_ps1(s); }
	__forceinline __m128 operator*(const NiVector3& rhs) const { return PS() * rhs.PS3(); }
	__forceinline __m128 operator*(const NiVector4& vec) const { return PS() * vec.PS(); }
	__forceinline __m128 operator*(__m128 packedPS) const { return PS() * packedPS; }

	__forceinline NiVector4& operator+=(const NiVector3& rhs) { return SetPS(*this + rhs); }
	__forceinline NiVector4& operator+=(const NiVector4& vec) { return SetPS(*this + vec.PS()); }
	__forceinline NiVector4& operator+=(__m128 packedPS) { return SetPS(*this + packedPS); }

	__forceinline NiVector4& operator-=(const NiVector3& rhs) { return SetPS(*this - rhs); }
	__forceinline NiVector4& operator-=(const NiVector4& vec) { return SetPS(*this - vec.PS()); }
	__forceinline NiVector4& operator-=(__m128 packedPS) { return SetPS(*this - packedPS); }

	__forceinline NiVector4& operator*=(float s) { return SetPS(*this * s); }
	__forceinline NiVector4& operator*=(const NiVector3& rhs) { return SetPS(*this * rhs); }
	__forceinline NiVector4& operator*=(const NiVector4& vec) { return SetPS(*this * vec.PS()); }
	__forceinline NiVector4& operator*=(__m128 packedPS) { return SetPS(*this * packedPS); }

	inline operator float* () { return &x; }
	inline operator NiVector3& () const { return *(NiVector3*)this; }

	__forceinline __m128 PS() const { return _mm_loadu_ps(&x); }

	D3DXVECTOR4		toD3DXVEC4() {
		return D3DXVECTOR4(x, y, z, w);
	}
};

struct alignas(16) AlignedVector4
{
	float	x, y, z, w;

	AlignedVector4() {}
	__forceinline AlignedVector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	__forceinline AlignedVector4(const AlignedVector4& rhs) { *this = rhs; }
	__forceinline AlignedVector4(const NiVector4& rhs) { *this = rhs; }
	__forceinline explicit AlignedVector4(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(AlignedVector4&& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
	}
	__forceinline void operator=(const AlignedVector4& rhs) { SetPS(rhs.PS()); }
	__forceinline void operator=(const NiVector4& rhs) { SetPS(rhs.PS()); }

	__forceinline AlignedVector4& SetPS(const __m128 rhs)
	{
		_mm_store_ps(&x, rhs);
		return *this;
	}

	__forceinline __m128 operator+(__m128 packedPS) const { return PS() + packedPS; }
	__forceinline __m128 operator-(__m128 packedPS) const { return PS() - packedPS; }
	__forceinline __m128 operator*(float s) const { return PS() * _mm_set_ps1(s); }
	__forceinline __m128 operator*(__m128 packedPS) const { return PS() * packedPS; }

	__forceinline AlignedVector4& operator+=(__m128 packedPS) { return SetPS(*this + packedPS); }
	__forceinline AlignedVector4& operator-=(__m128 packedPS) { return SetPS(*this - packedPS); }
	__forceinline AlignedVector4& operator*=(float s) { return SetPS(*this * s); }
	__forceinline AlignedVector4& operator*=(__m128 packedPS) { return SetPS(*this * packedPS); }

	inline operator float* () { return &x; }
	inline operator NiVector3& () const { return *(NiVector3*)this; }

	__forceinline __m128 PS() const { return _mm_load_ps(&x); }
};

// 10
struct AxisAngle
{
	NiVector3	axis;
	float		theta;

	AxisAngle() {}
	__forceinline AxisAngle(float x, float y, float z, float t) : axis({ x, y, z }), theta(t) {}
	__forceinline explicit AxisAngle(const NiVector3& _axis, float t) : axis(_axis), theta(t) {}
	__forceinline AxisAngle(const AxisAngle& from) { *this = from; }
	__forceinline explicit AxisAngle(const NiVector3& pry) { *this = pry; }
	__forceinline explicit AxisAngle(const NiMatrix33& rotMat) { *this = rotMat; }
	__forceinline explicit AxisAngle(const NiQuaternion& qt) { *this = qt; }
	__forceinline explicit AxisAngle(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(AxisAngle&& from)
	{
		axis.x = from.axis.x;
		axis.y = from.axis.y;
		axis.z = from.axis.z;
		theta = from.theta;
	}
	__forceinline void operator=(const AxisAngle& from) { SetPS(from.PS()); }
	__forceinline void operator=(const NiVector3& pry) { FromEulerPRY(pry.PS()); }
	__forceinline void operator=(const NiMatrix33& rotMat) { FromRotationMatrix(rotMat); }
	__forceinline void operator=(const NiQuaternion& qt) { FromQuaternion(qt); }

	__forceinline AxisAngle& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&axis.x, rhs);
		return *this;
	}

	inline operator float* () { return &axis.x; }
	__forceinline __m128 PS() const { return _mm_loadu_ps(&axis.x); }

	__forceinline bool operator==(const AxisAngle& rhs) const { return Equal_V4(PS(), rhs.PS()); }
	__forceinline bool operator!=(const AxisAngle& rhs) const { return !(*this == rhs); }

	AxisAngle& __vectorcall FromEulerPRY(__m128 pry);
	AxisAngle& __fastcall FromRotationMatrix(const NiMatrix33& rotMat);
	AxisAngle& __fastcall FromQuaternion(const NiQuaternion& qt);

	__m128 __vectorcall ToEulerPRY() const;

	void Dump() const;
};

// 24
struct NiMatrix33
{
	float	data[3][3];

	NiMatrix33() {}
	__forceinline NiMatrix33(float m00, float m10, float m20, float m01, float m11, float m21, float m02, float m12, float m22)
	{
		data[0][0] = m00; data[0][1] = m10; data[0][2] = m20;
		data[0][3] = m01; data[0][4] = m11; data[0][5] = m21;
		data[0][6] = m02; data[0][7] = m12; data[0][8] = m22;
	}
	__forceinline explicit NiMatrix33(const NiMatrix33& from) { *this = from; }
	__forceinline explicit NiMatrix33(const NiVector3& pry) { *this = pry; }
	__forceinline explicit NiMatrix33(__m128 pry) { *this = pry; }
	__forceinline explicit NiMatrix33(const NiQuaternion& qt) { *this = qt; }
	__forceinline explicit NiMatrix33(const AxisAngle& axisAngle) { *this = axisAngle; }

	__forceinline void operator=(NiMatrix33&& rhs)
	{
		data[0][0] = rhs.data[0][0]; data[0][1] = rhs.data[0][1]; data[0][2] = rhs.data[0][2];
		data[0][3] = rhs.data[0][3]; data[0][4] = rhs.data[0][4]; data[0][5] = rhs.data[0][5];
		data[0][6] = rhs.data[0][6]; data[0][7] = rhs.data[0][7]; data[0][8] = rhs.data[0][8];
	}
	__forceinline NiMatrix33(NiMatrix33& rhs)
	{
		data[0][0] = rhs.data[0][0]; data[0][1] = rhs.data[0][1]; data[0][2] = rhs.data[0][2];
		data[0][3] = rhs.data[0][3]; data[0][4] = rhs.data[0][4]; data[0][5] = rhs.data[0][5];
		data[0][6] = rhs.data[0][6]; data[0][7] = rhs.data[0][7]; data[0][8] = rhs.data[0][8];
	}
	__forceinline NiMatrix33(const NiMatrix33&& rhs)
	{
		data[0][0] = rhs.data[0][0]; data[0][1] = rhs.data[0][1]; data[0][2] = rhs.data[0][2];
		data[0][3] = rhs.data[0][3]; data[0][4] = rhs.data[0][4]; data[0][5] = rhs.data[0][5];
		data[0][6] = rhs.data[0][6]; data[0][7] = rhs.data[0][7]; data[0][8] = rhs.data[0][8];
	}
	__forceinline NiMatrix33(NiMatrix33* rhs)
	{
		data[0][0] = rhs->data[0][0]; data[0][1] = rhs->data[0][1]; data[0][2] = rhs->data[0][2];
		data[0][3] = rhs->data[0][3]; data[0][4] = rhs->data[0][4]; data[0][5] = rhs->data[0][5];
		data[0][6] = rhs->data[0][6]; data[0][7] = rhs->data[0][7]; data[0][8] = rhs->data[0][8];
	}
	__forceinline void operator=(const NiMatrix33& rhs)
	{
		_mm_storeu_ps(&data[0][0], _mm_loadu_ps(&rhs.data[0][0]));
		_mm_storeu_ps(&data[0][4], _mm_loadu_ps(&rhs.data[0][4]));
		data[0][8] = rhs.data[0][8];
	}
	__forceinline void operator=(const NiVector3& pry) { FromEulerPRY(pry.PS()); }
	__forceinline void operator=(__m128 pry) { FromEulerPRY(pry); }
	__forceinline void operator=(const NiQuaternion& qt) { FromQuaternion(qt); }
	__forceinline void operator=(const AxisAngle& axisAngle) { FromAxisAngle(axisAngle); }

	__forceinline NiMatrix33& operator*=(const NiMatrix33& rhs) { return MultiplyMatrices(rhs); }
	__forceinline __m128 operator*(__m128 vec) const { return MultiplyVector(vec); }
	__forceinline __m128 operator*(const NiVector3& vec) const { return MultiplyVector(vec.PS()); }

	inline operator float* () const { return (float*)this; }

	inline bool operator==(const NiMatrix33& rhs) const
	{
		for (UInt32 i = 0; i < 9; i++)
			if (fabs((*this)[i] - rhs[i]) > 0.00001F)
				return false;
		return true;
	}
	NiMatrix33 operator * (const NiMatrix33 mat) const {
		NiMatrix33 prd;

		prd.data[0][0] =
			data[0][0] * mat.data[0][0] +
			data[0][1] * mat.data[1][0] +
			data[0][2] * mat.data[2][0];
		prd.data[1][0] =
			data[1][0] * mat.data[0][0] +
			data[1][1] * mat.data[1][0] +
			data[1][2] * mat.data[2][0];
		prd.data[2][0] =
			data[2][0] * mat.data[0][0] +
			data[2][1] * mat.data[1][0] +
			data[2][2] * mat.data[2][0];
		prd.data[0][1] =
			data[0][0] * mat.data[0][1] +
			data[0][1] * mat.data[1][1] +
			data[0][2] * mat.data[2][1];
		prd.data[1][1] =
			data[1][0] * mat.data[0][1] +
			data[1][1] * mat.data[1][1] +
			data[1][2] * mat.data[2][1];
		prd.data[2][1] =
			data[2][0] * mat.data[0][1] +
			data[2][1] * mat.data[1][1] +
			data[2][2] * mat.data[2][1];
		prd.data[0][2] =
			data[0][0] * mat.data[0][2] +
			data[0][1] * mat.data[1][2] +
			data[0][2] * mat.data[2][2];
		prd.data[1][2] =
			data[1][0] * mat.data[0][2] +
			data[1][1] * mat.data[1][2] +
			data[1][2] * mat.data[2][2];
		prd.data[2][2] =
			data[2][0] * mat.data[0][2] +
			data[2][1] * mat.data[1][2] +
			data[2][2] * mat.data[2][2];
		return prd;
	}
	NiMatrix33 operator * (const NiMatrix33&& mat) const {
		NiMatrix33 prd;

		prd.data[0][0] =
			data[0][0] * mat.data[0][0] +
			data[0][1] * mat.data[1][0] +
			data[0][2] * mat.data[2][0];
		prd.data[1][0] =
			data[1][0] * mat.data[0][0] +
			data[1][1] * mat.data[1][0] +
			data[1][2] * mat.data[2][0];
		prd.data[2][0] =
			data[2][0] * mat.data[0][0] +
			data[2][1] * mat.data[1][0] +
			data[2][2] * mat.data[2][0];
		prd.data[0][1] =
			data[0][0] * mat.data[0][1] +
			data[0][1] * mat.data[1][1] +
			data[0][2] * mat.data[2][1];
		prd.data[1][1] =
			data[1][0] * mat.data[0][1] +
			data[1][1] * mat.data[1][1] +
			data[1][2] * mat.data[2][1];
		prd.data[2][1] =
			data[2][0] * mat.data[0][1] +
			data[2][1] * mat.data[1][1] +
			data[2][2] * mat.data[2][1];
		prd.data[0][2] =
			data[0][0] * mat.data[0][2] +
			data[0][1] * mat.data[1][2] +
			data[0][2] * mat.data[2][2];
		prd.data[1][2] =
			data[1][0] * mat.data[0][2] +
			data[1][1] * mat.data[1][2] +
			data[1][2] * mat.data[2][2];
		prd.data[2][2] =
			data[2][0] * mat.data[0][2] +
			data[2][1] * mat.data[1][2] +
			data[2][2] * mat.data[2][2];
		return prd;
	}
	NiMatrix33 operator * (NiMatrix33* mat) const {
		NiMatrix33 prd;

		prd.data[0][0] =
			data[0][0] * mat->data[0][0] +
			data[0][1] * mat->data[1][0] +
			data[0][2] * mat->data[2][0];
		prd.data[1][0] =
			data[1][0] * mat->data[0][0] +
			data[1][1] * mat->data[1][0] +
			data[1][2] * mat->data[2][0];
		prd.data[2][0] =
			data[2][0] * mat->data[0][0] +
			data[2][1] * mat->data[1][0] +
			data[2][2] * mat->data[2][0];
		prd.data[0][1] =
			data[0][0] * mat->data[0][1] +
			data[0][1] * mat->data[1][1] +
			data[0][2] * mat->data[2][1];
		prd.data[1][1] =
			data[1][0] * mat->data[0][1] +
			data[1][1] * mat->data[1][1] +
			data[1][2] * mat->data[2][1];
		prd.data[2][1] =
			data[2][0] * mat->data[0][1] +
			data[2][1] * mat->data[1][1] +
			data[2][2] * mat->data[2][1];
		prd.data[0][2] =
			data[0][0] * mat->data[0][2] +
			data[0][1] * mat->data[1][2] +
			data[0][2] * mat->data[2][2];
		prd.data[1][2] =
			data[1][0] * mat->data[0][2] +
			data[1][1] * mat->data[1][2] +
			data[1][2] * mat->data[2][2];
		prd.data[2][2] =
			data[2][0] * mat->data[0][2] +
			data[2][1] * mat->data[1][2] +
			data[2][2] * mat->data[2][2];
		return prd;
	}

	__m128 __vectorcall ToEulerPRY() const;
	__m128 __vectorcall ToEulerPRYInv() const;
	float __vectorcall ExtractPitch() const;
	float __vectorcall ExtractRoll() const;
	float __vectorcall ExtractYaw() const;
	NiMatrix33& __vectorcall FromEulerPRY(__m128 pry);
	NiMatrix33& __vectorcall FromEulerPRYInv(__m128 pry);
	NiMatrix33& __vectorcall FromEulerYPR(__m128 pry);
	float __vectorcall From2Points(__m128 pt1, __m128 pt2);
	NiMatrix33& __fastcall FromQuaternion(const NiQuaternion& qt);
	NiMatrix33& __fastcall FromAxisAngle(const AxisAngle& axisAngle);
	__m128 __vectorcall MultiplyVector(__m128 vec) const;
	__m128 __vectorcall MultiplyVectorInv(__m128 vec) const;
	__m128 __vectorcall MultiplyVectorRow(__m128 vec, UInt32 whichRow) const;
	NiMatrix33& __fastcall MultiplyMatrices(const NiMatrix33& matB);
	NiMatrix33& __fastcall MultiplyMatricesInv(const NiMatrix33& matB);
	NiMatrix33& __vectorcall Rotate(__m128 rot);
	NiMatrix33& __fastcall Transpose(NiMatrix33& out);

	void GenerateRotationMatrixZXY(NiVector3* v, bool degrees);
};


// 10
struct NiQuaternion
{
	float	w, x, y, z;

	NiQuaternion() {}
	__forceinline NiQuaternion(float _w, float _x, float _y, float _z) : w(_w), x(_x), y(_y), z(_z) {}
	__forceinline NiQuaternion(const NiQuaternion& from) { *this = from; }
	__forceinline explicit NiQuaternion(const NiMatrix33& rotMat) { *this = rotMat; }
	__forceinline explicit NiQuaternion(const NiVector3& pry) { *this = pry; }
	__forceinline explicit NiQuaternion(const AxisAngle& axisAngle) { *this = axisAngle; }
	__forceinline explicit NiQuaternion(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiQuaternion&& rhs)
	{
		w = rhs.w;
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}
	__forceinline void operator=(const NiQuaternion& rhs) { SetPS(rhs.PS()); }
	__forceinline void operator=(const NiMatrix33& rotMat) { FromRotationMatrix(rotMat); }
	__forceinline void operator=(const NiVector3& pry) { FromEulerPRY(pry.PS()); }
	__forceinline void operator=(const AxisAngle& axisAngle) { FromAxisAngle(axisAngle); }

	__forceinline NiQuaternion& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&w, rhs);
		return *this;
	}

	__forceinline __m128 operator+(const NiQuaternion& rhs) const { return PS() + rhs.PS(); }

	__forceinline __m128 operator-(const NiQuaternion& rhs) const { return PS() - rhs.PS(); }

	__forceinline __m128 operator*(float s) const { return PS() * _mm_set_ps1(s); }
	__forceinline __m128 operator*(const NiQuaternion& rhs) const { return MultiplyQuaternion(rhs); }
	__forceinline __m128 operator*(NiVector4& vec) const { return MultiplyVector(vec.PS()); }
	__forceinline __m128 operator*(__m128 vec) const { return MultiplyVector(vec); }

	__forceinline NiQuaternion& operator+=(const NiQuaternion& rhs) { return SetPS(*this + rhs); }
	__forceinline NiQuaternion& operator-=(const NiQuaternion& rhs) { return SetPS(*this - rhs); }
	__forceinline NiQuaternion& operator*=(float s) { return SetPS(*this * s); }
	__forceinline NiQuaternion& operator*=(const NiQuaternion& rhs) { return SetPS(*this * rhs); }

	__forceinline bool operator==(const NiQuaternion& rhs) const { return Equal_V4(PS(), rhs.PS()); }
	__forceinline bool operator!=(const NiQuaternion& rhs) const { return !(*this == rhs); }

	inline operator float* () { return &w; }
	__forceinline __m128 PS() const { return _mm_loadu_ps(&w); }

	NiQuaternion& __vectorcall FromEulerPRY(__m128 pry);
	NiQuaternion& __vectorcall FromEulerYPR(__m128 pry);
	NiQuaternion& __fastcall FromRotationMatrix(const NiMatrix33& rotMat);
	float __vectorcall From2Points(__m128 pt1, __m128 pt2);
	NiQuaternion& __fastcall FromAxisAngle(const AxisAngle& axisAngle);

	__m128 __vectorcall MultiplyVector(__m128 vec) const;
	__m128 __vectorcall MultiplyQuaternion(const NiQuaternion& rhs) const;

	__forceinline float __vectorcall DotProduct(const NiQuaternion& rhs) const
	{
		__m128 k = _mm_setzero_ps();
		return _mm_cvtss_f32(_mm_hadd_ps(_mm_hadd_ps(PS() * rhs.PS(), k), k));
	}

	NiQuaternion& Normalize() { return SetPS(Normalize_V4(PS())); }

	__m128 __vectorcall ToEulerPRY() const;
	__m128 __vectorcall ToEulerYPR() const;

	NiQuaternion& Rotate(const NiQuaternion& rot) { return *this *= rot; }
	NiQuaternion& Rotate(const AxisAngle& rot) { return *this *= NiQuaternion(rot); }
	NiQuaternion& __vectorcall Rotate(__m128 pry);

	__m128 __vectorcall Lerp(const NiQuaternion& qb, float t) const;
	__m128 __vectorcall Slerp(const NiQuaternion& qb, float t) const;

	void Dump() const;
};


// 34
struct NiTransform
{
	NiMatrix33	rotate;		// 00
	NiVector3	translate;	// 24
	float		scale;		// 30

	NiTransform() : scale(0) {}
	NiTransform(const NiTransform& from) { *this = from; }
	NiTransform(const NiMatrix33& rot, const NiVector3& trnsl)
	{
		rotate = rot;
		_mm_storeu_ps(translate, trnsl.PS());
		scale = 0;
	}
	NiTransform(const NiVector3& pry, const NiVector3& trnsl)
	{
		rotate.FromEulerPRYInv(pry.PS());
		_mm_storeu_ps(translate, trnsl.PS());
		scale = 0;
	}

	inline void operator=(const NiTransform& rhs)
	{
		float* d = rotate, * s = rhs.rotate;
		_mm_storeu_ps(d, _mm_loadu_ps(s));
		_mm_storeu_ps(d + 4, _mm_loadu_ps(s + 4));
		_mm_storeu_ps(d + 8, _mm_loadu_ps(s + 8));
	}

	NiTransform& __fastcall RotateOrigin(const NiTransform& origin);
	NiTransform& __fastcall GetInverse(NiTransform& out);
	void Dump() const;
};

// 20
struct NiQuatTransform
{
	NiQuaternion	rotate;		// 00
	NiVector3		translate;	// 10
	float			scale;		// 1C
};

// 10
struct NiPlane
{
	NiVector3	Normal;
	float		Constant;

	enum Sides
	{
		kSide_None,
		kSide_Positive,
		kSide_Negative
	};

	NiPlane() {}
	__forceinline NiPlane(float nX, float nY, float nZ, float offs) : Normal({ nX, nY, nZ }), Constant(offs) {}
	__forceinline NiPlane(const NiPlane& rhs) { *this = rhs; }
	__forceinline explicit NiPlane(const NiVector3& _nrm, const NiVector3& point) : Normal(_nrm), Constant(_nrm.DotProduct(point)) {}
	__forceinline explicit NiPlane(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiPlane&& rhs)
	{
		Normal.x = rhs.Normal.x;
		Normal.y = rhs.Normal.y;
		Normal.z = rhs.Normal.z;
		Constant = rhs.Constant;
	}
	__forceinline void operator=(const NiPlane& rhs) { SetPS(rhs.PS()); }

	__forceinline NiPlane& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&Normal.x, rhs);
		return *this;
	}

	inline operator float* () { return &Normal.x; }
	__forceinline __m128 PS() const { return _mm_loadu_ps(&Normal.x); }

	UInt8 __fastcall CalculateSide(const NiVector3& point) const;
};

// 10
struct NiBound
{
	NiVector3	Center;
	float		Radius;

	NiBound() {}
	__forceinline NiBound(float cX, float cY, float cZ, float rad) : Center({ cX, cY, cZ }), Radius(rad) {}
	__forceinline NiBound(const NiBound& rhs) { *this = rhs; }
	__forceinline explicit NiBound(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiBound&& rhs)
	{
		Center.x = rhs.Center.x;
		Center.y = rhs.Center.y;
		Center.z = rhs.Center.z;
		Radius = rhs.Radius;
	}
	__forceinline void operator=(const NiBound& rhs) { SetPS(rhs.PS()); }

	__forceinline NiBound& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&Center.x, rhs);
		return *this;
	}

	inline operator float* () { return &Center.x; }
	__forceinline __m128 PS() const { return _mm_loadu_ps(&Center.x); }

	UInt8 __fastcall CalculateSide(const NiPlane& plane) const;
	void Merge(const NiBound* other) { ThisStdCall(0xA7F3F0, this, other); }
	UInt32 WhichSide(NiPlane* Plane);
};

// 10
struct NiViewport
{
	float	l, r, t, b;

	NiViewport() {}
	__forceinline NiViewport(float _l, float _r, float _t, float _b) : l(_l), r(_r), t(_t), b(_b) {}
	__forceinline explicit NiViewport(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiViewport&& rhs)
	{
		l = rhs.l;
		r = rhs.r;
		t = rhs.t;
		b = rhs.b;
	}

	__forceinline NiViewport& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&l, rhs);
		return *this;
	}

	__forceinline __m128 PS() const { return _mm_loadu_ps(&l); }

	void __vectorcall SetFOV(float fov);
};

// 1C
struct NiFrustum
{
	float	Left;		// 00
	float	Right;		// 04
	float	Top;		// 08
	float	Bottom;		// 0C
	float	Near;		// 10
	float	Far;		// 14
	UInt8	Ortho;		// 18
	UInt8	pad18[3];
};
static_assert(sizeof(NiFrustum) == 0x01C);
// C
struct NiColor
{
	float	r, g, b;

	NiColor() {}
	__forceinline NiColor(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
	__forceinline NiColor(const NiColor& rhs) { *this = rhs; }
	__forceinline explicit NiColor(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiColor&& rhs)
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
	}
	__forceinline void operator=(const NiColor& rhs)
	{
		_mm_storeu_si64(this, _mm_loadu_si64(&rhs));
		b = rhs.b;
	}

	__forceinline NiColor& SetPS(const __m128 rhs)
	{
		_mm_storeu_si64(this, _mm_castps_si128(rhs));
		_mm_store_ss(&r, _mm_unpackhi_ps(rhs, rhs));
		return *this;
	}

	inline operator float* () { return &r; }
	__forceinline __m128 PS() const { return _mm_loadu_ps(&r); }

	void Dump() const;
};

// 10
struct NiColorAlpha
{
	float	r, g, b, a;

	NiColorAlpha() {}
	__forceinline NiColorAlpha(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
	__forceinline NiColorAlpha(const NiColorAlpha& rhs) { *this = rhs; }
	__forceinline explicit NiColorAlpha(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiColorAlpha&& rgba)
	{
		r = rgba.r;
		g = rgba.g;
		b = rgba.b;
		a = rgba.a;
	}
	__forceinline void operator=(const NiColorAlpha& rhs) { SetPS(rhs.PS()); }

	__forceinline NiColorAlpha& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&r, rhs);
		return *this;
	}

	__forceinline NiColorAlpha& operator*=(float value) { return SetPS(_mm_mul_ps(PS(), _mm_set_ps1(value))); }

	inline operator float* () { return &r; }
	__forceinline __m128 PS() const { return _mm_loadu_ps(&r); }
};

// 64
struct NiFrustumPlanes
{
	enum PlaneIndex
	{
		kPlaneIdx_Near,
		kPlaneIdx_Far,
		kPlaneIdx_Left,
		kPlaneIdx_Right,
		kPlaneIdx_Top,
		kPlaneIdx_Bottom
	};

	enum PlaneBit
	{
		kPlaneBit_Near = 1,
		kPlaneBit_Far = 2,
		kPlaneBit_Left = 4,
		kPlaneBit_Right = 8,
		kPlaneBit_Top = 0x10,
		kPlaneBit_Bottom = 0x20
	};

	NiPlane		cullingPlanes[6];
	UInt32		activePlanes;	//	Bitmask
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

const char* __cdecl GetNiFixedString(const char* inStr);

class NiFixedString
{
	const char* str;

	UInt32* Meta() const { return (UInt32*)(str - 8); }

	void Set(const char* inStr)
	{
		str = inStr;
		if (str) InterlockedIncrement(Meta());
	}

	void Unset()
	{
		if (str)
		{
			InterlockedDecrement(Meta());
			str = nullptr;
		}
	}

public:
	NiFixedString() : str(nullptr) {}
	NiFixedString(const char* inStr) : str(GetNiFixedString(inStr)) {}
	NiFixedString(const NiFixedString& inStr) { Set(inStr.str); }
	~NiFixedString() { Unset(); }

	const char* Get() const { return str ? str : "NULL"; }

	UInt32 Length() const { return str ? Meta()[1] : 0; }

	explicit operator bool() const { return str != nullptr; }

	operator const char* () const { return str; }

	const char* operator*() const { return str; }

	inline void operator=(const char* inStr)
	{
		Unset();
		str = GetNiFixedString(inStr);
	}
	inline void operator=(const NiFixedString& inStr)
	{
		if (str != inStr.str)
			Set(inStr.str);
	}

	inline bool operator==(const NiFixedString& rhs) const { return str == rhs.str; }
	inline bool operator<(const NiFixedString& rhs) const { return str < rhs.str; }

	UInt32 RefCount() const { return str ? Meta()[0] : 0; }
};


template <typename T_Key, typename T_Data> struct MappedPair
{
	T_Key		key;
	T_Data		value;
};

template <typename T_Key> class MapKey
{
	using Key_Arg = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;
	using Key_Res = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;

	T_Key		key;

public:
	__forceinline Key_Res Get() const { return key; }
	__forceinline char Compare(Key_Arg inKey) const
	{
		return (inKey < key) ? -1 : (key < inKey);
	}
	__forceinline void operator=(Key_Arg inKey) { key = inKey; }
	__forceinline bool operator==(const MapKey& other) const { return key == other.key; }
};

template <> class MapKey<const char*>
{
	const char* key;

public:
	__forceinline const char* Get() const { return key; }
	__forceinline char Compare(const char* inKey) const { return StrCompareCI(inKey, key); }
	__forceinline void operator=(const char* inKey) { key = inKey; }
	__forceinline bool operator==(const MapKey& other) const { return !Compare(other.key); }
};

template <> class MapKey<char*>
{
	char* key;

public:
	__forceinline char* Get() const { return key; }
	__forceinline char Compare(const char* inKey) const { return StrCompareCI(inKey, key); }
	__forceinline void operator=(const char* inKey) { key = CopyStringKey(inKey); }
	__forceinline bool operator==(const MapKey& other) const { return !Compare(other.key); }
	~MapKey() { FreeStringKey(key); }
};

template <> class MapKey<const SInt8*>
{
	const SInt8* key;

public:
	__forceinline const SInt8* Get() const { return key; }
	__forceinline char Compare(const SInt8* inKey) const { return StrCompareCS((const char*)inKey, (const char*)key); }
	__forceinline void operator=(const SInt8* inKey) { key = inKey; }
	__forceinline bool operator==(const MapKey& other) const { return !Compare(other.key); }
};

template <> class MapKey<SInt8*>
{
	SInt8* key;

public:
	__forceinline SInt8* Get() const { return key; }
	__forceinline char Compare(SInt8* inKey) const { return StrCompareCS((const char*)inKey, (const char*)key); }
	__forceinline void operator=(SInt8* inKey) { key = (SInt8*)CopyStringKey((const char*)inKey); }
	__forceinline bool operator==(const MapKey& other) const { return !Compare(other.key); }
	~MapKey() { FreeStringKey((char*)key); }
};

template <typename T_Data> class MapValue
{
	T_Data		value;

public:
	__forceinline T_Data* Init() { return &value; }
	__forceinline T_Data& Get() { return value; }
	__forceinline T_Data* Ptr() { return &value; }
	__forceinline bool operator==(const MapValue& other) const { return value == other.value; }
};

template <typename T_Data> class MapValue_p
{
	T_Data* value;

public:
	__forceinline T_Data* Init()
	{
		value = Pool_Alloc<T_Data>();
		return value;
	}
	__forceinline T_Data& Get() { return *value; }
	__forceinline T_Data* Ptr() { return value; }
	__forceinline bool operator==(const MapValue_p& other) const { return *value == *other.value; }
	~MapValue_p()
	{
		value->~T_Data();
		Pool_Free<T_Data>(value);
	}
};

template <typename T_Key, typename T_Data, const UInt32 _default_alloc = MAP_DEFAULT_ALLOC> class Map
{
	using M_Key = MapKey<T_Key>;
	using M_Value = std::conditional_t<(sizeof(T_Data) <= 8) || (sizeof(T_Data) <= alignof(T_Key)), MapValue<T_Data>, MapValue_p<T_Data>>;
	using Key_Arg = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;
	using Key_Res = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;
	using Data_Arg = std::conditional_t<std::is_scalar_v<T_Data>, T_Data, const T_Data&>;
	using Data_Res = std::conditional_t<std::is_scalar_v<T_Data>, T_Data, T_Data&>;
	using M_Pair = MappedPair<T_Key, T_Data>;
	using Init_List = std::initializer_list<M_Pair>;

	struct Entry
	{
		M_Key		key;
		M_Value		value;

		bool operator==(const Entry& other) const { return (key == other.key) && (value == other.value); }
	};

	Entry* entries;		// 00
	UInt32		numEntries;		// 04
	UInt32		numAlloc;		// 08

	UInt32 GetIndex(Key_Arg key) const
	{
		UInt32 lBound = 0, uBound = numEntries;
		while (lBound != uBound)
		{
			UInt32 index = (lBound + uBound) >> 1;
			char cmpr = entries[index].key.Compare(key);
			if (cmpr < 0)
				uBound = index;
			else if (cmpr > 0)
				lBound = index + 1;
			else
				return index;
		}
		return lBound | 0x80000000;
	}

	Entry* End() const { return entries + numEntries; }

public:
	Map(UInt32 _alloc = _default_alloc) : entries(nullptr), numEntries(0), numAlloc(_alloc) {}
	Map(Init_List&& inList) : entries(nullptr), numEntries(0), numAlloc(inList.size()) { InsertList(std::forward<Init_List>(inList)); }
	~Map()
	{
		if (entries)
		{
			Clear();
			Pool_CFree<Entry>(entries, numAlloc);
		}
	}

	void Destroy()
	{
		this->~Map();
		entries = nullptr;
	}

	UInt32 Size() const { return numEntries; }
	bool Empty() const { return !numEntries; }

	void operator=(Map&& rhs)
	{
		this->~Map();
		entries = rhs.entries;
		numEntries = rhs.numEntries;
		numAlloc = rhs.numAlloc;
		rhs.entries = nullptr;
		rhs.numEntries = 0;
		rhs.numAlloc = MAP_DEFAULT_ALLOC;
	}

	void operator=(const Map& rhs)
	{
		Destroy();
		if (numEntries = rhs.numEntries)
		{
			numAlloc = AlignNumAlloc<Entry>(numEntries);
			entries = Pool_CAlloc<Entry>(numAlloc);
			Entry* pEntry = entries, * pSource = rhs.entries, * pEnd = rhs.End();
			do
			{
				pEntry->key = pSource->key.Get();
				*(pEntry->value.Init()) = pSource->value.Get();
				pEntry++;
			} while (++pSource != pEnd);
		}
	}

	bool InsertKey(Key_Arg key, T_Data** outData)
	{
		UInt32 index = GetIndex(key);
		if (SInt32(index) >= 0)
		{
			*outData = entries[index].value.Ptr();
			return false;
		}
		index &= 0x7FFFFFFF;
		if (!entries)
		{
			numAlloc = AlignNumAlloc<Entry>(numAlloc);
			entries = Pool_CAlloc<Entry>(numAlloc);
		}
		else if (numAlloc <= numEntries)
		{
			UInt32 newAlloc = numAlloc << 1;
			entries = Pool_CRealloc<Entry>(entries, numAlloc, newAlloc);
			numAlloc = newAlloc;
		}
		Entry* pEntry = entries + index;
		if (UInt32 movCnt = numEntries - index)
			MemCopy(pEntry + 1, pEntry, sizeof(Entry) * movCnt);
		numEntries++;
		pEntry->key = key;
		*outData = pEntry->value.Init();
		return true;
	}

	bool Insert(Key_Arg key, T_Data** outData)
	{
		if (InsertKey(key, outData))
		{
			new (*outData) T_Data();
			return true;
		}
		return false;
	}

	T_Data& operator[](Key_Arg key)
	{
		T_Data* outData;
		if (InsertKey(key, &outData))
			new (outData) T_Data();
		return *outData;
	}

	template <typename ...Args>
	T_Data* Emplace(Key_Arg key, Args&& ...args)
	{
		T_Data* outData;
		if (InsertKey(key, &outData))
			new (outData) T_Data(std::forward<Args>(args)...);
		return outData;
	}

	void InsertList(Init_List&& inList)
	{
		T_Data* outData;
		for (auto iter = inList.begin(); iter != inList.end(); ++iter)
		{
			InsertKey(iter->key, &outData);
			*outData = std::move(iter->value);
		}
	}

	bool HasKey(Key_Arg key) const
	{
		return SInt32(GetIndex(key)) >= 0;
	}

	T_Data Get(Key_Arg key) const
	{
		static_assert(std::is_scalar_v<T_Data>);
		if (UInt32 index = GetIndex(key); SInt32(index) >= 0)
			return entries[index].value.Get();
		return NULL;
	}

	T_Data* GetPtr(Key_Arg key) const
	{
		if (UInt32 index = GetIndex(key); SInt32(index) >= 0)
			return entries[index].value.Ptr();
		return nullptr;
	}

	bool Erase(Key_Arg key)
	{
		if (UInt32 index = GetIndex(key); SInt32(index) >= 0)
		{
			Entry* pEntry = entries + index;
			pEntry->~Entry();
			numEntries--;
			if (UInt32 cpyCnt = numEntries - index)
				MemCopy(pEntry, pEntry + 1, sizeof(Entry) * cpyCnt);
			return true;
		}
		return false;
	}

	T_Data GetErase(Key_Arg key)
	{
		static_assert(std::is_scalar_v<T_Data>);
		if (UInt32 index = GetIndex(key); SInt32(index) >= 0)
		{
			Entry* pEntry = entries + index;
			T_Data outVal = pEntry->value.Get();
			pEntry->~Entry();
			numEntries--;
			if (UInt32 cpyCnt = numEntries - index)
				MemCopy(pEntry, pEntry + 1, sizeof(Entry) * cpyCnt);
			return outVal;
		}
		return NULL;
	}

	void Clear()
	{
		for (Entry* pEntry = entries, *pEnd = End(); pEntry != pEnd; pEntry++)
			pEntry->~Entry();
		numEntries &= 0;
	}

	bool operator==(const Map& other) const
	{
		if (numEntries != other.numEntries)
			return false;
		for (UInt32 i = 0; i < numEntries; i++)
			if (!(entries[i] == other.entries[i]))
				return false;
		return true;
	}

	class Iterator
	{
	protected:
		Entry* pEntry;
		UInt32		count;

		Iterator() {}

	public:
		Key_Res Key() const { return pEntry->key.Get(); }
		Data_Res operator()() const { return pEntry->value.Get(); }
		Data_Res operator*() const { return pEntry->value.Get(); }
		Data_Res operator->() const { return pEntry->value.Get(); }
		T_Data& Ref() { return pEntry->value.Get(); }

		explicit operator bool() const { return count != 0; }
		void operator++()
		{
			pEntry++;
			count--;
		}

		void Find(Map& source, Key_Arg key)
		{
			if (UInt32 index = source.GetIndex(key); SInt32(index) >= 0)
			{
				pEntry = source.entries + index;
				count = source.numEntries - index;
			}
			else count = 0;
		}

		Iterator(Map& source) : pEntry(source.entries), count(source.numEntries) {}
		Iterator(Map& source, Key_Arg key) { Find(source, key); }
	};

	class OpIterator : public Iterator
	{
		using Iterator::pEntry, Iterator::count, Iterator::Find;
	public:
		void operator--()
		{
			pEntry--;
			count--;
		}

		void Remove(Map& source, bool frwrd = true)
		{
			pEntry->~Entry();
			UInt32 size = frwrd ? (count - 1) : (source.numEntries - count);
			source.numEntries--;
			if (size)
				MemCopy(pEntry, pEntry + 1, size * sizeof(Entry));
			if (frwrd) pEntry--;
		}

		OpIterator(Map& source)
		{
			pEntry = source.entries;
			count = source.numEntries;
		}
		OpIterator(Map& source, Key_Arg key) { Find(source, key); }
		OpIterator(Map& source, Key_Arg key, bool frwrd)
		{
			if (source.numEntries)
			{
				UInt32 index = source.GetIndex(key);
				bool match = SInt32(index) >= 0;
				index &= 0x7FFFFFFF;
				if (frwrd)
				{
					pEntry = source.entries + index;
					count = source.numEntries - index;
				}
				else
				{
					pEntry = source.entries + (index - !match);
					count = index + match;
				}
			}
			else count = 0;
		}
	};

	class CpIterator : public Iterator
	{
		using Iterator::pEntry, Iterator::count;

		AuxBuffer<Entry>	buffer;

	public:
		CpIterator(Map& source)
		{
			count = source.numEntries;
			if (count > 1)
				pEntry = buffer.Copy(count, source.entries);
			else
				pEntry = source.entries;
		}
	};

	Iterator Begin() { return Iterator(*this); }
	Iterator Find(Key_Arg key) { return Iterator(*this, key); }

	OpIterator BeginOp() { return OpIterator(*this); }
	OpIterator FindOp(Key_Arg key) { return OpIterator(*this, key); }
	OpIterator FindOpDir(Key_Arg key, bool frwrd) { return OpIterator(*this, key, frwrd); }

	CpIterator BeginCp() { return CpIterator(*this); }
};

template <typename T_Key, const UInt32 _default_alloc = MAP_DEFAULT_ALLOC> class Set
{
	using M_Key = MapKey<T_Key>;
	using Key_Arg = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;
	using Key_Res = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;
	using Init_List = std::initializer_list<T_Key>;

	M_Key* keys;		// 00
	UInt32		numKeys;	// 04
	UInt32		numAlloc;	// 08

	UInt32 GetIndex(Key_Arg key) const
	{
		UInt32 lBound = 0, uBound = numKeys;
		while (lBound != uBound)
		{
			UInt32 index = (lBound + uBound) >> 1;
			char cmpr = keys[index].Compare(key);
			if (cmpr < 0)
				uBound = index;
			else if (cmpr > 0)
				lBound = index + 1;
			else
				return index;
		}
		return lBound | 0x80000000;
	}

	M_Key* End() const { return keys + numKeys; }

public:
	Set(UInt32 _alloc = _default_alloc) : keys(nullptr), numKeys(0), numAlloc(_alloc) {}
	Set(Init_List&& inList) : keys(nullptr), numKeys(0), numAlloc(inList.size()) { InsertList(std::forward<Init_List>(inList)); }
	~Set()
	{
		if (keys)
		{
			Clear();
			Pool_CFree<M_Key>(keys, numAlloc);
		}
	}

	void Destroy()
	{
		this->~Set();
		keys = nullptr;
	}

	UInt32 Size() const { return numKeys; }
	bool Empty() const { return !numKeys; }
	T_Key* Keys() { return reinterpret_cast<T_Key*>(keys); }

	void operator=(Set&& rhs)
	{
		this->~Set();
		keys = rhs.keys;
		numKeys = rhs.numKeys;
		numAlloc = rhs.numAlloc;
		rhs.keys = nullptr;
		rhs.numKeys = 0;
		rhs.numAlloc = MAP_DEFAULT_ALLOC;
	}

	void operator=(const Set& rhs)
	{
		Clear();
		if (numKeys = rhs.numKeys)
		{
			if (numAlloc < numKeys)
			{
				if (keys)
				{
					Pool_CFree<M_Key>(keys, numAlloc);
					keys = nullptr;
				}
				numAlloc = AlignNumAlloc<M_Key>(numKeys);
			}
			if (!keys)
				keys = Pool_CAlloc<M_Key>(numAlloc);
			MemCopy(keys, rhs.keys, sizeof(M_Key) * numKeys);
		}
	}

	const T_Key& operator[](UInt32 index) const { return keys[index]; }

	bool Insert(Key_Arg key)
	{
		if (UInt32 index = GetIndex(key); SInt32(index) < 0)
		{
			index &= 0x7FFFFFFF;
			if (!keys)
			{
				numAlloc = AlignNumAlloc<M_Key>(numAlloc);
				keys = Pool_CAlloc<M_Key>(numAlloc);
			}
			else if (numAlloc <= numKeys)
			{
				UInt32 newAlloc = numAlloc << 1;
				keys = Pool_CRealloc<M_Key>(keys, numAlloc, newAlloc);
				numAlloc = newAlloc;
			}
			M_Key* pKey = keys + index;
			if (UInt32 movCnt = numKeys - index)
				MemCopy(pKey + 1, pKey, sizeof(M_Key) * movCnt);
			numKeys++;
			*pKey = key;
			return true;
		}
		return false;
	}

	void InsertList(Init_List&& inList)
	{
		for (auto iter = inList.begin(); iter != inList.end(); ++iter)
			Insert(*iter);
	}

	bool HasKey(Key_Arg key) const
	{
		return SInt32(GetIndex(key)) >= 0;
	}

	bool Erase(Key_Arg key)
	{
		if (UInt32 index = GetIndex(key); SInt32(index) >= 0)
		{
			M_Key* pKey = keys + index;
			pKey->~M_Key();
			numKeys--;
			if (UInt32 cpyCnt = numKeys - index)
				MemCopy(pKey, pKey + 1, sizeof(M_Key) * cpyCnt);
			return true;
		}
		return false;
	}

	void Clear()
	{
		for (M_Key* pKey = keys, *pEnd = End(); pKey != pEnd; pKey++)
			pKey->~M_Key();
		numKeys &= 0;
	}

	inline bool operator==(const Set& rhs) const
	{
		return (numKeys == rhs.numKeys) && (!numKeys || MemCmp(keys, rhs.keys, sizeof(M_Key) * numKeys));
	}
	inline bool operator!=(const Set& rhs) const { return !(*this == rhs); }

	class Iterator
	{
	protected:
		M_Key* pKey;
		UInt32		count;

		Iterator() {}

	public:
		Key_Res operator()() const { return pKey->Get(); }
		Key_Res operator*() const { return pKey->Get(); }
		Key_Res operator->() const { return pKey->Get(); }

		explicit operator bool() const { return count != 0; }
		void operator++()
		{
			pKey++;
			count--;
		}

		Iterator(Set& source) : pKey(source.keys), count(source.numKeys) {}
		Iterator(Set& source, Key_Arg key)
		{
			if (UInt32 index = source.GetIndex(key); SInt32(index) >= 0)
			{
				pKey = source.keys + index;
				count = source.numKeys - index;
			}
			else count = 0;
		}
	};

	class CpIterator : public Iterator
	{
		using Iterator::pKey, Iterator::count;

		AuxBuffer<M_Key>	buffer;

	public:
		CpIterator(Set& source)
		{
			count = source.numKeys;
			if (count > 1)
				pKey = buffer.Copy(count, source.keys);
			else
				pKey = source.keys;
		}
	};

	void Remove(Iterator& iter)
	{
		iter.pKey->~M_Key();
		numKeys--;
		if (iter.count > 1)
		{
			MemCopy(iter.pKey, iter.pKey + 1, (iter.count - 1) * sizeof(M_Key));
			iter.pKey--;
		}
	}

	Iterator Begin() { return Iterator(*this); }
	Iterator Find(Key_Arg key) { return Iterator(*this, key); }

	CpIterator BeginCp() { return CpIterator(*this); }
};

template <typename T_Key> __forceinline UInt32 HashKey(T_Key inKey)
{
	union
	{
		T_Key		raw;
		UInt8		u8;
		UInt16		u16;
		UInt32		u32;
		const char* str;
	}
	key = { inKey };
	if (std::is_same_v<T_Key, UINT>)
		return key.u32;
	if (std::is_same_v<T_Key, char*> || std::is_same_v<T_Key, const char*>)
		return StrHashCI(key.str);
	if (std::is_same_v<T_Key, SInt8*> || std::is_same_v<T_Key, const SInt8*>)
		return StrHashCS(key.str);
	UInt32 uKey;
	if (sizeof(T_Key) == 1)
		uKey = key.u8;
	else if (sizeof(T_Key) == 2)
		uKey = key.u16;
	else
	{
		uKey = key.u32;
		if (sizeof(T_Key) > 4)
		{
			uKey += ULNG(inKey + 1) ^ 0x7ED55D16;
			if (sizeof(T_Key) > 8)
			{
				uKey += ULNG(inKey + 2) ^ 0x165667B1;
				if (sizeof(T_Key) > 0xC)
					uKey += ULNG(inKey + 3) ^ 0xFD7046C5;
			}
		}
	}
	return (uKey * 0xD) ^ (uKey >> 0xF);
}

template <typename T_Key> class HashedKey
{
	using Key_Arg = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;
	using Key_Res = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;

	T_Key		key;

public:
	__forceinline bool Equal(Key_Arg inKey, UInt32) const { return key == inKey; }
	__forceinline Key_Res Get() const { return key; }
	__forceinline void Set(Key_Arg inKey, UInt32) { key = inKey; }
	__forceinline UInt32 GetHash() const { return HashKey<T_Key>(key); }
};

template <> class HashedKey<const char*>
{
	UInt32		hashVal;

public:
	__forceinline bool Equal(const char*, UInt32 inHash) const { return hashVal == inHash; }
	__forceinline const char* Get() const { return ""; }
	__forceinline void Set(const char*, UInt32 inHash) { hashVal = inHash; }
	__forceinline UInt32 GetHash() const { return hashVal; }
};

template <> class HashedKey<char*>
{
	UInt32		hashVal;
	char* key;

public:
	__forceinline bool Equal(char*, UInt32 inHash) const { return hashVal == inHash; }
	__forceinline char* Get() const { return key; }
	__forceinline void Set(const char* inKey, UInt32 inHash)
	{
		hashVal = inHash;
		key = CopyStringKey(inKey);
	}
	__forceinline UInt32 GetHash() const { return hashVal; }
	~HashedKey() { FreeStringKey(key); }
};

template <> class HashedKey<const SInt8*>
{
	UInt32		hashVal;

public:
	__forceinline bool Equal(const SInt8*, UInt32 inHash) const { return hashVal == inHash; }
	__forceinline const SInt8* Get() const { return (const SInt8*)""; }
	__forceinline void Set(const SInt8*, UInt32 inHash) { hashVal = inHash; }
	__forceinline UInt32 GetHash() const { return hashVal; }
};

template <> class HashedKey<SInt8*>
{
	UInt32		hashVal;
	SInt8* key;

public:
	__forceinline bool Equal(SInt8*, UInt32 inHash) const { return hashVal == inHash; }
	__forceinline SInt8* Get() const { return key; }
	__forceinline void Set(SInt8* inKey, UInt32 inHash)
	{
		hashVal = inHash;
		key = (SInt8*)CopyStringKey((const char*)inKey);
	}
	__forceinline UInt32 GetHash() const { return hashVal; }
	~HashedKey() { FreeStringKey((char*)key); }
};

#define NUM_BUCKETS (_allow_resize ? numBuckets : _default_bucket_count)

template <typename T_Key, typename T_Data, const UInt32 _default_bucket_count = MAP_DEFAULT_BUCKET_COUNT, const bool _allow_resize = true> class UnorderedMap
{
	static_assert(std::has_single_bit(_default_bucket_count));

	using H_Key = HashedKey<T_Key>;
	using Key_Arg = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;
	using Key_Res = std::conditional_t<std::is_scalar_v<T_Key>, T_Key, const T_Key&>;
	using Data_Arg = std::conditional_t<std::is_scalar_v<T_Data>, T_Data, const T_Data&>;
	using Data_Res = std::conditional_t<std::is_scalar_v<T_Data>, T_Data, T_Data&>;
	using M_Pair = MappedPair<T_Key, T_Data>;
	using Init_List = std::initializer_list<M_Pair>;
	Use_HashMapUtils(UnorderedMap)

		struct alignas(16) Entry
	{
		Entry* next;
		H_Key		key;
		T_Data		value;
	};

	struct Bucket
	{
		Entry* entries;

		void Insert(Entry* entry)
		{
			entry->next = entries;
			entries = entry;
		}

		Entry* Remove(Entry* entry)
		{
			Entry* prev = nullptr;
			for (Entry* curr = entries; curr != entry; curr = curr->next)
				prev = curr;
			if (prev) prev->next = entry->next;
			else entries = entry->next;
			entry->~Entry();
			Pool_CFree<Entry>(entry);
			return prev;
		}

		~Bucket()
		{
			while (Entry* pEntry = entries)
			{
				entries = entries->next;
				pEntry->~Entry();
				Pool_CFree<Entry>(pEntry);
			}
		}

		UInt32 Size() const
		{
			UInt32 size = 0;
			for (Entry* pEntry = entries; pEntry; pEntry = pEntry->next, size++);
			return size;
		}
	};

	Bucket* buckets;		// 00
	UInt32		numBuckets;		// 04
	UInt32		numEntries;		// 08

	Bucket* GetBuckets() const { return buckets; }
	Bucket* End() const { return buckets + NUM_BUCKETS; }

	__declspec(noinline) void __fastcall ResizeTable(UInt32 newCount)
	{
		Bucket* pBucket = buckets, * pEnd = End(), * newBuckets = (Bucket*)AllocBuckets(newCount);
		newCount--;
		do
		{
			Entry* pEntry = pBucket->entries;
			while (Entry* pTemp = pEntry)
			{
				pEntry = pEntry->next;
				newBuckets[pTemp->key.GetHash() & newCount].Insert(pTemp);
			}
		} while (++pBucket != pEnd);
		Pool_CFree<Bucket>(buckets, numBuckets);
		buckets = newBuckets;
		numBuckets = newCount + 1;
	}

	Entry* FindEntry(Key_Arg key) const
	{
		if (numEntries)
		{
			UInt32 hashVal = HashKey<T_Key>(key);
			for (Entry* pEntry = buckets[hashVal & (NUM_BUCKETS - 1)].entries; pEntry; pEntry = pEntry->next)
				if (pEntry->key.Equal(key, hashVal)) return pEntry;
		}
		return nullptr;
	}

public:
	UnorderedMap(UInt32 _numBuckets = _default_bucket_count) : buckets(nullptr), numBuckets(_allow_resize ? _numBuckets : _default_bucket_count), numEntries(0) {}
	UnorderedMap(Init_List&& inList) : buckets(nullptr), numBuckets(_allow_resize ? AlignBucketCount(inList.size()) : _default_bucket_count), numEntries(0)
	{
		InsertList(std::forward<Init_List>(inList));
	}
	~UnorderedMap()
	{
		if (buckets)
		{
			Clear();
			Pool_CFree<Bucket>(buckets, NUM_BUCKETS);
		}
	}

	void Destroy()
	{
		this->~UnorderedMap();
		buckets = nullptr;
	}

	UInt32 Size() const { return numEntries; }
	bool Empty() const { return !numEntries; }
	UInt32 BucketCount() const { return numBuckets; }

	void operator=(const UnorderedMap& rhs) = delete;

	void operator=(UnorderedMap&& rhs)
	{
		this->~UnorderedMap();
		buckets = rhs.buckets;
		numBuckets = rhs.numBuckets;
		numEntries = rhs.numEntries;
		rhs.buckets = nullptr;
		rhs.numBuckets = MAP_DEFAULT_BUCKET_COUNT;
		rhs.numEntries = 0;
	}

	__declspec(noinline) void SetBucketCount(UInt32 newCount)
	{
		if (!_allow_resize) return;
		newCount = AlignBucketCount(newCount);
		if (numBuckets == newCount)
			return;
		if (!numEntries)
		{
			if (buckets)
			{
				Pool_CFree<Bucket>(buckets, numBuckets);
				buckets = nullptr;
			}
			numBuckets = newCount;
		}
		else if (numEntries <= newCount)
			ResizeTable(newCount);
	}

	float LoadFactor() const { return (float)numEntries / (float)numBuckets; }

	bool InsertKey(Key_Arg key, T_Data** outData)
	{
		if (!buckets)
			buckets = (Bucket*)AllocBuckets(NUM_BUCKETS);
		else if (_allow_resize)
		{
			if ((numEntries > numBuckets) && (numBuckets < MAP_MAX_BUCKET_COUNT))
				ResizeTable(numBuckets << 1);
		}
		UInt32 hashVal = HashKey<T_Key>(key);
		Bucket* pBucket = &buckets[hashVal & (NUM_BUCKETS - 1)];
		for (Entry* pEntry = pBucket->entries; pEntry; pEntry = pEntry->next)
		{
			if (!pEntry->key.Equal(key, hashVal))
				continue;
			*outData = &pEntry->value;
			return false;
		}
		numEntries++;
		Entry* newEntry = Pool_CAlloc<Entry>();
		newEntry->key.Set(key, hashVal);
		pBucket->Insert(newEntry);
		*outData = &newEntry->value;
		return true;
	}

	bool Insert(Key_Arg key, T_Data** outData)
	{
		if (InsertKey(key, outData))
		{
			new (*outData) T_Data();
			return true;
		}
		return false;
	}

	T_Data& operator[](Key_Arg key)
	{
		T_Data* outData;
		if (InsertKey(key, &outData))
			new (outData) T_Data();
		return *outData;
	}

	template <typename ...Args>
	T_Data* Emplace(Key_Arg key, Args&& ...args)
	{
		T_Data* outData;
		if (InsertKey(key, &outData))
			new (outData) T_Data(std::forward<Args>(args)...);
		return outData;
	}

	void InsertList(Init_List&& inList)
	{
		T_Data* outData;
		for (auto iter = inList.begin(); iter != inList.end(); ++iter)
		{
			InsertKey(iter->key, &outData);
			*outData = std::move(iter->value);
		}
	}

	bool HasKey(Key_Arg key) const { return FindEntry(key) != nullptr; }

	T_Data Get(Key_Arg key) const
	{
		static_assert(std::is_scalar_v<T_Data>);
		Entry* pEntry = FindEntry(key);
		return pEntry ? pEntry->value : NULL;
	}

	T_Data* GetPtr(Key_Arg key) const
	{
		Entry* pEntry = FindEntry(key);
		return pEntry ? &pEntry->value : nullptr;
	}

	bool Erase(Key_Arg key)
	{
		if (numEntries)
		{
			UInt32 hashVal = HashKey<T_Key>(key);
			Bucket* pBucket = &buckets[hashVal & (NUM_BUCKETS - 1)];
			for (Entry* pEntry = pBucket->entries; pEntry; pEntry = pEntry->next)
			{
				if (!pEntry->key.Equal(key, hashVal))
					continue;
				numEntries--;
				pBucket->Remove(pEntry);
				return true;
			}
		}
		return false;
	}

	T_Data GetErase(Key_Arg key)
	{
		static_assert(std::is_scalar_v<T_Data>);
		if (numEntries)
		{
			UInt32 hashVal = HashKey<T_Key>(key);
			Bucket* pBucket = &buckets[hashVal & (NUM_BUCKETS - 1)];
			for (Entry* pEntry = pBucket->entries; pEntry; pEntry = pEntry->next)
			{
				if (!pEntry->key.Equal(key, hashVal))
					continue;
				T_Data outVal = pEntry->value;
				numEntries--;
				pBucket->Remove(pEntry);
				return outVal;
			}
		}
		return NULL;
	}

	void Clear()
	{
		if (numEntries)
		{
			Bucket* pBucket = buckets, * pEnd = End();
			do
			{
				pBucket->~Bucket();
			} while (++pBucket != pEnd);
		}
		numEntries &= 0;
	}

	class Iterator
	{
	protected:
		UnorderedMap* table;
		Bucket* bucket;
		Entry* entry;

		void FindNonEmpty()
		{
			for (Bucket* pEnd = table->End(); bucket != pEnd; bucket++)
				if (entry = bucket->entries) return;
		}

	public:
		void Init(UnorderedMap& _table)
		{
			table = &_table;
			entry = nullptr;
			if (table->numEntries)
			{
				bucket = table->buckets;
				FindNonEmpty();
			}
		}

		void Find(Key_Arg key)
		{
			if (table->numEntries)
			{
				UInt32 hashVal = HashKey<T_Key>(key);
				bucket = &table->buckets[hashVal & (table->numBuckets - 1)];
				for (entry = bucket->entries; entry; entry = entry->next)
					if (entry->key.Equal(key, hashVal)) break;
			}
			else entry = nullptr;
		}

		UnorderedMap* Table() const { return table; }
		Key_Res Key() const { return entry->key.Get(); }
		Data_Res operator()() const { return entry->value; }
		T_Data& Ref() { return entry->value; }
		Data_Res operator*() const { return entry->value; }
		Data_Res operator->() const { return entry->value; }

		explicit operator bool() const { return entry != nullptr; }
		void operator++()
		{
			if (entry)
				entry = entry->next;
			else entry = bucket->entries;
			if (!entry)
			{
				bucket++;
				FindNonEmpty();
			}
		}

		bool IsValid()
		{
			if (entry)
			{
				for (Entry* temp = bucket->entries; temp; temp = temp->next)
					if (temp == entry) return true;
				entry = nullptr;
			}
			return false;
		}

		void Remove()
		{
			table->numEntries--;
			entry = bucket->Remove(entry);
		}

		Iterator(UnorderedMap* _table = nullptr) : table(_table), entry(nullptr) {}
		Iterator(UnorderedMap& _table) { Init(_table); }
		Iterator(UnorderedMap& _table, Key_Arg key) : table(&_table) { Find(key); }
	};

	Iterator Begin() { return Iterator(*this); }
	Iterator Find(Key_Arg key) { return Iterator(*this, key); }
};

template <class Item> class DList
{
	Use_LinkedListUtils(DList, Item*)
public:
	struct DNode
	{
		DNode* next;
		DNode* prev;
		Item* data;

		DNode* GetNth(UInt32 index)
		{
			DNode* pNode = this;
			while (index--)
				if (!(pNode = pNode->next))
					break;
			return pNode;
		}
	};

private:
	DNode* first;
	DNode* last;
	UInt32		count;

	DNode* GetNthNode(UInt32 index) const
	{
		return first ? first->GetNth(index) : nullptr;
	}

	static DNode* NewNode() { return CdeclCall<DNode*>(0x43A010); }

	void ReleaseNode(DNode* pNode)
	{
		if (pNode->prev)
			pNode->prev->next = pNode->next;
		else first = pNode->next;
		if (pNode->next)
			pNode->next->prev = pNode->prev;
		else last = pNode->prev;
		StdCall(0x45CEE0, pNode);
		count--;
	}

	DNode* FindNode(Item* pItem) const
	{
		for (auto iter = first; iter; iter = iter->next)
			if (iter->data == pItem)
				return iter;
		return nullptr;
	}

	template <class Matcher>
	DNode* FindNode(const Matcher& matcher) const
	{
		for (auto iter = first; iter; iter = iter->next)
			if (matcher == iter->data)
				return iter;
		return nullptr;
	}

public:
	DList() : first(nullptr), last(nullptr), count(0) {}
	DList(DNode* _first, DNode* _last, UInt32 _count) : first(_first), last(_last), count(_count) {}
	DList(const DList& rhs) { *this = rhs; }

	void operator=(const DList& rhs)
	{
		first = rhs.first;
		last = rhs.last;
		count = rhs.count;
	}

	bool Empty() const { return !first; }
	DNode* Head() const { return first; }
	DNode* Tail() const { return last; }
	UInt32 Count() const { return count; }

	void RemoveAll() { ThisCall(0x4ED900, this); }

	void Prepend(Item* pData)
	{
		DNode* newNode = NewNode();
		newNode->next = first;
		newNode->data = pData;
		if (first)
			first->prev = newNode;
		first = newNode;
		if (!last)
			last = newNode;
		count++;
	}

	void Append(Item* pData)
	{
		DNode* newNode = NewNode();
		newNode->prev = last;
		newNode->data = pData;
		if (last)
			last->next = newNode;
		last = newNode;
		if (!first)
			first = newNode;
		count++;
	}

	Item* GetNthItem(UInt32 index) const
	{
		DNode* node = GetNthNode(index);
		return node ? node->data : NULL;
	}

	bool Remove(Item* pData)
	{
		if (DNode* pNode = FindNode(pData))
		{
			ReleaseNode(pNode);
			return true;
		}
		return false;
	}

	template <class Matcher>
	bool Remove(const Matcher& matcher)
	{
		if (DNode* pNode = FindNode(matcher))
		{
			ReleaseNode(pNode);
			return true;
		}
		return false;
	}

	SInt32 GetIndexOf(Item* pItem) const
	{
		SInt32 resIdx = 0;
		for (auto iter = first; iter; iter = iter->next)
		{
			if (iter->data == pItem)
				return resIdx;
			resIdx++;
		}
		return -1;
	}

	template <class Matcher>
	SInt32 GetIndexOf(const Matcher& matcher) const
	{
		SInt32 resIdx = 0;
		for (auto iter = first; iter; iter = iter->next)
		{
			if (matcher == iter->data)
				return resIdx;
			resIdx++;
		}
		return -1;
	}

	class Iterator
	{
		DNode* pNode;

	public:
		Iterator() : pNode(nullptr) {}
		Iterator(DNode* bgn) : pNode(bgn) {}

		explicit operator bool() const { return pNode != nullptr; }
		void operator++() { pNode = pNode->next; }
		void operator--() { pNode = pNode->prev; }

		DNode* GetNode() const { return pNode; }

		Item* operator*() const { return pNode->data; }
		Item* operator->() const { return pNode->data; }
		DNode* operator()() const { return pNode; }
	};

	Iterator Begin() { return Iterator(first); }
	Iterator RBegin() { return Iterator(last); }

	Iterator Find(Item* pItem) { return Iterator(FindNode(pItem)); }

	template <class Matcher>
	Iterator Find(const Matcher& matcher) { return Iterator(FindNode(matcher)); }

	bool Remove(Iterator& iter, bool forward = true)
	{
		DNode* pNode = iter();
		if (forward) ++iter;
		else --iter;
		ReleaseNode(pNode);
		return iter() != nullptr;
	}
};

template <typename T_Data> class BSSimpleArray
{
	Use_ArrayUtils(BSSimpleArray, T_Data)
public:
	virtual void	Destroy(bool doFree);
	virtual T_Data* Allocate(UInt32 size);

	T_Data* data;		// 04
	UInt32		size;		// 08
	UInt32		alloc;		// 0C

	UInt32 Size() const { return size; }
	bool Empty() const { return !size; }
	T_Data* Data() const { return const_cast<T_Data*>(data); }

	T_Data& operator[](UInt32 idx) { return data[idx]; }

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

		Iterator(BSSimpleArray& source) : pData(source.data), count(source.size) {}
	};

	Iterator Begin() { return Iterator(*this); }

	__forceinline void Append(T_Data* item)
	{
		ThisStdCall(0x7CB2E0, this, item);
	}
};
typedef void* (*_FormHeap_Allocate)(UInt32 size);
extern const _FormHeap_Allocate FormHeap_Allocate;

typedef void (*_FormHeap_Free)(void* ptr);
extern const _FormHeap_Free FormHeap_Free;

// 10
// this is a NiTPointerMap <UInt32, T_Data>
// todo: generalize key
template <typename T_Data>
class NiTPointerMap
{
public:
	NiTPointerMap();
	virtual ~NiTPointerMap();

	struct Entry
	{
		Entry* next;
		UInt32	key;
		T_Data* data;
	};

	// note: traverses in non-numerical order
	class Iterator
	{
		friend NiTPointerMap;

	public:
		Iterator(NiTPointerMap* table, Entry* entry = NULL, UInt32 bucket = 0)
			:m_table(table), m_entry(entry), m_bucket(bucket) {
			FindValid();
		}
		~Iterator() { }

		T_Data* Get(void);
		UInt32		GetKey(void);
		bool		Next(void);
		bool		Done(void);

	private:
		void		FindValid(void);

		NiTPointerMap* m_table;
		Entry* m_entry;
		UInt32		m_bucket;
	};

	virtual UInt32	CalculateBucket(UInt32 key);
	virtual bool	CompareKey(UInt32 lhs, UInt32 rhs);
	virtual void	Fn_03(UInt32 arg0, UInt32 arg1, UInt32 arg2);	// assign to entry
	virtual void	Fn_04(UInt32 arg);
	virtual void	Fn_05(void);	// locked operations
	virtual void	Fn_06(void);	// locked operations

	T_Data* Lookup(UInt32 key);
	bool		Insert(Entry* nuEntry);

	//	void	** _vtbl;		// 0
	UInt32	m_numBuckets;	// 4
	Entry** m_buckets;	// 8
	UInt32	m_numItems;		// C
};

template <typename T_Data>
T_Data* NiTPointerMap <T_Data>::Lookup(UInt32 key)
{
	for (Entry* traverse = m_buckets[key % m_numBuckets]; traverse; traverse = traverse->next)
		if (traverse->key == key)
			return traverse->data;

	return NULL;
}

template <typename T_Data>
bool NiTPointerMap<T_Data>::Insert(Entry* nuEntry)
{
	// game code does not appear to care about ordering of entries in buckets
	UInt32 bucket = nuEntry->key % m_numBuckets;
	Entry* prev = NULL;
	for (Entry* cur = m_buckets[bucket]; cur; cur = cur->next) {
		if (cur->key == nuEntry->key) {
			return false;
		}
		else if (!cur->next) {
			prev = cur;
			break;
		}
	}

	if (prev) {
		prev->next = nuEntry;
	}
	else {
		m_buckets[bucket] = nuEntry;
	}

	m_numBuckets++;
	return true;
}

template <typename T_Data>
T_Data* NiTPointerMap <T_Data>::Iterator::Get(void)
{
	if (m_entry)
		return m_entry->data;

	return NULL;
}

template <typename T_Data>
UInt32 NiTPointerMap <T_Data>::Iterator::GetKey(void)
{
	if (m_entry)
		return m_entry->key;

	return 0;
}

template <typename T_Data>
bool NiTPointerMap <T_Data>::Iterator::Next(void)
{
	if (m_entry)
		m_entry = m_entry->next;

	while (!m_entry && (m_bucket < (m_table->m_numBuckets - 1)))
	{
		m_bucket++;

		m_entry = m_table->m_buckets[m_bucket];
	}

	return m_entry != NULL;
}

template <typename T_Data>
bool NiTPointerMap <T_Data>::Iterator::Done(void)
{
	return m_entry == NULL;
}

template <typename T_Data>
void NiTPointerMap <T_Data>::Iterator::FindValid(void)
{
	// validate bucket
	if (m_bucket >= m_table->m_numBuckets) return;

	// get bucket
	m_entry = m_table->m_buckets[m_bucket];

	// find non-empty bucket
	while (!m_entry && (m_bucket < (m_table->m_numBuckets - 1)))
	{
		m_bucket++;

		m_entry = m_table->m_buckets[m_bucket];
	}
}

template <typename T_Data>
struct ListNode
{
	T_Data* data;
	ListNode* next;

	ListNode() : data(NULL), next(NULL) {}
	ListNode(T_Data* _data) : data(_data), next(NULL) {}

	T_Data* Data() const { return data; }
	ListNode* Next() const { return next; }

	ListNode* RemoveMe()
	{
		if (next)
		{
			ListNode* pNext = next;
			data = next->data;
			next = next->next;
			FormHeap_Free(pNext);
			return this;
		}
		data = NULL;
		return NULL;
	}

	ListNode* RemoveNext()
	{
		ListNode* pNext = next;
		next = next->next;
		FormHeap_Free(pNext);
		return next;
	}

	ListNode* Append(T_Data* _data)
	{
		ListNode* newNode = (ListNode*)FormHeap_Allocate(sizeof(ListNode));
		newNode->data = _data;
		newNode->next = next;
		next = newNode;
		return newNode;
	}

	ListNode* Insert(T_Data* _data)
	{
		ListNode* newNode = (ListNode*)FormHeap_Allocate(sizeof(ListNode));
		newNode->data = data;
		data = _data;
		newNode->next = next;
		next = newNode;
		return newNode;
	}
};

template <class Item>
class TList
{
public:
	typedef ListNode<Item> Entry;

	Entry First;

	template <class Op>
	UInt32 FreeNodes(Entry* node, Op&& compareOp) const
	{
		static UInt32 nodeCount = 0, numFreed = 0, lastNumFreed = 0;
		if (node->next)
		{
			nodeCount++;
			FreeNodes(node->next, compareOp);
			nodeCount--;
		}
		if (compareOp.Accept(node->data))
		{
			node->RemoveMe();
			numFreed++;
		}
		if (!nodeCount)
		{
			lastNumFreed = numFreed;
			numFreed = 0;
		}
		return lastNumFreed;
	}

	Entry* GetLastNode(SInt32* outIdx = NULL) const
	{
		SInt32 index = 0;
		Entry* node = Head();
		while (node->next)
		{
			node = node->next;
			index++;
		}
		if (outIdx)
			*outIdx = index;
		return node;
	}

	Entry* GetNthNode(SInt32 index) const
	{
		if (index >= 0)
		{
			Entry* node = Head();
			do
			{
				if (!index)
					return node;
				index--;
			} while (node = node->next);
		}
		return NULL;
	}

	void Init(Item* item = NULL)
	{
		First.data = item;
		First.next = NULL;
	}

	Entry* Head() const { return const_cast<Entry*>(&First); }

	bool Empty() const { return !First.data; }

	class Iterator
	{
		Entry* m_curr;

	public:
		Iterator operator++()
		{
			if (m_curr)
				m_curr = m_curr->next;
			return *this;
		}
		bool End() const { return !m_curr || (!m_curr->data && !m_curr->next); }
		Item* operator->() const { return m_curr->data; }
		Item*& operator*() const { return m_curr->data; }
		const Iterator& operator=(const Iterator& rhs)
		{
			m_curr = rhs.m_curr;
			return *this;
		}
		Item* Get() const { return m_curr->data; }
		void Next()
		{
			if (m_curr)
				m_curr = m_curr->next;
		}
		void Find(Item* _item)
		{
			while (m_curr)
			{
				if (m_curr->data == _item)
					break;
				m_curr = m_curr->next;
			}

		}
		bool operator!=(const Iterator& other) const
		{
			return m_curr != other.m_curr;
		}

		bool operator!=(Iterator&& other) const
		{
			return m_curr != other.m_curr;
		}

		Iterator(Entry* node = NULL) : m_curr(node) {}
		Iterator(TList& _list) : m_curr(&_list.First) {}
		Iterator(TList* _list) : m_curr(&_list->First) {}
		Iterator(TList& _list, Item* _item) : m_curr(&_list.First) { Find(_item); }
		Iterator(TList* _list, Item* _item) : m_curr(&_list->First) { Find(_item); }
	};

	Iterator Begin() const { return Iterator(Head()); }

	UInt32 Count() const
	{
		if (!First.data)
			return 0;
		Entry* node = Head();
		UInt32 count = 1;
		while (node = node->next)
			count++;
		return count;
	};

	bool IsInList(Item* item) const
	{
		Entry* node = Head();
		do
		{
			if (node->data == item)
				return true;
			node = node->next;
		} while (node);
		return false;
	}

	Item* GetFirstItem() const
	{
		return First.data;
	}

	Item* GetLastItem() const
	{
		return GetLastNode()->data;
	}

	Item* GetNthItem(SInt32 index) const
	{
		if (eListEnd == index)
			return GetLastNode()->data;
		Entry* node = GetNthNode(index);
		return node ? node->data : NULL;
	}

	SInt32 AddAt(Item* item, SInt32 index)
	{
		if (!item)
			return eListInvalid;
		Entry* node;
		if (!index)
		{
			if (First.data)
				First.Insert(item);
			else
				First.data = item;
		}
		else if (eListEnd == index)
		{
			node = GetLastNode(&index);
			if (node->data)
				node->Append(item);
			else
				node->data = item;
		}
		else
		{
			node = GetNthNode(index);
			if (!node)
				return eListInvalid;
			node->Insert(item);
		}
		return index;
	}

	SInt32 Append(Item* item)
	{
		SInt32 index = eListInvalid;
		if (item)
		{
			Entry* node = GetLastNode(&index);
			if (node->data)
				node->Append(item);
			else
				node->data = item;
		}
		return index;
	}

	void Insert(Item* item)
	{
		if (item)
		{
			if (First.data)
				First.Insert(item);
			else
				First.data = item;
		}
	}

	void CopyFrom(TList& sourceList)
	{
		Entry* target = Head(), * source = sourceList.Head();
		RemoveAll();
		if (!source->data)
			return;
		target->data = source->data;
		while (source = source->next)
			target = target->Append(source->data);
	}

	template <class Op>
	void Visit(Op&& op, Entry* prev = NULL) const
	{
		Entry* curr = prev ? prev->next : Head();
		while (curr)
		{
			if (!curr->data || !op.Accept(curr->data))
				break;
			curr = curr->next;
		}
	}

	template <class Op>
	Item* Find(Op&& op) const
	{
		Entry* curr = Head();
		Item* pItem;
		do
		{
			pItem = curr->data;
			if (pItem && op.Accept(pItem))
				return pItem;
			curr = curr->next;
		} while (curr);
		return NULL;
	}

	template <class Op>
	Iterator Find(Op&& op, Iterator& prev) const
	{
		Iterator curIt = prev.End() ? Begin() : ++prev;
		while (!curIt.End())
		{
			if (*curIt && op.Accept(*curIt))
				break;
			++curIt;
		}
		return curIt;
	}

	template <class Op>
	UInt32 CountIf(Op&& op) const
	{
		UInt32 count = 0;
		Entry* curr = Head();
		do
		{
			if (curr->data && op.Accept(curr->data))
				count++;
			curr = curr->next;
		} while (curr);
		return count;
	}

	class AcceptAll
	{
	public:
		bool Accept(Item* item) { return true; }
	};

	void RemoveAll() const
	{
		Entry* nextNode = Head(), * currNode = nextNode->next;
		nextNode->data = NULL;
		nextNode->next = NULL;
		while (currNode)
		{
			nextNode = currNode->next;
			FormHeap_Free(currNode);
			currNode = nextNode;
		}
	}

	void DeleteAll() const
	{
		Entry* nextNode = Head(), * currNode = nextNode->next;
		FormHeap_Free(nextNode->data);
		nextNode->data = NULL;
		nextNode->next = NULL;
		while (currNode)
		{
			nextNode = currNode->next;
			currNode->data->~Item();
			FormHeap_Free(currNode->data);
			FormHeap_Free(currNode);
			currNode = nextNode;
		}
	}

	Item* RemoveNth(SInt32 idx)
	{
		Item* removed = NULL;
		if (idx <= 0)
		{
			removed = First.data;
			First.RemoveMe();
		}
		else
		{
			Entry* node = Head();
			while (node->next && --idx)
				node = node->next;
			if (!idx)
			{
				removed = node->next->data;
				node->RemoveNext();
			}
		}
		return removed;
	};

	UInt32 Remove(Item* item)
	{
		UInt32 removed = 0;
		Entry* curr = Head(), * prev = NULL;
		do
		{
			if (curr->data == item)
			{
				curr = prev ? prev->RemoveNext() : curr->RemoveMe();
				removed++;
			}
			else
			{
				prev = curr;
				curr = curr->next;
			}
		} while (curr);
		return removed;
	}

	Item* ReplaceNth(SInt32 index, Item* item)
	{
		Item* replaced = NULL;
		if (item)
		{
			Entry* node;
			if (eListEnd == index)
				node = GetLastNode();
			else
			{
				node = GetNthNode(index);
				if (!node)
					return NULL;
			}
			replaced = node->data;
			node->data = item;
		}
		return replaced;
	}

	UInt32 Replace(Item* item, Item* replace)
	{
		UInt32 replaced = 0;
		Entry* curr = Head();
		do
		{
			if (curr->data == item)
			{
				curr->data = replace;
				replaced++;
			}
			curr = curr->next;
		} while (curr);
		return replaced;
	}

	template <class Op>
	UInt32 RemoveIf(Op&& op)
	{
		return FreeNodes(Head(), op);
	}

	SInt32 GetIndexOf(Item* item)
	{
		SInt32 idx = 0;
		Entry* curr = Head();
		do
		{
			if (curr->data == item)
				return idx;
			idx++;
			curr = curr->next;
		} while (curr);
		return -1;
	}

	template <class Op>
	SInt32 GetIndexOf(Op&& op)
	{
		SInt32 idx = 0;
		Entry* curr = Head();
		do
		{
			if (curr->data && op.Accept(curr->data))
				return idx;
			idx++;
			curr = curr->next;
		} while (curr);
		return -1;
	}

	template <typename F>
	Item* FindFirst(F&& func) const
	{
		for (auto iter = Begin(); !iter.End(); ++iter)
		{
			if (*iter && func(*iter))
				return *iter;
		}
		return nullptr;
	}

	template <typename F>
	void ForEach(F&& func) const
	{
		for (auto iter = Begin(); !iter.End(); ++iter)
		{
			if (*iter)
				func(*iter);
		}
	}

	template <typename F>
	bool Contains(F&& func) const
	{
		return FindFirst(func) != nullptr;
	}

	// for use with C++11 range based loops only.
	Iterator begin() const
	{
		return Begin();
	}

	// for use with C++11 range based loops only.
	Iterator end() const
	{
		return Iterator(static_cast<Entry*>(nullptr));
	}

};
static_assert(sizeof(TList<void>) == 0x008);

// 10
template <typename T_Data> class NiTPtrMap
{
	Use_HashMapUtils(NiTPtrMap)

		struct Entry
	{
		Entry* next;
		UInt32		key;
		T_Data* data;
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

	UInt32		m_numBuckets;	// 04
	Bucket* m_buckets;		// 08
	UInt32		m_numItems;		// 0C

	Bucket* GetBuckets() const { return m_buckets; }
	Bucket* End() const { return m_buckets + m_numBuckets; }

public:
	virtual void	Destroy(bool doFree);
	virtual UInt32	CalculateBucket(UInt32 key);
	virtual bool	CompareKey(UInt32 lhs, UInt32 rhs);
	virtual void	FillEntry(Entry* entry, UInt32 key, T_Data data);
	virtual void	FreeKey(Entry* entry);
	virtual Entry* AllocNewEntry();
	virtual void	FreeEntry(Entry* entry);

	UInt32 Size() const { return m_numItems; }
	bool Empty() const { return !m_numItems; }
	UInt32 BucketCount() const { return m_numBuckets; }

	bool HasKey(UInt32 key) const
	{
		for (Entry* entry = m_buckets[key % m_numBuckets]; entry; entry = entry->next)
			if (key == entry->key) return true;
		return false;
	}

	T_Data* Lookup(UInt32 key) const;
	void Insert(UInt32 key, T_Data value);

	class Iterator
	{
		NiTPtrMap* table;
		Bucket* bucket;
		Entry* entry;

		void FindNonEmpty()
		{
			for (Bucket* end = table->End(); bucket != end; bucket++)
				if (entry = bucket->entries) break;
		}

	public:
		Iterator(NiTPtrMap& _table) : table(&_table), bucket(table->m_buckets), entry(nullptr) { FindNonEmpty(); }

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
		T_Data* Get() const { return entry->data; }
		UInt32 Key() const { return entry->key; }
	};

	Iterator Begin() { return Iterator(*this); }
};

template <typename T_Data>
__declspec(naked) T_Data* NiTPtrMap<T_Data>::Lookup(UInt32 key) const
{
	__asm
	{
		mov		eax, [esp + 4]
		xor edx, edx
		div		dword ptr[ecx + 4]
		mov		eax, [ecx + 8]
		mov		eax, [eax + edx * 4]
		test	eax, eax
		jz		done
		mov		edx, [esp + 4]
		ALIGN 16
		iterHead:
		cmp[eax + 4], edx
			jz		found
			mov		eax, [eax]
			test	eax, eax
			jnz		iterHead
			retn	4
			found:
		mov		eax, [eax + 8]
			done :
			retn	4
	}
}

UInt32* NiTPtrMap<UInt32>::Lookup(UInt32 key) const;

template <typename T_Data>
__declspec(naked) void NiTPtrMap<T_Data>::Insert(UInt32 key, T_Data value)
{
	__asm
	{
		mov		eax, [esp + 4]
		xor edx, edx
		div		dword ptr[ecx + 4]
		mov		eax, [ecx + 8]
		lea		eax, [eax + edx * 4]
		push	eax
		inc		dword ptr[ecx + 0xC]
		CALL_EAX(0x43A010)
		pop		ecx
		mov		edx, [ecx]
		mov[eax], edx
		mov		edx, [esp + 4]
		mov[eax + 4], edx
		mov		edx, [esp + 8]
		mov[eax + 8], edx
		mov[ecx], eax
		retn	8
	}
}