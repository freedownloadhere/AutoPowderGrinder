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

	/*auto beginning = std::chrono::high_resolution_clock::now();

	Vector3
		start = this->minecraft->player->getBlockBelowPosition(),
		end{ 1188, 80, -1321 };

	auto path = this->pathfinder->makePath(start, end);

	auto finish = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::seconds>(finish - beginning);

	this->minecraft->player->sendMessageToPlayer(
		"§7Finding path took §a" + std::to_string(duration.count()) + " §7seconds   "
	);

	for (const auto& i : path)
	{
		if (GetAsyncKeyState(VK_NUMPAD0))
			break;

		this->minecraft->player->sendMessageFromPlayer(
			"/setblock " + i.toString() + " lapis_block"
		);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}*/

	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		std::cout << this->minecraft->chat->getLatestChatMessage() << '\n';

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

double apg::clampAngle(double angle, float min, float max)
{
	if (angle >= 90)
		angle -= 360;

	return std::clamp<double>(angle, min, max);
}