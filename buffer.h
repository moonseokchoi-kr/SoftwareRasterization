#ifndef BUFFER_H_
#define  BUFFER_H_
//==============================
//AUTHOR : Choi-MoonSeok
//CREATE DATE : 2021-03-12
// PURPOSE : 렌더타겟을 저장하는 버퍼 템플릿 생상.
//==============================

#pragma once
#include <SDL.h>
#undef main
#include <type_traits>
//GPU버퍼와 유사한 형태로 만듭니다.
//프레임버퍼와 Z버퍼를 만들 수 있어야합니다.
//데이터를 자신이 보관하고 유지할 수 있어야합니다.

template<class T>
struct Buffer
{
	int mWidth, mHeight, mPixelCount, mPitch;
	T *buffer;

	//버퍼는 1차원배열이지만 인덱스가 유사 2차원배열로 배치됩니다 등호를 통해 접근할 수 있습니다.
	T& operator()(size_t x, size_t y) 
	{
		return buffer[x + y * mWidth];
	}

	Buffer(int w, int h, T*array) : mWidth(w), mHeight(h), mPixelCount(w* h), mPitch(w * sizeof(T)),buffer(array){}

	~Buffer() { delete [] buffer; }

	void clear()
	{
		if (std::is_same<T, float>::value)
		{
			for (int i = 0; i < mPixelCount; i++)
			{
				buffer[i] = 0.0f;
			}
		}
		else
		{
			memset(buffer, 0xD, mPitch*mHeight);
		}
	}

};
#endif

