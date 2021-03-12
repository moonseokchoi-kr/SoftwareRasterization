#ifndef DISPLAYMANAGER_H_
#define DISPLAYMANAGER_H_
//==============================
//AUTHOR : Choi Moon Seok
//CREATE DATE : 2021-03-12
// PURPOSE : SDL로 만든 화면을 관리하고 렌더링결과를 그린다.
//==============================
//SDL 2.0버전을 사용합니다. 화면을 그리는기능만 이용하며 다른 부가적인 기능은 최대한 사용하지 않습니다.
//==============================

#pragma once
//include
#include "buffer.h"
#include <SDL.h>
#undef main
class DisplayManager
{
public:
	//화면크기는 고정합니다.
	const static int SCREEN_WIDTH = 1280;
	const static int SCREEN_HEIGHT = 720;
	const static int SCREEN_PITCH = SCREEN_HEIGHT * sizeof(Uint32);
	constexpr static float SCREEN_ASPECT_RATIO = SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	//더미

	DisplayManager();
	~DisplayManager();

	//SDL 설정과 화면을 만들기위한 값을 초기화
	bool StartUp();
	//종료시 메모리를 반환하는 곳
	void ShutDown();

	//렌더링타겟있는 버퍼와 윈도우 버퍼를 교환하고 화면에 타겟을 그린다.
	void SwapBuffers(Buffer<Uint32> *pixelBuffer);

	void Display();

private:
	bool startSDL();
	bool createWindow();
	bool createScreenSurface();

	SDL_Surface *mSurface;
	SDL_Window *mWindow;
};
#endif


