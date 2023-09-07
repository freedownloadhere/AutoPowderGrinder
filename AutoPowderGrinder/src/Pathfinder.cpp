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
	const std::deque<std::shared_ptr<AstarVector3>>& heap
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
	bool result{
		!Block::nonSolid.contains(this->minecraft->world->getBlockID(*coordinates)) &&
		this->minecraft->world->getBlockID(*coordinates + Vector3{ 0, 1, 0 }) == 0 &&
		this->minecraft->world->getBlockID(*coordinates + Vector3{ 0, 2, 0 }) == 0
	};

	return result;
	
	// Ehh maybe inefficient! Look at those method calls mate. Ugh. Total crap.
}

std::list<Vector3> AutoPowderGrinder::Pathfinder::makePath(const Vector3& start, const Vector3& target)
{
	// Can be optimized in space and time complexity.
	// Mostly meant to just test if it works really. And to pretend I did something useful.

	std::deque<std::shared_ptr<AstarVector3>> heapToSearch;
	std::map<AstarVector3, bool> processed;

	std::shared_ptr<AstarVector3> current{ std::make_shared<AstarVector3>(start) };
	current->setG(0);
	current->setH((int)Vector3::distance(*current, target));

	heapToSearch.push_back( current );
	processed.insert({ *current, false });

	while (!heapToSearch.empty())
	{
		std::make_heap(heapToSearch.begin(), heapToSearch.end(), AstarVector3());

		current = heapToSearch.front();

		heapToSearch.pop_front();
		processed[*current] = true;

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
			float distanceToNeighbour = (current->y == neighbour->y ? current->G + 1 : current->G + apg::SQRT_2);

			if (processed[*neighbour] == true || !this->isWalkable(neighbour))
				continue;

			if (neighbour->G > distanceToNeighbour)
			{
				neighbour->setG(distanceToNeighbour);
				neighbour->connection = current;
			}

			if (!this->listContains(neighbour, heapToSearch))
			{
				neighbour->setH((int)Vector3::distance(*current, target));
				heapToSearch.push_back(neighbour);
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