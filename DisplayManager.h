#ifndef DISPLAYMANAGER_H_
#define DISPLAYMANAGER_H_
//==============================
//AUTHOR : Choi Moon Seok
//CREATE DATE : 2021-03-12
// PURPOSE : SDL�� ���� ȭ���� �����ϰ� ����������� �׸���.
//==============================
//SDL 2.0������ ����մϴ�. ȭ���� �׸��±�ɸ� �̿��ϸ� �ٸ� �ΰ����� ����� �ִ��� ������� �ʽ��ϴ�.
//==============================

#pragma once
//include
#include "buffer.h"
#include <SDL.h>
#undef main
class DisplayManager
{
public:
	//ȭ��ũ��� �����մϴ�.
	const static int SCREEN_WIDTH = 1280;
	const static int SCREEN_HEIGHT = 720;
	const static int SCREEN_PITCH = SCREEN_HEIGHT * sizeof(Uint32);
	constexpr static float SCREEN_ASPECT_RATIO = SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	//����

	DisplayManager();
	~DisplayManager();

	//SDL ������ ȭ���� ��������� ���� �ʱ�ȭ
	bool StartUp();
	//����� �޸𸮸� ��ȯ�ϴ� ��
	void ShutDown();

	//������Ÿ���ִ� ���ۿ� ������ ���۸� ��ȯ�ϰ� ȭ�鿡 Ÿ���� �׸���.
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


