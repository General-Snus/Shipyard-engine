#pragma once 
#include "PollingStations/PollingStation.h"
#include <Tools/Utilities/System/SingletonTemplate.h>

class AIPollingManager : public Singleton<AIPollingManager>
{
	friend class Singleton<AIPollingManager>;
public:
	void AddStation(const std::string& aGivenName,std::shared_ptr<PollingStation> aNewStation);
	std::shared_ptr<PollingStation> GetStation(const std::string& aName);

	template<typename T>
	std::shared_ptr<T> GetStation(const std::string& aName);
private:
	std::unordered_map<std::string,std::shared_ptr<PollingStation>> myPollingStations;
};

