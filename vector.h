#ifndef VECTOR_H_
#define VECTOR_H_
#include <cmath>
#pragma once

template<class T> struct Vec2 
{
	union 
	{
		struct{T u, v;};
		struct { T x, y; };
		T raw[2];
	};
	Vec2() :u(0), v(0) {};
	Vec2(T _u, T _v) :u(_u), v(_v) {};
	inline Vec2<T> operator +(const Vec2<T> &V) const { return Vec2<T>(u + V.u, v + V.v); }
	inline Vec2<T> operator -(const Vec2<T> &V) const { return Vec2<T>(u - V.u, v - V.v); }
	inline Vec2<T> operator *(float f ) const { return Vec2<T>(u*f, v*f); }
	inline T operator * (const Vec2<T> &V)const { return (u*V.u + v * V.v); }
	Vec2<T> cross(const Vec2<T> &V0, const Vec2<T> &V1) const { return Vec2<T>(0, V0.u*V1.v - V0.v*V1.u); }

};

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

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

#endif

