#include "AutoPowderGrinder.h"

using namespace apg;

AutoPowderGrinder::BlockManager::BlockManager(const std::shared_ptr<AutoPowderGrinder::Minecraft>& minecraft)
{
	this->initialized = this->initialize(minecraft);

	if (!this->initialized)
		std::cout << "Could not properly initialize the StoneMiner object\n";
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

bool AutoPowderGrinder::BlockManager::positionMeetsCriteria(
	const Vector3& pos,
	const EnumFacing& facing,
	const Vector3& playerPosition
) const
{
	if (playerPosition.y != pos.y && (playerPosition.y - 1) != pos.y) // check whether block is at foot level or head level
		return false;
	if (pos.y < 0 || pos.y >= 255)
		return false;

	if (
		facing == EnumFacing::NORTH ||
		facing == EnumFacing::SOUTH
		)
		return
		(
			abs(playerPosition.x - pos.x) <= AutoPowderGrinder::BlockManager::MAX_SEARCH_DISTANCE_SIDE &&
			abs(playerPosition.z - pos.z) <= AutoPowderGrinder::BlockManager::MAX_SEARCH_DISTANCE_FRONT
		);

	else
		return
		(
			abs(playerPosition.z - pos.z) <= AutoPowderGrinder::BlockManager::MAX_SEARCH_DISTANCE_SIDE &&
			abs(playerPosition.x - pos.x) <= AutoPowderGrinder::BlockManager::MAX_SEARCH_DISTANCE_FRONT
		);
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

//void AutoPowderGrinder::BlockManager::findInitialStone()
//{
//	if (this->blockQueue.size() >= AutoPowderGrinder::BlockManager::MAX_QUEUE_SIZE)
//		return;
//
//	std::set<Vector3> foundPositions;
//	std::queue<Vector3> positionsToSearch;
//	EnumFacing facing = this->minecraft->player->getFacing();
//	Vector3 playerPosition = this->minecraft->player->getPosition();
//
//	playerPosition.truncate();
//	//playerPosition = playerPosition + this->direction[(int)facing] * 2; // To make blocks in front more likely to be picked
//
//	positionsToSearch.push(playerPosition);
//
//	while (!positionsToSearch.empty())
//	{
//		Vector3 currentPos = positionsToSearch.front();
//		foundPositions.insert(currentPos);
//
//		for (const Vector3& direction : this->d)
//		{
//			Vector3 neighbouringBlock = currentPos + direction;
//			if (this->positionMeetsCriteria(neighbouringBlock, facing, playerPosition) && !foundPositions.contains(neighbouringBlock))
//			{
//				positionsToSearch.push(neighbouringBlock);
//				foundPositions.insert(neighbouringBlock);
//			}
//		}
//
//		if (this->toBlock(currentPos).toBreak())
//		{
//			this->queueBlocks(
//				currentPos,
//				facing,
//				playerPosition
//			);
//			return;
//		}
//
//		positionsToSearch.pop();
//	}
//}

void AutoPowderGrinder::BlockManager::queueBlocks()
{
	std::deque<Vector3> positionsToSearch;
	EnumFacing facing = this->minecraft->player->getFacing();
	Vector3 playerPosition = this->minecraft->player->getHeadPosition();

	playerPosition.truncate2();

	Vector3 nextInFront = playerPosition;
	for(int k = 0; k < 9; ++k)
	{
		if((toBlock(nextInFront).toBreak()))
			positionsToSearch.push_back(nextInFront);

		nextInFront = nextInFront + this->d[(int)facing];
	}

	while (!positionsToSearch.empty() && this->blockQueue.size() < AutoPowderGrinder::BlockManager::MAX_QUEUE_SIZE)
	{
		Vector3 currentBlockPos = positionsToSearch.front();

		for (const Vector3& direction : this->d_Straight[(int)facing])
		{
			Vector3 neighbouringBlockPos = currentBlockPos + direction;

			if 
			(
				this->blockQueue.size() < AutoPowderGrinder::BlockManager::MAX_QUEUE_SIZE &&
				this->positionMeetsCriteria(neighbouringBlockPos, facing, playerPosition)
			)
			{
				Block neighbouringBlock = this->toBlock(neighbouringBlockPos);
				if(!this->alreadyInQueue(neighbouringBlock))
					if (neighbouringBlock.toBreak() || neighbouringBlock.toOpen())
						this->blockQueue.push_back(neighbouringBlock);
			}
		}

		positionsToSearch.pop_front();
	}
}


bool AutoPowderGrinder::BlockManager::aimForBlock(const Block& targettedBlock)
{
	Vector3 localPosition = this->minecraft->player->getHeadPosition();
	Vector3 blockPosition = targettedBlock.pos + this->getBlockCenter;

	if (Vector3::distance(blockPosition, localPosition) > apg::MAX_REACH)
		return false;

	Vector3 delta = blockPosition - localPosition;

	float newYaw = -apg::clampAngle(std::atan2(delta.x, delta.z) * apg::TO_RADIANS, -360, 360);
	float newPitch = std::atan2(-delta.y, std::hypot(-delta.z, -delta.x)) * apg::TO_RADIANS;

	this->minecraft->player->setYawPitch(newYaw, newPitch);

	return true;
}

void AutoPowderGrinder::BlockManager::actUponBlock(const Block& targettedBlock)
{
	/*if (this->minecraft->player->getLookingAt() != targettedBlock.pos)
		return;*/

	if (targettedBlock.toBreak())
	{
		this->minecraft->player->leftClick();
	}
	else if (targettedBlock.toOpen())
	{
		this->minecraft->player->rightClick();
	}
}

void AutoPowderGrinder::BlockManager::doRoutine()
{
	this->queueBlocks();

	while (!this->blockQueue.empty() && !checkBlockValidity(this->blockQueue.front()))
		this->blockQueue.pop_front();

	if (!this->blockQueue.empty())
	{
		Block currentBlock = this->blockQueue.front();

		this->minecraft->player->sendChatMessage(
			"§7Aiming for §3" +
			std::to_string((int)currentBlock.pos.x) + " " +
			std::to_string((int)currentBlock.pos.y) + " " +
			std::to_string((int)currentBlock.pos.z) +
			"   "
		);

		bool lockedOntoBlock = this->aimForBlock(currentBlock);
		if (lockedOntoBlock)
			this->actUponBlock(currentBlock);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}