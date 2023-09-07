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

	while(!GetAsyncKeyState(VK_NUMPAD0))
	{
		Vector3
			start = this->minecraft->player->getBlockBelowPosition(),
			end{ -1389, 3, 439 };

		auto path = this->pathfinder->makePath(start, end);

		for (const auto& i : path)
		{
			this->minecraft->player->sendMessageFromPlayer(
				"/setblock " + i.toString() + " redstone_block"
			);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		for (const auto& i : path)
		{
			this->minecraft->player->sendMessageFromPlayer(
				"/setblock " + i.toString() + " stone"
			);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

double apg::clampAngle(double angle, float min, float max)
{
	if (angle >= 90)
		angle -= 360;

	return std::clamp<double>(angle, min, max);
}