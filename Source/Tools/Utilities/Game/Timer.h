#pragma once
#include <chrono>
#include <Tools/Utilities/System/ServiceLocator.h>
#include <Tools/Utilities/System/SingletonTemplate.h>
#define TimerInstance ServiceLocator::Instance().GetService<Timer>()
class SHIPYARD_API Singleton;

class Timer : public Singleton
{
public:
	void                 initialize();
	void                 Update();
	[[nodiscard]] float  getDeltaTime() const;
	float                getUnscaledDeltaTime();
	[[nodiscard]] double getTotalTime() const;
	static double        getUnscaledTotalTime();
	float                timeScale = 1;

private:
	std::chrono::high_resolution_clock::time_point myTimeSinceLastUpdate;
	std::chrono::high_resolution_clock::time_point myStartTime;
	float                                          myDeltaTime = 0.00001f;
};
