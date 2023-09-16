#include "AutoPowderGrinder.h"

using namespace apg;

Timer::Timer()
{
	this->start = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
	this->end = std::chrono::high_resolution_clock::now();

	std::cout << "[Time] " << (end - start).count() / 1000000 << " ms\n";
}