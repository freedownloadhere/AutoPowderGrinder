#include "AutoPowderGrinder.h"

using namespace apg;

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

	this->blockManager = std::make_unique<BlockManager>(this->minecraft);
	if (!this->blockManager->isInitialized())
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
		this->blockManager->doRoutine();
	}
}

float apg::clampAngle(float angle, float min, float max)
{
	if (angle >= 90)
		angle -= 360;

	return std::clamp(angle, min, max);
}