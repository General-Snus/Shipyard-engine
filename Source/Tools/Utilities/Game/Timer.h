#pragma once
#include <Tools/Utilities/System/ServiceLocator.h>
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <chrono>
#define TimerInstance ServiceLocator::Instance().GetService<Timer>()

class Timer : public Singleton
{
  public:
    void Initialize();
    void Update();
    float GetDeltaTime();
    float GetUnscaledDeltaTime();
    double GetTotalTime();
    double GetUnscaledTotalTime();
    float timeScale = 1;

  private:
    std::chrono::high_resolution_clock::time_point myTimeSinceLastUpdate;
    std::chrono::high_resolution_clock::time_point myStartTime;
    float myDeltaTime = 0.00001f;
};
