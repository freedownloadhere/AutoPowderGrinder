#include "AutoPowderGrinder.h"

using namespace apg;

AutoPowderGrinder::BlockManager::BlockManager(const std::shared_ptr<AutoPowderGrinder::Minecraft>& minecraft)
{
	this->initialized = this->initialize(minecraft);

	if (!this->initialized)
		std::cout << "Could not properly initialize the BlockManager object\n";
}

bool AutoPowderGrinder::BlockManager::initialize(const std::shared_ptr<AutoPowderGrinder::Minecraft>& minecraft)
{
	this->minecraft = minecraft;

	return true;
}

bool AutoPowderGrinder::BlockManager::isInitialized()
{
	return this->initialized;
}

Block AutoPowderGrinder::BlockManager::toBlock(const Vector3& pos)
{
	Block result{ this->minecraft->world->getBlockID(pos), pos };
	return result;
}

bool AutoPowderGrinder::BlockManager::positionIsValid(
	const Vector3& pos,
	const EnumFacing& facing,
	const Vector3& playerPosition
) const
{
	if (playerPosition.y != pos.y && (playerPosition.y - 1) != pos.y) // check whether block is at foot level or head level
		return false;

	if (pos.y < 0 || pos.y >= 255)
		return false;

	return true;
}

bool AutoPowderGrinder::BlockManager::checkBlockValidity(const Block& block) const
{
	if (this->minecraft->world->getBlockID(block.pos) == block.id)
		return true;
	return false;
}

bool AutoPowderGrinder::BlockManager::alreadyInQueue(const Block& block) const
{
	for (const auto& i : this->blockQueue)
	{
		if (block.pos == i.pos)
			return true;
	}
	return false;
}

bool AutoPowderGrinder::BlockManager::queueIsFull() const
{
	return this->blockQueue.size() >= AutoPowderGrinder::BlockManager::MAX_QUEUE_SIZE;
}

void AutoPowderGrinder::BlockManager::queueBlocks()
{
	std::deque<Vector3> positionsToSearch;
	EnumFacing facing = this->minecraft->player->getFacing();
	Vector3 playerPosition = this->minecraft->player->getHeadPosition();

	playerPosition.truncate2();

	Vector3 nextInFront = playerPosition;
	for(int k = 0; k < 9; ++k)
	{
		positionsToSearch.push_back(nextInFront);

		nextInFront = nextInFront + this->enumFacingVec[(int)facing];
	}

	while (!positionsToSearch.empty() && !this->queueIsFull())
	{
		Vector3 currentBlockPos = positionsToSearch.front();

		for (const Vector3& direction : this->directionalVector[(int)facing])
		{
			Vector3 neighbouringBlockPos = currentBlockPos + direction;

			if (this->queueIsFull())
				continue;
			if (!this->positionIsValid(neighbouringBlockPos, facing, playerPosition))
				continue;

			Block neighbouringBlock = this->toBlock(neighbouringBlockPos);

			if (this->alreadyInQueue(neighbouringBlock))
				continue;
			if (!neighbouringBlock.toBreak() && !neighbouringBlock.toOpen())
				continue;
					
			this->blockQueue.push_back(neighbouringBlock);
		}

		positionsToSearch.pop_front();
	}
}

bool AutoPowderGrinder::BlockManager::aimForBlock(const Block& targettedBlock)
{
	Vector3 localPosition = this->minecraft->player->getHeadPosition();
	Vector3 blockPosition = targettedBlock.pos + this->getBlockCenter;
	ViewAngles oldViewAngles = this->minecraft->player->getViewAngles();

	if (Vector3::distance(blockPosition, localPosition) > apg::MAX_REACH)
		return false;

	Vector3 deltaVector = blockPosition - localPosition;
	ViewAngles newViewAngles
	{
		-apg::clampAngle(std::atan2(deltaVector.x, deltaVector.z) * apg::TO_RADIANS, -360, 360),
		std::atan2(-deltaVector.y, std::hypot(-deltaVector.z, -deltaVector.x)) * apg::TO_RADIANS
	};

	newViewAngles.yaw = apg::clampAngle(newViewAngles.yaw, -180, 180);
	
	ViewAngles smoothed = (newViewAngles - oldViewAngles) / 10;

	for (int i = 0; i < 10; ++i)
	{
		this->minecraft->player->setViewAngles(oldViewAngles + smoothed);
		oldViewAngles = this->minecraft->player->getViewAngles();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	this->minecraft->player->setViewAngles(newViewAngles);

	return true;
}

void AutoPowderGrinder::BlockManager::actUponBlock(const Block& targettedBlock)
{
	if (this->minecraft->player->getLookingAt() != targettedBlock.pos)
		return;

	if (targettedBlock.toBreak())
		this->minecraft->player->leftClick();

	else if (targettedBlock.toOpen())
		this->minecraft->player->rightClick();
}

void AutoPowderGrinder::BlockManager::cleanUpQueue()
{
	while (!this->blockQueue.empty() && !checkBlockValidity(this->blockQueue.front()))
		this->blockQueue.pop_front();
}

void AutoPowderGrinder::BlockManager::doRoutine()
{
	this->queueBlocks();

	this->cleanUpQueue();

	if (this->blockQueue.empty())
		return;

	Block currentBlock = this->blockQueue.front();

	this->minecraft->player->sendChatMessage(
		"§7Aiming for §3" +
		std::to_string((int)currentBlock.pos.x) + " " +
		std::to_string((int)currentBlock.pos.y) + " " +
		std::to_string((int)currentBlock.pos.z) + "  "
	);

	bool lockedOntoBlock = this->aimForBlock(currentBlock);

	if (lockedOntoBlock)
		this->actUponBlock(currentBlock);

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}