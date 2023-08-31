#include "AutoPowderGrinder.h"

using namespace apg;

double Vector3::distance(const Vector3& pos1, const Vector3& pos2)
{
	return 
		sqrt(
			(pos1.x - pos2.x) * (pos1.x - pos2.x) + 
			(pos1.y - pos2.y) * (pos1.y - pos2.y) + 
			(pos1.z - pos2.z) * (pos1.z - pos2.z)
		);
}

bool Vector3::operator==(const Vector3& other) const
{
	return
		this->x == other.x &&
		this->y == other.y &&
		this->z == other.z;
}

bool Vector3::operator!=(const Vector3& other) const
{
	return !(*this == other);
}

Vector3 Vector3::operator+(const Vector3& other) const
{
	Vector3 result = { this->x + other.x, this->y + other.y, this->z + other.z };
	return result;
}

Vector3 Vector3::operator-(const Vector3& other) const
{
	Vector3 result = { this->x - other.x, this->y - other.y, this->z - other.z };
	return result;
}

bool Vector3::operator<(const Vector3& other) const
{
	if (this->x < other.x)
		return true;
	if (this->x == other.x && this->y < other.y)
		return true;
	if (this->x == other.x && this->y == other.y && this->z < other.z)
		return true;
	return false;
}

Vector3 Vector3::operator*(int multiplier) const
{
	Vector3 result = { this->x * multiplier, this->y * multiplier, this->z * multiplier };
	return result;
}

void Vector3::truncate()
{
	this->x = static_cast<int>(this->x);
	this->y = static_cast<int>(this->y);
	this->z = static_cast<int>(this->z);
}