#include "AssetManager.pch.h"
#include "../AIPollingManager.h" 


void AIPollingManager::AddStation(const std::string& aGivenName,std::shared_ptr<PollingStation> aNewStation)
{
	myPollingStations.try_emplace(aGivenName,aNewStation);
}

std::shared_ptr<PollingStation> AIPollingManager::GetStation(const std::string& aName)
{
	if(myPollingStations.contains(aName))
	{
		return myPollingStations.at(aName);
	}
	return nullptr;
}

template<typename T>
std::shared_ptr<T> AIPollingManager::GetStation(const std::string& aName)
{
	if(myPollingStations.contains(aName))
	{
		return std::static_pointer_cast<T>(myPollingStations.at(aName));
	}
	return nullptr;
}