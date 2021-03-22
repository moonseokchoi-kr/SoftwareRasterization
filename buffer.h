#ifndef BUFFER_H_
#define  BUFFER_H_
//==============================
//AUTHOR : Choi-MoonSeok
//CREATE DATE : 2021-03-12
// PURPOSE : ����Ÿ���� �����ϴ� ���� ���ø� ����.
//==============================

#pragma once
#include <SDL.h>
#undef main
#include <type_traits>
//GPU���ۿ� ������ ���·� ����ϴ�.
//�����ӹ��ۿ� Z���۸� ���� �� �־���մϴ�.
//�����͸� �ڽ��� �����ϰ� ������ �� �־���մϴ�.

template<class T>
struct Buffer
{
	int mWidth, mHeight, mPixelCount, mPitch;
	T *buffer;

	//���۴� 1�����迭������ �ε����� ���� 2�����迭�� ��ġ�˴ϴ� ��ȣ�� ���� ������ �� �ֽ��ϴ�.
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

