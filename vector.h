#ifndef VECTOR_H_
#define VECTOR_H_
#include <cmath>
#pragma once

//만약 2차 벡터가 필요하면 z를 0으로 설정하고 사용
template<class T> struct Vec3
{
	union 
	{
		struct { T x, y, z; };
		struct { T ivert, iuv, inorm; };
		T raw[3];
	};
	Vec3() : x(0).y(0).z(0) {};
	Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {};
	inline Vec3<T> operator +(const Vec3<T> &V) const { return Vec3<T>(x + V.x, y + V.y, z+V.z); }
	inline Vec3<T> operator -(const Vec3<T> &V) const { return Vec3<T>(x - V.x, y - V.y, z-V.z); }
	inline Vec3<T> operator *(float f) const { return Vec3<T>(x*f, y*f, z*f); }
	inline T operator *(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; };
	float norm() const { return std::sqrt(x*x + y * y + z * z); }
	Vec3<T>& normalize(T l = 1) { *this = (*this)*(1 / norm()); return *this; }
	Vec3<T> cross(const Vec3<T> &V0, const Vec3<T> &V1) const { return Vec3<T>(V0.y*V1.z-V0.z*V1.y, V0.x*V1.z-V0.z*V1.x, V0.x*V1.y - V1.x*V0.y); }
};

typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

#endif

