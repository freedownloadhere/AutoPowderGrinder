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
	else
	{
		Block::minecraft = this->minecraft;
	}

	this->blockManager = std::make_unique<BlockManager>(minecraft);
	if (!this->blockManager->isInitialized())
	{
		std::cout << "An error occured while initializing BlockManager\n";
		return false;
	}

	this->pathfinder = std::make_unique<Pathfinder>(minecraft);
	if (!this->pathfinder->isInitialized())
	{
		std::cout << "An error occured while initializing Pathfinder\n";
		return false;
	}

	return true;
}

void AutoPowderGrinder::run()
{
	if (!this->initialized)
	{
		std::cout << "Cannot run the auto powder grinder as it has not been properly initialized\n";
		return;
	}

	std::cout << "Hi, this does nothing atm :)\n";
}

double apg::clampAngle(double angle, double min, double max)
{
	if (angle >= 90)
		angle -= 360;

	return std::clamp<double>(angle, min, max);
}