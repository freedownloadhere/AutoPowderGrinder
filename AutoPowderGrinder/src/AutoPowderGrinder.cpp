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

	return true;
}

void AutoPowderGrinder::run()
{
	Position pos = this->minecraft->player->getLookingAt();
	int blockID = this->minecraft->world->getBlockID(pos); // Could be optimized to use the already existing blockPos instance.

	this->minecraft->player->sendChatMessage(
		"§7You are looking at §a" + std::to_string((int)pos.x) + " " + std::to_string((int)pos.y) + " " + std::to_string((int)pos.z)
		+
		" §7which has the ID§3 " + std::to_string(blockID) + "\n    "
	);

	std::this_thread::sleep_for(std::chrono::seconds(1));
}