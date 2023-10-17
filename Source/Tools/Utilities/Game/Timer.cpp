#include "Timer.h" 

CommonUtilities::Timer::Timer() 
{
	myTimeSinceLastUpdate = std::chrono::high_resolution_clock::now();
	myStartTime = std::chrono::high_resolution_clock::now();
	myDeltaTime = 0.0f;
} 

CommonUtilities::Timer& CommonUtilities::Timer::GetInstance()
{
	static CommonUtilities::Timer instance;
	return  instance;
}

void CommonUtilities::Timer::Update()
{
	myDeltaTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - myTimeSinceLastUpdate).count();
	myTimeSinceLastUpdate = std::chrono::high_resolution_clock::now();
}

float CommonUtilities::Timer::GetDeltaTime() const
{
	return myDeltaTime;
}

double CommonUtilities::Timer::GetTotalTime() const
{
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - myStartTime).count();
}
