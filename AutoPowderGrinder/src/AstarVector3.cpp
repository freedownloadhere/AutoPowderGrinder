#include "AutoPowderGrinder.h"

using namespace apg;

void AstarVector3::setG(int value)
{
	this->G = value;
	this->F = this->G + this->H;
}

void AstarVector3::setH(int value)
{
	this->H = value;
	this->F = this->G + this->H;
}