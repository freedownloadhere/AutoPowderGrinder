#include "AutoPowderGrinder.h"

using namespace apg;

ViewAngles ViewAngles::operator+(const ViewAngles& other) const
{
	ViewAngles result{ this->yaw + other.yaw, this->pitch + other.pitch };
	return result;
}

ViewAngles ViewAngles::operator-(const ViewAngles& other) const
{
	ViewAngles result{ this->yaw - other.yaw, this->pitch - other.pitch };
	return result;
}

ViewAngles ViewAngles::operator/(int divisor) const
{
	ViewAngles result{ this->yaw / divisor, this->pitch / divisor };
	return result;
}

ViewAngles ViewAngles::operator*(int multiplier) const
{
	ViewAngles result{ this->yaw * multiplier, this->pitch * multiplier };
	return result;
}