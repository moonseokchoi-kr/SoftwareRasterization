#ifndef ENGINE_H_
#define ENGINE_H_
#pragma once
#include "DisplayManager.h"
#include <SDL.h>
class Engine
{
public:
	Engine();
	~Engine();

	bool StartUp();
	bool ShutDown();
	void Run();
private:
	DisplayManager gDisplayManger;
};
#endif


