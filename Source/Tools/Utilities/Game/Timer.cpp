#include <assert.h>
#include "Timer.h"

#include <Tools/Optick/include/optick.h>

Timer::Timer()
{
	myTimeSinceLastUpdate = std::chrono::high_resolution_clock::now();
	myStartTime = std::chrono::high_resolution_clock::now();
	myDeltaTime = 0.0f;
} 

void Timer::Update()
{
	OPTICK_EVENT();
	myDeltaTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - myTimeSinceLastUpdate).count();
	myTimeSinceLastUpdate = std::chrono::high_resolution_clock::now();
}

float Timer::GetDeltaTime()  
{
	return myDeltaTime;
}

float Timer::GetUnscaledDeltaTime()  
{
	assert(false && "Not implemented");
	return 0.0f;
}

//Nano seconds
double Timer::GetTotalTime()  
{
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - myStartTime).count();
}

double Timer::GetUnscaledTotalTime()  
{
	assert(false && "Not implemented");
	return 0.0;
}
