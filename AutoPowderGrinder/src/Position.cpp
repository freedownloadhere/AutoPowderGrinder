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