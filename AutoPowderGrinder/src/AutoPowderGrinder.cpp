#include "AutoPowderGrinder.h"

AutoPowderGrinder::AutoPowderGrinder()
{
	this->initialized = this->initialize();

	if (!this->initialized)
		std::cout << "An error occured while initializing AutoPowderGrinder\n";
}

bool AutoPowderGrinder::initialize()
{
	this->minecraft = std::make_shared<Minecraft>();
	if (!this->minecraft->isInitialized())
	{
		std::cout << "An error occured while initializing Minecraft\n";
		return false;
	}

	this->stoneMiner = std::make_unique<StoneMiner>(this->minecraft);
	if (!this->stoneMiner->isInitialized())
	{
		std::cout << "An error occured while initializing StoneMiner\n";
		return false;
	}

	return true;
}

void AutoPowderGrinder::run()
{
	while(!GetAsyncKeyState(VK_NUMPAD0))
	{
		this->stoneMiner->doRoutine();
	}
}