#include "Engine.h"
int main()
{
	Engine engine;

	if (engine.StartUp())
		engine.Run();
	engine.ShutDown();
	return 0;
}