#include "AutoPowderGrinder.h"

using namespace apg;

void AstarVector3::setG(float value)
{
	this->G = value;
	this->F = this->G + this->H;
}

void AstarVector3::setH(float value)
{
	this->H = value;
	this->F = this->G + this->H;
}

bool AstarVector3::operator()(const std::shared_ptr<AstarVector3>& lhs, const std::shared_ptr<AstarVector3>& rhs) const
{
	if (lhs->F > rhs->F)
		return true;
	if (lhs->F == rhs->F && lhs->H > rhs->H)
		return true;
	return false;
}