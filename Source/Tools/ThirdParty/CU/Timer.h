#pragma once
#include <chrono> 
namespace CommonUtilities
{
	class Timer
	{
	public:
		Timer();
		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;

		static Timer& GetInstance();

		void Update();

		float GetDeltaTime() const;
		double GetTotalTime() const;  
	private: 
		std::chrono::high_resolution_clock::time_point  myTimeSinceLastUpdate;
		std::chrono::high_resolution_clock::time_point  myStartTime;
		float myDeltaTime; 
	};
}

