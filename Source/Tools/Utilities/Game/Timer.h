#pragma once
#include <chrono>  
class Timer
{
public:
	Timer(const Timer& aTimer) = delete;
	Timer& operator=(const Timer& aTimer) = delete;

	static Timer& GetInstance();

	void Update();

	float GetDeltaTime() const;
	float GetUnscaledDeltaTime() const;
	double GetTotalTime() const;
	double GetUnscaledTotalTime() const;
	float timeScale = 1; 
private:
	Timer();
	std::chrono::high_resolution_clock::time_point  myTimeSinceLastUpdate;
	std::chrono::high_resolution_clock::time_point  myStartTime;
	float myDeltaTime;
};

