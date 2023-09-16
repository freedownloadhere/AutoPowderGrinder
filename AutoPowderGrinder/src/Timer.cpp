#include "AutoPowderGrinder.h"

using namespace apg;

Timer::Timer() : start(std::chrono::high_resolution_clock::now()), topic("Time")
{

}

Timer::Timer(const char* topic) : start(std::chrono::high_resolution_clock::now()), topic(topic)
{

}

Timer::~Timer()
{
	this->end = std::chrono::high_resolution_clock::now();

	std::cout << "[" << this->topic << "] Timer ran for " << (end - start).count() / 1000000 << " ms\n";
}