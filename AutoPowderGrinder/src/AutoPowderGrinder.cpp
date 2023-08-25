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
	//this->minecraft->player->sendChatMessage("Running!\n");
	
	this->minecraft->player->sendChatMessage("§7Items currently in your inventory: ");
	for (int i = 0; i < 36; ++i)
	{
		std::string inventoryItem = this->minecraft->player->getInventoryItem(i);
		if (inventoryItem != "Air")
			this->minecraft->player->sendChatMessage("§7> §f" + inventoryItem + "§7 in slot §a" + std::to_string(i) + "    ");
	}
	this->minecraft->player->sendChatMessage("\n");

	std::this_thread::sleep_for(std::chrono::seconds(4));
}