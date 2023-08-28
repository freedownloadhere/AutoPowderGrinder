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
		return false;

	return true;
}

void AutoPowderGrinder::run()
{
	int blockID = this->minecraft->world->getBlockID(
		{ 0, 4, 0 }
	);

	this->minecraft->player->sendChatMessage(
		"\n\n§7The block at position §a0 4 0 §7has the ID §a" + std::to_string(blockID) + "    \n\n"
	);

	std::this_thread::sleep_for(std::chrono::seconds(3));
}