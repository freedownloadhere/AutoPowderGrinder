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

	this->pathfinder = std::make_unique<Pathfinder>();
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
			end{ 0, 3, 0 };

		auto path = this->pathfinder->makePath(start, end);
		auto ind = this->pathfinder->makeIndications(path);
		std::string indications = "";

		this->minecraft->player->sendChatMessage("§7Follow this path:\n ");
		for (const auto& indication : ind)
		{
			switch (indication)
			{
			case EnumFacing::NORTH: indications += "North; "; break;
			case EnumFacing::SOUTH: indications += "South; "; break;
			case EnumFacing::WEST: indications += "West; "; break;
			case EnumFacing::EAST: indications += "East; "; break;
			}
		}
		this->minecraft->player->sendChatMessage(indications);

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}
}

double apg::clampAngle(double angle, float min, float max)
{
	if (angle >= 90)
		angle -= 360;

	return std::clamp<double>(angle, min, max);
}