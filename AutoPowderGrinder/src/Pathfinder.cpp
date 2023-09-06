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

bool AutoPowderGrinder::Pathfinder::listContains(
	const std::shared_ptr<AstarVector3>& element, 
	const std::deque<std::shared_ptr<AstarVector3>>& list
)
{
	for (const auto& i : list)
	{
		if (i == element)
			return true;
	}
	return false;
}

std::list<Vector3> AutoPowderGrinder::Pathfinder::makePath(const Vector3& start, const Vector3& end)
{
	std::deque<std::shared_ptr<AstarVector3>> queue;
	std::map<AstarVector3, bool> processed;

	std::shared_ptr<AstarVector3> current{ std::make_shared<AstarVector3>(start) };
	current->setG(0);
	current->setH(Vector3::distance(*current, end));

	queue.push_back( current );
	processed.insert({ *current, false });

	while (!queue.empty())
	{
		std::cout << "Looking at " << *current << " , G: " << current->G << " H: " << current->H << " P: " << processed[*current] << "\n";

		std::make_heap(queue.begin(), queue.end(), AstarVector3());

		current = queue.front();

		queue.pop_front();
		processed[*current] = true;

		if (*current == end)
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
			
			if (processed[*neighbour] == true || !Block::toBlock(*neighbour).isWalkable())
				continue;

			if (neighbour->G <= current->G + 1)
				continue;
			
			neighbour->setG(current->G + 1);
			neighbour->connection = current;

			if (this->listContains(neighbour, queue))
				continue;

			neighbour->setH(Vector3::distance(*current, end));
			queue.push_back(neighbour);
		}
	}

	return {};
}

std::list<EnumFacing> AutoPowderGrinder::Pathfinder::makeIndications(const std::list<Vector3>& path)
{
	if (path.size() < 2)
		return {};

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