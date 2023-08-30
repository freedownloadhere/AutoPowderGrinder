#include "AutoPowderGrinder.h"

double Position::distance(const Position& pos1, const Position& pos2)
{
	return 
		sqrt(
			(pos1.x - pos2.x) * (pos1.x - pos2.x) + 
			(pos1.y - pos2.y) * (pos1.y - pos2.y) + 
			(pos1.z - pos2.z) * (pos1.z - pos2.z)
		);
}

bool Position::operator==(const Position& other) const
{
	return
		this->x == other.x &&
		this->y == other.y &&
		this->z == other.z;
}

bool Position::operator!=(const Position& other) const
{
	return !(*this == other);
}

Position Position::operator+(const Position& other) const
{
	Position result = { this->x + other.x, this->y + other.y, this->z + other.z };
	return result;
}

bool Position::operator<(const Position& other) const
{
	if (this->x < other.x)
		return true;
	if (this->x == other.x && this->y < other.y)
		return true;
	if (this->x == other.x && this->y == other.y && this->z < other.z)
		return true;
	return false;
}

Position Position::operator*(int multiplier) const
{
	Position result = { this->x * multiplier, this->y * multiplier, this->z * multiplier };
	return result;
}