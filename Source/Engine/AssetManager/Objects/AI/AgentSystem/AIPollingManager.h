#pragma once
#include <Tools/Utilities/System/SingletonTemplate.h>

class AIPollingManager : public Singleton<AIPollingManager>
{
	friend class Singleton<AIPollingManager>;
public:
	void AddStation(PollingStation aNewStation,const std::string& aGivenName);

	PollingStation* GetStation(const std::string& aName);

	template<typename T>
	T* GetStation(const std::string& aName);
private:
	std::unordered_map<std::string, PollingStation*> myPollingStations;
};

