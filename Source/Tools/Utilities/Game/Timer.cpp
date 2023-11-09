#include "Timer.h" 

Timer::Timer() 
{
	myTimeSinceLastUpdate = std::chrono::high_resolution_clock::now();
	myStartTime = std::chrono::high_resolution_clock::now();
	myDeltaTime = 0.0f;
} 

Timer& Timer::GetInstance()
{
	static Timer instance;
	return  instance;
}

void Timer::Update()
{
	myDeltaTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - myTimeSinceLastUpdate).count();
	myTimeSinceLastUpdate = std::chrono::high_resolution_clock::now();
}

float Timer::GetDeltaTime() const
{
	return myDeltaTime;
}

//Nano seconds
double Timer::GetTotalTime() const
{
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - myStartTime).count();
}
