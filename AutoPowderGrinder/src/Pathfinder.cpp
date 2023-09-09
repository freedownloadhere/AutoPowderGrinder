#include "AutoPowderGrinder.h"

using namespace apg;

AutoPowderGrinder::Pathfinder::Pathfinder(const std::shared_ptr<Minecraft>& minecraft)
{
	this->initialized = this->initialize(minecraft);

	if (!this->initialized)
		std::cout << "Failed to initialize Pathfinder class\n";
}

bool AutoPowderGrinder::Pathfinder::initialize(const std::shared_ptr<Minecraft>& minecraft)
{
	this->minecraft = minecraft;

	return true;
}

bool AutoPowderGrinder::Pathfinder::isInitialized()
{
	return this->initialized;
}

bool AutoPowderGrinder::Pathfinder::listContains(
	const std::shared_ptr<AstarVector3>& element, 
	const std::vector<std::shared_ptr<AstarVector3>>& heap
)
{
	for (const auto& i : heap)
	{
		if (i == element)
			return true;
	}
	return false;
}

bool AutoPowderGrinder::Pathfinder::isWalkable(const std::shared_ptr<AstarVector3>& coordinates)
{
	Vector3 v[3] = { *coordinates, *coordinates + this->upOne, *coordinates + this->upTwo };

	for (const auto& i : v)
		if (!this->walkableMap.contains(i))
			this->walkableMap[i] = !Block::nonSolid.contains(this->minecraft->world->getBlockID(i));

	return(
		this->walkableMap[v[0]] && !this->walkableMap[v[1]] && !this->walkableMap[v[2]]
		);
}

std::list<Vector3> AutoPowderGrinder::Pathfinder::makePath(const Vector3& start, const Vector3& target)
{
	this->walkableMap.clear();

	std::vector<std::shared_ptr<AstarVector3>> heapToSearch;
	std::set<AstarVector3> processed;

	std::shared_ptr<AstarVector3> current{ std::make_shared<AstarVector3>(start) };
	current->setG(0);
	current->setH(Vector3::manhattanDistance(*current, target));

	heapToSearch.push_back( current );

	while (!heapToSearch.empty())
	{
		current = heapToSearch.front();

		std::pop_heap(heapToSearch.begin(), heapToSearch.end(), AstarVector3());
		heapToSearch.pop_back();
		processed.insert(*current);

		if (*current == target)
		{
			std::list<Vector3> result;

			while (*current != start)
			{
				result.push_front({current->x, current->y, current->z});
				current = current->connection;
			}
			result.push_front({ current->x, current->y, current->z });

			return result;
		}

		for (const auto& k : this->directionalVector)
		{
			std::shared_ptr<AstarVector3> neighbour{ std::make_shared<AstarVector3>(*current + k) };
			double distanceToNeighbour = (current->y == neighbour->y ? current->G + 1 : current->G + apg::SQRT_2);

			if (!this->isWalkable(neighbour))
				continue;

			if (processed.contains(*neighbour))
				continue;

			if (neighbour->G > distanceToNeighbour)
			{
				neighbour->setG(distanceToNeighbour);
				neighbour->connection = current;
			}

			if (!this->listContains(neighbour, heapToSearch))
			{
				neighbour->setH(Vector3::manhattanDistance(*current, target));
				heapToSearch.push_back(neighbour);
				std::push_heap(heapToSearch.begin(), heapToSearch.end(), AstarVector3());
			}
		}
	}

	return {};
}

void AutoPowderGrinder::Pathfinder::moveThroughPath(const std::list<Vector3>& path)
{
	std::list<Vector3>::const_iterator it1, it2;
	it1 = path.begin();
	it2 = it1++;

	while (it2 != path.end())
	{

	}
}