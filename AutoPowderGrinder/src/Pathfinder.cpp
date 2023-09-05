#include "AutoPowderGrinder.h"

using namespace apg;

AutoPowderGrinder::Pathfinder::Pathfinder()
{
	this->initialized = this->initialize();

	if (!this->initialized)
		std::cout << "Failed to initialize Pathfinder class\n";
}

bool AutoPowderGrinder::Pathfinder::initialize()
{
	return true;
}

bool AutoPowderGrinder::Pathfinder::isInitialized()
{
	return this->initialized;
}

std::list<Vector3> AutoPowderGrinder::Pathfinder::makePath(const Vector3& start, const Vector3& end)
{
	if (start.y != end.y)
		return {};

	std::set<std::shared_ptr<AstarVector3>> toSearch, processed;

	std::shared_ptr<AstarVector3> startNode{ std::make_shared<AstarVector3>(Vector3::truncate2(start)) };

	toSearch.insert(startNode);

	while (!toSearch.empty())
	{
		std::shared_ptr<AstarVector3> currentNode{ nullptr };

		for (const auto& i : toSearch)
		{
			if (currentNode == nullptr || i->F < currentNode->F || (i->F == currentNode->F && i->H < currentNode->H))
				currentNode = i;
		}

		processed.insert(currentNode);
		toSearch.erase(currentNode);

		if (*currentNode == end)
		{
			std::shared_ptr<AstarVector3> currentPathNode = currentNode;
			std::list<Vector3> path;

			while (currentPathNode != startNode)
			{
				path.push_front({currentPathNode->x, currentPathNode->y, currentPathNode->z});
				currentPathNode = currentPathNode->connection;
			}

			return path;
		}

		for (const auto& k : this->directionalVector)
		{
			std::shared_ptr<AstarVector3> neighbourNode = std::make_shared<AstarVector3>(*currentNode + k);

			if (processed.contains(neighbourNode) || !Block::toBlock(*neighbourNode).isWalkable())
				continue;

			bool inSearch = toSearch.contains(neighbourNode);
			int costToNeighbour = currentNode->G + 1;

			if (!inSearch || costToNeighbour < neighbourNode->G)
			{
				neighbourNode->setG(costToNeighbour);
				neighbourNode->connection = currentNode;

				if (!inSearch)
				{
					neighbourNode->setH((int)Vector3::distance(*neighbourNode, end));
					toSearch.insert(neighbourNode);
				}
			}
		}
	}

	return {};
}

std::list<EnumFacing> AutoPowderGrinder::Pathfinder::makeIndications(const std::list<Vector3>& path)
{
	std::list<EnumFacing> result;

	std::list<Vector3>::const_iterator it1, it2;
	it1 = it2 = path.begin();
	++it2;

	while (it2 != path.end())
	{
		Vector3 delta = *it2 - *it1;

		for (int k = 0; k < 4; ++k)
			if (delta == this->directionalVector[k])
			{
				result.push_back((EnumFacing)(k + 2));
			}

		++it1;
		++it2;
	}

	return result;
}