#include "AutoPowderGrinder.h"

AutoPowderGrinder::StoneMiner::StoneMiner(const std::shared_ptr<AutoPowderGrinder::Minecraft>& minecraft)
{
	this->initialized = this->initialize(minecraft);

	if (!this->initialized)
		std::cout << "Could not properly initialize the StoneMiner object\n";
}

bool AutoPowderGrinder::StoneMiner::initialize(const std::shared_ptr<AutoPowderGrinder::Minecraft>& minecraft)
{
	this->minecraft = minecraft;

	return true;
}

bool AutoPowderGrinder::StoneMiner::isInitialized()
{
	return this->initialized;
}

bool AutoPowderGrinder::StoneMiner::isStone(const Position& pos)
{
	return this->minecraft->world->getBlockID(pos) == AutoPowderGrinder::StoneMiner::STONE_ID;
}

bool AutoPowderGrinder::StoneMiner::alreadyInQueue(const Position& pos)
{
	for (const auto& i : this->stoneToMine)
	{
		if (i == pos)
			return true;
	}

	return false;
}

bool AutoPowderGrinder::StoneMiner::positionMeetsCriteria(
	const Position& pos,
	AutoPowderGrinder::Minecraft::Player::EnumFacing facing,
	const Position& playerPosition
)
{
	if (playerPosition.y != pos.y && (playerPosition.y + 1) != pos.y) // check whether block is at foot level or head level
		return false;
	if (pos.y < 0 || pos.y >= 255)
		return false;

	if (
		facing == AutoPowderGrinder::Minecraft::Player::EnumFacing::NORTH ||
		facing == AutoPowderGrinder::Minecraft::Player::EnumFacing::SOUTH
		)
		return
		(
			abs(playerPosition.x - pos.x) <= AutoPowderGrinder::StoneMiner::MAX_SEARCH_DISTANCE_SIDE &&
			abs(playerPosition.z - pos.z) <= AutoPowderGrinder::StoneMiner::MAX_SEARCH_DISTANCE_FRONT
		);

	else
		return
		(
			abs(playerPosition.z - pos.z) <= AutoPowderGrinder::StoneMiner::MAX_SEARCH_DISTANCE_SIDE &&
			abs(playerPosition.x - pos.x) <= AutoPowderGrinder::StoneMiner::MAX_SEARCH_DISTANCE_FRONT
		);
}

void AutoPowderGrinder::StoneMiner::findStone()
{
	if (this->stoneToMine.size() >= AutoPowderGrinder::StoneMiner::MAX_QUEUE_SIZE)
	{
		this->minecraft->player->sendChatMessage("§8Stone queue is full. No more stone has been added.");
		return;
	}

	std::set<Position> foundPositions;
	std::queue<Position> positionsToSearch;
	AutoPowderGrinder::Minecraft::Player::EnumFacing facing = this->minecraft->player->getFacing();
	Position playerPosition = this->minecraft->player->getPosition();

	playerPosition = playerPosition + this->direction[(int)facing] * 2; // To make blocks in front more likely to be picked

	positionsToSearch.push(playerPosition);

	while 
	(
		!positionsToSearch.empty() && 
		this->stoneToMine.size() < AutoPowderGrinder::StoneMiner::MAX_QUEUE_SIZE
	)
	{
		Position currentPos = positionsToSearch.front();
		foundPositions.insert(currentPos);

		for (const Position& d : this->direction)
		{
			Position neighbouringBlock = currentPos + d;
			if (this->positionMeetsCriteria(neighbouringBlock, facing, playerPosition) && !foundPositions.contains(neighbouringBlock))
			{
				positionsToSearch.push(neighbouringBlock);
				foundPositions.insert(neighbouringBlock);
			}
		}

		if(this->isStone(currentPos) && !this->alreadyInQueue(currentPos))
			this->stoneToMine.push_back(currentPos);

		positionsToSearch.pop();
	}
}

void AutoPowderGrinder::StoneMiner::doRoutine()
{
	this->findStone();

	while (
		!this->stoneToMine.empty() &&
		this->minecraft->world->getBlockID(this->stoneToMine.front()) != AutoPowderGrinder::StoneMiner::STONE_ID
	)
		this->stoneToMine.pop_front();

	if (!this->stoneToMine.empty())
	{
		this->minecraft->player->sendChatMessage("\n\n§7The queue contains §a" + std::to_string(this->stoneToMine.size()) + " §7elements:   \n");

		bool frontOfQueue{ true };
		for (const auto& i : this->stoneToMine)
		{
			if (frontOfQueue)
				this->minecraft->player->sendChatMessage(
					"§3" + std::to_string(i.x) + " " + std::to_string(i.y) + " " + std::to_string(i.z) + " §7(front of queue)  "
				);
			else
				this->minecraft->player->sendChatMessage(
					"§2" + std::to_string(i.x) + " " + std::to_string(i.y) + " " + std::to_string(i.z) + " "
				);
			frontOfQueue = false;
		}
	}

	std::this_thread::sleep_for(std::chrono::seconds(2));
}