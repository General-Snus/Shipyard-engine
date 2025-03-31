#include "Timer.h"
#include <cassert>

#include <External/Optick/include/optick.h>

void Timer::initialize()
{
	myTimeSinceLastUpdate = std::chrono::high_resolution_clock::now();
	myStartTime = std::chrono::high_resolution_clock::now();
	myDeltaTime = 0.0f;
}

void Timer::Update()
{
	OPTICK_EVENT();
	myDeltaTime =
		std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - myTimeSinceLastUpdate).count(); 
	myTimeSinceLastUpdate = std::chrono::high_resolution_clock::now();
}

float Timer::getDeltaTime() const
{
	return myDeltaTime;
}

float Timer::getUnscaledDeltaTime()
{
	assert(false && "Not implemented");
	return 0.0f;
}

// Nano seconds
double Timer::getTotalTime() const
{
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - myStartTime).count();
}

double Timer::getUnscaledTotalTime()
{
	assert(false && "Not implemented");
	return 0.0;
}
