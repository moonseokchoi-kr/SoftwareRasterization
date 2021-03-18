#include "Engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

bool Engine::StartUp()
{
	bool succees = true;
	if (!gDisplayManger.StartUp())
	{
		succees = false;
	}
	return succees;
}

bool Engine::ShutDown()
{
	gDisplayManger.ShutDown();
	return false;
}

void Engine::Run()
{

	while (true)
	{
		gDisplayManger.Display();
	}
	
}
