#include "AutoPowderGrinder.h"

using namespace apg;

//bool Block::operator()(const Block& lhs, const Block& rhs) const
//{
//	bool
//		lhsIsChest = this->blocksToOpen.contains(lhs.id),
//		rhsIsChest = this->blocksToOpen.contains(rhs.id);
//
//	if (lhsIsChest && rhsIsChest)
//	{
//		/*Vector3 playerPosition = minecraft->player->getPosition();
//		return Vector3::distance(playerPosition, lhs.pos) > Vector3::distance(playerPosition, rhs.pos);*/
//		return true;
//	}
//	else if (lhsIsChest)
//		return true;
//	else if (rhsIsChest)
//		return false;
//	else
//		return true;
//}

bool Block::toBreak() const
{
	return this->blocksToBreak.contains(this->id);
}

bool Block::toOpen() const
{
	return this->blocksToOpen.contains(this->id);
}

bool Block::isAir() const
{
	return this->id == 0;
}

bool Block::isWalkable() const
{
	return !this->nonSolid.contains(this->id);
}

Block Block::toBlock(const Vector3& coordinates)
{
	Block result{ Block::minecraft->world->getBlockID(coordinates), coordinates };
	return result;
}