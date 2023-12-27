#include "AssetManager.pch.h"
#include "../AIPollingManager.h"


void AIPollingManager::AddStation(PollingStation aNewStation,const std::string& aGivenName)
{
	myPollingStations.try_emplace(aGivenName,new PollingStation(aNewStation));
}

PollingStation* AIPollingManager::GetStation(const std::string& aName)
{
	if(myPollingStations.contains(aName))
	{
		return myPollingStations.at(aName);
	}
	return nullptr;
}

template<typename T>
T* AIPollingManager::GetStation(const std::string& aName)
{
	if(myPollingStations.contains(aName))
	{
		return static_cast<T*>(myPollingStations.at(aName));
	}
	return nullptr;
}