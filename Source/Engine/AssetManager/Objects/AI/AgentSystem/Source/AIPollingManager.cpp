#include "AssetManager.pch.h"
#include "../AIPollingManager.h" 

#include "Engine/AssetManager/Objects/AI/AgentSystem/PollingStations/Target_PollingStation.h"


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

//Why do i need you?
template<>
std::shared_ptr<MultipleTargets_PollingStation> AIPollingManager::GetStation(const std::string& aName)
{
	if(myPollingStations.contains(aName))
	{
		return std::static_pointer_cast<MultipleTargets_PollingStation>(myPollingStations.at(aName));
	}
	return nullptr;
}