#pragma once
#include <chrono>
class Timer
{
  public:
    Timer(const Timer &aTimer) = delete;
    Timer &operator=(const Timer &aTimer) = delete;

    static void Initialize();
    static void Update();
    static float GetDeltaTime();
    static float GetUnscaledDeltaTime();
    static double GetTotalTime();
    static double GetUnscaledTotalTime();
    static inline float timeScale = 1;

  private:
     
    static inline std::chrono::high_resolution_clock::time_point myTimeSinceLastUpdate;
    static inline std::chrono::high_resolution_clock::time_point myStartTime;
    static inline float myDeltaTime = 0.00001f;
};
