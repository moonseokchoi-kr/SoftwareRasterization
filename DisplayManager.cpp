#include "DisplayManager.h"
#include <stdio.h>
#include <string.h>
DisplayManager::DisplayManager()
{
}

DisplayManager::~DisplayManager()
{
}

bool DisplayManager::StartUp()
{
	bool success = true;
	if (!startSDL())
	{
		success = false;
	}
	else {
		if (!createWindow())
		{
			success = false;
		}
		if (!createScreenSurface())
		{
			success = false;
		}
	}
	return success;
}

void DisplayManager::ShutDown()
{
	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;
	SDL_Quit();
}

void DisplayManager::SwapBuffers(Buffer<Uint32>* pixelBuffer)
{
	//Allows surface editing
	SDL_LockSurface(mSurface);

	//픽셀버퍼를 surface 로 복사
	memcpy(mSurface->pixels, pixelBuffer->buffer, pixelBuffer->mHeight*pixelBuffer->mWidth);
	SDL_UnlockSurface(mSurface);

	//화면을 바꿉니다.
	SDL_UpdateWindowSurface(mWindow);
}

void DisplayManager::Display()
{
	//Fill the surface white
	SDL_FillRect(mSurface, NULL, SDL_MapRGB(mSurface->format, 0xFF, 0xFF, 0xFF));

	

	//Update the surface
	SDL_UpdateWindowSurface(mWindow);

	//Wait two seconds
	SDL_Delay(2000);
}

bool DisplayManager::startSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) !=0 )
	{
		printf("Failed to initialize SDL, Error %s\n", SDL_GetError());
		return false;
	}
	return true;
}

bool DisplayManager::createWindow()
{
	mWindow = SDL_CreateWindow("Software Rasterization", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (mWindow == nullptr)
	{
		printf("Could not create Window: %s\n", SDL_GetError());
		return false;
	}
	return true;
}
//화면을 가져옵니다.
bool DisplayManager::createScreenSurface()
{
	mSurface = SDL_GetWindowSurface(mWindow);
	if (mSurface == nullptr)
	{
		printf("Could not create Surface: %s\n", SDL_GetError());
		return false;
	}
	return true;
}
